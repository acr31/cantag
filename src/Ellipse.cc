/**
 * $Header$
 */

#include <Config.hh>
#include <Ellipse.hh>
#include <gaussianelimination.hh>
#include <eigenvv.hh>
#include <cmath>
#include <iostream>

#ifdef TEXT_DEBUG
# undef ELLIPSE_DEBUG
# undef ELLIPSE_DEBUG_DUMP_POINTS
# undef CIRCLE_TRANSFORM_DEBUG
# undef DECOMPOSE_DEBUG
#endif

#define MAXFITERROR 0.0001
#define COMPARETHRESH 0.0001

static void print(const char* label, double* array, int rows, int cols);
static void print(const char* label, double** array, int rows, int cols);

Ellipse::Ellipse(const float* points, int numpoints) {
  FitEllipse(points,numpoints);
}

Ellipse::Ellipse(const float* points, int numpoints, bool prev_fit) {
  if (!prev_fit) {
    FitEllipse(points,numpoints);
  }
  else {
    m_fitted =false;
  }
}

Ellipse::Ellipse(float a, float b, float c, float d, float e, float f) :
  m_a(a), m_b(b), m_c(c), m_d(d), m_e(e), m_f(f), m_fitted(true) {}

bool Ellipse::Compare(const Ellipse& o) const {
  /**
   *  \todo{this is really naive because the ellipse equation is good
   *  up to a scale factor.  Lets fix it later...}
   */
  return ((fabs(m_a - o.m_a) < COMPARETHRESH) &&
	  (fabs(m_b - o.m_b) < COMPARETHRESH) &&
	  (fabs(m_c - o.m_c) < COMPARETHRESH) &&
	  (fabs(m_d - o.m_d) < COMPARETHRESH) &&
	  (fabs(m_e - o.m_e) < COMPARETHRESH) &&
	  (fabs(m_f - o.m_f) < COMPARETHRESH));
}

void Ellipse::FitEllipse(const float* points, int numpoints) {
  if (numpoints < 6) {
    m_fitted = false;
    return;
  }

#ifdef ELLIPSE_DEBUG_DUMP_POINTS
  for(int i=0;i<numpoints*2;i+=2) {
    PROGRESS(points[i] << " " << points[i+1] <<";");
  }
#endif

  double d1[numpoints*3];
  double d2[numpoints*3];
  
  // Compute

  // D1 = [ x.^2 , x.*y , y.^2 ]
  // D2 = [ x , y , 1]

  int pointer = 0;
  for(int i=0;i<numpoints*2;i+=2) {
    d1[pointer] = points[i]*points[i];
    d2[pointer++] = points[i];

    d1[pointer] = points[i]*points[i+1];
    d2[pointer++] = points[i+1];

    d1[pointer] = points[i+1]*points[i+1];
    d2[pointer++] = 1;
  }

#ifdef ELLIPSE_DEBUG_DUMP_POINTS
  print("D1",d1,numpoints,3);
  print("D2",d2,numpoints,3);
#endif

  double s1[9];
  double s2[9];
  double s30[3];
  double s31[3];
  double s32[3];
  double* s3[] = { s30,s31,s32};  // store s3 in col major format so we can use it for gaussian elimination

  // Compute

  // S1 = D1' * D1
  // S2 = D1' * D2
  // S3 = D2' * D2

  for(int i=0;i<3;i++) { // rows
    for(int j=0;j<3;j++) { // cols
      s1[i*3+j] = d1[i]*d1[j];
      s2[i*3+j] = d1[i]*d2[j];
      s3[j][i] = d2[i]*d2[j];
      for(int k=1;k<numpoints;k++) {
	s1[i*3+j] += d1[i+k*3] * d1[j+k*3];
	s2[i*3+j] += d1[i+k*3] * d2[j+k*3];
	s3[j][i] += d2[i+k*3] * d2[j+k*3];
      }
    }
  }
#ifdef ELLIPSE_DEBUG
  print("S1",s1,3,3);
  print("S2",s2,3,3);
  print("S3",s3,3,3);
#endif
  // Compute

  // T = -inv(S3) * S2'
  
  // this is column major representation => this is -S2 transpose.
  double t0[] = { -s2[0], -s2[1], -s2[2]};
  double t1[] = { -s2[3], -s2[4], -s2[5]};
  double t2[] = { -s2[6], -s2[7], -s2[8]};
  double* t[] = { t0,t1,t2 };
  
  predivide(s3,t,3,3);


#ifdef ELLIPSE_DEBUG
  print("T",t,3,3);
#endif
  // Compute

  // M = S1 + S2 * T

  double m[9];
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      m[i*3+j] = s1[i*3+j];
      for(int k=0;k<3;k++) {
	m[i*3+j] += s2[i*3+k] * t[j][k]; // ith row, kth column of s2 * kth row,jth column of t
      }
    }
  }

