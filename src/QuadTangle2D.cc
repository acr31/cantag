/**
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/02/11 08:23:49  acr31
 * *** empty log message ***
 *
 * Revision 1.3  2004/02/10 18:12:43  acr31
 * moved gaussian elimination functionality around a bit
 *
 * Revision 1.2  2004/02/03 07:48:30  acr31
 * added template tag
 *
 * Revision 1.1  2004/02/01 14:26:24  acr31
 * moved rectangle2d to quadtangle2d and refactored implmentations
 *
 * Revision 1.7  2004/01/30 16:54:27  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.6  2004/01/30 08:05:23  acr31
 * changed rectangle2d to use gaussian elimination
 *
 * Revision 1.5  2004/01/29 12:47:27  acr31
 * factoring around with the alpha calculation stuff
 *
 * Revision 1.4  2004/01/27 18:07:27  acr31
 * added another constructor for construct at origin
 *
 * Revision 1.3  2004/01/26 09:07:42  acr31
 * added comments to new method of solving linear equation
 *
 * Revision 1.2  2004/01/26 08:56:22  acr31
 * rewrite of the find central point method - needs to be finished
 *
 * Revision 1.1  2004/01/25 14:53:35  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.3  2004/01/24 17:53:22  acr31
 * Extended TripOriginalCoder to deal with base 2 encodings.  MatrixTag
 * implementation now works.
 *
 * Revision 1.2  2004/01/23 18:18:11  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 * Revision 1.1  2004/01/23 15:42:29  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#include <QuadTangle2D.hh>
#include <gaussianelimination.hh>
#include <Drawing.hh>

#define ZERO_DISTANCE 0.00001

QuadTangle2D::QuadTangle2D(float width) {
  float diff = width/2;
  m_x0 = 0;
  m_y0 = 0;
  m_x1 = width;
  m_y1 = 0;
  m_x2 = width;
  m_y2 = width;
  m_x3 = 0;
  m_y3 = width;

  compute_central_point();
  sort_points();
  compute_alpha();
}

QuadTangle2D::QuadTangle2D(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3) :
  m_x0(x0),
  m_y0(y0),
  m_x1(x1),
  m_y1(y1),
  m_x2(x2),
  m_y2(y2),
  m_x3(x3),
  m_y3(y3) {

  compute_central_point();
  sort_points();
  compute_alpha();
};

QuadTangle2D::QuadTangle2D(float* coords) :
  m_x0(coords[0]),
  m_y0(coords[1]),
  m_x1(coords[2]),
  m_y1(coords[3]),
  m_x2(coords[4]),
  m_y2(coords[5]),
  m_x3(coords[6]),
  m_y3(coords[7]) {

  compute_central_point();
  sort_points();
  compute_alpha();
};

void QuadTangle2D::ProjectPoint(float x, float y, float *projX, float *projY) const {
  *projX = (m_alpha[0]*x+m_alpha[1]*y+m_alpha[2])/(m_alpha[6]*x+m_alpha[7]*y+1);
  *projY = (m_alpha[3]*x+m_alpha[4]*y+m_alpha[5])/(m_alpha[6]*x+m_alpha[7]*y+1);
  //  PROGRESS("Projecting point ("<<x <<","<<y<<") on to ("<< *projX<<","<<*projY<<")");
}

Location3D* QuadTangle2D::EstimatePose(float width, float height) {
  /*
   * We need to work out c1...c9 in order to be able to work out the
   * xyz positions for any u and v
   *
   * We already know alpha[0]...alpha[7] which correspond to c1/c9...c8/c9
   * 
   * We know r = the ratio width/height
   *
   * So to work out c9 we set u=v=0 this gives x=c3, y=c6, z=c9
   * Then set u=0,v=1 this gives x=c2+c3, y=c5+c6, z=c8+c9
   * These are the co-ordinates for the height of the tag
   *
   * So  (c3 - (c2+c3))^2 + (c6 - (c5+c6))^2 + (c9 - (c8+c9))^2  = width^2
   *     c2^2 + c5^2 + c8^2 = width^2
   *     c2^2/c9^9 + c5^2/c9^2 + c8^2/c9^2 = width^2/c9^2
   *     c9^2 = width^2/(alpha[1]^2+alpha[4]^2+alpha[7]^2)
   *
   * Now, the unit normal is the cross product of two vectors lying in the plane
   *
   * Lets use v1 = (0,0)->(0,1) and v2 = (0,0)->(height/width,0) (this is the
   * vector to the other corner)
   *
   * v1 = (c2+c3)     v2 = ( (height/width)c1+c3 )
   *      (c5+c6)          ( (height/width)c4+c6 )
   *      (c8+c9)          ( (height/width)c8+c9 )
   *
   * v1 x v2 = ( (c5+c6)*((height/width)c8+c9) - (c8+c9)*((height/width)c4+c6) )
   *           ( (c8+c9)*((height/width)c1+c3) - (c2+c3)*((height/width)c8+c9) )
   *           ( (c2+c3)*((height/width)c4+c6) - (c5+c6)*((height/width)c1+c3) )
   *
   *
   *
   *     
   */
  return NULL;
}

