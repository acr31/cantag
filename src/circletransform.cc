/**
 * $Header$
 */

#include <circletransform.hh>
#include <eigenvv.hh>

#undef POSE_DEBUG

void GetTransform(const Ellipse& ellipse, float transform1[16], float transform2[16]) {
#ifdef POSE_DEBUG
  std::cout << "Ellipse params (a-f) are ["<<
    ellipse.GetA() << "," <<
    ellipse.GetB() << "," <<
    ellipse.GetC() << "," <<
    ellipse.GetD() << "," <<
    ellipse.GetE() << "," <<
    ellipse.GetF() << "];" << std::endl;
#endif

  double eigvects[9];
  double eigvals[9];

  /**
   * Solve eigenvectors of ( m_a     m_2/b    m_d/2 )
   *                       ( m_b/2   m_c      m_e/2 )
   *                       ( m_d/2   m_e/2    m_f   )
   */
  eigensolve(ellipse.GetA(),ellipse.GetB()/2, ellipse.GetD()/2,
	     /*          */ ellipse.GetC()  , ellipse.GetE()/2,
	     /*                           */  ellipse.GetF(),
	     eigvects, eigvals);

#ifdef POSE_DEBUG
  std::cout << "Eigen Vectors: e=[" << eigvects[0] << "," << eigvects[1] << "," << eigvects[2] << ";" << std::endl;
  std::cout << "                  " << eigvects[3] << "," << eigvects[4] << "," << eigvects[5] << ";" << std::endl;
  std::cout << "                  " << eigvects[6] << "," << eigvects[7] << "," << eigvects[8] << "];" << std::endl;

  std::cout << "Eigen Values: v=[" << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";" << std::endl;
  std::cout << "                 " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";" << std::endl;
  std::cout << "                 " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];" << std::endl;
#endif

  // bubble sort the vectors...I'm so embarresed ;-)
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

  // our eigenvectors might incorporate reflections about various axes
  // so we need to check that we still have a right handed frame in a
  // righthanded frame v1 x v2 = v3 so we cross v1 and v2 and check
  // the sign compared with v3 if they are different we multiply v3 by
  // -1 so this is v1 cross v2 dot v3 - if this is +ve v3 is parallel
  // with where it should be for a right handed axis if not then we
  // scale it

  // cross product of u and v 
  /*   ( ux )    ( vx )     ( uy*vz - uz*vy )
   *   ( uy ) x  ( vy )  =  ( uz*vx - ux*vz )
   *   ( uz )    ( vz )     ( ux*vy - uy*vx )
   *
   * now dot product with w
   *
   *  ( uy*vz - uz*vy )   ( wx )   
   *  ( uz*vx - ux*vz ) . ( wy ) = (uy*vz - uz*vy)*wx + (uz*vx - ux*vz)*wy + (ux*vy - uy*vx)*wz
   *  ( ux*vy - uy*vx )   ( wz )
   * 
   */
  double crossx = eigvects[3]*eigvects[7] - eigvects[6]*eigvects[4];
  double crossy = eigvects[6]*eigvects[1] - eigvects[0]*eigvects[7];
  double crossz = eigvects[0]*eigvects[4] - eigvects[3]*eigvects[1];
  
#ifdef POSE_DEBUG
  std::cout << "Cross = " << crossx <<"," << crossy << "," << crossy << std::endl;
#endif

  double dotcross = crossx*eigvects[2] + crossy*eigvects[5] + crossz*eigvects[8];

#ifdef POSE_DEBUG
  std::cout << "Dotcross = " << dotcross << std::endl;
#endif

  if (dotcross < 0) {
#ifdef POSE_DEBUG
    std::cout << "Reversing z vector" << std::endl;
#endif
    eigvects[2] *= -1;
    eigvects[5] *= -1;
    eigvects[8] *= -1;
  }
  
  double r1[] = { eigvects[0], eigvects[1], eigvects[2], 0,
		  eigvects[3], eigvects[4], eigvects[5], 0,
		  eigvects[6], eigvects[7], eigvects[8], 0,
		  0,           0,           0,           1};

#ifdef POSE_DEBUG
  std::cout << "Rotation 1: r1=[ " << eigvects[0] << "," << eigvects[1] << "," << eigvects[2] << ";" << std::endl;
  std::cout << "                 " << eigvects[3] << "," << eigvects[4] << "," << eigvects[5] << ";" << std::endl;
  std::cout << "                 " << eigvects[6] << "," << eigvects[7] << "," << eigvects[8] << "];" << std::endl;

  std::cout << "Sorted Eigen Values: sev=[ " << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";" << std::endl;
  std::cout << "                           " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";" << std::endl;
  std::cout << "                           " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];" << std::endl;
#endif
      

  double denom = ( eigvals[8] - eigvals[0] );
  double cossq = ( eigvals[8] - eigvals[4] ) / denom;
  double sinsq = ( eigvals[4] - eigvals[0] ) / denom;
  
  /*
  if (fabs(cc) < 0.0001) {
    cc = 0;
  }
  if (fabs(s) < 0.0001) {
    s = 0;
  }
  */

  double pmcos = sqrt(cossq);
  double pmsin = sqrt(sinsq);

  // here is our first ambiguity choice point.  We need to decide plus or minus theta
  float r2c1[] = { pmcos, 0, pmsin, 0,
		   0,     1, 0,     0, 
		   -pmsin, 0, pmcos, 0,
		   0,     0, 0,     1 };
  
  float r2c2[] = { pmcos, 0, -pmsin, 0,
		   0,     1, 0,     0, 
		   pmsin, 0, pmcos, 0,
		   0,     0, 0,     1 };

