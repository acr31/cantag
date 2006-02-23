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


#ifndef REMOVE_NONCONCENTRIC_ELLIPSE_GUARD
#define REMOVE_NONCONCENTRIC_ELLIPSE_GUARD

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/EntityTree.hh>
#include <cantag/Ellipse.hh>
#include <cantag/EllipseRestrictions.hh>

namespace Cantag {

  class CANTAG_EXPORT RemoveNonConcentricEllipse : public Function<TL0,TL1(TreeNode<ShapeEntity<Ellipse> >)> {
  private:
    const EllipseRestrictions& m_restrict;
  public:
    RemoveNonConcentricEllipse(const EllipseRestrictions& restrict) : m_restrict(restrict) {};
    bool operator()(TreeNode<ShapeEntity<Ellipse> >& treenode) const;
  };
}
#endif//REMOVE_NONCONCENTRIC_ELLIPSE_GUARD