#ifdef ELLIPSE_DEBUG
  print("M",m,3,3);
#endif

  double m2[9];
  // premultiply M by inv(C1)
  //
  // ( M31/2 M32/2 M33/2 )
  // ( -M21  -M22  -M23  )
  // ( M11/2 M12/2 M13/2 )
  m2[0] = m[6]/2;
  m2[1] = m[7]/2;
  m2[2] = m[8]/2;

  m2[3] = -m[3];
  m2[4] = -m[4];
  m2[5] = -m[5];
  
  m2[6] = m[0]/2;
  m2[7] = m[1]/2;
  m2[8] = m[2]/2;

#ifdef ELLIPSE_DEBUG
  print("M2",m2,3,3);
#endif

  double eigvals[9];
  double eigvects[9];

  eigensolve(m2[0],m2[1],m2[2],
	     m2[3],m2[4],m2[5],
	     m2[6],m2[7],m2[8],
	     eigvects,
	     eigvals);

#ifdef ELLIPSE_DEBUG
  print("Eigvals",eigvals,3,3);
  print("Eigvects",eigvects,3,3);
#endif

  for(int i=0;i<9;i++) {
    eigvects[i] = -eigvects[i];
  }

  for(int i = 0; i < 3; i++) {
    if (4*eigvects[i]*eigvects[i+6]-eigvects[i+3]*eigvects[i+3] >= 0.0) {
      m_a = eigvects[i];
      m_b = eigvects[i+3];
      m_c = eigvects[i+6];
      m_d = t[0][0]*eigvects[i]+t[1][0]*eigvects[i+3]+t[2][0]*eigvects[i+6];
      m_e = t[0][1]*eigvects[i]+t[1][1]*eigvects[i+3]+t[2][1]*eigvects[i+6];
      m_f = t[0][2]*eigvects[i]+t[1][2]*eigvects[i+3]+t[2][2]*eigvects[i+6];

     
#ifdef ELLIPSE_DEBUG
      PROGRESS("Fitted ellipse: a="<<m_a<<","<<m_b<<","<<m_c<<","<<m_d<<","<<m_e<<","<<m_f);
#endif      

      m_fitted = (GetError(points,numpoints) < MAXFITERROR);
    }
  }
}

float Ellipse::GetError(const float* points, int count) const {
  // calculate the algebraic distance inversly weighted by the
  // gradient
  float total=0;
  for (int pt=0;pt<count*2;pt+=2) {
    float x = points[pt];
    float y = points[pt+1];
    float dist = m_a*x*x+m_b*x*y+m_c*y*y+m_d*x+m_e*y+m_f;
    
    float dx = 2*m_a*x+m_b*y+m_d;
    float dy = m_b*x+2*m_c*y+m_e;

    float norm = dx*dx + dy*dy;
    total+= dist*dist/norm;
  }
#ifdef ELLIPSE_DEBUG
  PROGRESS("Total error was "<< total/count);
#endif
  return total/count;
}

