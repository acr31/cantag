/**
 * $Header$
 */

#ifndef CONVEXHULL_ENTITY
#define CONVEXHULL_ENTITY

#include <vector>

#include <total/entities/Entity.hh>

namespace Total {

  /**
   * Represent the convex hull of a contour.  We store the indices of
   * the points that lie on the convex hull.
   */
  class ConvexHullEntity : public Entity {
  private:
    std::vector<int> m_indices;
  public:
    ConvexHullEntity() : m_indices() {}
    inline std::vector<int>& GetIndices() { return m_indices; }
    inline const std::vector<int>& GetIndices() const { return m_indices;} 
  };

}

#endif//CONVEXHULL_ENTITY
