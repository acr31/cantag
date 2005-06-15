/**
 * $Header$
 */

#ifndef COMPOSE_ENTITY_GUARD
#define COMPOSE_ENTITY_GUARD

#include <iostream>
#include <total/entities/Entity.hh>
#include <total/TemplateUtils.hh>
#include <total/Function.hh>

namespace Total {

//   /**
//    * The list of all possible entities for a particular shape and payload size
//    */
//   template<class Shape,int PAYLOADSIZE> 
//   struct PipelineList {
//     typedef TypeList<ContourEntity,
// 	    TypeList<ShapeEntity<Shape>,
//             TypeList<TransformEntity, 
//             TypeList<DecodeEntity<PAYLOADSIZE>,
//             TypeList<LocatedEntity,
//             TypeListEOL> > > > > Entities;
//   };

//   /**
//    * Select a range from the entity pipeline
//    */
//   template<class Shape, int PAYLOADSIZE, class Start, class Stop>
//   struct Select {
//     typedef typename SelectPipeline<typename PipelineList<Shape,PAYLOADSIZE>::Entities,Start,Stop>::Selected Selected;
//   };


  template<class List>
  class TEntity : public List::Head, public TEntity<typename List::Tail> {
  private:
    typedef TEntity<typename List::Tail> RecSuper;
    template<class CopyTail> TEntity(const TEntity<TypeList<typename List::Head, CopyTail> >& copyme) : 
      List::Head(copyme), RecSuper(copyme) {};

    TEntity(const TEntity<TypeList<typename List::Head, TypeListEOL> >& copyme) : List::Head(copyme), RecSuper() {};
    
  public:
    TEntity() : List::Head(), RecSuper() {};
  };

  template<> class TEntity<TypeListEOL> {
  public:
    TEntity() {};
  };

  template<class List>
  class ComposedEntity : public TEntity<List> {
  private:
    /**
     * Will be set to the index of the highest sucessfully completed stage
     */
    int m_progress;
    
    /*
    template<class WorkingList, class Dummy = WorkingList>
    class Check {
    public:
      inline static bool CheckValid(int index, ComposedEntity& me) {
	std::cout << "Checkvalid " << index << std::endl;
	if (index == 1) { return (static_cast<typename WorkingList::Head&>(me)).IsValid(); }
	else {
	  return Check<typename WorkingList::Tail>::CheckValid(index-1,me);
	}
      }
    };

    template<class Dummy>
    class Check<TypeListEOL,Dummy> {
    public:
      inline static bool CheckValid(int index, ComposedEntity& me) { return false; }
    };
    */
  public:
    typedef List Typelist;

    ComposedEntity() : TEntity<List>(), m_progress(-1) {}

    inline bool IsPipelineValid() const {
      return true;
      //      return Check<List>::CheckValid(m_progress,*this);
    }

    template<class L, class Algorithm> friend bool Apply(ComposedEntity<L>& entity,Algorithm& algorithm);
  };

  
}
#endif//COMPOSE_ENTITY_GUARD
