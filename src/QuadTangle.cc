/**
 * $Header$
 */

#include <total/Config.hh>
#include <total/QuadTangle.hh>
#include <total/gaussianelimination.hh>

#include <cmath>
#include <cassert>
#include <iostream>
#include <map>
#define COMPARE_THRESH 1

#undef QUADTANGLE_DEBUG
#define POLYGON_DEBUG

#define LOGMAXWINDOW 5
#define CURVTHRESH -0.8

namespace Total {

#define MASK(x) ((x) & ((1<<LOGMAXWINDOW)-1))

  QuadTangle::QuadTangle() {
    m_fitted = false;
  }

  QuadTangle::QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3) : m_x0(x0), m_y0(y0),m_x1(x1), m_y1(y1),m_x2(x2), m_y2(y2),m_x3(x3), m_y3(y3), m_fitted(true) {
    compute_central_point();
    sort_points();
  }


  void QuadTangle::Draw(Image& image,const Camera& camera) const {
    if (m_fitted) {
      camera.DrawQuadTangle(image,*this);
    }
  }

  float dist(float x0, float y0, float x1, float y1) {
    return sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) );
  }

  bool QuadTangle::Compare(const QuadTangle& o) const {
    return ((dist(m_x0,m_y0,o.m_x0,o.m_y0) < COMPARE_THRESH) &&
	    (dist(m_x1,m_y1,o.m_x1,o.m_y1) < COMPARE_THRESH) &&
	    (dist(m_x2,m_y2,o.m_x2,o.m_y2) < COMPARE_THRESH) &&
	    (dist(m_x3,m_y3,o.m_x3,o.m_y3) < COMPARE_THRESH));
  }

  inline void QuadTangle::compute_central_point() {
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
    assert(false);
  }

  void QuadTangle::swap( float *a, float *b) {
    float t = *a;
    *a = *b;
    *b = t;
  }

  /**
   * courtesy of Rob Harle <rkh23@cam.ac.uk>
   */
  void QuadTangle::sort_points()
  {
    // take the vector centre -> 0
    float p1x = m_x0-m_xc;
    float p1y = m_y0-m_yc;

    // take the vector centre -> 1
    float p2x = m_x1-m_xc;
    float p2y = m_y1-m_yc;

    // Look at the z comp of a cross prod p1 x p2
    float z = p1x*p2y-p2x*p1y;

    if (z<0.0) {
      // Order is acw - fix it!
      double x0=m_x0, y0=m_y0;
      double x1=m_x1, y1=m_y1;
      double x2=m_x2, y2=m_y2;
      double x3=m_x3, y3=m_y3;

      m_x0= x0; m_y0=y0;
      m_x1= x3; m_y1=y3;
      m_x2= x2; m_y2=y2;
      m_x3= x1; m_y3=y1;
    }
  }

  QuadTangle::QuadTangle(Socket& socket) {
    m_x0 = socket.RecvFloat();
    m_y0 = socket.RecvFloat();
    m_x1 = socket.RecvFloat();
    m_y1 = socket.RecvFloat();
    m_x2 = socket.RecvFloat();
    m_y2 = socket.RecvFloat();
    m_x3 = socket.RecvFloat();
    m_y3 = socket.RecvFloat();
    m_xc = socket.RecvFloat();
    m_yc = socket.RecvFloat();
    m_fitted = (bool)socket.RecvInt();
  }

  int QuadTangle::Save(Socket& socket) const {
    int count = socket.Send(m_x0);
    count += socket.Send(m_y0);
    count += socket.Send(m_x1);
    count += socket.Send(m_y1);
    count += socket.Send(m_x2);
    count += socket.Send(m_y2);
    count += socket.Send(m_x3);
    count += socket.Send(m_y3);
    count += socket.Send(m_xc);
    count += socket.Send(m_yc);
    count += socket.Send((int)m_fitted);
    return count;
  }


  // BEGIN CornerQuadTangle


  static void compute(const float* xwindow, const float* ywindow, int datapointer, int k, float* lik, float* rik) {
    float chordx = xwindow[MASK(datapointer+k)] - xwindow[MASK(datapointer-k)];
    float chordy = ywindow[MASK(datapointer+k)] - ywindow[MASK(datapointer-k)];
  
    *lik = sqrt(chordx*chordx + chordy*chordy);
  
    float apx = xwindow[MASK(datapointer-k)] - xwindow[datapointer];
    float apy = ywindow[MASK(datapointer-k)] - ywindow[datapointer];

    float modchord = *lik;

    if (modchord < 1e-5) { *rik = 0; return; }
  
    float denom = fabs((chordx*apy - apx*chordy));
  
    *rik = denom/modchord/modchord;

  }

  static bool check(const float* xwindow, const float* ywindow, int datapointer, int k) {
    if (k+1 == (1<<(LOGMAXWINDOW-1))) return false;

    float lik,rik,lik1,rik1;
    compute(xwindow,ywindow,datapointer,k,&lik,&rik);
    compute(xwindow,ywindow,datapointer,k+1,&lik1,&rik1);

    bool result = (lik < lik1) && (rik < rik1);

    return result;
  }

  static float curvature(const float* xwindow, const float* ywindow, int datapointer, int k) {
    float ax = xwindow[MASK(datapointer+k)] - xwindow[datapointer];
    float ay = ywindow[MASK(datapointer+k)] - ywindow[datapointer];

    float bx = xwindow[MASK(datapointer-k)] - xwindow[datapointer];
    float by = ywindow[MASK(datapointer-k)] - ywindow[datapointer];

    float moda = sqrt(ax*ax+ay*ay);
    float modb = sqrt(bx*bx+by*by);

    float result = (ax*bx+ay*by)/moda/modb;

    return result;
  }

  CornerQuadTangle::CornerQuadTangle() : QuadTangle() {};

  CornerQuadTangle::CornerQuadTangle(const std::vector<float>& points, bool prev_fitted) : QuadTangle() {
    if (!prev_fitted) {
      m_fitted = Fit(points);
    }
    else {
      m_fitted = false;
    }
  };

  bool CornerQuadTangle::Fit(const std::vector<float>& points) {
    if (points.size() > (2<<LOGMAXWINDOW) && points.size() > 50) {
      float xcorners[4];
      float ycorners[4];
      float curvecorners[4];
      int indexcorners[4];
      int corner_counter = 0;
      float xwindow[1<<LOGMAXWINDOW];
      float ywindow[1<<LOGMAXWINDOW];
    
      int loadpointer = 0;
      std::vector<float>::const_iterator i = points.begin();
      for(;loadpointer < (1<<LOGMAXWINDOW);++loadpointer) {
	xwindow[loadpointer] = *i;
	++i;
	ywindow[loadpointer] = *i;
	++i;
      }
      int datapointer = 1<<(LOGMAXWINDOW-1);
    
      float curve2 = curvature(xwindow,ywindow,datapointer,10);
      if (curve2 > CURVTHRESH) {
	xcorners[0] = xwindow[datapointer];
	ycorners[0] = ywindow[datapointer];
	curvecorners[0] = curve2;
	indexcorners[0] = 0;
	++corner_counter;
      }
    
      float previous = curve2;
      float currentmax = -10;
      int count = points.size()/2;
      for(int c=1;c<count;++c) {
	datapointer = MASK(datapointer+1);
	loadpointer = MASK(loadpointer+1);
	xwindow[loadpointer] = *i;
	++i;
	ywindow[loadpointer] = *i;
	++i;
      
	if (i == points.end()) { i = points.begin(); }
      
	float curve = curvature(xwindow,ywindow,datapointer,10);
	curve = -fabs(curve);
	if (curve < CURVTHRESH) { 
	  if (previous > CURVTHRESH) {
	    ++corner_counter;
	    currentmax = -10;
	    if (corner_counter > 4) { return false; }
	  }
	}
	else {
	  if (curve > currentmax && corner_counter < 4) { 
	    currentmax = curve;
	    xcorners[corner_counter] = xwindow[datapointer];
	    ycorners[corner_counter] = ywindow[datapointer];
	  }
	}
	previous = curve;
      }
    
      if (corner_counter == 4) {
	m_x0 = xcorners[0];
	m_y0 = ycorners[0];
	m_x1 = xcorners[1];
	m_y1 = ycorners[1];
	m_x2 = xcorners[2];
	m_y2 = ycorners[2];
	m_x3 = xcorners[3];
	m_y3 = ycorners[3];
	compute_central_point();
	sort_points();
	return true;
      }
    }
    return false;
  }

  // BEGIN RegressionQuadTangle
  RegressionQuadTangle::RegressionQuadTangle() : QuadTangle() {};

  RegressionQuadTangle::RegressionQuadTangle(const std::vector<float>& points, bool prev_fitted) : QuadTangle() {
    if (!prev_fitted) {
      m_fitted = Fit(points);
    }
    else {
      m_fitted = false;
    }
  };
  
  float RegressionQuadTangle::isLeft( const std::vector<float> &V, 
				      int l0, int l1, int p) {
    return (V[l1*2] - V[l0*2])*(V[p*2+1] - V[l0*2+1]) - 
      (V[p*2] - V[l0*2])*(V[l1*2+1] - V[l0*2+1]);
  }


  /** The hull code is adapted from softSurfer
   * who requires the following copyright be displayed:
   * Copyright 2001, softSurfer (www.softsurfer.com)
   * This code may be freely used and modified for any purpose
   * providing that this copyright notice is included with it.
   */
  int RegressionQuadTangle::ConvexHull(const std::vector<float> &V, int n, int* H) {
    // initialize a deque D[] from bottom to top so that the
    // 1st three vertices of V[] are a counterclockwise triangle
    
    int D[2*n+1];
    int bot = n-2, top = bot+3;   // initial bottom and top deque indices
    D[bot] = D[top] = 2;       // 3rd vertex is at both bot and top
    if (isLeft(V, 0, 1, 2) > 0) {
      D[bot+1] = 0;
      D[bot+2] = 1;          // ccw vertices are: 2,0,1,2
    }
    else {
      D[bot+1] = 1;
      D[bot+2] = 0;          // ccw vertices are: 2,1,0,2
    }
    
    // compute the hull on the deque D[]
    for (int i=3; i < n; i++) {   // process the rest of vertices
      // test if next vertex is inside the deque hull
      if ((isLeft(V, D[bot], D[bot+1], i) >= 0) &&
	  (isLeft(V, D[top-1],D[top], i) >= 0) )
	continue;         // skip an interior vertex
      
      // incrementally add an exterior vertex to the deque hull
      // get the rightmost tangent at the deque bot
      while (isLeft(V,D[bot], D[bot+1], i) <= 0)
	++bot;                // remove bot of deque
      D[--bot] = i;          // insert V[i] at bot of deque
      
      // get the leftmost tangent at the deque top
      while (isLeft(V,D[top-1],D[top], i) <= 0)
	--top;                // pop top of deque
      D[++top] = i;          // push V[i] onto top of deque
    }
    
    // transcribe deque D[] to the output hull array H[]
    int h;        // hull vertex counter
    for (h=0; h <= (top-bot); h++) {
      H[h] = D[bot + h];
    } 
    return h-1;
  }

  
  bool RegressionQuadTangle::Fit(const std::vector<float>& points) {
    // Take a convex hull of the polyline ( O(n) )
    int h[points.size()/2];
    int n = ConvexHull(points,points.size()/2,h);

    // Throw away vertices with large angles ~ 180
    // It doesn't matter too much if some get through
    std::multimap<float,int> angles;
    for (int i=0; i<n; i++) {
      int lastidx = i-1;
      if (lastidx==-1) lastidx=n-1;
      float lx = points[h[lastidx]*2] - points[h[i]*2];
      float ly = points[h[lastidx]*2+1] - points[h[i]*2+1];
      float nx = points[h[(i+1)%n]*2] - points[h[i]*2];
      float ny = points[h[(i+1)%n]*2+1] - points[h[i]*2+1];
      float ct = (lx*nx+ly*ny)/(sqrt(lx*lx+ly*ly)*sqrt(nx*nx+ny*ny));
      if (fabs(ct) <0.98) angles.insert( std::pair<float,int>(fabs(ct),h[i]) );
    }

    // If there aren't 4 vertices left, this can't be a quadtangle
    if (angles.size()<4) { return false; }

    std::multimap<float,int>::const_iterator ci = angles.begin();
    std::vector<int> indexes;
    
    // Copy the remaining vertex indices
    // to a vector and sort them numerically
    ci = angles.begin();
    for (int i=0; i<angles.size(); i++) {
      indexes.push_back (ci->second);
      ++ci;
    }
    sort(indexes.begin(), indexes.end());


    if (indexes.size()>4) {
      // Have too many vertices left :-(
      
      // Find the longest side in the current poly
      float longest = 0.0;
      
      for (int i=0; i<indexes.size(); i++) {
	float xd = points[indexes[i]*2] - points[indexes[(i+1)%indexes.size()]*2];
	float yd = points[indexes[i]*2+1] - points[indexes[(i+1)%indexes.size()]*2+1];
	if ((xd*xd+yd*yd) > longest) longest = xd*xd+yd*yd;
      }
      
      // Iterate over again, storing the index and
      // its ensuing side length relative to the longest
      std::multimap<float,int> dist;    
      for (int i=0; i<indexes.size(); i++) {
	float xd = points[indexes[i]*2] - points[indexes[(i+1)%indexes.size()]*2];
	float yd = points[indexes[i]*2+1] - points[indexes[(i+1)%indexes.size()]*2+1];
	dist.insert( std::pair<float,int>(-(xd*xd+yd*yd)/longest,indexes[i]));
      }
      
      // The vertices associated with the 4 longest sides 
      // will do
      std::multimap<float,int>::const_iterator di = dist.begin();
      indexes.clear();
      for (int i=0; i<4; i++) {
	indexes.push_back(di->second);
	di++;
      }
    }
    sort(indexes.begin(), indexes.end());
    
    // Now we have estimates of the corner indexes within points
    // So do some regression. We ignore the 4 points next to 
    // an estimated corner since these are less reliable indicators
    // of the side
    // Each line has equation y=mx+c
    // OR x=c
    float m[4]={0.0};
    float c[4]={0.0};
    bool  yeq[4]={1};
    
    for (int j=0; j<4; j++) {
      float xsum=0.0;
      float ysum = 0.0;
      float xxsum=0.0;
      float xysum=0.0;
      int count=0;
      
      int start = indexes[j];
      int end = indexes[(j+1)%4]%(points.size()/2);
      
      if (end < start) end+=points.size()/2;
      
      // Ignore the first and last 3 points
      float lastx=points[(start+4)*2];
      bool vertical=true;
      for (int i=start+4; i<end-4; i++) {
	int ii = i%(points.size()/2);
	
	float x = points[ii*2];
	float y = points[ii*2+1];
	
	xsum+=x;
	ysum+=y;
	xxsum+=x*x;
	xysum+=x*y;
	count++;
	if (x!=lastx) vertical=false;
	lastx = x;
      }
      
      if (vertical) {
	c[j] = lastx;
	yeq[j]=0;
      }
      else {
	float numer = (xysum - xsum*ysum/(float)count);
	float denom = (xxsum - xsum*xsum/(float)count);
	m[j] = numer/denom;
	c[j] = ysum/(float)count - m[j]*xsum/(float)count;
	yeq[j]=1;
      }
    }
    
    // Now have four lines and we need the intersections
    
    float xres[4];
    float yres[4];
    
    
    
    for (int j=0; j<4; j++) {
      //    std::cout << "Y " << yeq[j] << " " << (j-1)%4 << std::endl;
      int lastj = (j-1);
      if (lastj==-1) lastj=3;
      if (yeq[j] && yeq[lastj]) {
	xres[j] = (c[lastj]-c[j])/(m[j]-m[lastj]);
	yres[j] = m[j]*xres[j]+c[j];
      }
      else if (yeq[j] && !yeq[lastj]) {
	xres[j] = c[lastj];
	yres[j] = m[j]*xres[j] + c[j];
      }
      else if (!yeq[j] && yeq[lastj]) {
	xres[j] = c[j];
	yres[j] = m[lastj]*xres[j] + c[lastj];
      }
      else {
	// so they don't intersect
	return false;
      }
    }
    
    // Copy results
    m_x0 = xres[0];
    m_y0 = yres[0];
    m_x1 = xres[1];
    m_y1 = yres[1];
    m_x2 = xres[2];
    m_y2 = yres[2]; 
    m_x3 = xres[3];
    m_y3 = yres[3];
    compute_central_point();
    sort_points();
    return true;
  }




  // BEGIN PolygonQuadTangle
  PolygonQuadTangle::PolygonQuadTangle() : QuadTangle() {};

  PolygonQuadTangle::PolygonQuadTangle(const std::vector<float>& points, bool prev_fitted) : QuadTangle() {
    if (!prev_fitted) {
      m_fitted = Fit(points);
    }
    else {
      m_fitted = false;
    }
  };
  
  /**
   * The corner curvature we use for weeding out false vertexes
   * e.g. for max angle 0.9 => 154 deg [= 360*math.acos(-0.9)/(2*math.pi)]
   */
