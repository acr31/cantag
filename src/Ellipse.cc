/**
 * $Header$
 */

#include <Config.hh>
#include <Ellipse.hh>
#include <gaussianelimination.hh>
#include <eigenvv.hh>
#include <cmath>
#include <iostream>
#include <findtransform.hh>

#ifdef TEXT_DEBUG
# define ELLIPSE_DEBUG
# undef ELLIPSE_DEBUG_DUMP_POINTS
# define CIRCLE_TRANSFORM_DEBUG
# undef DECOMPOSE_DEBUG
#endif

#define MAXFITERROR 10
#define COMPARETHRESH 0.0001
#define MAXDISTANCE

static void print(const char* label, double* array, int rows, int cols);
static void print(const char* label, double** array, int rows, int cols);

Ellipse::Ellipse(): m_fitted(false) {}

Ellipse::Ellipse(const float* points, int numpoints) {
  m_fitted = FitEllipse(points,numpoints);
  if (m_fitted) { Decompose(); }
}

Ellipse::Ellipse(const float* points, int numpoints, bool prev_fit) {
  if (!prev_fit) {
    m_fitted = FitEllipse(points,numpoints);
    if (m_fitted) { Decompose(); }
  }
  else {
    m_fitted =false;
  }
}

Ellipse::Ellipse(float a, float b, float c, float d, float e, float f) :
  m_a(a), m_b(b), m_c(c), m_d(d), m_e(e), m_f(f), m_fitted(true) { Decompose();}

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

