/**
 * $Header$
 */

#ifndef FULL_ELLIPSE_TRANSFORM_GUARD
#define FULL_ELLIPSE_TRANSFORM_GUARD

#include <total/Entity.hh>
#include <total/Ellipse.hh>

namespace Total {

  class TransformEllipseFull {
  public:
    typedef ShapeEntity<Ellipse> SourceType;
    typedef TransformEntity DestinationType;

    bool operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const;
  };

}

#endif//FULL_ELLIPSE_TRANSFORM_GUARD
