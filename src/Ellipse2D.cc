/**
 * $Header$
 *
 * $Log$
 * Revision 1.12  2004/02/17 08:01:29  acr31
 * *** empty log message ***
 *
 * Revision 1.11  2004/02/16 16:02:27  acr31
 * *** empty log message ***
 *
 * Revision 1.10  2004/02/13 21:47:39  acr31
 * work on ellipse fitting
 *
 * Revision 1.9  2004/02/09 21:39:56  acr31
 * manual matrix stuff in ellipse fitter.  Added camera functions and a test.
 *
 * Revision 1.8  2004/02/08 20:30:17  acr31
 * changes to interfaces to add the ImageFilter class
 *
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
#include <eigenvv.hh>

#undef DECOMPOSE_DEBUG
#define POSE_DEBUG

Ellipse2D::Ellipse2D(float width) :
  m_x(width/2),
  m_y(width/2),
  m_width(width),
  m_height(width),
  m_angle_radians(0)
 {
  ToGeneralConic();
  ComputePose();
}

Ellipse2D::Ellipse2D(float a, float b, float c, float d, float e, float f) :
  m_a(a),
  m_b(b),
  m_c(c),
  m_d(d),
  m_e(e),
  m_f(f) {  
  FromGeneralConic();
  ComputePose();
}

Ellipse2D::Ellipse2D(float x, float y, float width, float height, float angle_radians) :
  m_x(x),
  m_y(y),
  m_width(width),
  m_height(height),
  m_angle_radians(angle_radians)
 {
  ToGeneralConic();
  ComputePose();
}

void Ellipse2D::ProjectPoint(float angle_radians, float radius, float *projX, float *projY) const {
  // work out where we want our point to be if our tag was a circle
  // centred on the origin and then transform it with the transform
  // matrix

  float x = radius*cos(angle_radians);
  float y = radius*sin(angle_radians);
  float z = 1;
  float h = 1;
  
  *projX = m_transform[0]*x + m_transform[1]*y + m_transform[2]*z + m_transform[3]*h;
  *projY = m_transform[4]*x + m_transform[5]*y + m_transform[6]*z + m_transform[7]*h;
  float projZ = m_transform[8]*x + m_transform[9]*y + m_transform[10]*z + m_transform[11]*h;
  float projH = m_transform[12]*x + m_transform[13]*y + m_transform[14]*z + m_transform[15]*h;
  
  // now do a perspective transform - note that the homogenous
  // parameter H that we need to divide through by cancels out: X/H /
  // (Z/H) = X/H /Z * H = X/Z
  *projX /= projZ;
  *projY /= projZ;
  


  PROGRESS("Projecting point radius "<<radius<<" angle "<<angle_radians<<" on to ("<< *projX <<","<< *projY <<")");
}

void Ellipse2D::ToGeneralConic() {
  float sintheta = sin(m_angle_radians);
  float costheta = cos(m_angle_radians);
  float x0 = m_x;
  float y0 = m_y;
  float asq = m_width * m_width * 0.25;
  float bsq = m_height * m_height *0.25;

  float sinthetasq = sintheta*sintheta;
  float costhetasq = costheta*costheta;
  
  float x0sq = x0*x0;
  float y0sq = y0*y0;

  float m_a = 1/asq*sinthetasq + 1/bsq*costhetasq;
  float m_b = 2/asq*sintheta*costheta - 2/bsq*sintheta*costheta;
  float m_c = 1/asq*costhetasq + 1/bsq*sinthetasq;
  float m_d = -2/asq*sinthetasq*x0 - 2/asq*sintheta*costheta*y0 + 2/bsq*sintheta*costheta*y0 - 2/bsq*costhetasq*x0;
  float m_e = -2/asq*sintheta*x0*costheta - 2/asq*costhetasq*y0 - 2/bsq*sinthetasq*y0 + 2/bsq*sintheta*x0*costheta;
  float m_f = 1/asq*sinthetasq*x0sq + 2/asq*sintheta*x0*costheta*y0 + 1/asq*costhetasq*y0sq + 1/bsq*sinthetasq*y0sq - 2/bsq*sintheta*x0*costheta*y0 + 1/bsq*costhetasq*x0sq-1;
}


void Ellipse2D::FromGeneralConic() { 
  /*
    NOTES ON HOW TO EXTRACT ELLIPSE DETAILS FROM EQNS OF A CONIC SECTION
    (See The Geometry Toolbox for graphics and modelling by Farin and Hansford)

    ellipse is a form of conic
      ax**2 + bxy + cy**2 + dx + ey + f = 0

    with the constraint that
      b**2 - 4ac < 0
  */
  float disc = m_b*m_b - 4*m_a*m_c;

