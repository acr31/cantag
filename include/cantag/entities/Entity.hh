/**
 * $Header$
 */

#ifndef ENTITY_GUARD
#define ENTITY_GUARD

#include <total/Config.hh>

#include <list>
#include <vector>

namespace Total {
  class Entity {
  protected:
    bool m_valid;
    Entity() : m_valid(false) {};
  public:
    void SetValid(bool valid) { m_valid = valid; }
    bool IsValid() const { return m_valid; }
  };



  template<class Shape>
  class ShapeEntity : public Entity {
  public:
    Shape* m_shapeDetails;
    bool m_shapeFitted;
  public:
    ShapeEntity() : m_shapeDetails(NULL), m_shapeFitted(false) {};

    ~ShapeEntity() {
      if (m_shapeDetails) delete m_shapeDetails;
    }

  private:
    ShapeEntity(const ShapeEntity<Shape>& copyme) : m_shapeDetails(copyme.m_shapeDetails ? new Shape(*copyme.m_shapeDetails) : NULL) {}
  };
  
  class LocatedEntity : public Entity {
  protected:    
    float normal[3];
    float location[3];
    float angle;
  public:
    LocatedEntity() {};
    ~LocatedEntity() {};

  private:
    LocatedEntity(const LocatedEntity& copyme) {}
  };

}

#endif//ENTITY_GUARD
