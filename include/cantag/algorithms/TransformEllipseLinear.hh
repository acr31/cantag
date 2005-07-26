/**
 * $Header$
 */

#ifndef LINEAR_ELLIPSE_TRANSFORM_GUARD
#define LINEAR_ELLIPSE_TRANSFORM_GUARD

#include <cantag/entities/Entity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Ellipse.hh>
#include <cantag/Function.hh>

namespace Cantag {

  class TransformEllipseLinear : public Function1<ShapeEntity<Ellipse>,TransformEntity> {
  public:
    typedef ShapeEntity<Ellipse> SourceType;
    typedef TransformEntity DestinationType;

    bool operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const;
  };

}

#endif//LINEAR_ELLIPSE_TRANSFORM_GUARD
