/**
 * $Header$
 */

#include <total/Config.hh>
#include <total/QuadTangle.hh>
#include <total/gaussianelimination.hh>

#include <cmath>
#include <cassert>
#include <iostream>
#define COMPARE_THRESH 1

#undef QUADTANGLE_DEBUG

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
    
    if (denom < 1e-8)
      return -1;
    else
      return dot/denom;
  }
  
  void PolygonQuadTangle::DPJoin(std::list<std::pair<float,float> >& fulllist,
				 std::list<std::pair<float,float> >::iterator start,
				 std::list<std::pair<float,float> >::iterator mid,
				 std::list<std::pair<float,float> >::iterator end) {

    // keep a copy of this iterator so we can work forwards with the second list
    std::list<std::pair<float,float> >::iterator fwd_mid = mid;

    // move back to the last element of the first list
    --mid;

    std::list<std::pair<float,float> >::iterator z = mid;

    // a is the first element of the second list
    std::list<std::pair<float,float> >::iterator a = fwd_mid;

    // check if we have more than one element in list 1
    if (mid != start) {
      --mid;
      std::list<std::pair<float,float> >::iterator y = mid;      
      float yza = DPAngle(*y,*z,*a);
      if (fabs(yza) > DPMAX_ANGLE) {
	fulllist.erase(z);
	z = y;
      }
    }


    ++fwd_mid;
    // check we have more than one element in list 2
    if (fwd_mid != end) {
      std::list<std::pair<float,float> >::iterator b = fwd_mid;
      float zab = DPAngle(*z,*a,*b);
      if (fabs(zab) > DPMAX_ANGLE) {
	fulllist.erase(a);    
      }
    }

  }

  std::list<std::pair<float,float> >::iterator
  PolygonQuadTangle::DPSplit(std::list<std::pair<float,float> >& fulllist,
			     std::list<std::pair<float,float> >::iterator start,
			     std::list<std::pair<float,float> >::iterator end) {
    
    // move end back to the last element of the list
    --end;
    
    assert(start != end);

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
    
    //    std::cerr << " - DPSplit: maxi = (" << maxi->first << "," << maxi->second 
    //	      << "), d = " << maxd << " tmpmaxi = "<< tmpmaxi << std::endl;
    
    return split_iterator;
  }
  
  void PolygonQuadTangle::DPRecurse(std::list<std::pair<float,float> >& fulllist,
				    std::list<std::pair<float,float> >::iterator start,
				    std::list<std::pair<float,float> >::iterator end) {
    
    
    std::list<std::pair<float,float> >::iterator split_iterator = DPSplit(fulllist,start,end);
    if (split_iterator != start) { // we split the list
      DPRecurse(fulllist,start,split_iterator);
      DPRecurse(fulllist,split_iterator,end);
      DPJoin(fulllist,start,split_iterator,end);
    }
    else { // we didn't split the list so we remove everything except the first and last element
      ++start;
      if (start == end) { // its only a 1 element list so nothing to do
	return;
      }
      --end;
      if (start == end) { // its only a 2 element list so nothing to do
	return;
      }
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

      //must copy since we want to modify this datastructure and vector is const!
      for(std::vector<float>::const_iterator i = points.begin(); i != points.end(); ++i) {
	float x = *i;
	++i;
	float y = *i;
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
