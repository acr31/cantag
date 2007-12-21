/*
  Copyright (C) 2005 Andrew C. Rice

  This program is free software; you c redistribute it and/or
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
#include <cantag/entities/DecodeEntity.hh>
#include <cantag/QuadTangle.hh>

namespace Cantag {
  class CANTAG_EXPORT PrintEntityContour : public Function<TL0,TL1(ContourEntity)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityContour(std::ostream& output) : m_output(output) {};
    bool operator()(ContourEntity& contourentity) const ;
  };

  class CANTAG_EXPORT PrintEntityConvexHull : public Function<TL1(ContourEntity),TL1(ConvexHullEntity)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityConvexHull(std::ostream& output) : m_output(output) {};
    bool operator()(const ContourEntity& contourentity, ConvexHullEntity& convexhull) const ;
  };
  
  class CANTAG_EXPORT PrintEntityShapeSquare : public Function<TL0,TL1(ShapeEntity<QuadTangle>)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityShapeSquare(std::ostream& output) : m_output(output) {};
    bool operator()(ShapeEntity<QuadTangle>& shapeentity) const;
  };

  class CANTAG_EXPORT PrintEntityShapeCircle : public Function<TL0,TL1(ShapeEntity<Ellipse>)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityShapeCircle(std::ostream& output) : m_output(output) {};
    bool operator()(ShapeEntity<Ellipse>& shapeentity) const;
  };

  class CANTAG_EXPORT PrintEntityTransform : public Function<TL0,TL1(TransformEntity)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityTransform(std::ostream& output) : m_output(output) {};
    bool operator()(TransformEntity& t) const;
  };

  template<int PAYLOAD_SIZE>
  class PrintEntity3DPosition : public Function<TL1(TransformEntity), TL1(DecodeEntity<PAYLOAD_SIZE>)> {
  private:
    std::ostream& m_output;
    const Camera& m_camera;
  public:
    PrintEntity3DPosition(std::ostream& output,const Camera& camera) : m_output(output), m_camera(camera) {};
    bool operator()(const TransformEntity& t, DecodeEntity<PAYLOAD_SIZE>& de) const;
  };

  template<int PAYLOAD_SIZE>
  class PrintEntityDecode : public Function<TL0,TL1(DecodeEntity<PAYLOAD_SIZE>)> {
  private:
    std::ostream& m_output;
  public:
    PrintEntityDecode(std::ostream& output) : m_output(output) {};
    bool operator()(DecodeEntity<PAYLOAD_SIZE>& d) const;
  };

  template<int PAYLOAD_SIZE>
  bool PrintEntityDecode<PAYLOAD_SIZE>::operator()(DecodeEntity<PAYLOAD_SIZE>& d) const {
    for(typename std::vector<typename DecodeEntity<PAYLOAD_SIZE>::Data*>::const_iterator i = d.GetPayloads().begin(); i != d.GetPayloads().end(); ++i) {
      m_output << (*i)->confidence << "\t" << (*i)->payload << std::endl;
    }
    return true;
  }

  template<int PAYLOAD_SIZE>
  bool PrintEntity3DPosition<PAYLOAD_SIZE>::operator()(const TransformEntity& te, DecodeEntity<PAYLOAD_SIZE>& de) const {
      const Transform* t = te.GetPreferredTransform();
      if (t==NULL) return false;
      float normal[3];
      t->GetNormalVector(m_camera,normal);
      float location[3];
      t->GetLocation(m_camera,location,1);

      for(typename std::vector<typename DecodeEntity<PAYLOAD_SIZE>::Data*>::const_iterator i = de.GetPayloads().begin(); i != de.GetPayloads().end();++i) {
	m_output << (*i)->confidence << " " << (*i)->payload << " ";
	m_output << normal[0] << " " << normal[1] << " " << normal[2] << " ";
	m_output << location[0] << " " << location[1] << " " << location[2] << std::endl;
      }
      return true;
  }

  
}
#endif//PRINT_ENTITY_GUARD
