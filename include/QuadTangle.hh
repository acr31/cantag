/**
 * A class to fit points to a quadtangle and encapsulate the result
 *
 * $Header$ 
 */
#ifndef QUADTANGLE_GUARD
#define QUADTANGLE_GUARD

class QuadTangle {
private:
  float m_x0;
  float m_y0;
  float m_x1;
  float m_y1;
  float m_x2;
  float m_y2;
  float m_x3;
  float m_y3;

  bool m_fitted;

public:
  QuadTangle();
  QuadTangle(float* points, int numpoints, bool prev_fitted=false);
  QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3);
  inline bool IsFitted() const { return m_fitted; }
  bool Compare(const QuadTangle& o) const;
  
  inline float GetX0() const { return m_x0; }
  inline float GetY0() const { return m_y0; }
  inline float GetX1() const { return m_x1; }
  inline float GetY1() const { return m_y1; }
  inline float GetX2() const { return m_x2; }
  inline float GetY2() const { return m_y2; }
  inline float GetX3() const { return m_x3; }
  inline float GetY3() const { return m_y3; }
};


#endif//QUADTANGLE_GUARD