void Ellipse::GetTransform(float transform1[16], float transform2[16]) const {
  float a = GetA();
  float b = GetB();
  float c = GetC();
  float d = GetD();
  float e = GetE();
  float f = GetF();

  // it turns out to be really important to the pose extraction
  // that our conic has a positive sense!
  if (f > 0) {
    a*=-1;
    b*=-1;
    c*=-1;
    d*=-1;
    e*=-1;
    f*=-1;
#ifdef ELLIPSE_DEBUG
    PROGRESS("Corrected for negative scale factor");
#endif
  }
  

#ifdef CIRCLE_TRANSFORM_DEBUG
  PROGRESS("Ellipse params (a-f) are ["<<
	   a << "," <<
	   b << "," <<
	   c << "," <<
	   d << "," <<
	   e << "," <<
	   f << "];");
#endif

  
  double eigvects[9];
  double eigvals[9];

  /**
   * Solve eigenvectors of ( a     b/2    d/2 )
   *                       ( b/2   c      e/2 )
   *                       ( d/2   e/2    f   )
   */
  eigensolve(a,b/2, d/2,
	     c,e/2,
	     f,
	     eigvects, eigvals);

#ifdef CIRCLE_TRANSFORM_DEBUG
  PROGRESS("Eigen Vectors: e=[" << eigvects[0] << "," << eigvects[1] << "," << eigvects[2] << ";");
  PROGRESS("                  " << eigvects[3] << "," << eigvects[4] << "," << eigvects[5] << ";");
  PROGRESS("                  " << eigvects[6] << "," << eigvects[7] << "," << eigvects[8] << "];");

  PROGRESS("Eigen Values: v=[" << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";");
  PROGRESS("                 " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";");
  PROGRESS("                 " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];");
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
  PROGRESS("Cross = " << crossx <<"," << crossy << "," << crossy);
#endif

  double dotcross = crossx*eigvects[2] + crossy*eigvects[5] + crossz*eigvects[8];

#ifdef CIRCLE_TRANSFORM_DEBUG
  PROGRESS("Dotcross = " << dotcross);
#endif

  if (dotcross < 0) {
#ifdef CIRCLE_TRANSFORM_DEBUG
    PROGRESS("Reversing z vector to create right handed axis");
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
  PROGRESS("Rotation 1: r1=[ " << eigvects[0] << "," << eigvects[1] << "," << eigvects[2] << ";");
  PROGRESS("                 " << eigvects[3] << "," << eigvects[4] << "," << eigvects[5] << ";");
  PROGRESS("                 " << eigvects[6] << "," << eigvects[7] << "," << eigvects[8] << "];");

  PROGRESS("Sorted Eigen Values: sev=[ " << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";");
  PROGRESS("                           " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";");
  PROGRESS("                           " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];");
#endif
      

  double denom = ( eigvals[8] - eigvals[0] );
  double cossq = ( eigvals[8] - eigvals[4] ) / denom;
  double sinsq = ( eigvals[4] - eigvals[0] ) / denom;
   
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
  PROGRESS("Rotation 2(1): r2c1=[" << r2c1[0] << "," << r2c1[1] << "," << r2c1[2] << "," << r2c1[3] << ";");
  PROGRESS("                " << r2c1[4] << "," << r2c1[5] << "," << r2c1[6] << "," << r2c1[7] << ";");
  PROGRESS("                " << r2c1[8] << "," << r2c1[9] << "," << r2c1[10] << "," << r2c1[11] << ";");
  PROGRESS("                " << r2c1[12] << "," << r2c1[13] << "," << r2c1[14] << "," << r2c1[15] << "];");

  PROGRESS("Rotation 2(2): r2c2=[" << r2c2[0] << "," << r2c2[1] << "," << r2c2[2] << "," << r2c2[3] << ";");
  PROGRESS("                " << r2c2[4] << "," << r2c2[5] << "," << r2c2[6] << "," << r2c2[7] << ";");
  PROGRESS("                " << r2c2[8] << "," << r2c2[9] << "," << r2c2[10] << "," << r2c2[11] << ";");
  PROGRESS("                " << r2c2[12] << "," << r2c2[13] << "," << r2c2[14] << "," << r2c2[15] << "];");
#endif

  // now build the transformation matrix that goes from the unit circle to the 3d circle
  // this is a translation in x to align on the axis  premultiplied by
  // a scale factor premultiplied by
  // rotation r2 premuliplied by
  // rotation r1
  
  // apply the relevant translation - we have to choose again here based on our choice of theta
  double tx = sqrt((eigvals[4]-eigvals[0])*(eigvals[8]-eigvals[4]))/eigvals[4];
#ifdef CIRCLE_TRANSFORM_DEBUG
  PROGRESS("Translation tx = "<<tx);
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
  PROGRESS("Scale factor " << scale);
#endif
  // this multiplies rows 0 and 1 of the transform by scale
  for(int col=0;col<4;col++) {
    transc1[col*4] *= scale;
    transc1[col*4+1] *= scale;

    transc2[col*4] *= scale;
    transc2[col*4+1] *= -scale;
    transc2[col*4+2] *= 1;
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
  PROGRESS("M_Transform: mt=[" << transform1[0] << "," << transform1[1] << "," << transform1[2] << "," << transform1[3] << ";");
  PROGRESS("                 " << transform1[4] << "," << transform1[5] << "," << transform1[6] << "," << transform1[7] << ";");
  PROGRESS("                 " << transform1[8] << "," << transform1[9] << "," << transform1[10] << "," << transform1[11] << ";");
  PROGRESS("                 " << transform1[12] << "," << transform1[13] << "," << transform1[14] << "," << transform1[15] << ";");

  PROGRESS("M_Transform: mt2=[" << transform2[0] << "," << transform2[1] << "," << transform2[2] << "," << transform2[3] << ";");
  PROGRESS("                 " << transform2[4] << "," << transform2[5] << "," << transform2[6] << "," << transform2[7] << ";");
  PROGRESS("                 " << transform2[8] << "," << transform2[9] << "," << transform2[10] << "," << transform2[11] << ";");
  PROGRESS("                 " << transform2[12] << "," << transform2[13] << "," << transform2[14] << "," << transform2[15] << ";");
#endif
}


static void print(const char* label, double* array, int rows, int cols) {
  std::cout << label << "= [ ";
  for(int i=0;i<rows;i++) {
    for(int j=0;j<cols;j++) {
      std::cout << array[i*cols+j] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << "]" << std::endl;
}

static void print(const char* label, double** array, int rows, int cols) {
  std::cout << "-----------------------------------" << std::endl;
  std::cout << label << "= [ ";
  for(int i=0;i<rows;i++) {
    for(int j=0;j<cols;j++) {
      std::cout << array[j][i] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << "]" << std::endl;
}


LinearEllipse::LinearEllipse(float* points, int numpoints) : Ellipse(points,numpoints) {};
LinearEllipse::LinearEllipse(float* points, int numpoints, bool prev_fit): Ellipse(points,numpoints,prev_fit) {};
LinearEllipse::LinearEllipse(float a, float b, float c, float d, float e,float f): Ellipse(a,b,c,d,e,f) {};

void LinearEllipse::GetTransform(float transform1[16], float transform2[16]) const {
  float a = GetA();
  float b = GetB();
  float c = GetC();
  float d = GetD();
  float e = GetE();
  float f = GetF();
  
#ifdef DECOMPOSE_DEBUG
  PROGRESS("a+c = " << a+c);
  PROGRESS("b = " << b);
#endif
  if (a+c < 0) {
    a*=-1;
    b*=-1;
    c*=-1;
    d*=-1;
    e*=-1;
    f*=-1;
#ifdef DECOMPOSE_DEBUG
    PROGRESS("Corrected for negative scale factor");
#endif
  }

  float disc = b*b - 4*a*c;
#ifdef DECOMPOSE_DEBUG
  if (disc >= 0)
    PROGRESS("Constraint Error: this is not an ellipse");
#endif

  float x0 = (2*c*d - b*e) / disc;
  float y0 = (2*a*e - b*d) / disc;

#ifdef DECOMPOSE_DEBUG
  PROGRESS("X= " << x0);
  PROGRESS("Y= " << y0);
#endif
  
  float tmproot = sqrt( (a-c)*(a-c) + b*b );
  float lambda1 = (a+c + tmproot)/2;
  float lambda2 = (a+c - tmproot)/2;
  float lambda1t = lambda1;
  lambda1 = 1/sqrt(lambda1);
  lambda2 = 1/sqrt(lambda2);

#ifdef DECOMPOSE_DEBUG
  PROGRESS("tmproot= " << tmproot);
  PROGRESS("lambda1= " << lambda1);
  PROGRESS("lambda2= " << lambda2);
#endif
  
  float scale_factor = sqrt( -f + a*x0*x0 + b*x0*y0 + c*y0*y0);

#ifdef DECOMPOSE_DEBUG
  PROGRESS("scale= " << scale_factor);
#endif

  float width = lambda1 * scale_factor;
  float height = lambda2 * scale_factor;

#ifdef DECOMPOSE_DEBUG
  PROGRESS("width= " << width);
  PROGRESS("height= " <<height);
#endif

  float angle_radians = atan( -(a-lambda1t)/(0.5*b) );

#ifdef DECOMPOSE_DEBUG
  PROGRESS("angle= " << angle_radians);
#endif
  /*
  // copied and pasted from tngtrip - I have no idea why my method fails or what this method does ;-)
  f -= a*x0*x0 + b*x0*y0+ c*y0*y0 ;
  float lump_1 = a + c;
  float lump_2 = (float)sqrt(a*a + b*b + c*c - 2*a*c);

  width = (float)sqrt((float)-2.0*f/(lump_1 + lump_2));
  height = -(float)sqrt((float)-2.0*f/(lump_1 - lump_2));

  angle_radians = M_PI/2 -(float)0.5*(float)atan(2*b/(c-a));
  //  angle_radians = (float)0.5*(float)atan(2*b/(c-a));
  */
  transform1[0] = width*cos(angle_radians);
  transform1[1] = -height*sin(angle_radians); 
  transform1[2] = 0;
  transform1[3] = x0;
  transform1[4] = width*sin(angle_radians);
  transform1[5] = height*cos(angle_radians); 
  transform1[6] = 0;
  transform1[7] = y0;
  transform1[8] = 0;
  transform1[9] = 0;
  transform1[10] = 1;
  transform1[11] = 0;
  transform1[12] = 0;
  transform1[13] = 0;
  transform1[14] = 0;
  transform1[15] = 1;

  for(int i=0;i<16;i++) {
    transform2[i] = transform1[i];
  }
  
}