#ifdef POSE_DEBUG  
  std::cout << "Rotation 2(1): r2c1=[" << r2c1[0] << "," << r2c1[1] << "," << r2c1[2] << "," << r2c1[3] << ";" << std::endl;
  std::cout << "                " << r2c1[4] << "," << r2c1[5] << "," << r2c1[6] << "," << r2c1[7] << ";" << std::endl;
  std::cout << "                " << r2c1[8] << "," << r2c1[9] << "," << r2c1[10] << "," << r2c1[11] << ";" << std::endl;
  std::cout << "                " << r2c1[12] << "," << r2c1[13] << "," << r2c1[14] << "," << r2c1[15] << "];" << std::endl;

  std::cout << "Rotation 2(2): r2c2=[" << r2c2[0] << "," << r2c2[1] << "," << r2c2[2] << "," << r2c2[3] << ";" << std::endl;
  std::cout << "                " << r2c2[4] << "," << r2c2[5] << "," << r2c2[6] << "," << r2c2[7] << ";" << std::endl;
  std::cout << "                " << r2c2[8] << "," << r2c2[9] << "," << r2c2[10] << "," << r2c2[11] << ";" << std::endl;
  std::cout << "                " << r2c2[12] << "," << r2c2[13] << "," << r2c2[14] << "," << r2c2[15] << "];" << std::endl;
#endif

  // now build the transformation matrix that goes from the unit circle to the 3d circle
  // this is a translation in x to align on the axis  premultiplied by
  // a scale factor premultiplied by
  // rotation r2 premuliplied by
  // rotation r1
  
  // apply the relevant translation - we have to choose again here based on our choice of theta
  double tx = sqrt((eigvals[4]-eigvals[0])*(eigvals[8]-eigvals[4]))/eigvals[4];
#ifdef POSE_DEBUG
  std::cout << "Translation tx = "<<tx <<std::endl;
#endif
  double transc1[] = {1,0,0,tx,
		      0,1,0,0,
		      0,0,1,0,
		      0,0,0,1};
  
  double transc2[] = {1,0,0,-tx,
		      0,1,0,0,
		      0,0,1,0,
		      0,0,0,1};

  // and another choice based on theta
  double scale = sqrt(-eigvals[0]*eigvals[8]/eigvals[4]/eigvals[4]);
#ifdef POSE_DEBUG
  std::cout << "Scale factor " << scale << std::endl;
#endif
  // this multiplies rows 0 and 1 of the transform by scale
  for(int col=0;col<4;col++) {
    transc1[col*4] *= scale;
    transc1[col*4+1] *= scale;

    transc2[col*4] *= -scale;
    transc2[col*4+1] *= -scale;
  }
    
  double rtotc1[16];
  double rtotc2[16];
  // premultiply by r2
  for(int row=0;row<4;row++) {
    for(int col=0;col<4;col++) {
      rtotc1[row*4+col] = 0;
      rtotc2[row*4+col] = 0;
      for(int k=0;k<4;k++) {
	rtotc1[row*4+col] += r2c1[row*4+k] * transc1[k*4+col];
	rtotc2[row*4+col] += r2c2[row*4+k] * transc2[k*4+col];
      }
    }
  }

  // premultiply by r1
  for(int row=0;row<4;row++) {
    for(int col=0;col<4;col++) {
      transform1[row*4+col] = 0;
      transform2[row*4+col] = 0;
      for(int k=0;k<4;k++) {
	transform1[row*4+col] += r1[row*4+k] * rtotc1[k*4+col];
	transform2[row*4+col] += r1[row*4+k] * rtotc2[k*4+col];
      }
    }
  }
  
#ifdef POSE_DEBUG  
  std::cout << "M_Transform: mt=[" << transform1[0] << "," << transform1[1] << "," << transform1[2] << "," << transform1[3] << ";" << std::endl;
  std::cout << "                 " << transform1[4] << "," << transform1[5] << "," << transform1[6] << "," << transform1[7] << ";" << std::endl;
  std::cout << "                 " << transform1[8] << "," << transform1[9] << "," << transform1[10] << "," << transform1[11] << ";" << std::endl;
  std::cout << "                 " << transform1[12] << "," << transform1[13] << "," << transform1[14] << "," << transform1[15] << ";" << std::endl;

  std::cout << "M_Transform: mt2=[" << transform2[0] << "," << transform2[1] << "," << transform2[2] << "," << transform2[3] << ";" << std::endl;
  std::cout << "                 " << transform2[4] << "," << transform2[5] << "," << transform2[6] << "," << transform2[7] << ";" << std::endl;
  std::cout << "                 " << transform2[8] << "," << transform2[9] << "," << transform2[10] << "," << transform2[11] << ";" << std::endl;
  std::cout << "                 " << transform2[12] << "," << transform2[13] << "," << transform2[14] << "," << transform2[15] << ";" << std::endl;
#endif
}


void ApplyTransform(const float transform[16], float x, float y, float* projX, float* projY) {
  *projX = transform[0]*x + transform[1]*y + transform[2] + transform[3];
  *projY = transform[4]*x + transform[5]*y + transform[6] + transform[7];
  float projZ = transform[8]*x + transform[9]*y + transform[10] + transform[11];
  float projH = transform[12]*x + transform[13]*y + transform[14] + transform[15];

#ifdef POSE_DEBUG
  PROGRESS("Transformed ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<","<<projZ<<","<<projH<<")");
#endif
  
  *projX /= projH;
  *projY /= projH;
  projZ /= projH;

  *projX /= projZ;
  *projY /= projZ;

#ifdef POSE_DEBUG
  PROGRESS("Projected ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<")");
#endif

}

void ApplyTransform(const float transform[16], float* points, int numpoints) {
  for(int i=0;i<numpoints*2;i+=2) {
    ApplyTransform(transform,points[i],points[i+1],points+i,points+i+1);
  }
}
