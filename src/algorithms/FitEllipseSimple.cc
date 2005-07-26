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

#include <total/algorithms/FitEllipseSimple.hh>

namespace Total {

  bool FitEllipseSimple::operator()(const ContourEntity& contour, ShapeEntity<Ellipse>& shape) const {
    const std::vector<float>& points = contour.GetPoints();
    if (points.size()/2 < 6) return false;

    float centrex = 0;
    float centrey = 0;
    int count = 0;
    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end(); ++i) {
      centrex += *i;
      ++i;
      centrey += *i;
      count++;
    }
    centrex/=count;
    centrey/=count;

    float majorx = -1;
    float majory = -1;
    float majorlen = 0;
    
    float minorx = -1;
    float minory = -1;
    float minorlen = 1e10;

    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end(); ++i) {
      float x = *i;
      ++i;
      float y = *i;
      float distsq = (centrex - x)*(centrex-x) + (centrey-y)*(centrey-y);
      if (distsq > majorlen) {
	majorx = x;
	majory = y;
	majorlen = distsq;
      }
      
      if (distsq < minorlen) {
	minorx = x;
	minory = y;
	minorlen = distsq;
      }
    }

    assert(majorlen != 0);
    assert(minorlen != 1e10);

    majorlen = sqrt(majorlen);
    minorlen = sqrt(minorlen);

    float theta = atan((majory-centrey)/(majorx-centrex)); // DATAN

    shape.m_shapeDetails = new Ellipse(centrex,centrey,theta,majorlen,minorlen);
    shape.m_shapeFitted = true;
    return true;
  }
}
