/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/23 15:42:29  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#ifndef RECTANGLE_2D_GUARD
#define RECTANGLE_2D_GUARD

#include <ostream>

#include "Config.hh"

class Rectangle2D {
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

private:
  inline void compute_central_point();

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
