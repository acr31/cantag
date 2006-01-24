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

#ifndef IEEE1394_IMAGE_SOURCE_GUARD
#define IEEE1394_IMAGE_SOURCE_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_DC1394_CONTROL_H
#error "IEEE1394ImageSource Requires libraw1394 and libdc1394 support"
#endif

#include <cantag/ImageSource.hh>

extern "C" {
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h> // libdc1394 interface
};

namespace Cantag {

  class IEEE1394ImageSource : public ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> {
  private:
    raw1394handle_t      mHandle;
    dc1394_cameracapture mCamera;
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>  *mImage;
  
  public:

    IEEE1394ImageSource(
			char *device_name,
			int port,
			int mode=MODE_640x480_MONO,
			int framerate= FRAMERATE_30,
			int shutter=-1,
			int gain=-1,
			u_int64_t guid=0
			);
    virtual ~IEEE1394ImageSource();
    virtual Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* Next();
    inline int GetWidth() const { return 640; }
    inline int GetHeight() const {  return 480; }

  };
}
#endif//V4L_IMAGE_SOURCE_GUARD
