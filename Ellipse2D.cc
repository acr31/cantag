/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/01/24 19:29:23  acr31
 * removed ellipsetoxy and put the project method in Ellipse2D objects
 *
 * Revision 1.2  2004/01/23 22:35:49  acr31
 * added FILENAME define
 *
 * Revision 1.1  2004/01/23 12:05:48  acr31
 * moved Tag to CircularTag in preparation for Squaretag
 *
 * Revision 1.4  2004/01/21 13:41:35  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.3  2004/01/21 12:01:41  acr31
 * moved Location2DChain definition to Location2D.hh and added a destructor
 *
 * Revision 1.2  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#include "Ellipse2D.hh"

#undef FILENAME
#define FILENAME "Ellipse2D.cc"

Ellipse2D::Ellipse2D(float x, float y, float width, float height, float angle_radians) : 
  m_x(x),
  m_y(y),
  m_width(width),
  m_height(height),
  m_angle_radians(angle_radians) {
  
  m_cost = cos(m_angle_radians);
  m_sint = sin(m_angle_radians);
}


void Ellipse2D::ProjectPoint(float angle_radians, float radius, float *projX, float *projY) const {
  float x = radius*cos(angle_radians)*m_width*0.5;
  float y = radius*sin(angle_radians)*m_height*0.5;
  float ix = x*m_cost-y*m_sint + m_x;
  float iy = x*m_sint+y*m_cost + m_y;

  *projX = ix;
  *projY = iy;
  
  PROGRESS("Projecting point radius "<<radius<<" angle "<<angle_radians<<" on to ("<< *projX <<","<< *projY <<")");
}


Ellipse2DChain::Ellipse2DChain(Ellipse2D *cur) : current(cur), nextchain(NULL) {};

Ellipse2DChain::~Ellipse2DChain() {
  delete(current);
  if (nextchain!=NULL) { delete(nextchain); }
}

std::ostream& operator<<(std::ostream& s, const Ellipse2D& z) { 
  s<< "Position ("<<z.m_x<<","<<z.m_y<<"), Size ("<<z.m_width<<","<<z.m_height<<")";
  return s;
}
