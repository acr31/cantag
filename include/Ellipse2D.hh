/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/02/06 21:11:38  acr31
 * adding ellipse fitting
 *
 * Revision 1.2  2004/01/27 18:06:58  acr31
 * changed inheriting classes to inherit publicly from their parents
 *
 * Revision 1.1  2004/01/25 14:54:36  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.2  2004/01/24 19:29:24  acr31
 * removed ellipsetoxy and put the project method in Ellipse2D objects
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
#ifndef ELLIPSE_2D_GUARD
#define ELLIPSE_2D_GUARD

#include <ostream>

#include "Config.hh"

class Ellipse2D {
private:
  float m_sint;
  float m_cost;

public:
  float m_x;
  float m_y;
  float m_width;
  float m_height;
  float m_angle_radians;

  Ellipse2D(float width);
  Ellipse2D(float a,float b,float c,float d, float e,float f);
  Ellipse2D(float x, float y, float width, float height, float angle_radians);
  void ProjectPoint(float angle_radians, float radius, float *projX, float *projY) const;
};

class Ellipse2DChain {
public:
  Ellipse2D* current;
  Ellipse2DChain* nextchain;

  Ellipse2DChain(Ellipse2D* current);
  ~Ellipse2DChain();
};

std::ostream& operator<<(std::ostream& s, const Ellipse2D& z);

#endif//ELLIPSE_2D_GUARD
