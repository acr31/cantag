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

#include <cantag/algorithms/ThresholdGlobal.hh>

namespace Cantag {
  ThresholdGlobal::ThresholdGlobal(int threshold) : m_threshold(threshold) {}
  
  bool ThresholdGlobal::operator()(const Image<Colour::Grey>& image, MonochromeImage& dest) const {
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    for(int i=0;i<height;++i) {
      const unsigned char* data_pointer = image.GetRow(i);
      for(int j=0;j<width;++j) {
	const unsigned char pixel = *data_pointer;
	dest.SetPixel(j,i, pixel < m_threshold);
	++data_pointer;
      }
    }

    for(int i=0;i<height;++i) {
      dest.SetPixel(0,i,false);
      dest.SetPixel(width-1,i,false);
      if (i == 0 || i == height-1) {
	for(int j=0;j<width-1;++j) {
	  dest.SetPixel(j,i,false);
	}
      }
    }
    return true;
  }
};
