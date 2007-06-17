/*
  Copyright (C) 2007 Tom Craig

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

  Email: jatc2@cam.ac.uk
*/

/**
 * $Header$
 */

#ifndef CONVOLUTION_GUARD
#define CONVOLUTION_GUARD

#include <cantag/Config.hh>
#include <cantag/ConvolutionFilter.hh>
#include <cantag/Function.hh>
#include <cantag/Image.hh>

#include <iostream>
using std::cout;
using std::endl;

namespace Cantag {

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout, int win_halfwidth, int win_halfheight>
  class Convolution : public Function<TypeList< Image<size, layout> >, // needed because the preprocessor can't parse the call to TL1(Image<size,layout>)
                                      TypeList< Image<size, layout> > > {
  private:
    ConvolutionFilter<win_halfwidth, win_halfheight> m_window;
  public:
    Convolution(const ConvolutionFilter<win_halfwidth, win_halfheight>& window) : m_window(window) {}
    bool operator()(const Image<size, layout>& src, Image<size, layout>& dest) const;
  };

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout, int win_halfwidth, int win_halfheight> 
  bool Convolution<size, layout, win_halfwidth, win_halfheight>::operator()(const Image<size, layout>& src, Image<size, layout>& dest) const
  {
    if (!src.SameDimensions(dest))
      return false;

    for (unsigned int i = 0; i < src.GetWidth(); i++)
    {
      for (unsigned int j = 0; j < src.GetHeight(); j++)
      {
	int newpix = 0;
	for (unsigned int k = 0; k < 2 * win_halfwidth + 1; k++)
	{
	  for (unsigned int l = 0; l < 2 * win_halfheight + 1; l++)
	  {
	    newpix += m_window.GetValue(k, l) * (int) (src.Sample(i + k - win_halfwidth, j + l - win_halfheight).intensity());
	  }
	}
	newpix /= m_window.GetNormalisationFactor();
	if (newpix < 0) newpix = 0;
	if (newpix > 255) newpix = 255;
	dest.DrawPixel(i, j, newpix);
      }
    }

    return true;
  }
}

#endif //CONVOLUTION_GUARD
