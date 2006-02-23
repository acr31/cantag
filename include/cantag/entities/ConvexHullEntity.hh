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

#ifndef CONVEXHULL_ENTITY
#define CONVEXHULL_ENTITY

#include <vector>

#include <cantag/entities/Entity.hh>

namespace Cantag {

  /**
   * Represent the convex hull of a contour.  We store the indices of
   * the points that lie on the convex hull.
   */
  class CANTAG_EXPORT ConvexHullEntity : public Entity {
  private:
    std::vector<int> m_indices;
  public:
    ConvexHullEntity() : m_indices() {}
    inline std::vector<int>& GetIndices() { return m_indices; }
    inline const std::vector<int>& GetIndices() const { return m_indices;} 
  };

}

#endif//CONVEXHULL_ENTITY
