/**
 * $Header$
 */

#ifndef ENTITY_GUARD
#define ENTITY_GUARD

#include <total/Config.hh>
#include <total/Transform.hh>
#include <total/CyclicBitSet.hh>

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


  class ContourEntity : public Entity {
  public:
    enum bordertype_t { UNKNOWN = 2, OUTER_BORDER = 1, HOLE_BORDER = 0};

    int nbd;
    bordertype_t bordertype;
    int parent_id;
    std::vector<float> points;
    bool m_contourFitted;
  public:
    ContourEntity() : m_contourFitted(false) {};
    ~ContourEntity() {};

  private:
    ContourEntity(const ContourEntity& copyme) : 
      nbd(copyme.nbd),
      bordertype(copyme.bordertype),
      parent_id(copyme.parent_id),
      points(copyme.points) {};
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
  
  class TransformEntity : public Entity  {
  public:
    std::list<Transform*> m_transforms;
    bool m_transformDone;
  public:
    TransformEntity() : m_transformDone(false) {};
    ~TransformEntity() {
      for(std::list<Transform*>::const_iterator i = m_transforms.begin(); i != m_transforms.end(); ++i) {
	delete *i;
      }
    };

  private:
    TransformEntity(const TransformEntity& copyme) {}
  };

  template<int PAYLOAD_SIZE>
  class DecodeEntity : public Entity {
  public:
    std::list<CyclicBitSet<PAYLOAD_SIZE>*> m_payloads;
    bool m_decodeDone;
  public:
    DecodeEntity() : m_decodeDone(false) {};
    ~DecodeEntity() {};

  private:
    DecodeEntity(const DecodeEntity<PAYLOAD_SIZE>& copyme) {}
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