void QuadTangle2D::compute_alpha() {
  //PROGRESS(m_x0<<","<<m_y0<< " " <<m_x1<<","<<m_y1<< " " <<m_x2<<","<<m_y2<< " " <<m_x3<<","<<m_y3);
  /*
   * Taken from:
   *
   * @InProceedings{ip:apchi:rekimoto98,
   *  author        = "Jun Rekimoto",
   *  title         = "Matrix: A Realtime Object Identification and Registration Method for Augmented Reality",
   *  booktitle     = "Proceedings of Asia Pacific Computer Human Interaction",
   *  year          = "1998",
   * }
   *
   *
   * If the co-ordinates relative to the matrix are (u,v).  Then the
   * co-ordinates in the camera frame of reference are (x,y,z)
   *
   * x = c0 * u + c1 * v + c2
   * y = c3 * u + c4 * v + c5
   * z = c6 * u + c7 * v + c8
   *
   * There is nothing clever about the above - they are just an
   * arbitrary linear combination of u and v
   *
   * Our screen co-ordinates X and Y are a perspective projection of (x,y,z)
   *
   * X = x/z
   * Y = y/z
   *
   * Since both of the equations are over z we can divide through by c9 to give
   *
   *  X = (a0*x + a1*y + a2)/(a6*x+a7*y+1)
   *  Y = (a3*x + a4*y + a5)/(a6*x+a7*y+1)
   *
   * Where a0 = c0/c8, a1 = c1/c8,  etc...
   *
   * Given four points on the matrix and their points in the final
   * image we can set up a set of simultaneous linear equations and
   * solve for a0 to a7
   *
   * ( X0 )   (  u0  v0  1   0   0  0  -X0*u0  -X0*v0 ) ( a0 )
   * ( X1 )   (  u1  v1  1   0   0  0  -X1*u1  -X1*v1 ) ( a1 )
   * ( X2 )   (  u2  v2  1   0   0  0  -X2*u2  -X2*v2 ) ( a2 )
   * ( X3 )   (  u3  v3  1   0   0  0  -X3*u3  -X3*v3 ) ( a3 )
   * ( Y0 ) = (   0   0  0  u0  v0  1  -Y0*u0  -Y0*v0 ) ( a4 )
   * ( Y1 )   (   0   0  0  u1  v1  1  -Y1*u1  -Y1*v1 ) ( a5 )
   * ( Y2 )   (   0   0  0  u2  v2  1  -Y2*u2  -Y2*v2 ) ( a6 )
   * ( Y3 )   (   0   0  0  u3  v3  1  -Y3*u3  -Y3*y3 ) ( a7 )
   *
   * In the above (X_n,Y_n) are pairs of screen co-ordinates that
   * correspond to (u_n,v_n) pairs of co-ordinates on the tag
   *
   * We have found four points - the four corners which we have sorted
   * so that we have
   *
   * (x3,y3) +----------+ (x0,y0)
   *         |          |
   *         |          |
   *         |          |
   *         |          |
   * (x2,y2) +----------+ (x1,y1)
   *
   * Our screen co-ordinate origin is top left so:
   *
   * x0,y0 -> 1,0 (tag co-ordinates)
   * x1,y1 -> 1,1
   * x2,y2 -> 0,1
   * x3,y3 -> 0,0
   *
   * So we can now substitute x_n,y_n (known tag co-ordinates from
   * above) and X_n,Y_n (known screen co-ordinates from quadtangle
   * into the above matrix.  Do this in such away that the matrix will
   * be easy to invert.
   *
   * x0,y0 -> u1,v1
   * x1,y1 -> u0,v0
   * x2,y2 -> u3,v3
   * x3,y3 -> u2,v2
   *
   * This gives (remembering to swap X and Y's too)
   *
   * ( X1 )   (  x1  y1  1   0   0  0  -X1*x1  -X1*y1 ) ( a0 )
   * ( X0 )   (  x0  y0  1   0   0  0  -X0*x0  -X0*y0 ) ( a1 )
   * ( X3 )   (  x3  y3  1   0   0  0  -X3*x3  -X3*y3 ) ( a2 )
   * ( X2 )   (  x2  y2  1   0   0  0  -X2*x2  -X2*y2 ) ( a3 )
   * ( Y1 ) = (   0   0  0  x1  y1  1  -Y1*x1  -Y1*y1 ) ( a4 )
   * ( Y0 )   (   0   0  0  x0  y0  1  -Y0*x0  -Y0*y0 ) ( a5 )
   * ( Y3 )   (   0   0  0  x3  y3  1  -Y3*x3  -Y3*y3 ) ( a6 )
   * ( Y2 )   (   0   0  0  x2  y2  1  -Y2*x2  -Y2*y2 ) ( a7 )
   *
   * which simplifies (because we know x_n,y_n) to:
   *
   * ( X1 )   (  1  1  1   0   0  0  -X1  -X1 ) ( a0 )
   * ( X0 )   (  1  0  1   0   0  0  -X0    0 ) ( a1 )
   * ( X3 )   (  0  0  1   0   0  0    0    0 ) ( a2 )
   * ( X2 )   (  0  1  1   0   0  0    0  -X2 ) ( a3 )
   * ( Y1 ) = (  0  0  0   1   1  1  -Y1  -Y0 ) ( a4 )
   * ( Y0 )   (  0  0  0   1   0  1  -Y0    0 ) ( a5 )
   * ( Y3 )   (  0  0  0   0   0  1    0    0 ) ( a6 )
   * ( Y2 )   (  0  0  0   0   1  1    0  -Y2 ) ( a7 )
   *
   */

  // we particularly want coeffs to be an array of pointers to arrays
  // containing the columns of the matrix - then we can swap columns
  // conveniently by swapping pointers
  double coeffs0[] = {1,1,0,0,0,0,0,0};
  double coeffs1[] = {1,0,0,1,0,0,0,0};
  double coeffs2[] = {1,1,1,1,0,0,0,0};
  double coeffs3[] = {0,0,0,0,1,1,0,0};
  double coeffs4[] = {0,0,0,0,1,0,0,1};
  double coeffs5[] = {0,0,0,0,1,1,1,1};
  double coeffs6[] = {-m_x1,-m_x0,0,0,-m_y1,-m_y0,0,0};
  double coeffs7[] = {-m_x1,0,0,-m_x2,-m_y1,0,0,-m_y2};
  double* coeffs[] = {coeffs0,
		     coeffs1,
		     coeffs2,
		     coeffs3,
		     coeffs4,
		     coeffs5,
		     coeffs6,
		     coeffs7};
		     
  double xvals[] = { m_x1,
		    m_x0,
		    m_x3,
		    m_x2,
		    m_y1,
		    m_y0,
		    m_y3,
		    m_y2 };



  solve_simultaneous(xvals,coeffs,m_alpha,8);

  PROGRESS("Computed alpha[0] "<<m_alpha[0]);
  PROGRESS("         alpha[1] "<<m_alpha[1]);
  PROGRESS("         alpha[2] "<<m_alpha[2]);
  PROGRESS("         alpha[3] "<<m_alpha[3]);
  PROGRESS("         alpha[4] "<<m_alpha[4]);
  PROGRESS("         alpha[5] "<<m_alpha[5]);
  PROGRESS("         alpha[6] "<<m_alpha[6]);
  PROGRESS("         alpha[7] "<<m_alpha[7]);

}