#ifdef DECOMPOSE_DEBUG
  if (disc >= 0)
    std::cerr << "Constraint Error: this is not an ellipse" << std::endl;
#endif

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

    this expands to: (where A = R'DR) - note A is symmetric so v'Ax = x'Av

     x'Ax - 2x'Av + v'Av - z = 0

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

  m_x = (2*m_c*m_d - m_b*m_e) / disc;
  m_y = (2*m_a*m_e - m_b*m_d) / disc;

#ifdef DECOMPOSE_DEBUG
  std::cout << "X= " << m_x<< std::endl;
  std::cout << "Y= " << m_y<< std::endl;
#endif
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
  float tmproot = sqrt( (m_a-m_c)*(m_a-m_c) + m_b*m_b );
  float lambda1 = m_a+m_c + tmproot/2;
  float lambda2 = m_a+m_c - tmproot/2;
  
#ifdef DECOMPOSE_DEBUG
  std::cout << "tmproot= " << tmproot << std::endl;
  std::cout << "lambda1= " << lambda1 << std::endl;
  std::cout << "lambda2= " << lambda2 << std::endl;
#endif

  /*
    The overall scaling factor is the square-root of the radius of the
    original circle before we transformed it - this is sqrt(z)

    We know that the co-efficients of the constant terms must equate:

     f = v'Av - z

    So the scale factor r is:

     r = sqrt(z) = sqrt(v'Av - f)
  */

  float scale_factor = sqrt( -m_f + m_a*m_x*m_x + m_b*m_x*m_y + m_c*m_y*m_y );

#ifdef DECOMPOSE_DEBUG
  std::cout << "scale= " << scale_factor << std::endl;
#endif

  /*
    The width and the height of the matrix are then straightforward
    multiplications of the two eigenvalues by the scale_factor and
    then by 2 because we want the whole width not just the "radius"
  */

  m_width = lambda1 * scale_factor * 2;
  m_height = lambda2 * scale_factor * 2;

#ifdef DECOMPOSE_DEBUG
  std::cout << "width= " << m_width << std::endl;
  std::cout << "height= " << m_height << std::endl;
#endif

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

  m_angle_radians = atan( (m_a-lambda1)/(0.5*m_b) );

#ifdef DECOMPOSE_DEBUG
  std::cout << "angle= " << m_angle_radians << std::endl;
#endif

  PROGRESS("Decomposed parameters:  centre ("<<m_x<<","<<m_y<<") size ("<<m_width<<"x"<<m_height<<") angle "<<m_angle_radians);

}

