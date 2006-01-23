/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */

#include <cantag/Config.hh>
#include <cantag/QuadTangle.hh>
#include <cantag/gaussianelimination.hh>

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

namespace Cantag {

#define TOTAL_MASK(x) ((x) & ((1<<LOGMAXWINDOW)-1))

  QuadTangle::QuadTangle() {}

  QuadTangle::QuadTangle(float x0, float y0,
			 float x1, float y1,
			 float x2, float y2,
			 float x3, float y3,
			 int index_0,
			 int index_1,
			 int index_2,
			 int index_3) : m_x0(x0), m_y0(y0),m_x1(x1), m_y1(y1),m_x2(x2), m_y2(y2),m_x3(x3), m_y3(y3),
					m_index_0(index_0), m_index_1(index_1), m_index_2(index_2), m_index_3(index_3) {
    compute_central_point();
    sort_points();
  }

  void QuadTangle::Set(float x0, float y0,
		       float x1, float y1,
		       float x2, float y2,
		       float x3, float y3,
		       int index_0,
		       int index_1,
		       int index_2,
		       int index_3) {
    m_x0 = x0; m_y0 = y0; m_x1 = x1; m_y1 = y1; m_x2 = x2; m_y2 = y2; m_x3 = x3; m_y3 = y3;
    m_index_0 = index_0; m_index_1 = index_1; m_index_2 = index_2; m_index_3 = index_3;
    compute_central_point();
    sort_points();
  }


  void QuadTangle::Draw(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image,const Camera& camera) const {
      camera.DrawQuadTangle(image,*this);
  }

  void QuadTangle::Interpolate(std::vector<float>& points, float start, float end, float index, short steps) const {
    float val = start + index * (end-start)/(float)steps;
    points.push_back( val );
  }
  
  void QuadTangle::Draw(std::vector<float>& points, short steps) const {
    points.push_back(m_x0); points.push_back(m_y0);
    for(short i=1;i<steps;++i) {
      Interpolate(points,m_x0,m_x1,i,steps);
      Interpolate(points,m_y0,m_y1,i,steps);
    }
    points.push_back(m_x1); points.push_back(m_y1);
    for(short i=1;i<steps;++i) {
      Interpolate(points,m_x1,m_x2,i,steps);
      Interpolate(points,m_y1,m_y2,i,steps);
    }
    points.push_back(m_x2); points.push_back(m_y2);
    for(short i=1;i<steps;++i) {
      Interpolate(points,m_x2,m_x3,i,steps);
      Interpolate(points,m_y2,m_y3,i,steps);
    }
    points.push_back(m_x3); points.push_back(m_y3);
    for(short i=1;i<steps;++i) {
      Interpolate(points,m_x3,m_x0,i,steps);
      Interpolate(points,m_y3,m_y0,i,steps);
    }
  }

  static float dist(float x0, float y0, float x1, float y1) {
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
      return FLT_PI/2.f+ atan( (y-cy) / (x-cx) );
    }
    else if ((x >= cx) && (y < cy)) {
      return atan( (cy-y) / (x-cx) );
    }
    else if ((x < cx) && (y < cy)) {
      return 3*FLT_PI/2.f + atan ( (cy-y) / (cx-x) );
    }
    else if ((x < cx) && (y >= cy)) {
      return FLT_PI + atan( (y-cy) / (cx-x));
    }
    assert(false);
    return 0.f; // to get rid of a warning from ICC
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
      float x0=m_x0, y0=m_y0; int index_0 = m_index_0;
      float x1=m_x1, y1=m_y1; int index_1 = m_index_1;
      float x2=m_x2, y2=m_y2; int index_2 = m_index_2;
      float x3=m_x3, y3=m_y3; int index_3 = m_index_3;

      m_x0= x0; m_y0=y0; m_index_0 = index_0;
      m_x1= x3; m_y1=y3; m_index_1 = index_3;
      m_x2= x2; m_y2=y2; m_index_2 = index_2;
      m_x3= x1; m_y3=y1; m_index_3 = index_1;
    }
  }

  /**
   * \todo tidy this function up 
   */
  bool QuadTangle::EstimatePoseQuadrant(float n[3]) const {

    float p[] = {m_x0,m_y0,m_x1,m_y1,m_x2,m_y2,m_x3,m_y3,m_xc,m_yc};
    // Now find where a vertical line through the centre intersects the shape
    int side1=0;
    float mu = (p[8]-p[2*side1])/(p[2*(side1+1)]-p[2*side1]);
    if (mu <=0.f || mu > 1.f) {
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

    float xratio = (d1>0.f) ? -d1/d2:-d2/d1;
 
 //    if (xratio>0.99 && xratio<1.01 && yratio>0.99 && yratio<1.01) {
//       // It's a rectangle
//       float size1 = (m_x0-p[2])*(p[0]-p[2]) + (p[1]-p[3])*(p[1]-p[3]);
//       float size2 = (p[2]-p[4])*(p[2]-p[4]) + (p[3]-p[5])*(p[3]-p[5]);
//       // if (size1/size2 !=1.0) return false;
//     }


    if (xratio > 1.f)  n[0]=1.f;
    else if (abs(xratio-1.f)<=DBL_EPSILON) n[0]=0.f;
    else n[1]=-1.f;

    if (yratio > 1.f) n[1]=1.f;
    else if (abs(yratio-1.f)<=FLT_EPSILON) n[1]=0.f;
    else n[1]=-1.f;

    n[2]=1.f;

    float m = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    for (int i=0; i<3;i++) n[i]/=m;
    return true;
  }

  bool QuadTangle::CheckError(const std::vector<float>& points, float threshold) const {
    double edge1 = (m_x1 - m_x0)*(m_x1 - m_x0) + (m_y1 - m_y0)*(m_y1 - m_y0);
    double edge2 = (m_x2 - m_x1)*(m_x2 - m_x1) + (m_y2 - m_y1)*(m_y2 - m_y1);
    double edge3 = (m_x3 - m_x2)*(m_x3 - m_x2) + (m_y3 - m_y2)*(m_y3 - m_y2);
    double edge4 = (m_x0 - m_x3)*(m_x0 - m_x3) + (m_y0 - m_y3)*(m_y0 - m_y3);

    for(std::vector<float>::const_iterator i = points.begin(); i != points.end(); ++i) {
      float x = *i;
      ++i;
      float y = *i;
      double distance1 = ((x - m_x0)*(m_x1 - m_x0) + (y - m_y0)*(m_y1 - m_y0))/edge1;
      double distance2 = ((x - m_x1)*(m_x2 - m_x1) + (y - m_y1)*(m_y2 - m_y1))/edge2;
      double distance3 = ((x - m_x2)*(m_x3 - m_x2) + (y - m_y2)*(m_y3 - m_y2))/edge3;
      double distance4 = ((x - m_x3)*(m_x0 - m_x3) + (y - m_y3)*(m_y0 - m_y3))/edge4;
      
      double min = distance1;
      if (min > distance2) min = distance2;
      if (min > distance3) min = distance3;
      if (min > distance4) min = distance4;
      
      if (min > threshold) return false;
    }
    return true;
  }
}
