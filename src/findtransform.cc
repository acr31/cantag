/**
 * $Header$
 */

#include <findtransform.hh>
#include <eigenvv.hh>
#include <gaussianelimination.hh>

#undef CIRCLE_TRANSFORM_DEBUG
#define SQUARE_TRANSFORM_DEBUG
#undef APPLY_TRANSFORM_DEBUG

void GetTransform(const QuadTangle& quad, float transform[16]) {
  // see the header file for a full explanation of what's going on here
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Calculating transform for :"
	   "("<<quad.GetX0()<<","<<quad.GetY0()<<"),"<<
	   "("<<quad.GetX1()<<","<<quad.GetY1()<<"),"<<
	   "("<<quad.GetX2()<<","<<quad.GetY2()<<"),"<<
	   "("<<quad.GetX3()<<","<<quad.GetY3()<<"),");	   
#endif

  // we particularly want coeffs to be an array of pointers to arrays
  // containing the columns of the matrix - then we can swap columns
  // conveniently by swapping pointers
  double coeffs0[] = {1,1,0,0,0,0,0,0};
  double coeffs1[] = {1,0,0,1,0,0,0,0};
  double coeffs2[] = {1,1,1,1,0,0,0,0};
  double coeffs3[] = {0,0,0,0,1,1,0,0};
  double coeffs4[] = {0,0,0,0,1,0,0,1};
  double coeffs5[] = {0,0,0,0,1,1,1,1};
  double coeffs6[] = {-quad.GetX1(),-quad.GetX0(),0,0,-quad.GetY1(),-quad.GetY0(),0,0};
  double coeffs7[] = {-quad.GetX1(),0,0,-quad.GetX2(),-quad.GetY1(),0,0,-quad.GetY2()};
  double* coeffs[] = {coeffs0,
		      coeffs1,
		      coeffs2,
		      coeffs3,
		      coeffs4,
		      coeffs5,
		      coeffs6,
		      coeffs7};
		     
  double xvals[] = { quad.GetX1(),
		     quad.GetX0(),
		     quad.GetX3(),
		     quad.GetX2(),
		     quad.GetY1(),
		     quad.GetY0(),
		     quad.GetY3(),
		     quad.GetY2() };
  double result[8];

  solve_simultaneous(xvals,coeffs,result,8);

#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Computed a0 "<<result[0]);
  PROGRESS("         a1 "<<result[1]);
  PROGRESS("         a2 "<<result[2]);
  PROGRESS("         a3 "<<result[3]);
  PROGRESS("         a4 "<<result[4]);
  PROGRESS("         a5 "<<result[5]);
  PROGRESS("         a6 "<<result[6]);
  PROGRESS("         a7 "<<result[7]);
#endif

  double scalefactor = result[1]*result[1]+result[4]*result[4]+result[7]*result[7];
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Scale factor is "<<scalefactor);
#endif

  for(int i=0;i<8;i++) {
    result[8] += scalefactor;
  }
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Scaled   a0 "<<result[0]);
  PROGRESS("         a1 "<<result[1]);
  PROGRESS("         a2 "<<result[2]);
  PROGRESS("         a3 "<<result[3]);
  PROGRESS("         a4 "<<result[4]);
  PROGRESS("         a5 "<<result[5]);
  PROGRESS("         a6 "<<result[6]);
  PROGRESS("         a7 "<<result[7]);
#endif
 
  transform[0] = result[0];  transform[1] = result[1];  transform[2] = 0;  transform[3] = result[2];
  transform[4] = result[3];  transform[5] = result[4];  transform[6] = 0;  transform[7] = result[5];
  transform[8] = result[6];  transform[9] = result[7];  transform[10]= 0;  transform[11]= result[8];
  transform[12]= 0;          transform[13]= 0;          transform[14]= 0;  transform[15]= 1;

#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";");
  PROGRESS("             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";");
  PROGRESS("             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";");
  PROGRESS("             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];");
#endif

}

void GetTransform(const Ellipse& ellipse, float transform1[16], float transform2[16]) {
#ifdef CIRCLE_TRANSFORM_DEBUG
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

#ifdef CIRCLE_TRANSFORM_DEBUG
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
  
#ifdef CIRCLE_TRANSFORM_DEBUG
  std::cout << "Cross = " << crossx <<"," << crossy << "," << crossy << std::endl;
#endif

  double dotcross = crossx*eigvects[2] + crossy*eigvects[5] + crossz*eigvects[8];

#ifdef CIRCLE_TRANSFORM_DEBUG
  std::cout << "Dotcross = " << dotcross << std::endl;
#endif

  if (dotcross < 0) {
#ifdef CIRCLE_TRANSFORM_DEBUG
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

#ifdef CIRCLE_TRANSFORM_DEBUG
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

#ifdef CIRCLE_TRANSFORM_DEBUG  
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
#ifdef CIRCLE_TRANSFORM_DEBUG
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
#ifdef CIRCLE_TRANSFORM_DEBUG
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
  
#ifdef CIRCLE_TRANSFORM_DEBUG  
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

#ifdef APPLY_TRANSFORM_DEBUG
  PROGRESS("Transformed ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<","<<projZ<<","<<projH<<")");
#endif
  
  *projX /= projH;
  *projY /= projH;
  projZ /= projH;

  *projX /= projZ;
  *projY /= projZ;

#ifdef APPLY_TRANSFORM_DEBUG
  PROGRESS("Projected ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<")");
#endif

}

void ApplyTransform(const float transform[16], float* points, int numpoints) {
  for(int i=0;i<numpoints*2;i+=2) {
    ApplyTransform(transform,points[i],points[i+1],points+i,points+i+1);
  }
}
