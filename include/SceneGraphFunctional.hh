/**
 * $Header$
 */

#ifndef SCENE_GRAPH_FUNCTIONAL_GUARD
#define SCENE_GRAPH_FUNCTIONAL_GUARD
#include <boost/smart_ptr.hpp>
#include <LocatedObject.hh>

template<int PAYLOAD_SIZE>
class SceneGraphFunctional {
public:
  virtual void Eval(LocatedObject<PAYLOAD_SIZE>* loc) = 0;
};

#endif//SCENE_GRAPH_FUNCTIONAL_GUARD
