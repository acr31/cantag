/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/23 15:42:29  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#include "Rectangle2D.hh"

Rectangle2D::Rectangle2D(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3) :
  m_x0(x0),
  m_y0(y0),
  m_x1(x1),
  m_y1(y1),
  m_x2(x2),
  m_y2(y2),
  m_x3(x3),
  m_y3(y3) {

  compute_central_point();
};

Rectangle2D::Rectangle2D(float* coords) :
  m_x0(coords[0]),
  m_y0(coords[1]),
  m_x1(coords[2]),
  m_y1(coords[3]),
  m_x2(coords[4]),
  m_y2(coords[5]),
  m_x3(coords[6]),
  m_y3(coords[7]) {
  compute_central_point();
};


inline void Rectangle2D::compute_central_point() {
  /*
    Find the central point of this rectangle (A,B,C,D)

     A + AC*a = B + BD*b

     Ax + ACx * a = Bx + BDx * b   (1)
     Ay + ACy * a = By + BDy * b   (2)

                a = ( Bx + BDx * b - Ax ) / ACx   (from 1)
                
     Ay + ACy * (Bx + BDx * b - Ax) / ACx = By + BDy * b  (subst in 2)

     b = ( Ay * ACx + ACy * Bx + ACy * Ax - ACx * By ) / ( ACx * (BDy + BDx) )

     Central point  = B+BD*b

  */
  float Ax = m_x0;
  float Ay = m_y0;

  float Bx = m_x1;
  float By = m_y1;

  float Cx = m_x2;
  float Cy = m_y2;

  float Dx = m_x3;
  float Dy = m_y3;

  float ACx = Cx-Ax;
  float ACy = Cy-Ay;

  float BDx = Dx-Bx;
  float BDy = Dy-By;

  float b = ( Ay * ACx + ACy * Bx + ACy * Ax - ACx * By ) / ( ACx * (BDy + BDx) );
  
  m_xc = Bx+BDx*b;
  m_yc = By+BDy*b;        
}

  
Rectangle2DChain::Rectangle2DChain(Rectangle2D *cur) : current(cur), nextchain(NULL) {};

Rectangle2DChain::~Rectangle2DChain() {
  delete(current);
  if (nextchain!=NULL) { delete(nextchain); }
}

std::ostream& operator<<(std::ostream& s, const Rectangle2D& z) { 
  s << "Position (" << z.m_x0 << "," << z.m_y0 << ") " << "(" << z.m_x1 << "," << z.m_y1 << ") " << "(" << z.m_x2 << "," <<z.m_y2 <<") " << "(" << z.m_x3 << "," << z.m_y3 << ")"; 
  return s;
}
