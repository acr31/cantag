/*
  Copyright (C) 2006 Andrew C. Rice

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

#include <iostream>

#include <Cantag.hh>

using namespace Cantag;

int main(int argc,char* argv[]) {
  const int image_source   = argc >= 2 ? atoi(argv[1]) : -1;
  const int num_images     = argc == 3 ? atoi(argv[2]) : -1;
  ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>* source = NULL;
  try {
    switch(image_source) {
    case 0:
#ifdef HAVE_LINUX_VIDEODEV_H
      source = new V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>("/dev/video0",0);
      break;
#else
      throw "Image source selection V4LImageSource is not available";
#endif
    case 1:
#ifdef HAVE_DC1394_CONTROL_H_V1
      source = new IEEE1394ImageSource("/dev/video1394/0",0,MODE_640x480_MONO, FRAMERATE_30,500,32 );      
      break;
#else
      throw "Image source selection IEEE1394ImageSource version 1 is not available";
#endif
    case 2:
#ifdef HAVE_DC1394_CONTROL_H_V2
      source = new IEEE1394ImageSource_V2(DC1394_VIDEO_MODE_640x480_MONO8, 640, 480, DC1394_FRAMERATE_30, DC1394_ISO_SPEED_400,0,10);
      break;
#else
      throw "Image source selection IEEE1394ImageSource_V2 is not available";
#endif
    case 3:
#ifdef HAVE_UEYE_H
      source = new UEyeImageSource();
      break;
#else
      throw "Image source selection UEyeImageSource is not available";
#endif
    default:
      throw "Unrecognised image source. Valid options are 0 - V4LImageSource; 1 - IEEE1394ImageSource version 1; 2 - IEEE1394ImageSource version 2; 3 - UEyeImageSource";
    }

    int count = 0;
    while(count++ != num_images) {
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = source->Next();
      std::cout << "P2" << std::endl;
      std::cout << i->GetWidth() << " " << i->GetHeight() << std::endl;
      std::cout << 255 << std::endl;
      for(unsigned int y=0;y<i->GetHeight();++y) {
	const PixRow<Pix::Fmt::Grey8> row = i->GetRow(y);
	for(PixRow<Pix::Fmt::Grey8>::const_iterator x=row.begin(); 
	    x!= row.end(); ++x) {
	  std::cout << (int)x.v() << std::endl;
	}
      }
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
  if (source != NULL) delete source;
}