#define DPMAX_ANGLE 0.7 
  
  /**
   * The threshold we use to determine if we should add an extra edge
   */
#define DPMAX_THRESH 0.00085

  float PolygonQuadTangle::DPAngle(const std::pair<float,float>& p, 
				   const std::pair<float,float>& q,
				   const std::pair<float,float>& r) {

    float ax = p.first-q.first;
    float ay = p.second-q.second;
    float bx = r.first-q.first;
    float by = r.second-q.second;
    
    float dot = ax*bx+ay*by;
    float denom = sqrt(ax*ax+ay*ay)+sqrt(bx*bx+by*by);

    float result = (denom < 1e-8) ? -1 : dot/denom;
#ifdef POLYGON_DEBUG
    PROGRESS("Angle between (" << p.first << "," << p.second << "), (" << q.first << "," << q.second << "), (" <<r.first << "," << r.second << ") is " << result);
#endif
    return result;
  }
  
  void PolygonQuadTangle::DPJoin(std::list<std::pair<float,float> >& fulllist,
				 std::list<std::pair<float,float> >::iterator start,
				 std::list<std::pair<float,float> >::iterator mid,
				 std::list<std::pair<float,float> >::iterator end) {

#ifdef POLYGON_DEBUG
    int start_index = -1;
    int mid_index = -1;
    int end_index = -1;
    int counter = 0;
    for(std::list<std::pair<float,float> >::iterator i = fulllist.begin();i!=fulllist.end();++i) {
      if (i == start) start_index = counter;
      if (i == mid) mid_index = counter;
      if (i == end) end_index = counter;
      ++counter;
    }
    if (start_index == -1) start_index = counter;
    if (mid_index == -1) mid_index = counter;
    if (end_index == -1) end_index = counter;
    PROGRESS("DPJoin with list 1 from " << start_index << " to " << mid_index << " (exclusive) and list 2 from " << mid_index << " to " << end_index << " (exclusive)");
#endif

    // keep a copy of this iterator so we can work forwards with the second list
    std::list<std::pair<float,float> >::iterator fwd_mid = mid;

    // move back to the last element of the first list
    --mid;

    std::list<std::pair<float,float> >::iterator z = mid;
#ifdef POLYGON_DEBUG
    PROGRESS("z is (" << z->first << "," << z->second << ")");
#endif
    // a is the first element of the second list
    std::list<std::pair<float,float> >::iterator a = fwd_mid;
#ifdef POLYGON_DEBUG
    PROGRESS("a is (" << a->first << "," << a->second << ")");
#endif

    // check if we have more than one element in list 1
    if (mid != start) {
      --mid;
      std::list<std::pair<float,float> >::iterator y = mid;      
#ifdef POLYGON_DEBUG
      PROGRESS("y is (" << y->first << "," << y->second << ")");
#endif
      float yza = DPAngle(*y,*z,*a);
#ifdef POLYGON_DEBUG
      PROGRESS("Angle yza is " << yza);
#endif
      if (fabs(yza) > DPMAX_ANGLE) {
#ifdef POLYGON_DEBUG
	PROGRESS("Erasing z");
#endif
	fulllist.erase(z);
	z = y;
      }
    }
    else {
#ifdef POLYGON_DEBUG
      PROGRESS("We have only one element in list 1");
#endif
    }


    ++fwd_mid;
    // check we have more than one element in list 2
    if (fwd_mid != end) {
      std::list<std::pair<float,float> >::iterator b = fwd_mid;
#ifdef POLYGON_DEBUG
      PROGRESS("b is (" << b->first << "," << b->second << ")");
#endif
      float zab = DPAngle(*z,*a,*b);
#ifdef POLYGON_DEBUG
      PROGRESS("Angle zab is " << zab);
#endif
      if (fabs(zab) > DPMAX_ANGLE) {
#ifdef POLYGON_DEBUG
	PROGRESS("Erasing a");
#endif
	fulllist.erase(a);    
      }
    }
    else {
#ifdef POLYGON_DEBUG
      PROGRESS("We have only one element in list 2");
#endif
    }

  }

  std::list<std::pair<float,float> >::iterator
  PolygonQuadTangle::DPSplit(std::list<std::pair<float,float> >& fulllist,
			     std::list<std::pair<float,float> >::iterator start,
			     std::list<std::pair<float,float> >::iterator end) {

#ifdef POLYGON_DEBUG
    int start_index = -1;
    int end_index = -1;
    int counter =0;
    for(std::list<std::pair<float,float> >::iterator i = fulllist.begin();i!=fulllist.end();++i) {
      if (i==start) start_index = counter;
      if (i==end) end_index = counter;
      ++counter;
    }
    if (start_index == -1) start_index = counter;
    if (end_index == -1) end_index = counter;
    PROGRESS("Splitting range " << start_index << " to " << end_index);
#endif
    
    assert(start != end);

    // move end back to the last element of the list
    --end;
    
    if (start == end) {
#ifdef POLYGON_DEBUG
      PROGRESS("DPSplit 1 element list - returning");
#endif
      return start;
    }

    float maxd = 0;
    std::list<std::pair<float,float> >::iterator split_iterator = start;

    float fx = start->first;
    float fy = start->second;
    float lx = end->first;
    float ly = end->second;
    
    //    std::cerr << " Size:" << a.size() << std::endl;
    //    std::cerr << " Vals:" << fx << " " << fy << " " << lx << " " << ly << std::endl;
    
    for(;start != end; ++start) {
      float px = start->first;
      float py = start->second;

      float a = (fx-px)*(fx-px)+(fy-py)*(fy-py);
      float b = (px-lx)*(px-lx)+(py-ly)*(py-ly);
      float c = (fx-lx)*(fx-lx)+(fy-ly)*(fy-ly);
      float d = 0;
      
      try {
	//do we want fabs(sqrt(...)) here?
	d = sqrt(b-(((b+c-a)*(b+c-a))/(4*c)));
      } catch (...) {
	d = 0;
      }
      
      //std::cerr << "a,b,c,d " << a << " " << b << " " << c << " " << d << std::endl;
      
      if (d > maxd) {
	split_iterator = start;
	maxd=d;
      }
    }
    
#ifdef POLYGON_DEBUG
    PROGRESS("maxd = " << maxd);
    counter = 0;
    int split_index = -1;
    for(std::list<std::pair<float,float> >::iterator i = fulllist.begin();i!=fulllist.end();++i) {
      if (i==split_iterator) split_index = counter;
      ++counter;
    }
    if (split_index == -1) split_index = counter;
    PROGRESS("Split index is " << split_index);
#endif

    //    std::cerr << " - DPSplit: maxi = (" << maxi->first << "," << maxi->second 
    //	      << "), d = " << maxd << " tmpmaxi = "<< tmpmaxi << std::endl;
    
    return split_iterator;
  }
  
  void PolygonQuadTangle::DPRecurse(std::list<std::pair<float,float> >& fulllist,
				    std::list<std::pair<float,float> >::iterator start,
				    std::list<std::pair<float,float> >::iterator end) {

    if (start == end) {
#ifdef POLYGON_DEBUG
      PROGRESS("DPRecurse Empty list - returning");
#endif
      return;
    }
    
    std::list<std::pair<float,float> >::iterator split_iterator = DPSplit(fulllist,start,end);
    if (split_iterator != start) { // we split the list
#ifdef POLYGON_DEBUG
      PROGRESS("DPRecurse Split list");
#endif
      DPRecurse(fulllist,start,split_iterator);
      DPRecurse(fulllist,split_iterator,end);
      DPJoin(fulllist,start,split_iterator,end);
    }
    else { // we didn't split the list so we remove everything except the first and last element
      ++start;
      if (start == end) { // its only a 1 element list so nothing to do
#ifdef POLYGON_DEBUG
	PROGRESS("DPRecurse 1-element list - returning");
#endif
	return;
      }
      --end;
      if (start == end) { // its only a 2 element list so nothing to do
#ifdef POLYGON_DEBUG
	PROGRESS("DPRecurse 2-element list - returning");
#endif
	return;
      }
#ifdef POLYGON_DEBUG
      PROGRESS("DPRecurse erasing middle of list");
#endif
      fulllist.erase(start,end);
    }
  }
  
  bool PolygonQuadTangle::Fit(const std::vector<float>& points) {
    if (points.size() > 20) {

      std::list<std::pair<float,float> > fulllist;
      
      std::list<std::pair<float,float> >::iterator maxi;
      float maxd=0;

      float firstx=*points.begin();
      float firsty=*++points.begin();
    
      std::cerr << "Size:" << points.size() << std::endl;

      std::cout << "Points"<< std::endl;
      //must copy since we want to modify this datastructure and vector is const!
      int index =0 ;
      for(std::vector<float>::const_iterator i = points.begin(); i != points.end(); ++i) {
	float x = *i;
	++i;
	float y = *i;
	std::cout << index++ << " " << x << " " << y << std::endl;
	fulllist.push_back(std::pair<float,float>(x,y));
	float d = (firstx-x)*(firstx-x)+(firsty-y)*(firsty-y);
	if (d > maxd) {
	  maxi = --fulllist.end();
	  maxd = d;
	}
      }
    
      ++maxi;
      DPRecurse(fulllist,fulllist.begin(),maxi);
      DPRecurse(fulllist,maxi,fulllist.end());
      DPJoin(fulllist,fulllist.begin(),maxi,fulllist.end());
    
      //Since we now know that this is a closed polygon, must check
      //that both ends of the polygon are joined
      if (fulllist.size() > 2) {
	float d = DPAngle(*fulllist.rbegin(),*fulllist.begin(),*++fulllist.begin());
	if (fabs(d) > DPMAX_ANGLE) {
	  fulllist.pop_front();
	}
      }
      if (fulllist.size() > 2) {
	float d = DPAngle(*++fulllist.rbegin(),*fulllist.rbegin(),*fulllist.begin());
	if (fabs(d) > DPMAX_ANGLE) {
	  fulllist.pop_back();
	}
      }    

      std::cerr << "Finished:" << fulllist.size() << std::endl;

      for(std::list<std::pair<float,float> >::iterator i=fulllist.begin();
	  i != fulllist.end(); ++i)
	std::cout << i->first << " " << i->second << std::endl;


      if (fulllist.size() == 4) {
	std::list<std::pair<float,float> >::iterator i = fulllist.begin();
	m_x0 = i->first;
	m_y0 = i->second;
	m_x1 = ++i->first;
	m_y1 = i->second;
	m_x2 = ++i->first;
	m_y2 = i->second;
	m_x3 = ++i->first;
	m_y3 = i->second;
	compute_central_point();
	sort_points();
	return true;
      }
    }
    return false;
  }
}
