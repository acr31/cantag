/**
 * $Header$
 */ 

#ifndef TRANSFORMQUADTANGLEREDUCED_GUARD
#define TRANSFORMQUADTANGLEREDUCED_GUARD

#include <total/QuadTangle.hh>
#include <total/entities/Entity.hh>
#include <total/Function.hh>

namespace Total {
  class TransformQuadTangleReduced : public Function1<ShapeEntity<QuadTangle>,TransformEntity> {
  public:
    /**
     * This algorithm is as the same as
     * ProjectiveQuadTangleTransform, but makes
     * better use of the information available
     * to reduce the parameter set.
     *
     * Given the quadtangle, calculate the point that
     * the square centre maps to (the intersection of
     * the diagonals). This is a projection Xc=x/z, Yc=y/z
     * for a tag at (x,y,z). Thus, given Xc and Yc, the tag
     * location is completely determined by z!
     *
     * Following the same analysis above, we find:
     * (X-Xc) = a0*u + a1*v - a5*u*X - a6*v*X
     * (Y-Yc) = a3*u + a4*v - a5*u*Y - a6*v*Y
     *
     * And so we solve for a0, a1, a3, a4, a5, a6
     * and calculate:
     *
     * c8 = 1.0/ sqrt( a0*a0 + a3*a3 +a5*a5 )
     *
     * which then gives us all the params we had before,
     * but has only required inverting a 6x6 matrix rather
     * than an 8x8
     */
    bool operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& transform) const;
  };
}

#endif//TRANSFORMQUADTANGLEREDUCED_GUARD
