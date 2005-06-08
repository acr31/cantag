/**
 * $Header$
 */

#ifndef LINEAR_ELLIPSE_TRANSFORM_GUARD
#define LINEAR_ELLIPSE_TRANSFORM_GUARD

#include <total/Entity.hh>
#include <total/Ellipse.hh>
#include <total/Function.hh>

namespace Total {

  class TransformEllipseLinear : public Function1<ShapeEntity<Ellipse>,TransformEntity> {
  public:
    typedef ShapeEntity<Ellipse> SourceType;
    typedef TransformEntity DestinationType;

    bool operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const;
  };

}

#endif//LINEAR_ELLIPSE_TRANSFORM_GUARD
