/**
 * $Header$
 */

#ifndef SIMPLE_ELLIPSE_FIT_GUARD
#define SIMPLE_ELLIPSE_FIT_GUARD

#include <total/shapefit/Ellipse.hh>
#include <total/Entity.hh>

namespace Total {
  class SimpleEllipseFit {
  public:
    void operator()(const ContourEntity* contour, ShapeEntity<Ellipse>* shape);
  };
}
#endif//SIMPLE_ELLIPSE_FIT_GUARD
