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

#include <cantag/algorithms/RemoveNonConcentricEllipse.hh>

namespace Cantag {

  bool RemoveNonConcentricEllipse::operator()(TreeNode<ShapeEntity<Ellipse> >& treenode) const {
    Ellipse& ellipse = *(treenode.GetNode()->GetShape());
    treenode.Reset();
    bool found = false;
    while(treenode.HasNext()) {
      ShapeEntity<Ellipse>* child = treenode.NextChild()->GetNode();
      if (child->IsValid()) {	
	float distancesq = 
	  (ellipse.GetX0() - child->GetShape()->GetX0())*(ellipse.GetX0() - child->GetShape()->GetX0()) +
	  (ellipse.GetY0() - child->GetShape()->GetY0())*(ellipse.GetY0() - child->GetShape()->GetY0());

	if (m_restrict.CheckDistanceSquared(distancesq)) found = true;	  
      }
      
    }
    return found;
  }

}
