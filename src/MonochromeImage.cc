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

#include <total/MonochromeImage.hh>
#include <total/Image.hh>

namespace Total {

  MonochromeImage::MonochromeImage(unsigned int width, unsigned int height) : m_width(width),m_height(height),m_data(new bool[width*height])  {}

  MonochromeImage::~MonochromeImage() {
    delete[] m_data;
  }

  void MonochromeImage::Save(const char* filename) const {
    Image<Colour::Grey> image(m_width,m_height);
    for(unsigned int i=0;i<m_height;++i) {
      unsigned char* data_pointer = image.GetRow(i);
      for(unsigned int j=0;j<m_width;++j) {
	*data_pointer = GetPixel(j,i) ? 255 : 0;
	++data_pointer;
      }
    }
    image.Save(filename);
  }
}