inline void QuadTangle2D::compute_central_point() {
  /*
   * Find the central point of this quadtangle (A,B,C,D)
   *
   *     A +--------------+ B
   *       |              |
   *       |              |
   *       |              |
   *       |              |
   *       |              |
   *       |              |
   *       |              |
   *     D +--------------+ C
   *
   *
   *       A + AC*a = B + BD*b
   *
   *    a*AC - b*BD = B - A
   *
   *  (ACx -BDx)(a) = (ABx) 
   *  (ACy -BDy)(b)   (ABy)
   *
   *            (a) = 1/( ACy*BDx - ACx*BDy ) * (-BDy  BDx)(ABx)
   *            (b)                             (-ACy  ACx)(ABy)
   *
   * Central point  = A+AC*a
   *
   */
  float Ax = m_x0;
  float Ay = m_y0;

  float Bx = m_x1;
  float By = m_y1;

  float Cx = m_x2;
  float Cy = m_y2;

  float Dx = m_x3;
  float Dy = m_y3;

  float ACx = Cx-Ax;
  float ACy = Cy-Ay;

  float BDx = Dx-Bx;
  float BDy = Dy-By;

  float ABx = Bx-Ax;
  float ABy = By-Ay;

  float b = (BDx*ABy - BDy*ABx)/(ACy*BDx - ACx*BDy);

  m_xc = Ax+ACx*b;
  m_yc = Ay+ACy*b;        
}


