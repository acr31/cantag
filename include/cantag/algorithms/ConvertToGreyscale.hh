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

#ifndef CONVERT_TO_GREYSCALE_GUARD
#define CONVERT_TO_GREYSCALE_GUARD

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/Image.hh>

namespace Cantag {

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> class ConvertToGreyscale
    : public Function<TypeList<Image<size,layout> >, TypeList<Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>>> {
  public:
    ConvertToGreyscale();
    bool operator()(const Image<size,layout>& source, Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>& dest) const;
  };

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout>
  ConvertToGreyscale<size,layout>::ConvertToGreyscale() {}
  
  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> 
  bool ConvertToGreyscale<size,layout>::operator()(const Image<size,layout>& image, Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>& dest) const {

    const unsigned int height=image.GetHeight();
    const unsigned int width=image.GetWidth();

    for(unsigned int y=0;y<height;++y) {
      const PixRow<layout> row = image.GetRow(y);
      typename PixRow<layout>::const_iterator pixel=row.begin();
      for(unsigned int x=0;x<width;x++) { 
        unsigned char grey = (*pixel).intensity();
        Pixel<Pix::Fmt::Grey8> p(grey);
        dest.DrawPixel(x,y,p);
        ++pixel;
      }
    }
    return true;
  }
}

#endif//CONVERT_TO_GREYSCALE_GUARD
