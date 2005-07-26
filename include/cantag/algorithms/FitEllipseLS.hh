/**
 * $Header$
 */

#ifndef FULL_ELLIPSE_FIT_GUARD
#define FULL_ELLIPSE_FIT_GUARD

#include <cantag/Ellipse.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/Function.hh>

namespace Cantag {
  
  class FitEllipseLS : public Function1<ContourEntity,ShapeEntity<Ellipse> > {
  public:
    bool operator()(const ContourEntity&  contour, ShapeEntity<Ellipse>& shape) const;
  };
}
#endif//FULL_ELLIPSE_FIT_GUARD
