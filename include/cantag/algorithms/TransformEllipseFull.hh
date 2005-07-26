/**
 * $Header$
 */

#ifndef FULL_ELLIPSE_TRANSFORM_GUARD
#define FULL_ELLIPSE_TRANSFORM_GUARD

#include <cantag/entities/Entity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Ellipse.hh>
#include <cantag/Function.hh>

namespace Cantag {

  class TransformEllipseFull : public Function1<ShapeEntity<Ellipse>, TransformEntity> {
  public:
    typedef ShapeEntity<Ellipse> SourceType;
    typedef TransformEntity DestinationType;

    bool operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const;
  };
}

#endif//FULL_ELLIPSE_TRANSFORM_GUARD
