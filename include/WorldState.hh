/**
 * $Header$
 */

#ifndef WORLD_STATE_GUARD
#define WORLD_STATE_GUARD

#include <Config.hh>
#include <LocatedObject.hh>
#include <vector>

#ifdef HAVE_BOOST_ARCHIVE
#include <boost/serialization/access.hpp>
#endif


/**
 * Stores a set of located objects - the world state
 */
template<int PAYLOAD_SIZE>
class WorldState {
private:
  std::vector<LocatedObject<PAYLOAD_SIZE>*> nodes;
public:
  WorldState();
  ~WorldState();
  void Add(LocatedObject<PAYLOAD_SIZE>* object);
  const std::vector<LocatedObject<PAYLOAD_SIZE>*>& GetNodes() { return nodes; }
private:
#ifdef HAVE_BOOST_ARCHIVE
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive & ar, const unsigned int version);
#endif
};

template<int PAYLOAD_SIZE> WorldState<PAYLOAD_SIZE>::WorldState() : nodes() {}

template<int PAYLOAD_SIZE> WorldState<PAYLOAD_SIZE>::~WorldState() {
  for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = nodes.begin();
      i != nodes.end();
      ++i) {
    delete *i;
  }
}

template<int PAYLOAD_SIZE> void WorldState<PAYLOAD_SIZE>::Add(LocatedObject<PAYLOAD_SIZE>* object) {
  nodes.push_back(object);
}

#ifdef HAVE_BOOST_ARCHIVE
//BOOST_CLASS_TRACKING(WorldState, boost::serialization::track_never);
namespace boost { 
  namespace serialization {
    template<int PAYLOAD_SIZE>
    struct tracking_level<WorldState<PAYLOAD_SIZE> >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<track_never> type;
      BOOST_STATIC_CONSTANT(
			    enum tracking_type, 
			    value = static_cast<enum tracking_type>(type::value)
			    );
    };
  } // serialization
} // boost

//BOOST_CLASS_IMPLEMENTATION(WorldState, boost::serialization::object_serializable);
namespace boost { 
  namespace serialization {
    template<int PAYLOAD_SIZE>
    struct implementation_level<WorldState<PAYLOAD_SIZE> >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<object_serializable> type;
      BOOST_STATIC_CONSTANT(
			    enum level_type,
			    value = static_cast<enum level_type>(type::value)
			    );
    };
  } // serialization
} // boost


template<int PAYLOAD_SIZE> template<class Archive> void WorldState<PAYLOAD_SIZE>::serialize(Archive & ar, const unsigned int version) {
  ar & nodes;
}
#endif

#endif//WORLD_STATE_GUARD
