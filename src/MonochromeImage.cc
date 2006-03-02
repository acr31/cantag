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

#include <cantag/MonochromeImage.hh>
#include <cantag/Image.hh>

namespace Cantag {

  MonochromeImage::MonochromeImage(unsigned int width, unsigned int height) : m_width(width),m_height(height),m_data(new bool[width*height]),m_flipv(false)  {}

  MonochromeImage::~MonochromeImage() {
    delete[] m_data;
  }

  void MonochromeImage::Save(const char* filename) const {
    Image<Pix::Sze::Byte1, Pix::Fmt::Grey8> image(m_width,m_height);
      for(unsigned int y=0;y<m_height;++y) {
	PixRow<Pix::Fmt::Grey8> row = image.GetRow(y);
	PixRow<Pix::Fmt::Grey8>::iterator pixel=row.begin();
	for(unsigned int x=0;x<m_width;++x) {
	  unsigned char val = GetPixel(x,y) ? 255 : 0;
	  //image.DrawPixel(x,y,Pixel<Pix::Fmt::Grey8>(val));
	  pixel.v(val);
	  ++pixel;
	}
      }
      image.Save(filename);
  }
}
