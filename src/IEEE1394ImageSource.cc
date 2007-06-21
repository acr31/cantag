/*
  Copyright (C) 2004 Robert Harle

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

  Email: rkh23@cam.ac.uk
*/

/**
 * $Header$
 */

#include <cantag/Config.hh>
#include <cerrno>



#ifdef HAVE_DC1394_CONTROL_H_V2
#include <cantag/IEEE1394ImageSource.hh>

#undef IEEE1394_DEBUG

namespace Cantag {

#ifdef HAVE_DC1394_CONTROL_H_V2

  ////////////////////////////////////////////////////////
  // v2 api
  ////////////////////////////////////////////////////////
  IEEE1394ImageSource::IEEE1394ImageSource(
					   int dc1394_video_mode,
					   int width,
					   int height,
					   int dc1394_framerate,
					   dc1394speed_t dc1394_iso_speed
					   ) 
    : mCamera(0), mWidth(width), mHeight(height), mFrame(0), mImage(0)
  {

    if (dc1394_find_cameras(&mCameras,&mNumCameras)!=DC1394_SUCCESS) {
      throw "Failed to find any firewire cameras on the bus";
    }

    // Reset as much as we can!
    for (unsigned int i=0; i<mNumCameras; i++) {
      dc1394_reset_camera(mCameras[i]);
      dc1394_reset_bus(mCameras[i]);
      dc1394_video_set_transmission(mCameras[i], DC1394_OFF);
      dc1394_capture_stop(mCameras[i]);

      dc1394_video_set_iso_speed(mCameras[i],dc1394_iso_speed);
      dc1394_video_set_mode(mCameras[i], (dc1394video_mode_t)dc1394_video_mode);
    }


    for (int i=0; i<32; i++) {
      mInit[i]=false;
    }
  }


  bool  IEEE1394ImageSource::SetDC1394Feature(unsigned int camera, int feature, int value, bool is_mode) {
    if (mInit[camera]) throw "IEEE1394ImageSource: Attempt to set feature after initialisation";
    if (camera >= mNumCameras) throw "Illegal camera ID";
    if (!is_mode) {
      if(dc1394_feature_set_value(mCameras[camera], (dc1394feature_t)feature, value)!=DC1394_SUCCESS) return false;
    }
    else {
      if(dc1394_feature_set_mode(mCameras[camera], (dc1394feature_t)feature, (dc1394feature_mode_t)value)!=DC1394_SUCCESS) return false;
    }
    return true;
  }

  dc1394camera_t ** IEEE1394ImageSource::GetCameraArray(int *size) { 
    *size = mNumCameras;
    return mCameras;
  }

  bool IEEE1394ImageSource::Initialise(unsigned int camera_id, int numBuffers=10) {
    if (mInit[camera_id]) throw "IEEE1394ImageSource: Attempt to set feature after initialisation";
    if (camera_id >= mNumCameras) throw "Illegal camera ID";
    if (dc1394_capture_setup(mCameras[camera_id], numBuffers, DC1394_CAPTURE_FLAGS_DEFAULT)) {
      throw "Failed to setup capture transmission";
    };

    if (dc1394_video_set_transmission(mCameras[camera_id],DC1394_ON) !=DC1394_SUCCESS) {
      throw "Failed to set transmission";
    }
    mInit[camera_id]=true;
    return true;
  }


  IEEE1394ImageSource::~IEEE1394ImageSource() {
    for (unsigned int i=0; i<mNumCameras; i++) {
      if (mInit[i]) {
	dc1394_video_set_transmission(mCameras[i], DC1394_OFF);
	dc1394_capture_stop(mCameras[i]);
      }
    }
  }

  void IEEE1394ImageSource::SelectCamera(unsigned int id) {
    if (id >= mNumCameras) throw "Illegal camera ID";
    mCamera=id;
  }

  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* IEEE1394ImageSource::Next() {
    if (!mInit[mCamera]) throw "Call to Next() on initialised camera!";
    if (mFrame!=0)dc1394_capture_enqueue(mCameras[mCamera],mFrame);
    dc1394_capture_dequeue(mCameras[mCamera], DC1394_CAPTURE_POLICY_WAIT, &mFrame);
    if (!mImage) mImage = new Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>(mWidth, mHeight);
    unsigned char *img = mFrame->image;
    for (int i=0; i<GetWidth(); i++) {
      for (int j=0; j<GetHeight(); j++) {
	mImage->DrawPixelNoCheck(i,j,img[i+j*mWidth]);
      }
    }
    mImage->SetValid(true);
    return mImage;
  }

#else

  ////////////////////////////////////////////////////////
  // v1 api
  ////////////////////////////////////////////////////////

  IEEE1394ImageSource::IEEE1394ImageSource(
					   char *device_name,
					   int port,
					   int mode,
					   int framerate,
					   int shutter,
					   int gain,
					   u_int64_t guid
					   ) :mImage(0)
  {
    
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

  
  IEEE1394ImageSource::~IEEE1394ImageSource() {
    dc1394_dma_unlisten( mHandle, &mCamera );
    dc1394_dma_release_camera(mHandle,&mCamera);
    dc1394_destroy_handle(mHandle);
  }


  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* IEEE1394ImageSource::Next() {
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

#endif

};
#endif//HAVE_DC1394_CONTROL_H
