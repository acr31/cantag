/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#ifndef LOCATION_2D_GUARD
#define LOCATION_2D_GUARD

#include "Config.hh"

class Location2D {
public:
  float m_x;
  float m_y;
  float m_width;
  float m_height;
  float m_angle_radians;

  Location2D(float x, float y, float width, float height, float angle_radians);
};

#endif//LOCATION_2D_GUARD
