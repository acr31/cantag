/**
 * $Header$
 */

#ifndef SIMPLE_ELLIPSE_FIT_GUARD
#define SIMPLE_ELLIPSE_FIT_GUARD

#include <cantag/Ellipse.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/Function.hh>

namespace Cantag {
  class FitEllipseSimple : public Function1<ContourEntity,ShapeEntity<Ellipse> > {
  public:
    bool operator()(const ContourEntity& contour, ShapeEntity<Ellipse>& shape) const;
  };
}
#endif//SIMPLE_ELLIPSE_FIT_GUARD
