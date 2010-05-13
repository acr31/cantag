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

#ifdef HAVELIB_DC1394_V2

#include <cerrno>
#include <cantag/IEEE1394ImageSource_V2.hh>

#undef IEEE1394_DEBUG

namespace Cantag {

  ////////////////////////////////////////////////////////
  // v2 api
  ////////////////////////////////////////////////////////
  IEEE1394ImageSource_V2::IEEE1394ImageSource_V2(int dc1394_video_mode,
						 int width,
						 int height,
						 int dc1394_framerate,
						 dc1394speed_t dc1394_iso_speed,
						 unsigned int camera_id,
						 int numBuffers)
    : mFrame(0), mCamera(0), mWidth(width), mHeight(height), mImage(0)  {

    dc1394_t* d = dc1394_new();
    dc1394camera_list_t* pList = NULL;
    
    if (dc1394_camera_enumerate(d, &pList) != DC1394_SUCCESS) {
      throw "Failed to find any firewire cameras on the bus";
    }

    // Reset as much as we can!
    for (unsigned int i=0; i<mNumCameras; i++) {
      dc1394_camera_reset(mCameras[i]);
      dc1394_reset_bus(mCameras[i]);
      dc1394_video_set_transmission(mCameras[i], DC1394_OFF);
      dc1394_capture_stop(mCameras[i]);

      dc1394_video_set_iso_speed(mCameras[i],dc1394_iso_speed);
      dc1394_video_set_mode(mCameras[i], (dc1394video_mode_t)dc1394_video_mode);
      
      // TOM
      SetDC1394Feature(i, DC1394_FEATURE_BRIGHTNESS, 304, false);
      SetDC1394Feature(i, DC1394_FEATURE_EXPOSURE, 511, false);
      SetDC1394Feature(i, DC1394_FEATURE_SHARPNESS, 82, false);
      SetDC1394Feature(i, DC1394_FEATURE_SATURATION, 90, false);
      SetDC1394Feature(i, DC1394_FEATURE_GAMMA, 1, false);
      SetDC1394Feature(i, DC1394_FEATURE_SHUTTER, 6, false);
      SetDC1394Feature(i, DC1394_FEATURE_GAIN, 87, false);
      dc1394_feature_whitebalance_set_value(mCameras[i], 95, 87);
    }


    for (int i=0; i<32; i++) {
      mInit[i]=false;
    }

    Initialise(camera_id,numBuffers);
  }


  bool  IEEE1394ImageSource_V2::SetDC1394Feature(unsigned int camera, int feature, int value, bool is_mode) {
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

  dc1394camera_t ** IEEE1394ImageSource_V2::GetCameraArray(int *size) { 
    *size = mNumCameras;
    return mCameras;
  }

  bool IEEE1394ImageSource_V2::Initialise(unsigned int camera_id, int numBuffers) {
    if (mInit[camera_id]) throw "IEEE1394ImageSource: Attempt to set feature after initialisation";
    if (camera_id >= mNumCameras) throw "Illegal camera ID";
    if (dc1394_capture_setup(mCameras[camera_id], numBuffers, 0)) {
      throw "Failed to setup capture transmission";
    };

    if (dc1394_video_set_transmission(mCameras[camera_id],DC1394_ON) !=DC1394_SUCCESS) {
      throw "Failed to set transmission";
    }
    mInit[camera_id]=true;
    return true;
  }


  IEEE1394ImageSource_V2::~IEEE1394ImageSource_V2() {
    for (unsigned int i=0; i<mNumCameras; i++) {
      if (mInit[i]) {
	dc1394_video_set_transmission(mCameras[i], DC1394_OFF);
	dc1394_capture_stop(mCameras[i]);
      }
    }
  }

  void IEEE1394ImageSource_V2::SelectCamera(unsigned int id) {
    if (id >= mNumCameras) throw "Illegal camera ID";
    mCamera=id;
  }

  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* IEEE1394ImageSource_V2::Next() {
    if (!mInit[mCamera]) throw "Call to Next() on uninitialised camera!";
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
  
}

#endif//HAVELIB_DC1394_V2

