/**
 * $Header$
 */

#ifndef ELLIPSE_GUARD
#define ELLIPSE_GUARD

/**
 * A class to represent ellipses in the image and fit one to a set of points.
 *
 * \todo{sometimes one orientation is not possible based on the
 * physical constraints of the camera - check for this}
 *
 * \todo regression test by giving it a set of points derived from an
 * ellipse and asking it to fit them
 *
 * \todo allow the projection method to be altered between ellipse and
 * linear ellipse
 */
class Ellipse {
private:
  float m_a;
  float m_b;
  float m_c;
  float m_d;
  float m_e;
  float m_f;

  bool m_fitted;

public:
  Ellipse();
  Ellipse(const float* points, int numpoints); 
  Ellipse(const float* points, int numpoints, bool prev_fit); 
  Ellipse(float a, float b, float c, float d, float e, float f);
  inline bool IsFitted() const { return m_fitted; }
  bool Compare(const Ellipse& o) const;
  
  inline float GetA() const { return m_a; }
  inline float GetB() const { return m_b; }
  inline float GetC() const { return m_c; }
  inline float GetD() const { return m_d; }
  inline float GetE() const { return m_e; }
  inline float GetF() const { return m_f; }

  float GetError(const float* points, int numpoints) const;
  float GetErrorAlgebraic(const float* points, int count) const;
  float GetErrorGradient(const float* points, int count) const;
  float GetErrorGetErrorNakagawa(const float* points, int numpoints) const;
  float GetErrorGetErrorSafaeeRad(const float* points, int numpoints) const;
  float GetErrorGetErrorSafaeeRad2(const float* points, int numpoints) const;
  float GetErrorGetErrorStricker(const float* points, int numpoints) const;

  virtual void GetTransform(float transform1[16], float transform2[16]);

private:
  bool FitEllipse(const float* points, int numpoints);
};

/**
 * Another ellipse class, functionally identical.  Use this one if you
 * want the get transform method to give you linear scalings of the
 * ellipse rather than the proper pose based transform
 */
class LinearEllipse : public Ellipse {
private:
  float x0;
  float y0;
  float width;
  float height;
  float angle_radians;

public:
  LinearEllipse();
  LinearEllipse(float* points, int numpoints);
  LinearEllipse(float* points, int numpoints, bool prev_fit);
  LinearEllipse(float a, float b, float c, float d, float e, float f);

  inline float GetX0() { return x0; }
  inline float GetY0() { return y0; }
  inline float GetWidth() { return width; }
  inline float GetHeight() { return height; }
  inline float GetAngle() { return angle_radians; }

