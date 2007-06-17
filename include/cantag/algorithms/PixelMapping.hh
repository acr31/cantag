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

#ifndef PIXEL_MAPPING_GUARD
#define PIXEL_MAPPING_GUARD

#include <map>
using std::map;

#include <cantag/Config.hh>
#include <cantag/EntityTree.hh>
#include <cantag/Function.hh>
#include <cantag/Image.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Pixel.hh>
#include <cantag/entities/HoughEntity.hh>

namespace Cantag
{
  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout>
  class CANTAG_EXPORT PixelMapping : public Function<TypeList<Image<size, layout> >,
						     TypeList<Image<size, layout> > >
  {
  private:
    typedef map<unsigned char, unsigned char> PixelMap; // would like to replace unsigned chars with Pixel<layout>, but then I get compiler errors when declaring an iterator
    PixelMap m_mapping;
  public:
    PixelMapping(const PixelMap& mapping) : m_mapping(mapping) {}
    bool operator()(const Image<size, layout>& src, Image<size, layout>& dest) const;
  };

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout>
  bool PixelMapping<size, layout>::operator()(const Image<size, layout>& src, Image<size, layout>& dest) const
  {
    if (!src.SameDimensions(dest))
      return false;
    
    for (unsigned int x = 0; x < src.GetWidth(); ++x)
    {
      for (unsigned int y = 0; y < src.GetHeight(); ++y)
      {
	PixelMap::const_iterator iter = m_mapping.find(src.Sample(x, y).intensity());
	bool found = (iter != m_mapping.end());
	dest.DrawPixel(x, y, found ? iter->second : src.Sample(x, y));
      }
    }

    return true;
  }
}

#endif // PIXEL_MAPPING_GUARD