bool Ellipse::FitEllipse(const float* points, int numpoints) {
  if (numpoints < 6) {
    return false;
  }

#ifdef ELLIPSE_DEBUG_DUMP_POINTS
  for(int i=0;i<numpoints*2;i+=2) {
    PROGRESS(points[i] << " " << points[i+1] <<";");
  }
#endif

  
  // Compute

  // S1 = D1' * D1
  // S2 = D1' * D2
  // S3 = D2' * D2

  // D1 is [ x.^2 , x.*y, y.^2 ]
  // d1(i,0) = points[2*i]*points[2*i]
  // d1(i,1) = points[2*i]*points[2*i+1]
  // d1(i,2) = points[2*i+1]*points[2*i+1]

  // D2 is [x,y,1]
  // d2(i,0) = points[2*i]
  // d2(i,1) = points[2*i+1]
  // d2(i,2) = 1

  double s1[9] = {0};
  double s2[9] = {0};
  double s30[3] = {0};
  double s31[3] = {0};
  double s32[3] = {0};
  double* s3[] = { s30,s31,s32};  // store s3 in row major format so we can use it for gaussian elimination

  for(int j=0;j<numpoints*2;j+=2) {
    double d1col[] = {points[j] * points[j],
		      points[j] * points[j+1],
		      points[j+1] * points[j+1]};
    double d2col[] = {points[j],
		      points[j+1],
		      1};
    
    for(int k=0;k<3;k++) {
      for(int l=0;l<3;l++) {
	s1[k*3+l] += d1col[k] * d1col[l];
	s2[k*3+l] += d1col[k] * d2col[l];
	s3[k][l] += d2col[k] * d2col[l];
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
  
  // this is row major representation => this is -S2 transpose.
  double t0[] = { -s2[0], -s2[3], -s2[6]};
  double t1[] = { -s2[1], -s2[4], -s2[7]};
  double t2[] = { -s2[2], -s2[5], -s2[8]};
  double* t[] = { t0,t1,t2 };
  
  predivide(s3,t,3,3);


#ifdef ELLIPSE_DEBUG
  print("T",t,3,3);
#endif
  // Compute

  // M = S1 + S2 * T
  // leave the result in S1

  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      for(int k=0;k<3;k++) {
	s1[i*3+j] += s2[i*3+k] * t[k][j]; // ith row, kth column of s2 * kth row,jth column of t
      }
    }
  }

#ifdef ELLIPSE_DEBUG
  print("M",s1,3,3);
#endif

  // premultiply M by inv(C1)
  //
  // ( M31/2 M32/2 M33/2 )
  // ( -M21  -M22  -M23  )
  // ( M11/2 M12/2 M13/2 )
  // and find eigenvectors

#ifdef ELLIPSE_DEBUG
  double m2[9];
  m2[0] = s1[6]/2;
  m2[1] = s1[7]/2;
  m2[2] = s1[8]/2;

  m2[3] = -s1[3];
  m2[4] = -s1[4];
  m2[5] = -s1[5];
  
  m2[6] = s1[0]/2;
  m2[7] = s1[1]/2;
  m2[8] = s1[2]/2;
  print("M2",m2,3,3);
#endif

  double eigvals[9];
  double eigvects[9];
  if (!eigensolve(s1[6]/2,s1[7]/2,s1[8]/2,
		  -s1[3],-s1[4],-s1[5],
		  s1[0]/2,s1[1]/2,s1[2]/2,
		  eigvects,
		  eigvals)) {
#ifdef ELLIPSE_DEBUG
    PROGRESS("Failed to solve eigenvectors of the non-symmetric matrix.  Fit failed.");
#endif
    return false;
  }


#ifdef ELLIPSE_DEBUG
  print("Eigvals",eigvals,3,3);
  print("Eigvects",eigvects,3,3);
#endif

  //  for(int i=0;i<9;i++) {
  //  eigvects[i] = -eigvects[i];
  // }

  for(int i = 0; i < 3; i++) {
    if (4*eigvects[i]*eigvects[i+6]-eigvects[i+3]*eigvects[i+3] >= 0.0) {
      m_a = eigvects[i];
      m_b = eigvects[i+3];
      m_c = eigvects[i+6];
      m_d = t[0][0]*eigvects[i]+t[0][1]*eigvects[i+3]+t[0][2]*eigvects[i+6];
      m_e = t[1][0]*eigvects[i]+t[1][1]*eigvects[i+3]+t[1][2]*eigvects[i+6];
      m_f = t[2][0]*eigvects[i]+t[2][1]*eigvects[i+3]+t[2][2]*eigvects[i+6];

     
#ifdef ELLIPSE_DEBUG
      PROGRESS("Fitted ellipse: a="<<m_a<<","<<m_b<<","<<m_c<<","<<m_d<<","<<m_e<<","<<m_f);
      PROGRESS("MAXFITERROR is " << MAXFITERROR);
#endif      

      return (GetErrorGradient(points,numpoints) < MAXFITERROR);
    }
  }
  return false;
}

float Ellipse::GetError(const float* points, int count) const {
  return GetErrorGradient(points,count);
}

float Ellipse::GetErrorAlgebraic(const float* points, int count) const {
  // calculate the algebraic distance
  float total=0;
  float maxdist = 0;
  for (int pt=0;pt<count*2;pt+=2) {
    float x = points[pt];
    float y = points[pt+1];
    float dist = fabs(m_a*x*x+m_b*x*y+m_c*y*y+m_d*x+m_e*y+m_f);
    if (dist > maxdist) { maxdist = dist; }
    total+= dist;
  }
#ifdef ELLIPSE_DEBUG
  PROGRESS("Total error from Algebraic method is "<< total/count << " maximimum distance was " << maxdist);
#endif

#ifdef MAXDISTANCE
  return maxdist;
#else
  return total/count;
#endif
}

float Ellipse::GetErrorGradient(const float* points, int count) const {
  // calculate the algebraic distance inversly weighted by the
  // gradient
  float total=0;
  float maxdist = 0;
  for (int pt=0;pt<count*2;pt+=2) {
    float x = points[pt];
    float y = points[pt+1];
    float dist = fabs(m_a*x*x+m_b*x*y+m_c*y*y+m_d*x+m_e*y+m_f);
    
    float dx = 2*m_a*x+m_b*y+m_d;
    float dy = m_b*x+2*m_c*y+m_e;

    float norm = dx*dx + dy*dy;

    dist /= sqrt(norm);

    if (dist > maxdist) { maxdist = dist; }
    total+= dist;
  }
#ifdef ELLIPSE_DEBUG
  PROGRESS("Total error from Gradient method is "<< total/count << " maximimum distance was " << maxdist);
#endif

#ifdef MAXDISTANCE
  return maxdist;
#else
  return total/count;
#endif
}

float Ellipse::GetErrorNakagawa(const float* points, int count) const {
  
  float total=0;
  float max_dist =0 ;
  for(int pt=0;pt<count*2;pt+=2) {
    float xi = points[pt];
    float yi = points[pt+1];

    float a = GetWidth();
    float b = GetHeight();
    float theta  = GetAngle();
    float x0 = GetX0();
    float y0 = GetY0();
    float k = (y0-yi)/(x0-xi);

    float sint = sin(theta);
    float cost = cos(theta);

    float rtsub1 = -sint+k*cost;
    float rtsub2 = cost+k*sint;
    float rt = sqrt(a*a*rtsub1*rtsub1 + b*b*rtsub2*rtsub2);
    
    // compute ix
    float ix;
    if (xi > x0) {
      ix = x0 + a*b/rt;
    }
    else {
      ix = x0 - a*b/rt;
    }
    
    // compute iy
    float iy;
    if ((yi >= y0 && k > 0) ||
	(yi < y0 && k < 0)) {
      iy = y0 + a*b*k/rt;
    }
    else {
      iy = y0 - a*b*k/rt;
    }
    
    float d = fabs((ix-xi)*(ix-xi) + (iy-yi)*(iy-yi));
    if (d > max_dist) { max_dist = d; }
    total+=d;
  }
  
#ifdef ELLIPSE_DEBUG
  PROGRESS("Total error from Nakagawa Method is "<< total/count<< " maximimum distance was " << max_dist);
#endif

#ifdef MAXDISTANCE
  return max_dist;
#else
  return total/count;
#endif
}

float Ellipse::GetErrorSafaeeRad(const float* points, int count) const {
  // draw a ray between each point and the centre of the ellipse C
  // intersecting the ellipse at Ij.  The lengths of the bisected
  // portions of the ray mj and nj are determined

  float total=0;
  float max_dist = 0;
  for(int pt=0;pt<count*2;pt+=2) {
    float xi = points[pt];
    float yi = points[pt+1];

    float a = GetWidth();
    float b = GetHeight();
    float theta  = GetAngle();
    float x0 = GetX0();
    float y0 = GetY0();
    float k = (y0-yi)/(x0-xi);

    float sint = sin(theta);
    float cost = cos(theta);


    float rtsub1 = -sint+k*cost;
    float rtsub2 = cost+k*sint;
    float rt = sqrt(a*a*rtsub1*rtsub1 + b*b*rtsub2*rtsub2);

    // compute ix
    float ix;
    if (xi > x0) {
      ix = x0 + a*b/rt;
    }
    else {
      ix = x0 - a*b/rt;
    }

    // compute iy
    float iy;
    if ((yi >= y0 && k > 0) ||
	(yi < y0 && k < 0)) {
      iy = y0 + a*b*k/rt;
    }
    else {
      iy = y0 - a*b*k/rt;
    }

    float m = sqrt((x0-ix)*(x0-ix)+(y0-iy)*(y0-iy));
    float n = sqrt((ix-xi)*(ix-xi)+(iy-yi)*(iy-yi));
    float q = m_a*xi*xi+m_b*xi*yi+m_c*yi*yi+m_d*xi+m_e*yi+m_f;

    float dist =  fabs(m*(1+ n/2/a)/(1+ n/2/m)*q);
    if (dist > max_dist) { max_dist = dist; }
    total += dist;
  }

#ifdef ELLIPSE_DEBUG
  PROGRESS("Total error from SafaeeRad Method is "<< total/count<< " maximimum distance was " << max_dist);
#endif

#ifdef MAXDISTANCE
  return max_dist;
#else
  return total/count;
#endif
}

float Ellipse::GetErrorSafaeeRad2(const float* points, int count) const {
  float total=0;
  float max_dist = 0;
  for(int pt=0;pt<count*2;pt+=2) {
    float xi = points[pt];
    float yi = points[pt+1];

    float a = GetWidth();
    float b = GetHeight();
    float theta  = GetAngle();
    float x0 = GetX0();
    float y0 = GetY0();
    float k = (y0-yi)/(x0-xi);

    float sint = sin(theta);
    float cost = cos(theta);


    float rtsub1 = -sint+k*cost;
    float rtsub2 = cost+k*sint;
    float rt = sqrt(a*a*rtsub1*rtsub1 + b*b*rtsub2*rtsub2);

    // compute ix
    float ix;
    if (xi > x0) {
      ix = x0 + a*b/rt;
    }
    else {
      ix = x0 - a*b/rt;
    }

    // compute iy
    float iy;
    if ((yi >= y0 && k > 0) ||
	(yi < y0 && k < 0)) {
      iy = y0 + a*b*k/rt;
    }
    else {
      iy = y0 - a*b*k/rt;
    }

    float m = sqrt((x0-ix)*(x0-ix)+(y0-iy)*(y0-iy));
    float q = m_a*xi*xi+m_b*xi*yi+m_c*yi*yi+m_d*xi+m_e*yi+m_f;

    float dist = fabs(m*q);

    if (dist > max_dist) { max_dist = dist; }
    total += dist;
  }

#ifdef ELLIPSE_DEBUG
  PROGRESS("Total error from SafaeeRad2 Method is "<< total/count<< " maximimum distance was " << max_dist);
#endif
#ifdef MAXDISTANCE
  return max_dist;
#else
  return total/count;
#endif
}

float Ellipse::GetErrorStricker(const float* points, int count) const {
  float a = GetWidth();
  float b = GetHeight();
  float x0 = GetX0();
  float y0 = GetY0();
  float theta = GetAngle();

  // we need a^2-b^2 to be >0 so check that
  if (a < b) {
    float swap = a;
    a=b;
    b=swap;
    
    theta=M_PI/2-theta;
  }

  float c = sqrt(a*a-b*b);

  float f1x = x0 + c*cos(theta);
  float f1y = y0 + c*sin(theta);

  float f2x = x0 - c*cos(theta);
  float f2y = x0 - c*sin(theta);

  float modf1_f2 = sqrt( (f1x-f2x)*(f1x-f2x) + (f1y-f2y)*(f1y-f2y) );

  float total =0;
  float max_dist =0 ;
  for(int pt=0;pt<count*2;pt+=2) {
    float x = points[pt];
    float y = points[pt+1];

    float aest = 0.5 * (sqrt( (x-f1x)*(x-f1x) + (y-f1y)*(y-f1y) ) + sqrt( (x-f2x)*(x-f2x) + (y-f2y)*(y-f2y)));
    float best = sqrt(aest*aest - a*a + b*b);

    float dest = 0.5*(aest - a + best - b);

    float ctilde = sqrt( (a+dest)*(a+dest) - (b+dest)*(b+dest) );

    float f1xtilde = (f1x + f2x)/2+ctilde*(f1x-f2x)/modf1_f2;
    float f1ytilde = (f1y + f2y)/2+ctilde*(f1y-f2y)/modf1_f2;

    float f2xtilde = (f1x + f2x)/2-ctilde*(f1x-f2x)/modf1_f2;
    float f2ytilde = (f1y + f2y)/2-ctilde*(f1y-f2y)/modf1_f2;

    float atilde = 0.5*(sqrt( (x-f1x)*(x-f1x) + (y-f1y)*(y-f1y) ) + sqrt( (x-f2x)*(x-f2x) + (y-f2y)*(y-f2y) ));

    float dist = fabs(atilde - a);
    
    if (dist > max_dist) { max_dist = dist; }
    total += dist;
  }

#ifdef ELLIPSE_DEBUG
  PROGRESS("Total error from Stricker Method is "<< total/count<< " maximimum distance was " << max_dist);
#endif
#ifdef MAXDISTANCE
  return max_dist;
#else
  return total/count;
#endif
}

void Ellipse::GetTransform(float transform1[16], float transform2[16]) {
  float a = GetA();
  float b = GetB();
  float c = GetC();
  float d = GetD();
  float e = GetE();
  float f = GetF();
  
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

  // if more than one of the eigenvalues is less than one then
  // multiply them all by -1.  We can do this because the ellipse
  // equation is only defined up to a scale factor.  We do this
  // because the y axis eigenvalue must have larger magnitude than the
  // x axis value in order for our next rotation to make sense.
  int count = 0;
  for(int i=0;i<3;i++) {
    if (eigvals[4*i] < 0) { ++count; }
  }
  if (count > 1) {
#ifdef CIRCLE_TRANSFORM_DEBUG
    PROGRESS("Two of the eigenvalues are less than zero - reversing ellipse equation");    
    PROGRESS("Eigen Values: v=[" << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";");
    PROGRESS("                 " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";");
    PROGRESS("                 " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];");
#endif
    for(int i=0;i<3;i++) {
      eigvals[4*i]*=-1;
    }
  }



  /*

  if (fabs(eigvals[0]) > fabs(eigvals[4])) {
    for(int i=0;i<3;i++) {
      eigvals[4*i]*=-1;
    }
#ifdef CIRCLE_TRANSFORM_DEBUG
    PROGRESS("Two of the eigenvalues are less than zero - reversing ellipse equation");    
    PROGRESS("Eigen Values: v=[" << eigvals[0] << "," << eigvals[1] << "," << eigvals[2] << ";");
    PROGRESS("                 " << eigvals[3] << "," << eigvals[4] << "," << eigvals[5] << ";");
    PROGRESS("                 " << eigvals[6] << "," << eigvals[7] << "," << eigvals[8] << "];");
#endif
  }
  */
  
  // bubble sort the vectors (based on their eigenvalue)...I'm so embarresed ;-)
  for(int i=0;i<4;i++) {
    for(int j=1;j<3;j++) {
      if (eigvals[j*4-4] < eigvals[j*4]) {
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
  /*
  float tx1 = eigvals[0];
  float te1 = eigvects[0];
  float te2 = eigvects[3];
  float te3 = eigvects[6];

  eigvals[0] = eigvals[4];
  eigvals[4] = eigvals[8];
  for(int i=0;i<3;i++) {
    eigvects[3*i] = eigvects[3*i+1];
    eigvects[3*i+1] = eigvects[3*i+2];
  }

  eigvals[8] = tx1;
  eigvects[2] = te1;
  eigvects[5] = te2;
  eigvects[8] = te3;
  */
  
  // testing degrees of freedom
  //  eigvects[0] *= -1;
  //  eigvects[3] *= -1;
  //  eigvects[6] *= -1;
  //  eigvects[2] *= -1;
  //  eigvects[5] *= -1;
  //  eigvects[8] *= -1;


  // make sure the normal vector will point in the right direction
  if (eigvects[8] < 0) {
    eigvects[2]*=-1;
    eigvects[5]*=-1;
    eigvects[8]*=-1;
  }

  // make sure that our eigenvectors do not include any reflections
  float determinant = 
    eigvects[0]*eigvects[4]*eigvects[8]
    - eigvects[0]*eigvects[5]*eigvects[7]
    - eigvects[1]*eigvects[3]*eigvects[8] 
    + eigvects[1]*eigvects[5]*eigvects[6] 
    + eigvects[2]*eigvects[3]*eigvects[7]
    - eigvects[2]*eigvects[4]*eigvects[6];
#ifdef CIRCLE_TRANSFORM_DEBUG
  PROGRESS("Determinant = " << determinant);
#endif

  if (determinant < 0) {
    // it is enough to swap one of the vectors to make the determinant positive. 
    eigvects[0] *= -1;
    eigvects[3] *= -1;
    eigvects[6] *= -1;
#ifdef CIRCLE_TRANSFORM_DEBUG
    determinant = 
      eigvects[0]*eigvects[4]*eigvects[8]
      - eigvects[0]*eigvects[5]*eigvects[7]
      - eigvects[1]*eigvects[3]*eigvects[8] 
      + eigvects[1]*eigvects[5]*eigvects[6] 
      + eigvects[2]*eigvects[3]*eigvects[7]
      - eigvects[2]*eigvects[4]*eigvects[6];
    PROGRESS("Swap x eigvect. Det = " << determinant);
#endif
        
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

  /*
  double normal_t1 = sqrt( (eigvals[4]-eigvals[0])/(eigvals[4]-eigvals[8]) );
  double normal_t2 = sqrt( (eigvals[0]-eigvals[8])/(eigvals[4]-eigvals[8]) );

  double normal_x = normal_t1*eigvects[1] + normal_t2*eigvects[2];
  double normal_y = normal_t1*eigvects[4] + normal_t2*eigvects[5];
  double normal_z = normal_t1*eigvects[7] + normal_t2*eigvects[8];

  double normal2_x = -normal_t1*eigvects[1] + normal_t2*eigvects[2];
  double normal2_y = -normal_t1*eigvects[4] + normal_t2*eigvects[5];
  double normal2_z = -normal_t1*eigvects[7] + normal_t2*eigvects[8];

  PROGRESS("Normal vector: n=[ " << normal_x << "," << normal_y << "," << normal_z <<";");
  PROGRESS("Normal vector: n=[ " << normal2_x << "," << normal2_y << "," << normal2_z <<";");
  */
#endif
      
  double denom = ( eigvals[8] - eigvals[0] );
  double cossq = ( eigvals[8] - eigvals[4] ) / denom;
  double sinsq = ( eigvals[4] - eigvals[0] ) / denom;
  double pmsin = sqrt(sinsq);
  double pmcos = sqrt(cossq);

  /*
  double denom = ( eigvals[8] - eigvals[4] );
  double sinsq = ( eigvals[0] - eigvals[4] ) / denom;
  double cossq = ( eigvals[8] - eigvals[0] ) / denom;
   
  double pmsin = sqrt(sinsq);
  double pmcos = sqrt(cossq);
  */

  // here is our first ambiguity choice point.  We need to decide plus or minus theta

  double r2c1[] = { pmcos, 0, -pmsin, 0,
		   0,     1, 0,     0, 
		   pmsin, 0, pmcos, 0,
		   0,     0, 0,     1 };
  
  double r2c2[] = { pmcos, 0, pmsin, 0,
		   0,     1, 0,     0, 
		   -pmsin, 0, pmcos, 0,
		   0,     0, 0,     1 };
  /*
  float r2c1[] = { 1,     0,     0,    0,
		   0,     pmcos, -pmsin,0, 
		   0,     pmsin,pmcos,0,
		   0,     0,     0,    1 };

  float r2c2[] = { 1,     0,     0,    0,
		   0,     pmcos,pmsin,0, 
		   0,     -pmsin, pmcos,0,
		   0,     0,     0,    1 };
  */
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
  //  double tx = sqrt( eigvals[0] - (eigvals[4] + eigvals[0]*eigvals[8]/eigvals[4]) + eigvals[8] );

#ifdef CIRCLE_TRANSFORM_DEBUG
  PROGRESS("Translation tx = "<<tx);
#endif
  // and another choice based on theta
  double scale = sqrt(-eigvals[0]*eigvals[8]/eigvals[4]/eigvals[4]);
  //double scale = sqrt(1 + (eigvals[8]-eigvals[0])*(eigvals[0]-eigvals[4])/eigvals[0]/eigvals[0]/eigvals[0] - (eigvals[4]+eigvals[8])/eigvals[0]/eigvals[0]);

#ifdef CIRCLE_TRANSFORM_DEBUG
  PROGRESS("Scale factor " << scale);
#endif
  
  double transc1[] = {1,0,0,tx/scale,
		      0,1,0,0,
		      0,0,1,1/scale,
		      0,0,0,1};
  
  double transc2[] = {1,0,0,-tx/scale,
		      0,1,0,0,
		      0,0,1,1/scale,
		      0,0,0,1};


  //  // this multiplies cols 0 and 1 of the transform by scale
  //  for(int col=0;col<4;col++) {
  //    for(int row=0;row<4;row++) {
  //      transc1[row*4+col] *= scale;
  //      transc2[row*4+col] *= scale;
  //    }
    //    transc1[col*4] *= scale;
    //    transc1[col*4+1] *= scale;
    //    transc1[col*4+2] *= scale;

    //    transc2[col*4] *= scale;
    //    transc2[col*4+1] *= scale;
    //    transc2[col*4+2] *= scale;

  //  }
    
#ifdef CIRCLE_TRANSFORM_DEBUG  
  PROGRESS("transc1=[" << transc1[0] << "," << transc1[1] << "," << transc1[2] << "," << transc1[3] << ";");
  PROGRESS("                 " << transc1[4] << "," << transc1[5] << "," << transc1[6] << "," << transc1[7] << ";");
  PROGRESS("                 " << transc1[8] << "," << transc1[9] << "," << transc1[10] << "," << transc1[11] << ";");
  PROGRESS("                 " << transc1[12] << "," << transc1[13] << "," << transc1[14] << "," << transc1[15] << ";");

  PROGRESS("transc2: mt2=[" << transc2[0] << "," << transc2[1] << "," << transc2[2] << "," << transc2[3] << ";");
  PROGRESS("                 " << transc2[4] << "," << transc2[5] << "," << transc2[6] << "," << transc2[7] << ";");
  PROGRESS("                 " << transc2[8] << "," << transc2[9] << "," << transc2[10] << "," << transc2[11] << ";");
  PROGRESS("                 " << transc2[12] << "," << transc2[13] << "," << transc2[14] << "," << transc2[15] << ";");
#endif


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
      std::cout << array[i*rows+j] << "\t";
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
      std::cout << array[i][j] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << "]" << std::endl;
}


void Ellipse::Decompose() {
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

  m_x0 = (2*c*d - b*e) / disc;
  m_y0 = (2*a*e - b*d) / disc;

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
  
  float scale_factor = sqrt( -f + a*m_x0*m_x0 + b*m_x0*m_y0 + c*m_y0*m_y0);

#ifdef DECOMPOSE_DEBUG
  PROGRESS("scale= " << scale_factor);
#endif

  m_width = lambda1 * scale_factor;
  m_height = lambda2 * scale_factor;

#ifdef DECOMPOSE_DEBUG
  PROGRESS("width= " << m_width);
  PROGRESS("height= " <<m_height);
#endif

  m_angle_radians = atan( -(a-lambda1t)/(0.5*b) );
  
#ifdef DECOMPOSE_DEBUG
  PROGRESS("angle= " << m_angle_radians);
#endif
  
}

void Ellipse::GetTransformLinear(float transform1[16], float transform2[16]) {
  transform1[0] = m_width*cos(m_angle_radians);
  transform1[1] = -m_height*sin(m_angle_radians); 
  transform1[2] = 0;
  transform1[3] = m_x0;
  transform1[4] = m_width*sin(m_angle_radians);
  transform1[5] = m_height*cos(m_angle_radians); 
  transform1[6] = 0;
  transform1[7] = m_y0;
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

