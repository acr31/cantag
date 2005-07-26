/**
 * $Header$
 */

#ifndef CONVEXHULL_RESTRICTIONS_GUARD
#define CONVEXHULL_RESTRICTIONS_GUARD

class ConvexHullRestrictions {
private:
  float m_maxDeviation;

public:
  ConvexHullRestrictions(float max) : m_maxDeviation(max) {}

  inline void SetConvexHullRestrictions(float maxDeviation) {
    m_maxDeviation = maxDeviation;
  }
  
  inline float GetMaxDeviation() const { return m_maxDeviation; }
};

#endif//CONTOUR_RESTRICTIONS_GUARD
