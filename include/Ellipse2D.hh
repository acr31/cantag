/**
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/02/13 21:47:36  acr31
 * work on ellipse fitting
 *
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
  /* Coefficients from generalised conic equation */
  float m_a;
  float m_b;
  float m_c;
  float m_d;
  float m_e;
  float m_f;

  /* 3D location */
  float m_px;
  float m_py;
  float m_pz;

  /* Normal vector */
  float m_nx;
  float m_ny;
  float m_nz;

  /* 3D transformation matrix to map from object co-ordinates to camera co-ordinates */
  float m_transform[9];

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

private:
  /**
   * Convert the centre, size and angle of this ellipse to parameters
   * for the generalised conic equation.
   */
  void ToGeneralConic();

  /**
   * Convert the parameters for the generalised conic equation to
   * centre, size and angle.
   */
  void FromGeneralConic();
  
  /**
   * Calculate the transformation matrix for this circle. 
   */
  void ComputePose();

  /**
   * Take the known radius of the circle and combine it with the pose
   * information to work out the 3D position of the circle.  Don't run
   * this before running ComputePose()!
   */
  void ComputePosition(double radius);
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
