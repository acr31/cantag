/**
 * $Header$
 */

#ifndef FULL_ELLIPSE_FIT_GUARD
#define FULL_ELLIPSE_FIT_GUARD

#include <total/shapefit/Ellipse.hh>
#include <total/Entity.hh>

namespace Total {
  
  class FullEllipseFit {
  public:
    void operator()(const ContourEntity* contour, ShapeEntity<Ellipse>* shape);
  };
}
#endif//FULL_ELLIPSE_FIT_GUARD
