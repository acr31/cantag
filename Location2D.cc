/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/01/21 12:01:41  acr31
 * moved Location2DChain definition to Location2D.hh and added a destructor
 *
 * Revision 1.2  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#include "Location2D.hh"

Location2D::Location2D(float x, float y, float width, float height, float angle_radians) : 
  m_x(x),
  m_y(y),
  m_width(width),
  m_height(height),
  m_angle_radians(angle_radians) {}


Location2DChain::Location2DChain(Location2D *cur) : current(cur) {};

Location2DChain::~Location2DChain() {
  delete(current);
  if (next!=NULL) { delete(next); }
}
