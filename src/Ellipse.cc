/**
 * $Header$
 */

#include <total/Config.hh>
#include <total/Ellipse.hh>
#include <total/gaussianelimination.hh>
#include <total/findtransform.hh>
#include <total/polysolve.hh>

#include <cmath>
#include <cassert>

#ifdef TEXT_DEBUG
# undef ELLIPSE_DEBUG
# undef ELLIPSE_DEBUG_DUMP_POINTS
# undef CIRCLE_TRANSFORM_DEBUG
# undef DECOMPOSE_DEBUG
#endif

#define MAXFITERROR 0.01
#define COMPARETHRESH 1
#define MAXDISTANCE 

namespace Total {

#ifdef ELLIPSE_DEBUG
  static void print(const char* label, double* array, int rows, int cols);
  static void print(const char* label, double** array, int rows, int cols);
#endif

  Ellipse::Ellipse(): m_fitted(false) {}

  Ellipse::Ellipse(const std::vector<float>& points) {
    m_fitted = FitEllipse(points);
  }

  Ellipse::Ellipse(const std::vector<float>& points, bool prev_fit) {
    if (!prev_fit) {
      m_fitted = FitEllipse(points);
    }
    else {
      m_fitted =false;
    }
  }

  Ellipse::Ellipse(float a, float b, float c, float d, float e, float f) :
    m_a(a), m_b(b), m_c(c), m_d(d), m_e(e), m_f(f), m_fitted(true) { Decompose();}

  Ellipse::Ellipse(float x0, float y0, float width, float height, float angle) :
    m_x0(x0), m_y0(y0),m_angle_radians(angle), m_width(width),m_height(height),m_fitted(true) { Compose(); }

  void Ellipse::Draw(Image& image,const Camera& camera) const {
    if (m_fitted) {
      camera.DrawEllipse(image,*this);
    }
  }

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

