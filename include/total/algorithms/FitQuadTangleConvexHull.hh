/**
 * $Header$
 */ 

#ifndef FITQUADTANGLECONVEXHULL_GUARD
#define FITQUADTANGLECONVEXHULL_GUARD

#include <total/QuadTangle.hh>
#include <total/Entity.hh>
#include <total/Function.hh>

namespace Total {
  class FitQuadTangleConvexHull : public Function1<ContourEntity,ShapeEntity<QuadTangle> > {
  public:

    /**
     * Take the set of points and form a convex hull to smooth out
     * quantization 'kinks'. Filter out vertices clearly not corners
     * based on angles.  What remains should be clusters of points at
     * the true corners. Take one from each cluster as an _estimate_
     * of that corner position.
     */
    bool operator()(const ContourEntity& contour, ShapeEntity<QuadTangle>& shape) const;

  private:

    /**
     * Check where a point p lies relative to a line specified by the
     * points l0 and l1.
     * >0 means p is on the left
     * =0 means on the line
     * <0 means to the right
     */
    float isLeft( const std::vector<float> &V, int l0, int l1, int p) const;
    
    /**
     * Get the convex hull of the n points in V. This assumes H has
     * been allocated to size n.  The _indexes_ to the points are then
     * returned in H, and the number of vertices (<=n) is also
     * returned.
     */
    int ConvexHull(const std::vector<float> &V, int n, int* H) const;
  };
}

#endif//FITQUADTANGLECONVEXHULL_GUARD