float QuadTangle2D::find_angle(float x, float y, float cx, float cy) {
  if ((x >= cx) && (y >= cy)) {
    return PI/2+ atan( (y-cy) / (x-cx) );
  }
  else if ((x >= cx) && (y < cy)) {
    return atan( (cy-y) / (x-cx) );
  }
  else if ((x < cx) && (y < cy)) {
    return 3*PI/2 + atan ( (cy-y) / (cx-x) );
  }
  else if ((x < cx) && (y >= cy)) {
    return PI + atan( (y-cy) / (cx-x));
  }
}

void QuadTangle2D::swap( float *a, float *b) {
  float t = *a;
  *a = *b;
  *b = t;
}

void QuadTangle2D::sort_points()
{
  // sort the points into clockwise order.
  float angles[4];
  angles[0] = find_angle(m_x0,m_y0,m_xc,m_yc);
  angles[1] = find_angle(m_x1,m_y1,m_xc,m_yc);
  angles[2] = find_angle(m_x2,m_y2,m_xc,m_yc);
  angles[3] = find_angle(m_x3,m_y3,m_xc,m_yc);
  //  PROGRESS("Centre (" << m_xc << "," << m_yc <<")");
  //  PROGRESS("Original order " << std::endl <<
  //	   "(" << m_x0 << "," << m_y0 << ") @ "<< angles[0] << std::endl <<
  //	   "(" << m_x1 << "," << m_y1 << ") @ "<< angles[1] << std::endl <<
  //	   "(" << m_x2 << "," << m_y2 << ") @ "<< angles[2] << std::endl <<
  //	   "(" << m_x3 << "," << m_y3 << ") @ "<< angles[3]);
  float tx,ty; 
  for(int i=0;i<4;i++) {
    if (angles[0] > angles[1]) {
      swap(&m_x0,&m_x1);
      swap(&m_y0,&m_y1);
      swap(&angles[0],&angles[1]);
    }

    if (angles[1] > angles[2]) {
      swap(&m_x1,&m_x2);
      swap(&m_y1,&m_y2);
      swap(&angles[1],&angles[2]);
    }

    if (angles[2] > angles[3]) {
      swap(&m_x2,&m_x3);
      swap(&m_y2,&m_y3);
      swap(&angles[2],&angles[3]);
    }
  }
  
  //  PROGRESS("Final order " << std::endl <<
  //	   "(" << m_x0 << "," << m_y0 << ") @ "<< angles[0] << std::endl <<
  //	   "(" << m_x1 << "," << m_y1 << ") @ "<< angles[1] << std::endl <<
  //	   "(" << m_x2 << "," << m_y2 << ") @ "<< angles[2] << std::endl <<
  //	   "(" << m_x3 << "," << m_y3 << ") @ "<< angles[3]);
}

  
QuadTangle2DChain::QuadTangle2DChain(QuadTangle2D *cur) : current(cur), nextchain(NULL) {};

QuadTangle2DChain::~QuadTangle2DChain() {
  delete(current);
  if (nextchain!=NULL) { delete(nextchain); }
}

std::ostream& operator<<(std::ostream& s, const QuadTangle2D& z) { 
  s << "Position (" << z.m_x0 << "," << z.m_y0 << ") " << "(" << z.m_x1 << "," << z.m_y1 << ") " << "(" << z.m_x2 << "," <<z.m_y2 <<") " << "(" << z.m_x3 << "," << z.m_y3 << ")"; 
  return s;
}