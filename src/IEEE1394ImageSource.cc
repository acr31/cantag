/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */

#include <total/IEEE1394ImageSource.hh>

#include <cerrno>

#undef IEEE1394_DEBUG

namespace Total {


  IEEE1394ImageSource::IEEE1394ImageSource(
					   char *device_name,
					   int port,
					   u_int64_t guid,
					   int framerate,
					   int shutter,
					   int gain
					   ) :mImage(0)
  {
    
    // Create a handle to the specified port
     mHandle = dc1394_create_handle(port);

     int nCam = 0;
     nodeid_t * camera_nodes = dc1394_get_camera_nodes(mHandle, &nCam, 0);
     
     if (nCam==0) {
       throw "Unable to find a 1394 Camera. Please check that you have modprobed raw1394 and videodev1394 and created the relevant device nodes in /dev";
     }

     if (nCam>1) {
       // More than one camera - use guid to select
       dc1394_camerainfo info;
       bool found = false;
       for (int i=0; i<nCam; i++) {
	 if (dc1394_get_camera_info(mHandle, camera_nodes[i], &info) == DC1394_SUCCESS) {
	   if (info.euid_64 == guid) {
	     dc1394_print_camera_info(&info);
	     mCamera.node = camera_nodes[i];
	     found=true;
	     break;
	   }
	 }
       }
       if (!found) throw "Unable to find a 1394 Camera. Please check that you have modprobed raw1394 and videodev1394 and created the relevant device nodes in /dev";
     } 
     else {
       mCamera.node = camera_nodes[0];
     }
     dc1394_free_camera_nodes(camera_nodes);

     if (shutter>=0) dc1394_set_shutter(mHandle, mCamera.node,shutter);
     if (gain>=0) dc1394_set_gain(mHandle, mCamera.node,gain);
     
     // Now we want to set up the capture parameters
     dc1394_dma_setup_capture(mHandle, mCamera.node,
			      0, /* channel */ 
			      FORMAT_VGA_NONCOMPRESSED,
			      MODE_640x480_MONO,
			      SPEED_400,
			      framerate,
			      8,1,  
			      device_name,
			      &mCamera);

     // Start the transmission
     dc1394_start_iso_transmission(mHandle, mCamera.node);
  }

  
  IEEE1394ImageSource::~IEEE1394ImageSource() {
    dc1394_dma_release_camera(mHandle,&mCamera);
    dc1394_destroy_handle(mHandle);
  }


  Image<Colour::Grey>* IEEE1394ImageSource::Next() {
    /*
     * This is what we would like,
     * but we can't write to the memory
     * yet  \todo
     
     if (mImage) {
     dc1394_dma_done_with_buffer(&mCamera);
     delete mImage;
     }
     dc1394_dma_single_capture(&mCamera);
     mImage = new Image(640,480,640,(unsigned char *)mCamera.capture_buffer);
     return mImage;
    */

    // Doing a copy instead at the moment!!
    if (!mImage) {
      mImage = new Image<Colour::Grey>(GetWidth(),GetHeight());
    }
    dc1394_dma_single_capture(&mCamera);
    unsigned char *img = (unsigned char*) mCamera.capture_buffer;
    for (int i=0; i<GetWidth(); i++) {
      for (int j=0; j<GetHeight(); j++) {
	mImage->DrawPixelNoCheck(i,j,img[i+j*GetWidth()]);
      }
    }
    dc1394_dma_done_with_buffer(&mCamera);
    mImage->SetValid(true);
    return mImage;
  }

}
