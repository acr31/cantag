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

#include <cantag/GetImageSource.hh>

namespace Cantag {
  void GetImageSourceHelp(std::ostream& ostream) {
    
    ostream << IMAGE_SOURCE_FILE << ": FileImageSource" << std::endl;

    ostream << IMAGE_SOURCE_V4L << ": V4LImageSource";
#ifndef HAVE_LINUX_VIDEODEV_H
    ostream << " (disabled)";
#endif
    ostream << std::endl;
    
    ostream << IMAGE_SOURCE_DC1394_V1 << ": IEEE1934ImageSource";
#ifndef HAVE_DC1394_CONTROL_H_V1
    ostream << " (disabled)";
#endif
    ostream << std::endl;
    
    
    ostream << IMAGE_SOURCE_DC1394_V2 << ": IEEE1934ImageSource_V2";
#ifndef HAVE_DC1394_CONTROL_H_V2
    ostream << " (disabled)";
#endif
    ostream << std::endl;
    
    
    ostream << IMAGE_SOURCE_VFW << ": VFWImageSource";
#ifndef WIN32
    ostream << " (disabled)";
#endif
    ostream << std::endl;
    
    
    ostream << IMAGE_SOURCE_DSVL << ": DSVLImageSource";
#ifndef WIN32
    ostream << " (disabled)";
#endif
    ostream << std::endl;
    
    
    ostream << IMAGE_SOURCE_UEYE << ": UEyeImageSource";
#ifndef HAVE_UEYE_H
    ostream << " (disabled)";
#endif
    ostream << std::endl;
    
  }
}
