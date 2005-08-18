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

#include <cantag/algorithms/DrawEntity.hh>

namespace Cantag {

  bool DrawEntityContour::operator()(ContourEntity& contour) const {
    for(std::vector<float>::const_iterator i = contour.GetPoints().begin();
	i != contour.GetPoints().end();
	++i) {
      const float x = *i;
      ++i;
      const float y = *i;
      m_image.DrawPixel(x,y,0);
    }    
    return true;
  }

  bool DrawEntityTransform::operator()(TransformEntity& transform) const {
    float pts[] = {-1,-1,
		   -1,1,
		   1,1,
		   1,-1};
    transform.GetPreferredTransform()->Apply(pts,4);
    m_camera.NPCFToImage(pts,4);
    m_image.DrawQuadTangle((int)pts[0],(int)pts[1],
			   (int)pts[2],(int)pts[3],
			   (int)pts[4],(int)pts[5],
			   (int)pts[6],(int)pts[7],
			   0,1);
			   
    return true;    
  }
}

