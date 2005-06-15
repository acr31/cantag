/**
 * $Header$
 */ 

#ifndef FITQUADTANGLECORNER_GUARD
#define FITQUADTANGLECORNER_GUARD

#include <total/QuadTangle.hh>
#include <total/entities/Entity.hh>
#include <total/entities/ContourEntity.hh>
#include <total/Function.hh>

namespace Total {
  class FitQuadTangleCorner : public Function1<ContourEntity,ShapeEntity<QuadTangle> >{
  public:
    bool operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const;
  };
}

#endif//FITQUADTANGLECORNER_GUARD
