/**
 * $Header$
 */ 

#ifndef TEXT_OUTPUT_MECHANISM_GUARD
#define TEXT_OUTPUT_MECHANISM_GUARD

#include <cantag/Config.hh>
#include <cantag/LocatedObject.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/ComposeEntity.hh>

#include <ostream>

namespace Cantag {
  class TextOutputMechanism {
  private:
    std::ostream& m_ostream;

    class ContourTreeAlgorithm {
      int m_accumulator;
    public:
      ContourTreeAlgorithm() : m_accumulator(0) {}
      int GetAccumulator() { return m_accumulator; }
      void operator()(const ContourEntity& contour) {
	if (contour.m_contourFitted) ++m_accumulator;
      }
    };
    
    template<class Shape>
    class ShapeAlgorithm {
      mutable int m_accumulator;
    public:
      ShapeAlgorithm() : m_accumulator(0) {}
      int GetAccumulator() { return m_accumulator; }
      void operator()(const ShapeEntity<Shape>& contour)  {
	if (contour.m_shapeFitted) ++m_accumulator;
      }
    };


    template<int PAYLOADSIZE>
    class DecodeAlgorithm {
    private:
      std::ostream& m_os;
    public:
      DecodeAlgorithm(std::ostream& os) : m_os(os) {}
      void operator()(const DecodeEntity<PAYLOADSIZE>& decode) {
	for(typename std::list<CyclicBitSet<PAYLOADSIZE>*>::const_iterator i = decode.m_payloads.begin();i!=decode.m_payloads.end();++i) {
	  if (!(*i)->IsInvalid()) {
	    m_os << "Decoded: " << **i << std::endl;
	  }
	}
      }
    };

    template<class Entity, class Dummy>
    struct Helper {
      static void Output(const Entity& root_element, std::ostream& os) {
	Helper<Entity,typename Dummy::Tail>::Output(root_element,os);
      }
    };

    template<class Entity>
    struct Helper<Entity,TypeListEOL> {
      static void Output(const Entity& root_element, std::ostream& os) {}
    };

    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<ContourEntity,Tail> > {
      static void Output(const Entity& root_element, std::ostream& os) {
	ContourTreeAlgorithm c;
	root_element.Apply(c);
	os << "ContourTreeFollower: Found " << c.GetAccumulator() << " contours" << std::endl;
	Helper<Entity,Tail>::Output(root_element,os);
      }
    };

    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<ShapeEntity<typename Entity::ShapeType>,Tail> > {
      static void Output(const Entity& root_element, std::ostream& os) {
	ShapeAlgorithm<typename Entity::ShapeType> s;
	root_element.Apply(s);
	os << "ShapeFitter: Fitted " << s.GetAccumulator() << " shapes" << std::endl;
	Helper<Entity,Tail>::Output(root_element,os);
      }
    };

    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<DecodeEntity<Entity::PayloadSize>,Tail> > {
      static void Output(const Entity& root_element, std::ostream& os) {
	DecodeAlgorithm<Entity::PayloadSize> d(os);
	root_element.Apply(d);
	Helper<Entity,Tail>::Output(root_element,os);
      }
    };


  public:
  
    TextOutputMechanism(std::ostream& ostream) : m_ostream(ostream) {};
  
    inline void FromImageSource(const Image& image) {};
    inline void FromThreshold(const Image& image) {};
    inline void FromContourTree(const ContourTree& contours) {};
    inline void FromRemoveIntrinsic(const ContourTree& contours) {};
    template<class ShapeType> inline void FromShapeTree(const ShapeTree<ShapeType>& shapes) {};
    template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);

    template<class Entity> void FromEntity(const Entity& root_entity);
  };

  template<class Entity> void TextOutputMechanism::FromEntity(const Entity& root_entity) {
    Helper<Entity,typename Entity::Typelist>::Output(root_entity,m_ostream);
  };

  template<int PAYLOADSIZE> void TextOutputMechanism::FromTag(const WorldState<PAYLOADSIZE>& world) {
    for(typename std::vector<LocatedObject<PAYLOADSIZE>*>::const_iterator i = world.GetNodes().begin();
	i != world.GetNodes().end();
	++i) {
      LocatedObject<PAYLOADSIZE>* loc = *i;
      if (loc->tag_codes.size() >0) {
	m_ostream << "Found tag: " << *(loc->tag_codes[0]) << 
	  " Location: " << loc->location[0] << " " << loc->location[1] << " " << loc->location[2] <<
	  " Normal: "   << loc->normal[0] << " " << loc->normal[1] << " " << loc->normal[2] << std::endl;
      }
    }
  }
}
#endif//TEXT_OUTPUT_MECHANISM_GUARD