void Ellipse2D::ComputePose() {
#ifdef POSE_DEBUG
  std::cout << "Ellipse params (a-f) are ["<<m_a<<","<<m_b<<","<<m_c<<","<<m_d<<","<<m_e<<","<<m_f<<"];"<<std::endl;
#endif

  double eigvects[9];
  double eigvals[9];

  /**
   * Solve eigenvectors of ( m_a     m_2/b    m_d/2 )
   *                       ( m_b/2   m_c      m_e/2 )
   *                       ( m_d/2   m_e/2    m_f   )
   */
  eigensolve(m_a,m_b/2,m_d/2,m_c,m_e/2,m_f, eigvects, eigvals);

#ifdef POSE_DEBUG
  std::cout << "Eigen Vectors: " << eigvects[0] << " " << eigvects[1] << " " << eigvects[2] << std::endl;
  std::cout << "               " << eigvects[3] << " " << eigvects[4] << " " << eigvects[5] << std::endl;
  std::cout << "               " << eigvects[6] << " " << eigvects[7] << " " << eigvects[8] << std::endl;

  std::cout << "Eigen Values: " << eigvals[0] << " " << eigvals[1] << " " << eigvals[2] << std::endl;
  std::cout << "              " << eigvals[3] << " " << eigvals[4] << " " << eigvals[5] << std::endl;
  std::cout << "              " << eigvals[6] << " " << eigvals[7] << " " << eigvals[8] << std::endl;
#endif

  // normalise the eigen vectors 
  /*
  double dete1 = sqrt(eigvects[0]*eigvects[0] +
		     eigvects[3]*eigvects[3] +
		     eigvects[6]*eigvects[6]);
  eigvects[0] /= dete1;
  eigvects[3] /= dete1;
  eigvects[6] /= dete1;
  
  double dete2 = sqrt(eigvects[1]*eigvects[1] + 
		     eigvects[4]*eigvects[4] +
		     eigvects[7]*eigvects[7]);
  eigvects[1] /= dete2;
  eigvects[4] /= dete2;
  eigvects[7] /= dete2;
  
  double dete3 = sqrt(eigvects[2]*eigvects[2] + 
		     eigvects[5]*eigvects[5] + 
		     eigvects[8]*eigvects[8]);
  eigvects[2] /= dete3;
  eigvects[5] /= dete3;
  eigvects[8] /= dete3;
  */
#ifdef POSE_DEBUG
  std::cout << "Normalised Eigen Vectors: " << eigvects[0] << " " << eigvects[1] << " " << eigvects[2] << std::endl;
  std::cout << "                          " << eigvects[3] << " " << eigvects[4] << " " << eigvects[5] << std::endl;
  std::cout << "                          " << eigvects[6] << " " << eigvects[7] << " " << eigvects[8] << std::endl;
#endif


  for(int i=0;i<4;i++) {
    for(int j=1;j<3;j++) {
      if (eigvals[j*4-4] > eigvals[j*4]) {
	// swap the vector
	for(int k=0;k<3;k++) {
	  double t = eigvects[j-1+k*3];
	  eigvects[j-1+k*3] = eigvects[j+k*3];
	  eigvects[j+k*3] = t;	  
	}

	// swap the value
	double t = eigvals[j*4-4];
	eigvals[j*4-4] = eigvals[j*4];
	eigvals[j*4]=t;
      }
    }
  }

#ifdef POSE_DEBUG
  std::cout << "Rotation 1: " << eigvects[0] << " " << eigvects[1] << " " << eigvects[2] << std::endl;
  std::cout << "            " << eigvects[3] << " " << eigvects[4] << " " << eigvects[5] << std::endl;
  std::cout << "            " << eigvects[6] << " " << eigvects[7] << " " << eigvects[8] << std::endl;

  std::cout << "Sorted Eigen Values: " << eigvals[0] << " " << eigvals[1] << " " << eigvals[2] << std::endl;
  std::cout << "                     " << eigvals[3] << " " << eigvals[4] << " " << eigvals[5] << std::endl;
  std::cout << "                     " << eigvals[6] << " " << eigvals[7] << " " << eigvals[8] << std::endl;
#endif
      

  double tcosn = ( eigvals[8] - eigvals[4] );
  double tsinn = ( eigvals[4] - eigvals[0] );
  double denom = ( eigvals[8] - eigvals[0] );
  
  /*
  if (fabs(cc) < 0.0001) {
    cc = 0;
  }
  if (fabs(s) < 0.0001) {
    s = 0;
  }
  */
  double tcosn2 = sqrt(tcosn);
  double tsinn2 = sqrt(tsinn);
  double denom2 = sqrt(denom);

  float r2[] = { tcosn2/denom2, 0, tsinn2/denom2,
		 0   , 1, 0,
		 -tsinn2/denom2,0, tcosn2/denom2};

#ifdef POSE_DEBUG  
  std::cout << "Rotation 2: " << r2[0] << " " << r2[1] << " " << r2[2] << std::endl;
  std::cout << "            " << r2[3] << " " << r2[4] << " " << r2[5] << std::endl;
  std::cout << "            " << r2[6] << " " << r2[7] << " " << r2[8] << std::endl;
#endif

  double rtot[16];
  // multiply r1 (eigvects) and r2
  for(int row=0;row<3;row++) {
    for(int col=0;col<3;col++) {
      rtot[row*4+col] = 0;
      for(int k=0;k<3;k++) {
	rtot[row*4+col] += eigvects[row*3+k] * r2[k*3+col];
      }
    }
  }
  rtot[3] = rtot[7] = rtot[11] = 0;
  rtot[12]=rtot[13]=rtot[14]=0;
  rtot[15] = 1;

  // now incorporate the scale factor 
  double scale = sqrt(tsinn*tcosn/eigvals[4] - ( (eigvals[8]*eigvals[8] - eigvals[0]*eigvals[0])/denom ) + eigvals[4]);
  // this multiplies columns 0 and 1 of the transform by scale
  for(int row=0;row<4;row++) {
    rtot[row*4] *= scale;
    rtot[row*4+1] *= scale;
  }

  // apply the relevant translation
  double tx = tsinn2*tcosn2/eigvals[4];
  double trans[16] = { 1,0,0,tx,
		       0,1,0,0,
		       0,0,1,0,
		       0,0,0,1 };

  for(int row=0;row<4;row++) {
    for(int col=0;col<4;col++) {
      m_transform[row*4+col] = 0;
      for(int k=0;k<4;k++) {
	m_transform[row*4+col] += rtot[row*4+k] * trans[k*4+col];
      }
    }
  }
  
  

#ifdef POSE_DEBUG  
  std::cout << "M_Transform: " << m_transform[0] << " " << m_transform[1] << " " << m_transform[2] << " " << m_transform[3] << std::endl;
  std::cout << "             " << m_transform[4] << " " << m_transform[5] << " " << m_transform[6] << " " << m_transform[7] << std::endl;
  std::cout << "             " << m_transform[8] << " " << m_transform[9] << " " << m_transform[10] << " " << m_transform[11] << std::endl;
  std::cout << "             " << m_transform[12] << " " << m_transform[13] << " " << m_transform[14] << " " << m_transform[15] << std::endl;
#endif

  // store the eigenvalues in the position field - we'll need them to
  // work out the position so this is a good place to keep them
  m_px = eigvals[0];
  m_py = eigvals[4];
  m_pz = eigvals[8];

}

