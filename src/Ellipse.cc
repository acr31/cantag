/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/02/20 22:25:59  acr31
 * major reworking of matching algorithms and architecture
 *
 */

#include <Config.hh>
#include <Ellipse.hh>
#include <fitellipse.hh>
#include <gaussianelimination.hh>
#include <eigenvv.hh>
#include <cmath>

#define ELLIPSE_DEBUG
#undef ELLIPSE_DEBUG_DUMP_POINTS

#define MAXFITERROR 0.000001
#define COMPARETHRESH 0.0001

static void print(const char* label, double* array, int rows, int cols);
static void print(const char* label, double** array, int rows, int cols);

Ellipse::Ellipse(float* points, int numpoints) {
  FitEllipse(points,numpoints);
}

Ellipse::Ellipse(float* points, int numpoints, bool prev_fit) {
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

void Ellipse::FitEllipse(float* points, int numpoints) {
#ifdef ELLIPSE_DEBUG_DUMP_POINTS
  for(int i=0;i<numpoints*2;i+=2) {
    std::cout << points[i] << " " << points[i+1] << " ; ";
  }
  std::cout << std::endl;
  std::cout << std::endl;
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

#ifdef ELLIPSE_DEBUG
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

      // it turns out to be really important to the pose extraction
      // that our conic has a positive sense!
      if (m_f > 0) {
	m_a*=-1;
	m_b*=-1;
	m_c*=-1;
	m_d*=-1;
	m_e*=-1;
	m_f*=-1;
#ifdef ELLIPSE_DEBUG
	PROGRESS("Corrected for negative scale factor");
#endif
      }

#ifdef ELLIPSE_DEBUG
      PROGRESS("Fitted ellipse: a="<<m_a<<","<<m_b<<","<<m_c<<","<<m_d<<","<<m_e<<","<<m_f);
#endif      

      m_fitted = CheckError(points,numpoints,MAXFITERROR);
    }
  }
}

bool Ellipse::CheckError(float* points, int count, float threshold) const {
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
  PROGRESS("Total error was "<< total/count << " and threshold is "<<threshold);
#endif
  return (total < threshold*count);
}


static void print(const char* label, double* array, int rows, int cols) {
  std::cout << "-----------------------------------" << std::endl;
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

