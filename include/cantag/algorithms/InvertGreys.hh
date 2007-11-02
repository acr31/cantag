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

#ifndef INVERT_GREYS_GUARD
#define INVERT_GREYS_GUARD

#include <iostream>

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/Image.hh>

namespace Cantag
{
  class CANTAG_EXPORT InvertGreys
    : public Function< TypeList< Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>, TypeListEOL>,
		       TypeList< Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>, TypeListEOL> >
  {
  private:
  public:
    InvertGreys() {}
    bool operator()(const Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>& in,
		    Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>& out) const
    {
      for (unsigned int y = 0; y < in.GetHeight(); ++y)
      {
	for (unsigned int x = 0; x < in.GetWidth(); ++x)
	{
	  const Pixel<Pix::Fmt::Grey8>& pix = in.Sample(x, y);
	  Pixel<Pix::Fmt::Grey8> newpix(255 - pix.intensity());
	  out.DrawPixel(x, y, newpix);
	}
      }
      return true;
    }
  };
}

#endif //INVERT_GREYS_GUARD