void Ellipse2D::ComputePosition(double radius) {
  float eig1 = m_px;
  float eig2 = m_py;
  float eig3 = m_pz;

  float dist = -eig2*eig2*radius/eig1/eig3;

  //if (fabs(dist) < 0.0001) { dist = 0.0; }
  dist = sqrt(dist);
#ifdef POSE_DEBUG  
  std::cout << "Dist = " << dist << std::endl;
#endif

  float alpha = (eig3-eig2)*(eig2-eig1)*dist*dist / eig2/eig2;
  //  if (fabs(alpha) < 0.0001) { alpha = 0.0; }
  alpha = sqrt(alpha);
#ifdef POSE_DEBUG  
  std::cout << "Alpha = " << alpha << std::endl;
#endif

  m_px = alpha * m_transform[0] + dist * m_transform[2];
  m_py = alpha * m_transform[3] + dist * m_transform[5];
  m_pz = alpha * m_transform[6] + dist * m_transform[8];

  m_nx = m_transform[2];
  m_ny = m_transform[5];
  m_nz = m_transform[8];

#ifdef POSE_DEBUG  
  std::cout << "Pos: (" << m_px << "," << m_py << "," << m_pz << ")" <<std::endl;
  std::cout << "Normal: ("<< m_nx << "," << m_ny << "," << m_nz << ")" <<std::endl;
#endif
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

