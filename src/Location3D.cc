/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:53:35  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.1  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 */
#include <Location3D.hh>

Location3D::Location3D(float x, float y, float z, float normal_x, float normal_y, float normal_z) :
  m_x(x),
  m_y(y),
  m_z(z),
  m_normal_x(x),
  m_normal_y(y),
  m_normal_z(z) {}


std::ostream& operator<<(std::ostream& s, const Location3D& z) { 
  s<< "Position ("<<z.m_x<<","<<z.m_y<<","<<z.m_z<<"), Normal ("<<z.m_normal_x<<","<<z.m_normal_y<<","<<z.m_normal_z<<")";
  return s;
}
