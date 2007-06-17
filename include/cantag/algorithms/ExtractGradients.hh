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

#ifndef EXTRACT_GRADIENTS_GUARD
#define EXTRACT_GRADIENTS_GUARD

#include <cantag/Config.hh>
#include <cantag/EntityTree.hh>
#include <cantag/Function.hh>

namespace Cantag
{
  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> class CANTAG_EXPORT ExtractGradients
    : public Function<TypeList<Image<size, layout> >,
		      TypeList<Image<size, layout> > >
  {
  public:
    bool operator()(const Image<size,layout>& src, Image<size,layout>& dest) const;
  };

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout>
  bool ExtractGradients<size, layout>::operator()(const Image<size,layout>& src, Image<size,layout>& dest) const
  {
    if (!src.SameDimensions(dest))
      return false;

    const int NORMALISATION_FACTOR = 8;
    unsigned int pixval = 0;
    for (int x = 0; x < (int) src.GetWidth(); ++x)
    {
      for (int y = 0; y < (int) src.GetHeight(); ++y)
      {
	for (int i = -1; i < 2; ++i)
	  for (int j = -1; j < 2; ++j)
	    if (i || j)
	      pixval += abs(src.Sample(x + i, y + j).intensity() - src.Sample(x, y).intensity());
	pixval /= NORMALISATION_FACTOR;
	dest.DrawPixel(x, y, pixval);
      }
    }

    return true;
  }
}

#endif // EXTRACT_GRADIENTS_GUARD
