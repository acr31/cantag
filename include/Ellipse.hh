/**
 * A class to represent ellipses in the image and fit one to a set of points
 *
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/02/21 10:05:57  acr31
 * got it working!
 *
 * Revision 1.1  2004/02/20 22:25:54  acr31
 * major reworking of matching algorithms and architecture
 *
 */

#ifndef ELLIPSE_GUARD
#define ELLIPSE_GUARD

class Ellipse {
private:
  float m_a;
  float m_b;
  float m_c;
  float m_d;
  float m_e;
  float m_f;

  bool m_fitted;

public:
  Ellipse(float* points, int numpoints); 
  Ellipse(float* points, int numpoints, bool prev_fit); 
  Ellipse(float a, float b, float c, float d, float e, float f);
  inline bool IsFitted() const { return m_fitted; }
  bool Compare(const Ellipse& o) const;
  
  inline float GetA() const { return m_a; }
  inline float GetB() const { return m_b; }
  inline float GetC() const { return m_c; }
  inline float GetD() const { return m_d; }
  inline float GetE() const { return m_e; }
  inline float GetF() const { return m_f; }

  float GetError(float* points, int numpoints) const;

private:
  void FitEllipse(float* points, int numpoints);
};

#endif//ELLIPSE_GUARD
