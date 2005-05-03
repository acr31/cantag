/**
 * $Header$
 */

#ifndef ENTITY_GUARD
#define ENTITY_GUARD

#include <total/Config.hh>
#include <total/LocatedObject.hh>

#include <list>
#include <vector>
#include <iostream>

namespace Total {

  class ContourEntity  {
  public:
    enum bordertype_t { UNKNOWN = 2, OUTER_BORDER = 1, HOLE_BORDER = 0};

    int nbd;
    bordertype_t bordertype;
    int parent_id;
    std::vector<float> points;
    bool m_contourFitted;
  public:
    ContourEntity() : m_contourFitted(false) {};
    virtual ~ContourEntity() {};

    virtual void SetWeeded() = 0;
    virtual bool GetWeeded() = 0;
  private:
    ContourEntity(const ContourEntity& copyme) : 
      nbd(copyme.nbd),
      bordertype(copyme.bordertype),
      parent_id(copyme.parent_id),
      points(copyme.points) {
      std::cerr << "Copy Contour Entity" << std::endl;	    
    };


  };

  template<class Shape>
  class ShapeEntity  {
  public:
    Shape* m_shapeDetails;
    bool m_shapeFitted;
  public:
    ShapeEntity() : m_shapeDetails(NULL), m_shapeFitted(false) {};

    virtual ~ShapeEntity() {
      if (m_shapeDetails) delete m_shapeDetails;
    }

    virtual void SetWeeded() = 0;
    virtual bool GetWeeded() = 0;

  private:
    ShapeEntity(const ShapeEntity<Shape>& copyme) : m_shapeDetails(copyme.m_shapeDetails ? new Shape(*copyme.m_shapeDetails) : NULL) {
      std::cerr << "Copy Shape" << std::endl;	    
    };
  };

  template<int PAYLOAD_SIZE>
  class DecodeEntity {
  protected:
    float* m_decode_transform;
    CyclicBitSet<PAYLOAD_SIZE>* m_payload;
  public:
    DecodeEntity() : m_decode_transform(NULL), m_payload(NULL) {};
    virtual ~DecodeEntity() {};
    virtual void SetWeeded() = 0;
    virtual bool GetWeeded() = 0;

  private:
    DecodeEntity(const DecodeEntity<PAYLOAD_SIZE>& copyme) {
      std::cerr << "Copy Decode" << std::endl;	    
    }
  };
  
  class TransformEntity  {
  protected:    
    float* m_3d_transform;
  public:
    TransformEntity() : m_3d_transform(NULL) {};
    virtual ~TransformEntity() {};
    virtual void SetWeeded() = 0;
    virtual bool GetWeeded() = 0;

  private:
    TransformEntity(const TransformEntity& copyme) {
      std::cerr << "Copy Transform" << std::endl;	    
    }
  };
  
  template<class H, class T> struct EntityList {
    typedef H Head;
    typedef T Tail;
  };

  class EntityListEOL {};

  // define a list templated on shape and payload size for the complete pipeline
  template<class Shape,int PAYLOADSIZE> 
  struct PipelineList {
    typedef EntityList<ContourEntity,
	    EntityList<ShapeEntity<Shape>,
            EntityList<DecodeEntity<PAYLOADSIZE>,
            EntityList<TransformEntity, 
            EntityListEOL> > > > Entities;
  };

  // select a subset of the list based on a start and stop value
  template<class List, class Start, class Stop>
  struct SelectPipeline {
    typedef typename SelectPipeline<typename List::Tail,Start,Stop>::Selected Selected;
  };

  template<class List, class Stop>
  struct SelectPipeline<List,typename List::Head,Stop> {
    typedef EntityList<typename List::Head,typename SelectPipeline<typename List::Tail,typename List::Tail::Head,Stop>::Selected > Selected;
  };

  template<class List>
  struct SelectPipeline<List,typename List::Head, typename List::Head> {
    typedef EntityList<typename List::Head,EntityListEOL> Selected;
  };

  // select the particular pipeline I want
  template<class Shape, int PAYLOADSIZE, class Start, class Stop>
  struct Select {
    typedef typename SelectPipeline<typename PipelineList<Shape,PAYLOADSIZE>::Entities,Start,Stop>::Selected Selected;
  };


  template<class List>
  class TEntity : public List::Head, public TEntity<typename List::Tail> {
  private:
    typedef TEntity<typename List::Tail> RecSuper;
    template<class CopyTail> TEntity(const TEntity<EntityList<typename List::Head, CopyTail> >& copyme) : 
      List::Head(copyme), RecSuper(copyme) {};

    TEntity(const TEntity<EntityList<typename List::Head, EntityListEOL> >& copyme) : List::Head(copyme), RecSuper() {};
    
  public:
    TEntity() : List::Head(), RecSuper() {};
  };

  template<> class TEntity<EntityListEOL> {
  public:
    TEntity() {};
  };


  template<class Shape, int PAYLOADSIZE, class Start = ContourEntity, class Stop = TransformEntity>
  class Entity : public TEntity<typename Select<Shape,PAYLOADSIZE,Start,Stop>::Selected>
  {
  public:
    typedef typename Select<Shape,PAYLOADSIZE,Start,Stop>::Selected Typelist;
    typedef Shape ShapeType;

  private:
    typedef Entity<Shape,PAYLOADSIZE,Start,Stop> MyType;
    typedef TEntity<typename Select<Shape,PAYLOADSIZE,Start,Stop>::Selected> SuperType;

    std::list<MyType*> m_children;
    bool m_weeded;

    template<class CopyShape, int COPYPAYLOAD, class CopyStart, class CopyStop> 
    Entity(const Entity<CopyShape,COPYPAYLOAD,CopyStart,CopyStop>& copyme) : SuperType(copyme) {};

  public:
    Entity() : SuperType(),m_weeded(false) {};
    

    template<class Algorithm> void Transform(const Algorithm& algorithm) {
      algorithm(*this,*this);
      for(typename std::list<MyType*>::iterator i = m_children.begin();
	  i != m_children.end();
	  ++i) {
	(*i)->Transform(algorithm);
      }
    }

    template<class Algorithm> void Apply(const Algorithm& algorithm) const {
      algorithm(*this);
      for(typename std::list<MyType*>::const_iterator i = m_children.begin();
	  i != m_children.end();
	  ++i) {
	(*i)->Apply(algorithm);
      }
    }

    void AddChild(MyType* newChild) {
      m_children.push_back(newChild);
    }

    void SetWeeded() { m_weeded = true; }    
    bool GetWeeded() { return m_weeded; }
  };
  


};


#endif//ENTITY_GUARD
