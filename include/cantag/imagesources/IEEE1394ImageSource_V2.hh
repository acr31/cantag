/*
  Copyright (C) 2004 Andrew C. Rice, 2007 Tom Craig

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

  Email: acr31@cam.ac.uk, jatc2@cam.ac.uk
*/

/**
 * $Header$
 */

#ifndef IEEE1394_IMAGE_SOURCE2_GUARD
#define IEEE1394_IMAGE_SOURCE2_GUARD

#ifndef HAVE_DC1394_CONTROL_H_V2
#error "This version has been configured without libdc1394 version 2 support"
#endif

#include <cantag/Config.hh>
#include <cantag/imagesources/ImageSource.hh>

extern "C" {
  #include <dc1394/control.h>
};

namespace Cantag {

 class IEEE1394ImageSource_V2 : public ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> {
     
 public:
   IEEE1394ImageSource_V2(
               int dc1394_video_mode,
               int width,
               int height,
               int dc1394_framerate,
               dc1394speed_t dc1394_iso_speed,
	       unsigned int camera_id,
	       int numBuffers = 10);

   virtual ~IEEE1394ImageSource_V2();
   virtual Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* Next();
   inline int GetWidth() const { return mWidth; }
   inline int GetHeight() const {  return mHeight; }

   bool              SetDC1394Feature(unsigned int camera, int feature, int value, bool is_mode);
   dc1394camera_t ** GetCameraArray(int *size);
   void              SelectCamera(unsigned int id);
   bool              Initialise(unsigned int camera_id, int numBuffers = 10);

  private:

   dc1394video_frame_t  *mFrame;
   dc1394camera_t      **mCameras;
   bool                  mInit[32];
   int                   mCamera;
   unsigned int          mNumCameras;

   int                   mWidth;
   int                   mHeight;

   Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>  *mImage;
 };

}

#endif//IEEE1394_IMAGE_SOURCE2_GUARD
