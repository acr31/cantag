/**
 * $Header$
 */ 

#ifndef FITQUADTANGLECORNER_GUARD
#define FITQUADTANGLECORNER_GUARD

#include <cantag/QuadTangle.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/Function.hh>

namespace Cantag {
  class FitQuadTangleCorner : public Function1<ContourEntity,ShapeEntity<QuadTangle> >{
  public:
    bool operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const;
  };
}

#endif//FITQUADTANGLECORNER_GUARD
