/**
 * $Header$
 */
#include <Config.hh>
#include <QuadTangle.hh>
#include <opencv/cv.h>
#include <cmath>

#define COMPARE_THRESH 1

#ifdef TEXT_DEBUG
#define QUADTANGLE_DEBUG
#endif

QuadTangle::QuadTangle() {
  m_fitted = false;
}

static float coskvect(const float* points, int index, int k, int numpoints) {
  int lower_index = index-k;
  int upper_index = index+k;

  if (lower_index < 0) {
    lower_index += numpoints;
  }

  if (upper_index >= numpoints) {
    upper_index -= numpoints;
  }

  float ax = points[2*index]-points[2*lower_index];
  float ay = points[2*index+1]-points[2*lower_index+1];

  float bx = points[2*index]-points[2*upper_index];
  float by = points[2*index+1]-points[2*upper_index+1];

  float adotb =  ax*bx + ay*by;      
  float maga = sqrt( ax*ax + ay*ay);
  float magb = sqrt( bx*bx + by*by);
  
  return adotb/maga/magb;
}



QuadTangle::QuadTangle(const float* points, int numpoints, bool prev_fitted) {
  
  if (!prev_fitted && points) {
    float curvature[numpoints];
    int support[numpoints];
    int m = numpoints/10;
    for(int i=0;i<numpoints;i++) {
      curvature[i] = -1;
      support[i] = 0;
      for(int k=m;k>0;k--) {
	float newcos = coskvect(points,i,k,numpoints);
	if (newcos < curvature[i]) {
	  break;
	}
	else {
	  support[i] = k;
	  curvature[i] = newcos;
	}
      }
    }

    int partition_indices[4];
    int ptr=0;
    for(int i=0;i<numpoints;i++) {
      bool accept = true;
      for(int j=0;j<=support[i]/2;j++) {
	int lower_index = i-j;
	int upper_index = i+j;
	if (lower_index <0) { lower_index += numpoints; }
	if (upper_index >= numpoints) { upper_index -= numpoints; }

	accept = accept && 
	  curvature[lower_index] <= curvature[i] &&
	  curvature[upper_index] <= curvature[i];		  	
      }
      if (accept) {
	if (ptr == 4) {
	  // we've just found our fifth point - can't be a quadtangle
#ifdef QUADTANGLE_DEBUG
	  PROGRESS("Found fifth point on this contour - it can't be a square");
#endif
	  m_fitted = false;
	  return;
	}
	else {
	  partition_indices[ptr++] = i;
	}
      }
    }
    
    if (ptr != 4) {
      // we havn't found enough points
#ifdef QUADTANGLE_DEBUG
      PROGRESS("Only found " << ptr << " points - it can't be a square");
#endif
      m_fitted = false;
      return;
    }

    m_x0 = points[partition_indices[0]*2];
    m_y0 = points[partition_indices[0]*2+1];
    m_x1 = points[partition_indices[1]*2];
    m_y1 = points[partition_indices[1]*2+1];
    m_x2 = points[partition_indices[2]*2];
    m_y2 = points[partition_indices[2]*2+1];
    m_x3 = points[partition_indices[3]*2];
    m_y3 = points[partition_indices[3]*2+1];

#ifdef QUADTANGLE_DEBUG
    PROGRESS("Accepting polygon with points " <<
	     "("<< m_x0 << "," << m_y0 << ") " <<
	     "("<< m_x1 << "," << m_y1 << ") " <<
	     "("<< m_x2 << "," << m_y2 << ") " <<
	     "("<< m_x3 << "," << m_y3 << ") ");

#endif
    sort_points();
    m_fitted = true;
  }
  else {
    m_fitted = false;
  }
}

QuadTangle::QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3) : m_x0(x0), m_y0(y0),m_x1(x1), m_y1(y1),m_x2(x2), m_y2(y2),m_x3(x3), m_y3(y3), m_fitted(true) {}

float dist(float x0, float y0, float x1, float y1) {
  return sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) );
}

bool QuadTangle::Compare(const QuadTangle& o) const {
  return ((dist(m_x0,m_y0,o.m_x0,o.m_y0) < COMPARE_THRESH) &&
	  (dist(m_x1,m_y1,o.m_x1,o.m_y1) < COMPARE_THRESH) &&
	  (dist(m_x2,m_y2,o.m_x2,o.m_y2) < COMPARE_THRESH) &&
	  (dist(m_x3,m_y3,o.m_x3,o.m_y3) < COMPARE_THRESH));
}

float QuadTangle::find_angle(float x, float y, float cx, float cy) {
  if ((x >= cx) && (y >= cy)) {
    return M_PI/2+ atan( (y-cy) / (x-cx) );
  }
  else if ((x >= cx) && (y < cy)) {
    return atan( (cy-y) / (x-cx) );
  }
  else if ((x < cx) && (y < cy)) {
    return 3*M_PI/2 + atan ( (cy-y) / (cx-x) );
  }
  else if ((x < cx) && (y >= cy)) {
    return M_PI + atan( (y-cy) / (cx-x));
  }
}

void QuadTangle::swap( float *a, float *b) {
  float t = *a;
  *a = *b;
  *b = t;
}

void QuadTangle::sort_points()
{
  // sort the points into clockwise order.
  float angles[4];
  angles[0] = find_angle(m_x0,m_y0,m_xc,m_yc);
  angles[1] = find_angle(m_x1,m_y1,m_xc,m_yc);
  angles[2] = find_angle(m_x2,m_y2,m_xc,m_yc);
  angles[3] = find_angle(m_x3,m_y3,m_xc,m_yc);

#ifdef QUADTANGLE_DEBUG
  PROGRESS("Centre (" << m_xc << "," << m_yc <<")");
  PROGRESS("Original order " << std::endl <<
  	   "(" << m_x0 << "," << m_y0 << ") @ "<< angles[0] << std::endl <<
  	   "(" << m_x1 << "," << m_y1 << ") @ "<< angles[1] << std::endl <<
  	   "(" << m_x2 << "," << m_y2 << ") @ "<< angles[2] << std::endl <<
  	   "(" << m_x3 << "," << m_y3 << ") @ "<< angles[3]);
#endif
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
#ifdef QUADTANGLE_DEBUG
  PROGRESS("Final order " << std::endl <<
  	   "(" << m_x0 << "," << m_y0 << ") @ "<< angles[0] << std::endl <<
  	   "(" << m_x1 << "," << m_y1 << ") @ "<< angles[1] << std::endl <<
  	   "(" << m_x2 << "," << m_y2 << ") @ "<< angles[2] << std::endl <<
  	   "(" << m_x3 << "," << m_y3 << ") @ "<< angles[3]);
#endif
}

void QuadTangle::compute_central_point() {
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

