/**
 * $Header$
 *
 * $Log$
 * Revision 1.7  2004/02/06 21:11:44  acr31
 * adding ellipse fitting
 *
 * Revision 1.6  2004/02/01 21:29:53  acr31
 * added template tags initial implementation
 *
 * Revision 1.5  2004/02/01 14:26:24  acr31
 * moved rectangle2d to quadtangle2d and refactored implmentations
 *
 * Revision 1.4  2004/01/30 16:54:27  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.3  2004/01/28 17:19:42  acr31
 * providing my own implementation of draw ellipse
 *
 * Revision 1.2  2004/01/27 18:07:27  acr31
 * added another constructor for construct at origin
 *
 * Revision 1.1  2004/01/25 14:53:35  acr31
 * moved over to autoconf/automake build system
 *
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
#include <Ellipse2D.hh>

#undef FILENAME
#define FILENAME "Ellipse2D.cc"

Ellipse2D::Ellipse2D(float width) {
  m_x = width/2;
  m_y = width/2;
  m_width = width;
  m_height = width;
  m_angle_radians = 0;

  m_cost = 1;
  m_sint = 0;
}

Ellipse2D::Ellipse2D(float a, float b, float c, float d, float e, float f) {
  /*
    NOTES ON HOW TO EXTRACT ELLIPSE DETAILS FROM EQNS OF A CONIC SECTION
    (See The Geometry Toolbox for graphics and modelling by Farin and Hansford)

    ellipse is a form of conic
      ax**2 + bxy + cy**2 + dx + ey + f = 0

    with the constraint that
      b**2 - 4ac < 0
  */
  float disc = b*b - 4*a*c;
  if (disc >= 0)
    std::cerr << "Constraint Error: this is not an ellipse" << std::endl;


  /*   
    we can re-write the conic equation in vector form as:
      [x y].|a    0.5b|.|x| + 2.[x y].|0.5d|+f = 0
            |0.5b    c| |y|           |0.5e|

    which is equivalent to (' implies transpose):

      x'.A.x    + x'.t + s = 0   A can be decomposed into scaling components
      x'.R'DR.x + x'.t + s = 0   D is a diagonal matrix, R rotational matrix

    now consider a general ellipse with scale factors given by the diagonal matrix D, rotated by R and translated by v

     D = ( a 0 )   R = ( cos(theta) -sin(theta) )   v = ( x0 )
         ( 0 b )       ( sin(theta) cos(theta)  )       ( y0 )

     [x'-v']R'DR[x-v]-z=0

    this expands to: (where A = R'DR)

     x'Ax + 2x'Av + v'Av - z = 0

    Thus we know that t = Av (by equating the co-efficients of the terms only in x)

    So the position of the ellipse (v) is:
     
      v = inv(A)*t

        = 1/(ca-b^2/4) * (c    -b/2 ) ( 0.5d)
                         (-b/2  a   ) ( 0.5e)

        = 2/(4ca-b^2) * ( c   -b/2) ( d )
	                ( -b/2  a ) ( e )

        = 2/(4ca-b^2) * ( cd - be/2 )
                        ( ae - bd/2 )
  
        = 1/disc  * ( 2cd - be )
	             ( 2ae - bd )
  */

  m_x = (2*c*d - b*e) / disc;
  m_y = (2*a*e - b*d) / disc;


  /*    
    The major and minor axes scaling factors are the co-efficients of
    D.  D on its own is a scaling matrix so the vectors (1,0) and
    (0,1) would be scaled to (a,0) and (0,b) - i.e. they are D's
    eigenvectors and a and b are D's eigenvalues.  Since R is only a
    rotation we know that the eigenvectors will be different but the
    eigenvalues will be unchanged.  So the eigenvalues of R'DR are the
    same as the eigenvalues of D which are a and b.

    The eigenvalues of a matrix are given by

      det[A-lambda.I] = 0 (i.e. solve to find the real eigenvalues)

      | a-lambda b/2     | = 0
      | b/2     c-lambda |

      (a-lambda)(c-lambda) - b^2/4 = 0

      lambda^2 - (a+c)lambda - b^2/4+ac = 0

      lambda = ( (a+c) +/- sqrt( (a+c)^2 - 4*ac + b^2 ) ) / 2
  */
  float tmproot = sqrt( a*a - 2*a*c + c*c + b*b );
  float lambda1 = a+c + tmproot/2;
  float lambda2 = a+c - tmproot/2;
  
  /*
    The overall scaling factor is the square-root of the radius of the
    original circle before we transformed it - this is sqrt(z)

    We know that the co-efficients of the constant terms must equate:

     f = v'Av - z

    So the scale factor r is:

     r = sqrt(z) = sqrt(v'Av - f)
  */

  float scale_factor = sqrt( a*m_x*m_x + b*m_x*m_y + c*m_y*m_y - f);

  /*
    The width and the height of the matrix are then straightforward
    multiplications of the two eigenvalues by the scale_factor and
    then by 2 because we want the whole width not just the "radius"
  */

  m_width = lambda1 * scale_factor;
  m_height = lambda2 * scale_factor;

  /*
    And since A = R'DR, and R'R=I => R'=R^, => RA=DR, therefore

      ( r11 r12 ) * ( a11 a12 )  =  ( d11 d12 ) * ( r11 r12 )
      ( r21 r22 )   ( a21 a22 )     ( d21 d22 )   ( r21 r22 )

      r11.a11 + r12.a21 = d11.r11 + d12.r21

    since this is a rotation matrix, r11=cos(theta), r12=-sin(theta) r21=sin(theta) r22=cos(theta)
    and since d is a scale matrix, d12=0:

      tan(theta) = (a11-d11)/(a21)

                 = (a - lambda1)/(b/2)

    (we use the same eigenvalue as the width of the ellipse because by
    convention that is the one that defines the angle)
  */

  m_angle_radians = atan( (a-lambda1)/(0.5*b) );
  m_cost = cos(m_angle_radians);
  m_sint = sin(m_angle_radians);

  PROGRESS("Decomposed parameters:  centre ("<<m_x<<","<<m_y<<") size ("<<m_width<<"x"<<m_height<<") angle "<<m_angle_radians);
}

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
  float x = radius*cos(angle_radians);
  float y = radius*sin(angle_radians);

  // scale to correct dimensions 
  x*=m_width/2;
  y*=m_height/2;
  
  // rotate to correct angle    
  float ix = x*m_cost-y*m_sint;
  float iy = x*m_sint+y*m_cost;

  // translate to the right origin
  ix+=m_x;
  iy+=m_y;

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
