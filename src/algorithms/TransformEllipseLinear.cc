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

#include <cantag/algorithms/TransformEllipseLinear.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {

  bool TransformEllipseLinear::operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const {    
    //  wc  hs  0  x0
    //  ws  hc  0  y0
    //  0   0   1  0
    //  0   0   0  1
    
    const Ellipse& ellipse = *source.GetShape();

    Transform* t = new Transform(1.f);
    dest.GetTransforms().push_back(t);

    (*t)[0] = ellipse.GetWidth()*DCOS(8,ellipse.GetAngle()); // DCOS
    (*t)[1] = -ellipse.GetHeight()*DSIN(8,ellipse.GetAngle());  // DSINE
    (*t)[2] = 0;
    (*t)[3] = ellipse.GetX0();

    (*t)[4] = ellipse.GetWidth()*DSIN(8,ellipse.GetAngle()); // DSINE
    (*t)[5] = ellipse.GetHeight()*DCOS(8,ellipse.GetAngle());  // DCOS
    (*t)[6] = 0;
    (*t)[7] = ellipse.GetY0();

    (*t)[8] = 0;
    (*t)[9] = 0;
    (*t)[10] = 1;
    (*t)[11] = 1;

    (*t)[12] = 0;
    (*t)[13] = 0;
    (*t)[14] = 0;
    (*t)[15] = 1;

    return true;
  }

}
