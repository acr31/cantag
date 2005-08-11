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

#ifndef THRESHOLD_ADAPTIVE_GUARD
#define THRESHOLD_ADAPTIVE_GUARD

#include <cantag/Image.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>

namespace Cantag {

  /**
   * An implementation of Pierre Wellner's Adaptive Thresholding
   *  @TechReport{t:europarc93:wellner,
   *     author       = "Pierre Wellner",
   *     title        = "Adaptive Thresholding for the {D}igital{D}esk",
   *     institution  = "EuroPARC",
   *     year         = "1993",
   *     number       = "EPC-93-110",
   *     comment      = "Nice introduction to global and adaptive thresholding.  Presents an efficient and effective adaptive thresholding technique and demonstrates on lots of example images.",
   *     file         = "ar/ddesk-threshold.pdf"
   *   }
   * 
   * Adapted to use a more efficient calculation for the moving
   * average. the window used is now 2^window_size
   *
   */
  class ThresholdAdaptive : public Function<TypeList<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >, // needed because the preprocesser can't parse the call to TL1(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>)
		    TL1(MonochromeImage)> {
  private:
    int m_window_size;
    int m_offset;
  public:
    ThresholdAdaptive(int window_size, int offset);
    bool operator()(const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& source, MonochromeImage& dest) const;
  };
}

#endif//THRESHOLD_ADAPTIVE_GUARD
