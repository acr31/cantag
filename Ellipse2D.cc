/**
 * $Header$
 *
 * $Log$
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

Ellipse2D::Ellipse2D(float x, float y, float width, float height, float angle_radians) : 
  m_x(x),
  m_y(y),
  m_width(width),
  m_height(height),
  m_angle_radians(angle_radians) {}


Ellipse2DChain::Ellipse2DChain(Ellipse2D *cur) : current(cur), nextchain(NULL) {};

Ellipse2DChain::~Ellipse2DChain() {
  delete(current);
  if (nextchain!=NULL) { delete(nextchain); }
}

std::ostream& operator<<(std::ostream& s, const Ellipse2D& z) { 
  s<< "Position ("<<z.m_x<<","<<z.m_y<<"), Size ("<<z.m_width<<","<<z.m_height<<")";
  return s;
}
