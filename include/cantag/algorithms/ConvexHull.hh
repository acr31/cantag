/**
 * $Header$
 */

#ifndef CONVEXHULL_GUARD
#define CONVEXHULL_GUARD

#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ConvexHullEntity.hh>
#include <cantag/Function.hh>
#include <cantag/ConvexHullRestrictions.hh>

namespace Cantag {
  class ConvexHull : public Function1<ContourEntity,ConvexHullEntity> {
  private:
    const ConvexHullRestrictions& m_restriction;
  public:
    ConvexHull(const ConvexHullRestrictions& restrict) : m_restriction(restrict) {};
    bool operator()(const ContourEntity& source, ConvexHullEntity& dest) const;
  };
}

#endif//CONVEXHULL_GUARD
