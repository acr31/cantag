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

#include <cantag/algorithms/FitEllipseSimple.hh>

namespace Cantag {

  bool FitEllipseSimple::operator()(const ContourEntity& contour, ShapeEntity<Ellipse>& shape) const {
    const std::vector<float>& points = contour.GetPoints();
    if (points.size()/2 < 6) return false;

    float centrex = 0.f;
    float centrey = 0.f;
    int count = 0;
    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end(); ++i) {
      centrex += *i;
      ++i;
      centrey += *i;
      count++;
    }
    centrex/=count;
    centrey/=count;

    float majorx = -1.f;
    float majory = -1.f;
    float majorlen = 0.f;
    
    float minorlen = flt_infinity();

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
	minorlen = distsq;
      }
    }
    
    // check that we've set the values
    assert(majorlen != 0);
    assert(minorlen != flt_infinity());

    majorlen = sqrt(majorlen);
    minorlen = sqrt(minorlen);

    float theta = atan((majory-centrey)/(majorx-centrex)); // DATAN
    //    std::cout << majory << " " << centrey << " " << majorx << " " << centrex << std::endl;
    //    std::cout << "theta = " << theta << std::endl;
    shape.SetShape(new Ellipse(centrex,centrey,majorlen,minorlen,theta));
    return true;
  }
}
