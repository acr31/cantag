/*
  Copyright (C) 2007 Robert Harle, Tom Craig

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

  Email: rkh23@cam.ac.uk, jatc2@cam.ac.uk
*/

/**
 * $Header$
 */

#include <cantag/Config.hh>

#ifdef HAVELIB_DC1394_V1

#include <cerrno>
#include <cantag/IEEE1394ImageSource_V1.hh>

#undef IEEE1394_DEBUG

namespace Cantag {

  IEEE1394ImageSource_V1::IEEE1394ImageSource_V1(char *device_name,
						 int port,
						 int mode,
						 int framerate,
						 int shutter,
						 int gain,
						 u_int64_t guid) :mImage(0) {
    // Create a handle to the specified port
    mHandle = dc1394_create_handle(port);

    int nCam = 0;
    nodeid_t * camera_nodes = dc1394_get_camera_nodes(mHandle, &nCam, 0);
     
    if (nCam==0 || mHandle==NULL) {
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


    if (shutter>=0) dc1394_set_shutter(mHandle, mCamera.node,shutter);
    if (gain>=0) dc1394_set_gain(mHandle, mCamera.node,gain);

    unsigned int channel, speed;
    if (dc1394_get_iso_channel_and_speed(mHandle, mCamera.node, &channel, &speed) != DC1394_SUCCESS) {
      throw "Unable to get Camera channel/speed";
    }
     
    // Now we want to set up the capture parameters
    dc1394_dma_setup_capture(mHandle, mCamera.node,
			     channel,
			     FORMAT_VGA_NONCOMPRESSED,
			     mode,
			     speed,
			     framerate,
			     8,1,  
			     0,  // Yes, this should be device_name but it segfaults if it's anything OTHER than 0!
			     &mCamera);

    
    // Start the transmission
    dc1394_start_iso_transmission(mHandle, mCamera.node);
    dc1394_free_camera_nodes(camera_nodes);
  }

  
  IEEE1394ImageSource_V1::~IEEE1394ImageSource_V1() {
    dc1394_dma_unlisten( mHandle, &mCamera );
    dc1394_dma_release_camera(mHandle,&mCamera);
    dc1394_destroy_handle(mHandle);
  }


  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* IEEE1394ImageSource_V1::Next() {
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
      mImage = new Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>(GetWidth(),GetHeight());
    }
    dc1394_dma_multi_capture(&mCamera,1);
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

#endif//HAVELIB_DC1394_V1

