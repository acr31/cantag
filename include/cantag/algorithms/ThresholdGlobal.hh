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

#ifndef THRESHOLD_GLOBAL_GUARD
#define THRESHOLD_GLOBAL_GUARD

#include <cantag/Image.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>

namespace Cantag {

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> class ThresholdGlobal 
    : public Function1<Image<size,layout>,MonochromeImage> {
  private:
    int m_threshold;
  public:
    ThresholdGlobal(int threshold);
    bool operator()(const Image<size,layout>& source, MonochromeImage& dest) const;
  };

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout>
  ThresholdGlobal<size,layout>::ThresholdGlobal(int threshold) : m_threshold(threshold) {}
  
  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> 
  bool ThresholdGlobal<size,layout>::operator()(const Image<size,layout>& image, MonochromeImage& dest) const {

    const unsigned int height=image.GetHeight();
    const unsigned int width=image.GetWidth();
	  
    for(unsigned int y=0;y<height;++y) {
      const PixRow<layout> row = image.GetRow(y);
      typename PixRow<layout>::const_iterator pixel=row.begin();
      for(unsigned int x=0;x<width;x++) { 
	dest.SetPixel(x,y, (*pixel).intensity() < m_threshold);
	++pixel;
      }
    }

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

#endif//THRESHOLD_GLOBAL_GUARD
