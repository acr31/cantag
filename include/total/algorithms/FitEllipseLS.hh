/**
 * $Header$
 */

#ifndef FULL_ELLIPSE_FIT_GUARD
#define FULL_ELLIPSE_FIT_GUARD

#include <total/Ellipse.hh>
#include <total/Entity.hh>

namespace Total {
  
  class FitEllipseLS {
  public:
    typedef ContourEntity SourceType;
    typedef ShapeEntity<Ellipse> DestinationType;

    bool operator()(const ContourEntity&  contour, ShapeEntity<Ellipse>& shape) const;
  };
}
#endif//FULL_ELLIPSE_FIT_GUARD
