/**
 * $Header$
 */

#ifndef SIMPLE_ELLIPSE_FIT_GUARD
#define SIMPLE_ELLIPSE_FIT_GUARD

#include <total/Ellipse.hh>
#include <total/Entity.hh>

namespace Total {
  class FitEllipseSimple {
  public:
    typedef ContourEntity SourceType;
    typedef ShapeEntity<Ellipse> DestinationType;

    bool operator()(const ContourEntity& contour, ShapeEntity<Ellipse>& shape) const;
  };
}
#endif//SIMPLE_ELLIPSE_FIT_GUARD
