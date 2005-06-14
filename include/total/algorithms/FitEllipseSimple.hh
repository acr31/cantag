/**
 * $Header$
 */

#ifndef SIMPLE_ELLIPSE_FIT_GUARD
#define SIMPLE_ELLIPSE_FIT_GUARD

#include <total/Ellipse.hh>
#include <total/Entity.hh>
#include <total/ContourEntity.hh>
#include <total/Function.hh>

namespace Total {
  class FitEllipseSimple : public Function1<ContourEntity,ShapeEntity<Ellipse> > {
  public:
    bool operator()(const ContourEntity& contour, ShapeEntity<Ellipse>& shape) const;
  };
}
#endif//SIMPLE_ELLIPSE_FIT_GUARD
