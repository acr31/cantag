/**
 * $Header$
 *
 * $Log$
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
