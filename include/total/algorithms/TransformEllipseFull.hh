/**
 * $Header$
 */

#ifndef FULL_ELLIPSE_TRANSFORM_GUARD
#define FULL_ELLIPSE_TRANSFORM_GUARD

#include <total/entities/Entity.hh>
#include <total/entities/TransformEntity.hh>
#include <total/Ellipse.hh>
#include <total/Function.hh>

namespace Total {

  class TransformEllipseFull : public Function1<ShapeEntity<Ellipse>, TransformEntity> {
  public:
    typedef ShapeEntity<Ellipse> SourceType;
    typedef TransformEntity DestinationType;

    bool operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const;
  };
}

#endif//FULL_ELLIPSE_TRANSFORM_GUARD
