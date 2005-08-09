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

#ifndef SHAPE_ENTITY_GUARD
#define SHAPE_ENTITY_GUARD

#include <cantag/entities/Entity.hh>

namespace Cantag {

  template<class Shape>
  class ShapeEntity : public Entity  {
  private:
    Shape* m_shapeDetails;

  public:
    ShapeEntity() : m_shapeDetails(NULL) {}

    ~ShapeEntity() {
      if (m_shapeDetails) delete m_shapeDetails;
    }

    Shape* GetShape() { return m_shapeDetails; }
    const Shape* GetShape() const { return m_shapeDetails; }
    
    void SetShape(Shape* shape) {
      if (m_shapeDetails) delete m_shapeDetails;
      m_shapeDetails = shape;
    }

  private:
    ShapeEntity(const ShapeEntity<Shape>& copyme) : m_shapeDetails(copyme.m_shapeDetails ? new Shape(*copyme.m_shapeDetails) : NULL) {}

  };
}
#endif//SHAPE_ENTITY_GUARD
