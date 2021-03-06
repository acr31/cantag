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

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/Image.hh>
#include <cantag/MonochromeImage.hh>

namespace Cantag {

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> class ThresholdGlobal 
    : public Function<TypeList<Image<size,layout> >, // needed because the preprocessor can't parse the call to TL1(Image<size,layout>)
		      TL1(MonochromeImage)> {
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
    int setpixels = 0;
    for(unsigned int y=0;y<height;++y) {
      const PixRow<layout> row = image.GetRow(y);
      typename PixRow<layout>::const_iterator pixel=row.begin();
      for(unsigned int x=0;x<width;x++) { 
	bool value = (*pixel).intensity() < m_threshold;
	++pixel;
	if (value) ++setpixels;
	dest.SetPixel(x,y,value);
      }
    }
    dest.SetVariation(setpixels);
    return true;
  }
}

#endif//THRESHOLD_GLOBAL_GUARD