  /**
   * Calculate the ellipse transform based on decomposing the general
   * conic to central point, axes and inclination.  Use these
   * parameters to linearly scale, shift and rotate the object
   * co-ordinates.
   *
   * Conic equation:
   *  ax**2 + bxy + cy**2 + dx + ey + f = 0
   *
   * with the constraint that
   *   b**2 - 4ac < 0

   * We need to ensure that our matrix has a positive trace and determinant
   * i.e.
   * a+c>0 and b>0
   * Which we can do by multiplying through by -1 if necessary   
   *
   * Re-write the conic equation in vector form as:
   *
   *   [x y].|a    0.5b|.|x| + 2.[x y].|0.5d|+f = 0
   *         |0.5b    c| |y|           |0.5e|
   *
   * which is equivalent to (' implies transpose):
   *
   *   x'.A.x    + x'.t + s = 0   A can be decomposed into scaling components
   *   x'.R'DR.x + x'.t + s = 0   D is a diagonal matrix, R rotational matrix
   *
   * now consider a general ellipse with scale factors given by the
   * diagonal matrix D, rotated by R and translated by v
   *
   *  D = ( a 0 )   R = ( cos(theta) -sin(theta) )   v = ( x0 )
   *      ( 0 b )       ( sin(theta) cos(theta)  )       ( y0 )
   *
   * [x'-v']D^(-2)R'[x-v]-z=0
   *
   * this expands to: (where A = RD^(-2)R') - note A is symmetric so v'Ax = x'Av
   *
   *  x'Ax - 2x'Av + v'Av - z = 0
   *
   * Thus we know that t = -Av (by equating the co-efficients of the terms only in x)
   *
   * So the position of the ellipse (v) is:
   *  
   *   v = -inv(A)*t
   *
   *     = -1/(ca-b^2/4) * (c    -b/2 ) ( 0.5d)
   *                       (-b/2  a   ) ( 0.5e)
   *
   *     = -1/(ca-b^2/4) * ( c   -b/2) ( 0.5d )
   *                       ( -b/2  a ) ( 0.5e )
   *
   *     = -1/(ca-b^2/4) * ( cd/2 - be/4 )
   *                       ( ae/2 - bd/4 )
   *
   *     = 4/(b^2-4ac) * ( cd/2 - be/4 )
   *                     ( ae/2 - bd/4 )
   *
   *     = 1/(b^2-4ac)  * ( 2cd - be)
   *                      ( 2ae - bd)
   *
   * The major and minor axes scaling factors are the co-efficients of
   * D.  D on its own is a scaling matrix so the vectors (1,0) and
   * (0,1) would be scaled to (a,0) and (0,b) - i.e. they are D's
   * eigenvectors and a and b are D's eigenvalues.  Since R is only a
   * rotation we know that the eigenvectors will be different but the
   * eigenvalues will be unchanged.  So the eigenvalues of RD^(-2)R' are the
   * same as the eigenvalues of D^(-2} which are 1/width^2 and 1/height^2.
   *
   * The eigenvalues of a matrix are given by
   *
   *   det[A-lambda.I] = 0 (i.e. solve to find the real eigenvalues)
   *
   *   | a-lambda b/2     | = 0
   *   | b/2     c-lambda |
   *
   *   (a-lambda)(c-lambda) - b^2/4 = 0
   *
   *   lambda^2 - (a+c)lambda - b^2/4+ac = 0
   *
   *   lambda = ( (a+c) +/- sqrt( (a-c)^2 + b^2 ) ) / 2
   *
   * The overall scaling factor is the square-root of the radius of the
   * original circle before we transformed it - this is sqrt(z)
   *
   * We know that the co-efficients of the constant terms must equate:
   *
   *  f = v'Av - z
   *
   * So the scale factor r is:
   *
   * r = sqrt(z) = sqrt(v'Av - f)
   *
   * And since A = RD^(-2)R', and R'R=I => R'=R^(-1), => AR=RD, therefore
   *
   *   ( a11 a12 ) * ( r11 r12 )  =  ( r11 r12 ) * ( d11 d12 )
   *   ( a21 a22 )   ( r21 r22 )     ( r21 r22 )   ( d21 d22 )
   *
   *   a11.r11 + a12.r21 = r11.d11 + r12.d21
   *
   * since this is a rotation matrix, r11=cos(theta), r12=-sin(theta) r21=sin(theta) r22=cos(theta)
   * and since d is a scale matrix, d12=0:
   *
   *   tan(theta) = (d11-a11)/(a21)
   *
   *              = (lambda1-a)/(b/2)
   *
   * (we use the same eigenvalue as the width of the ellipse because by
   *  convention that is the one that defines the angle)
   *
   */
  virtual void Decompose();
  

  /*
   * We then build the transformation matrix that transforms the unit circle onto the ellipse
   *
   * This is a scale factor   ( width 0      0 0 )
   *                          ( 0     height 0 0 ) 
   *                          ( 0     0      1 0 )
   *                          ( 0     0      0 1 )
   *
   * premultiplied by a rotation around the z axis  ( cos(theta)  -sin(theta) 0 0 )
   *                                                ( sin(theta)  cos(theta)  0 0 )
   *                                                ( 0           0           1 0 )
   *                                                ( 0           0           0 1 )
   *
   *
   * premultiplied by a translation in x and y ( 1 0 0 x0 )
   *                                           ( 0 1 0 y0 )
   *                                           ( 0 0 1 1  )
   *                                           ( 0 0 0 1  ) 
   *
   * (translate by 1 in z so that the perspective transform doesn't do
   * anything when we apply it later)
   *
   */
  virtual void GetTransform(float transform1[16], float transform2[16]);
};

#endif//ELLIPSE_GUARD
