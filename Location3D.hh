/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 */
#ifndef LOCATION3D_GUARD
#define LOCATION3D_GUARD

#include <iostream>
#include "Config.hh"

class Location3D {
public:
  float m_x;
  float m_y;
  float m_z;

  float m_normal_x;
  float m_normal_y;
  float m_normal_z;

  Location3D(float x, float y, float z, float normal_x, float normal_y, float normal_z);
};

std::ostream& operator<<(std::ostream& s, const Location3D& z);
#endif//LOCATION3D_GUARD