  bool Ellipse::FitEllipse(const std::vector<float>& points) {
    int numpoints = points.size()/2;
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

    for(std::vector<float>::const_iterator i = points.begin();
	i != points.end();
	++i) {
      float x = *i;
      ++i;
      float y = *i;
      double d1col[] = {x*x, x*y, y*y};
      double d2col[] = {x,y,1};
    
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

    // check the determinant of S3 is non-zero - if we have a straight line for example then this is zero
    /*
    
    | a b c | 
    | d e f |  = a(ei-hf) - b(di-fg) + c(dh-eg)
    | g h i | 
    */
    float determinant = 
      s3[0][0]*(s3[1][1]*s3[2][2]-s3[1][2]*s3[2][1]) -
      s3[1][0]*(s3[0][1]*s3[2][2]-s3[2][1]*s3[0][2]) +
      s3[2][0]*(s3[0][1]*s3[1][2]-s3[1][1]*s3[0][2]);
  
#ifdef ELLIPSE_DEBUG
    PROGRESS("Determinant of S3 " << determinant);
#endif    

    if (fabs(determinant) < 1e-5) { 
#ifdef ELLIPSE_DEBUG
      PROGRESS("Determinant of S3 is zero - no fit");
#endif
      return false; 
    }

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

	if (GetError(points) < MAXFITERROR) {
	  Decompose();
	  return true;
	}
      }
    }
    return false;
  }

  float Ellipse::GetError(const std::vector<float>& points) const {
    return GetErrorGradient(points);
  }

  float Ellipse::GetErrorAlgebraic(const std::vector<float>& points) const {
    // calculate the algebraic distance
    float total=0;
    float maxdist = 0;
    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {
      float x = *i;
      ++i;
      float y = *i;
      float dist = fabs(m_a*x*x+m_b*x*y+m_c*y*y+m_d*x+m_e*y+m_f);
      if (dist > maxdist) { maxdist = dist; }
      total+= dist;
    }
#ifdef ELLIPSE_DEBUG
    PROGRESS("Total error from Algebraic method is "<< total/points.size() << " maximimum distance was " << maxdist);
#endif

#ifdef MAXDISTANCE
    return maxdist;
#else
    return total/points.size();
#endif
  }

  float Ellipse::GetErrorGradient(const std::vector<float>& points) const {
    // calculate the algebraic distance inversly weighted by the
    // gradient
    float total=0;
    float maxdist = 0;

    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {  
      float x = *i;
      ++i;
      float y = *i;
      float dist = fabs(m_a*x*x+m_b*x*y+m_c*y*y+m_d*x+m_e*y+m_f);
    
      float dx = 2*m_a*x+m_b*y+m_d;
      float dy = m_b*x+2*m_c*y+m_e;

      float norm = dx*dx + dy*dy;

      dist /= sqrt(norm);

      if (dist > maxdist) { maxdist = dist; }
      total+= dist;
    }
#ifdef ELLIPSE_DEBUG
    PROGRESS("Total error from Gradient method is "<< total/points.size()<< " maximimum distance was " << maxdist);
#endif

#ifdef MAXDISTANCE
    return maxdist;
#else
    return total/points.size();
#endif
  }

  float Ellipse::GetErrorNakagawa(const std::vector<float>& points) const {
  
    float total=0;
    float max_dist =0 ;
    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {  
      float xi = *i;
      ++i;
      float yi = *i;

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
    PROGRESS("Total error from Nakagawa Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif

#ifdef MAXDISTANCE
    return max_dist;
#else
    return total/points.size();
#endif
  }

  float Ellipse::GetErrorSafaeeRad(const std::vector<float>& points) const {
    // draw a ray between each point and the centre of the ellipse C
    // intersecting the ellipse at Ij.  The lengths of the bisected
    // portions of the ray mj and nj are determined

    float total=0;
    float max_dist = 0;
    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {  
      float xi = *i;
      ++i;
      float yi = *i;
      ++i;
     
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
    PROGRESS("Total error from SafaeeRad Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif

#ifdef MAXDISTANCE
    return max_dist;
#else
    return total/points.size();
#endif
  }

  float Ellipse::GetErrorSafaeeRad2(const std::vector<float>& points) const {
    float total=0;
    float max_dist = 0;
    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {  
      float xi = *i;
      ++i;
      float yi = *i;
      ++i;

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
    PROGRESS("Total error from SafaeeRad2 Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif
#ifdef MAXDISTANCE
    return max_dist;
#else
    return total/points.size();
#endif
  }

  float Ellipse::GetErrorStricker(const std::vector<float>& points) const {
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
    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {  
      float x = *i;
      ++i;
      float y = *i;

      float aest = 0.5 * (sqrt( (x-f1x)*(x-f1x) + (y-f1y)*(y-f1y) ) + sqrt( (x-f2x)*(x-f2x) + (y-f2y)*(y-f2y)));
      float best = sqrt(aest*aest - a*a + b*b);

      float dest = 0.5*(aest - a + best - b);

      float ctilde = sqrt( (a+dest)*(a+dest) - (b+dest)*(b+dest) );

      float f1xtilde = (f1x + f2x)/2+ctilde*(f1x-f2x)/modf1_f2;
      float f1ytilde = (f1y + f2y)/2+ctilde*(f1y-f2y)/modf1_f2;

      float f2xtilde = (f1x + f2x)/2-ctilde*(f1x-f2x)/modf1_f2;
      float f2ytilde = (f1y + f2y)/2-ctilde*(f1y-f2y)/modf1_f2;

      float atilde = 0.5*(sqrt( (x-f1xtilde)*(x-f1xtilde) + (y-f1ytilde)*(y-f1ytilde) ) + sqrt( (x-f2xtilde)*(x-f2xtilde) + (y-f2ytilde)*(y-f2ytilde) ));

      float dist = fabs(atilde - a);
    
      if (dist > max_dist) { max_dist = dist; }
      total += dist;
    }

#ifdef ELLIPSE_DEBUG
    PROGRESS("Total error from Stricker Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif
#ifdef MAXDISTANCE
    return max_dist;
#else
    return total/points.size();
#endif
  }

#ifdef ELLIPSE_DEBUG
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
#endif

  void Ellipse::Compose() {
    float x0 = GetX0();
    float y0 = GetY0();
    float alpha1sq = GetWidth()*GetWidth();
    float alpha2sq = GetHeight()*GetHeight();

    float angle = GetAngle();

    float c = cos(angle);
    float s = sin(angle);
  
    m_a = c*c/alpha1sq + s*s/alpha2sq;
    m_b = 2*c*s*(1/alpha1sq - 1/alpha2sq);
    m_c = s*s/alpha1sq + c*c/alpha2sq;
    m_d = -2*x0*m_a - y0*m_b;
    m_e = -x0*m_b - 2*y0*m_c;
    m_f = x0*x0*m_a + x0*y0*m_b + y0*y0*m_c - 1;

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
    PROGRESS("X= " << m_x0);
    PROGRESS("Y= " << m_y0);
#endif
  
    float tmproot = sqrt( (a-c)*(a-c) + b*b );
    float lambda1 = ((a+c) - tmproot)/2;
    float lambda2 = ((a+c) + tmproot)/2;
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
  
    //  m_width = lambda1;
    //m_height = lambda2;

#ifdef DECOMPOSE_DEBUG
    PROGRESS("width= " << m_width);
    PROGRESS("height= " <<m_height);
#endif

    if (m_width == m_height) {
      // obviously the angle is undefined if we have a circle
      m_angle_radians = 0;
    }
    else {
      m_angle_radians = atan( -(a-lambda1t)/(0.5*b) );
    }
#ifdef DECOMPOSE_DEBUG
    PROGRESS("angle= " << m_angle_radians);
#endif
  
  }


  int Ellipse::Save(Socket& socket) const {
    int count = socket.Send(m_a);
    count += socket.Send(m_b);
    count += socket.Send(m_c);
    count += socket.Send(m_d);
    count += socket.Send(m_e);
    count += socket.Send(m_f);
    count += socket.Send(m_x0);
    count += socket.Send(m_y0);
    count += socket.Send(m_angle_radians);
    count += socket.Send(m_width);
    count += socket.Send(m_height);
    count += socket.Send((int)m_fitted);
    return count;
  }

  Ellipse::Ellipse(Socket& socket) {
    m_a = socket.RecvFloat();
    m_b = socket.RecvFloat();
    m_c = socket.RecvFloat();
    m_d = socket.RecvFloat();
    m_e = socket.RecvFloat();
    m_f = socket.RecvFloat();
    m_x0 = socket.RecvFloat();
    m_y0 = socket.RecvFloat();
    m_angle_radians = socket.RecvFloat();
    m_width = socket.RecvFloat();
    m_height = socket.RecvFloat();    
    m_fitted = (bool)socket.RecvInt();    
  }

  bool Ellipse::Check(const std::vector<float>& points) const {
    return (GetError(points) < MAXFITERROR); 
  }

  SimpleEllipse::SimpleEllipse() : Ellipse() {}

  SimpleEllipse::SimpleEllipse(const std::vector<float>& points) : Ellipse(points) {}

  SimpleEllipse::SimpleEllipse(const std::vector<float>& points, bool prev_fit) : Ellipse(points,prev_fit) {}

  SimpleEllipse::SimpleEllipse(Socket& socket) : Ellipse(socket) {}

  bool SimpleEllipse::FitEllipse(const std::vector<float>& points) {

    float centrex = 0;
    float centrey = 0;
    int count = 0;
    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end(); ++i) {
      centrex += *i;
      ++i;
      centrey += *i;
      count++;
    }
    centrex/=count;
    centrey/=count;

    float majorx = -1;
    float majory = -1;
    float majorlen = 0;

    float minorx = -1;
    float minory = -1;
    float minorlen = 1e10;

    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end(); ++i) {
      float x = *i;
      ++i;
      float y = *i;
      float distsq = (centrex - x)*(centrex-x) + (centrey-y)*(centrey-y);
      if (distsq > majorlen) {
	majorx = x;
	majory = y;
	majorlen = distsq;
      }
      if (distsq < minorlen) {
	minorx = x;
	minory = y;
	minorlen = distsq;
      }
    }

    assert(majorlen != 0);
    assert(minorlen != 1e10);

    majorlen = sqrt(majorlen);
    minorlen = sqrt(minorlen);

    float theta = atan((majory-centrey)/(majorx-centrex));
  
    m_x0 = centrex;
    m_y0 = centrey;
    m_angle_radians = theta;
    m_width = majorlen;
    m_height = minorlen;

    Compose();
    return true;
  }

}
