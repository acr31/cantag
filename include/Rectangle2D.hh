/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.2  2004/01/23 18:18:12  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 * Revision 1.1  2004/01/23 15:42:29  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#ifndef RECTANGLE_2D_GUARD
#define RECTANGLE_2D_GUARD

#include <ostream>

#include "Config.hh"
#include "Location3D.hh"

class Rectangle2D {
private:
  float m_alpha[8];

public:
  float m_x0;
  float m_y0;

  float m_x1;
  float m_y1;

  float m_x2;
  float m_y2;

  float m_x3;
  float m_y3;

  float m_xc;
  float m_yc;
  

  Rectangle2D(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3);
  Rectangle2D(float* coords);
  void ProjectPoint(float rectx, float recty, float *projX, float *projY) const;
  Location3D* EstimatePose(float width, float height);

private:
  inline void compute_central_point();
  inline void compute_alpha();
};

class Rectangle2DChain {
public:
  Rectangle2D* current;
  Rectangle2DChain* nextchain;

  Rectangle2DChain(Rectangle2D* current);
  ~Rectangle2DChain();
};

std::ostream& operator<<(std::ostream& s, const Rectangle2D& z);

#endif//RECTANGLE_2D_GUARD
