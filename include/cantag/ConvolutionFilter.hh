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

#ifndef CONVOLUTION_FILTER_GUARD
#define CONVOLUTION_FILTER_GUARD

#include <cantag/Array.hh>

namespace Cantag {

  template<int halfwidth, int halfheight> class CANTAG_EXPORT ConvolutionFilter
  {
  private:
    Array2<int, 2 * halfwidth + 1, 2 * halfheight + 1> m_vals;
    int m_normalisation_factor;
  public:
    ConvolutionFilter(const Array2<int, 2 * halfwidth + 1, 2 * halfheight + 1>& vals, int normalisation_factor)
      : m_vals(vals), m_normalisation_factor(normalisation_factor) {}
    int GetValue(int x, int y) const;
    int GetNormalisationFactor() const { return m_normalisation_factor; }
    void Print() const { m_vals.Print(" ", "\n"); }
  };

  template<int halfwidth, int halfheight>
  int ConvolutionFilter<halfwidth, halfheight>::GetValue(int x, int y) const
  {
    // if (x < 0 || x > 2 * halfwidth || y < 0 || y > 2 * halfheight)
    //  return 0;
    //else
      return m_vals[x][y];
  }

}

#endif //CONVOLUTION_FILTER_GUARD
