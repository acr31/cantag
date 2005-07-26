/**
 * $Header$
 */

#ifndef CONVEXHULL_GUARD
#define CONVEXHULL_GUARD

#include <total/entities/ContourEntity.hh>
#include <total/entities/ConvexHullEntity.hh>
#include <total/Function.hh>
#include <total/ConvexHullRestrictions.hh>

namespace Total {
  class ConvexHull : public Function1<ContourEntity,ConvexHullEntity> {
  private:
    const ConvexHullRestrictions& m_restriction;
  public:
    ConvexHull(const ConvexHullRestrictions& restrict) : m_restriction(restrict) {};
    bool operator()(const ContourEntity& source, ConvexHullEntity& dest) const;
  };
}

#endif//CONVEXHULL_GUARD
