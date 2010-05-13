/*
  Copyright (C) 2008 Andrew C. Rice

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
#ifndef GET_IMAGE_SOURCE_GUARD
#define GET_IMAGE_SOURCE_GUARD

#include <ostream>

#include <cantag/Config.hh>
#include <cantag/ImageSource.hh>
#include <cantag/FileImageSource.hh>

#ifdef HAVE_LINUX_VIDEODEV_H
# include <cantag/V4LImageSource.hh>
#endif

#if defined(HAVE_GL_GL_H) and defined(HAVE_GL_GLU_H) and defined(HAVE_GL_OSMESA_H) and defined(HAVELIB_GL) and defined(HAVELIB_OSMESA) and defined(HAVELIB_GLU)
# include <cantag/GLImageSource.hh>
#endif

#if defined(HAVE_DC1394_CONTROL_H_V1)
# include <cantag/IEEE1394ImageSource_V1.hh>
#endif

#if defined(HAVE_DC1394_CONTROL_H_V2)
# include <cantag/IEEE1394ImageSource_V2.hh>
#endif

#ifdef WIN32
# include <cantag/VFWImageSource.hh>
# include <cantag/DSVLImageSource.hh>
#endif

#ifdef HAVE_UEYE_H
# include <cantag/UEyeImageSource.hh>
#endif

namespace Cantag {

  enum image_source_t {
    IMAGE_SOURCE_FILE,
    IMAGE_SOURCE_V4L,
    IMAGE_SOURCE_DC1394_V1,
    IMAGE_SOURCE_DC1394_V2,
    IMAGE_SOURCE_VFW,
    IMAGE_SOURCE_DSVL,
    IMAGE_SOURCE_UEYE
  };

  void GetImageSourceHelp(std::ostream& ostream);

  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> 
  ImageSource<B,L>*
  GetImageSource(image_source_t image_source, const char* device_file) {
    switch(image_source) {
    case IMAGE_SOURCE_FILE:
      if (!device_file) return NULL;
      return new FileImageSource<B,L>(device_file);
    case IMAGE_SOURCE_V4L:
#ifdef HAVE_LINUX_VIDEODEV_H
      if (!device_file) device_file = "/dev/video0";
      return new V4LImageSource<B,L>(device_file,0);
#else
      return NULL;
#endif
    default:
      return NULL;
    }
  }

  template<>
  ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>*
  GetImageSource(image_source_t image_source, const char* device_file) {
    switch(image_source) {
    case IMAGE_SOURCE_FILE:
      if (!device_file) return NULL;
      return new FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>(device_file);
    case IMAGE_SOURCE_V4L:
#ifdef HAVE_LINUX_VIDEODEV_H
      if (!device_file) device_file = "/dev/video0";
      return new V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>(device_file,0);
#else
      return NULL;
#endif
    case IMAGE_SOURCE_DC1394_V1:
#ifdef HAVE_DC1394_CONTROL_H_V1
      if (!device_file) device_file = "/dev/video1394/0";
      return new IEEE1394ImageSource(device_file,0,MODE_640x480_MONO, FRAMERATE_30,500,32 );      
#else
      return NULL;
#endif
    case IMAGE_SOURCE_DC1394_V2:
#ifdef HAVE_DC1394_CONTROL_H_V2
      return new IEEE1394ImageSource_V2(DC1394_VIDEO_MODE_640x480_MONO8, 640, 480, DC1394_FRAMERATE_30, DC1394_ISO_SPEED_400,0,10);
#else
      return NULL;
#endif
    case IMAGE_SOURCE_UEYE:
#ifdef HAVE_UEYE_H
      return new UEyeImageSource();
#else
      return NULL;
#endif
    case IMAGE_SOURCE_VFW:
#ifdef WIN32
      return new VFWImageSource();
#else 
      return NULL;
#endif
    case IMAGE_SOURCE_DSVL:
#ifdef WIN32
      if (!device_file) return NULL;
      return new DSVLImageSource(device_file);
#else
      return NULL;
#endif
    default:
      return NULL;
    }
  };

}
#endif//GET_IMAGE_SOURCE_GUARD
