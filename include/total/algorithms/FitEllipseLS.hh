/**
 * $Header$
 */

#ifndef FULL_ELLIPSE_FIT_GUARD
#define FULL_ELLIPSE_FIT_GUARD

#include <total/Ellipse.hh>
#include <total/Entity.hh>
#include <total/Function.hh>

namespace Total {
  
  class FitEllipseLS : public Function1<ContourEntity,ShapeEntity<Ellipse> > {
  public:
    bool operator()(const ContourEntity&  contour, ShapeEntity<Ellipse>& shape) const;
  };
}
#endif//FULL_ELLIPSE_FIT_GUARD
