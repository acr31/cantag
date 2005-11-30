/*
  Copyright (C) 2005 Andrew C. Rice

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

#ifndef PRINT_ENTITY_GUARD
#define PRINT_ENTITY_GUARD

#include <ostream>

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ConvexHullEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/QuadTangle.hh>

namespace Cantag {
  class PrintEntityContour : public Function<TL0,TL1(ContourEntity)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityContour(std::ostream& output) : m_output(output) {};
    bool operator()(ContourEntity& contourentity) const ;
  };

  class PrintEntityConvexHull : public Function<TL1(ContourEntity),TL1(ConvexHullEntity)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityConvexHull(std::ostream& output) : m_output(output) {};
    bool operator()(const ContourEntity& contourentity, ConvexHullEntity& convexhull) const ;
  };
  
  class PrintEntityShapeSquare : public Function<TL0,TL1(ShapeEntity<QuadTangle>)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityShapeSquare(std::ostream& output) : m_output(output) {};
    bool operator()(ShapeEntity<QuadTangle>& shapeentity) const;
  };

  class PrintEntityShapeCircle : public Function<TL0,TL1(ShapeEntity<Ellipse>)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityShapeCircle(std::ostream& output) : m_output(output) {};
    bool operator()(ShapeEntity<Ellipse>& shapeentity) const;
  };

  class PrintEntityTransform : public Function<TL0,TL1(TransformEntity)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityTransform(std::ostream& output) : m_output(output) {};
    bool operator()(TransformEntity& t) const;
  };
}
#endif//PRINT_ENTITY_GUARD
