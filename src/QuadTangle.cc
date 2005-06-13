/**
 * $Header$
 */

#include <total/Config.hh>
#include <total/QuadTangle.hh>
#include <total/gaussianelimination.hh>

#include <cmath>
#include <cassert>
#include <map>

#include <deque>
#include <vector>

#define COMPARE_THRESH 1

#undef QUADTANGLE_DEBUG
#undef POLYGON_DEBUG

#define LOGMAXWINDOW 5
#define CURVTHRESH -0.8

namespace Total {

#define TOTAL_MASK(x) ((x) & ((1<<LOGMAXWINDOW)-1))

  QuadTangle::QuadTangle() {}

  QuadTangle::QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3) : m_x0(x0), m_y0(y0),m_x1(x1), m_y1(y1),m_x2(x2), m_y2(y2),m_x3(x3), m_y3(y3) {
    compute_central_point();
    sort_points();
  }


  void QuadTangle::Draw(Image& image,const Camera& camera) const {
      camera.DrawQuadTangle(image,*this);
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

  /**
   * \todo tidy this function up 
   */
  bool QuadTangle::EstimatePoseQuadrant(float n[3]) const {

    float p[] = {m_x0,m_y0,m_x1,m_y1,m_x2,m_y2,m_x3,m_y3};
    // Now find where a vertical line through the centre intersects the shape
    int side1=0;
    float mu = (p[8]-p[2*side1])/(p[2*(side1+1)]-p[2*side1]);
    if (mu!=mu || mu <=0.0 || mu > 1.0) {
      // It doesn't intersect this side: must intersect the next
      side1++;
      mu = (p[8]-p[2*side1])/(p[2*(side1+1)]-p[2*side1]);
    }
    int side2=(side1+2)%4;
    float mu2 = (p[8]-p[2*side2])/(p[2*(side2+1)]-p[2*side2]);

    // y co-ordinates: one will be -ve, one +ve
    float d1 = p[2*side1+1] + mu*(p[2*(side1+1)+1]-p[2*side1+1]) - p[9];
    float d2 = p[2*side2+1] + mu2*(p[2*(side2+1)+1]-p[2*side2+1]) - p[9];
    float yratio = (d1>0) ? -d1/d2:-d2/d1;

    // Now find where a horizontal line through the centre intersects the shape
    int sideh = side1+1;
    mu = (p[9]-p[2*sideh+1])/(p[2*(sideh+1)+1]-p[2*sideh+1]);
    d1 = p[2*sideh] + mu*(p[2*(sideh+1)]-p[2*sideh]) - p[8];
    
    sideh = (sideh+2)%4;
    mu = (p[9]-p[2*sideh+1])/(p[2*((sideh+1)%4)+1]-p[2*sideh+1]);
    d2 = p[2*sideh] + mu*(p[2*((sideh+1)%4)]-p[2*sideh]) - p[8];

    float xratio = (d1>0) ? -d1/d2:-d2/d1;
 
 //    if (xratio>0.99 && xratio<1.01 && yratio>0.99 && yratio<1.01) {
//       // It's a rectangle
//       float size1 = (m_x0-p[2])*(p[0]-p[2]) + (p[1]-p[3])*(p[1]-p[3]);
//       float size2 = (p[2]-p[4])*(p[2]-p[4]) + (p[3]-p[5])*(p[3]-p[5]);
//       // if (size1/size2 !=1.0) return false;
//     }


    if (xratio > 1.0)  n[0]=1.0;
    else if (xratio == 1.0) n[0]=0.0;
    else n[1]=-1.0;

    if (yratio > 1.0) n[1]=1.0;
    else if (yratio == 1.0) n[1]=0.0;
    else n[1]=-1.0;

    n[2]=1.0;

    float m = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    for (int i=0; i<3;i++) n[i]/=m;
    return true;
  }

  /**
   * \todo Check for fitted
   */
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
    //    m_fitted = (bool)socket.RecvInt();
  }


  /**
   * \todo Check for fitted
   */
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
    //  count += socket.Send((int)m_fitted);
    return count;
  }
}
