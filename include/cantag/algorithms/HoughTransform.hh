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

#ifndef HOUGH_TRANSFORM_GUARD
#define HOUGH_TRANSFORM_GUARD

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
  class CANTAG_EXPORT HoughTransform
    : public Function<TypeList<Image<size, layout> >,
		      TL1(TreeNode<HoughEntity>)>
  {
  private:
    mutable float m_latest_max_accumulator;
    int m_num_angle_divisions;
    int m_num_perpdist_divisions;
  public:
    HoughTransform(int num_angle_divisions, int num_perpdist_divisions)
      : m_latest_max_accumulator(0), m_num_angle_divisions(num_angle_divisions), m_num_perpdist_divisions(num_perpdist_divisions) {}
    bool operator()(const Image<size, layout>& source, TreeNode<HoughEntity>& result) const;
    float GetLatestMaxAccumulator() const { return m_latest_max_accumulator; }
  };
}

#endif // HOUGH_TRANSFORM_GUARD
