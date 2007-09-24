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

#include <cantag/algorithms/ContourFollowerClearImageBorder.hh>

namespace Cantag {

  bool ContourFollowerClearImageBorder::operator()(MonochromeImage& dest) const {
    const unsigned int height = dest.GetHeight();
    const unsigned int width = dest.GetWidth();
    for(unsigned int i=0;i<height;++i) {
      dest.SetPixel(0,i,false);
      dest.SetPixel(width-1,i,false);
      if (i == 0 || i == height-1) {
	for(unsigned int j=0;j<width-1;++j) {
	  dest.SetPixel(j,i,false);
	}
      }
    }
    return true;
  }
}
