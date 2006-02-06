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
#include <cantag/Ellipse.hh>
#include <cantag/gaussianelimination.hh>
#include <cantag/polysolve.hh>
#include <cantag/SpeedMath.hh>
#include <iostream>

#include <cmath>
#include <cassert>

#ifdef TEXT_DEBUG
# undef ELLIPSE_DEBUG
# undef ELLIPSE_DEBUG_DUMP_POINTS
# undef CIRCLE_TRANSFORM_DEBUG
# undef DECOMPOSE_DEBUG
#endif

#define MAXFITERROR 1000
#define COMPARETHRESH 1
#define MAXDISTANCE 

namespace Cantag {


  Ellipse::Ellipse(): m_fitted(false) {}
  
  Ellipse::Ellipse(float a, float b, float c, float d, float e, float f) :
    m_a(a), m_b(b), m_c(c), m_d(d), m_e(e), m_f(f), m_fitted(true) { Decompose();}

  Ellipse::Ellipse(float x0, float y0, float width, float height, float angle) :
    m_x0(x0), m_y0(y0),m_angle_radians(angle), m_width(width),m_height(height),m_fitted(true) { 
    assert(width >= height);
    Compose(); 
  }

  void Ellipse::Draw(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image,const Camera& camera) const {
    if (m_fitted) {
      camera.DrawEllipse(image,*this);
    }
  }
  void Ellipse::Draw(std::vector<float>& points) const {
    /**
     * The parametric equation for an ellipse is
     *
     * x = a*cos(t)
     * y = b*sin(t)
     *
     * We wish to rotate by angle_radians anti-clockwise so premultiply by a rotation matrix    
     *
     * x = xc + a*cos(angle_radians)*cos(t) - b*sin(angle_radians)*sin(t)
     * y = yc + a*sin(angle_radians)*cos(t) + b*cos(angle_radians)*sin(t)
     *
     * a = width/2; b=height/2
     * angle_radians is the angle between the axis given by width and the horizontal
     *
     */
    //    float cosa = DCOS(8,m_angle_radians); // DCOS (later)
    //    float sina = DSIN(8,m_angle_radians); // DSINE (later)
    float cosa = cos(m_angle_radians);
    float sina = sin(m_angle_radians);
    float a = m_width;
    float b = m_height;
    float currentAngle = 0.f;
    while(currentAngle < FLT_PI*2) {
      float cost = cos(currentAngle); // DCOS (later)
      float sint = sin(currentAngle); // DSINE (later)
      
      float pointsv[] = { m_x0 + a*cosa*cost - b*sina*sint,
			  m_y0 + a*sina*cost + b*cosa*sint };
      
      points.push_back(pointsv[0]);
      points.push_back(pointsv[1]);
      currentAngle += 0.01f;
    }
  }

  bool Ellipse::Compare(const Ellipse& o) const {
    /**
     *  \todo{this is really naive because the ellipse equation is good
     *  up to a scale factor.  Lets fix it later...}
     */
    return ((abs(m_a - o.m_a) < COMPARETHRESH) &&
	    (abs(m_b - o.m_b) < COMPARETHRESH) &&
	    (abs(m_c - o.m_c) < COMPARETHRESH) &&
	    (abs(m_d - o.m_d) < COMPARETHRESH) &&
	    (abs(m_e - o.m_e) < COMPARETHRESH) &&
	    (abs(m_f - o.m_f) < COMPARETHRESH));
  }

  bool Ellipse::CheckError(const std::vector<float>& points) const {
    return GetError(points) < MAXFITERROR;
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
      float dist = abs(m_a*x*x+m_b*x*y+m_c*y*y+m_d*x+m_e*y+m_f);
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
    float total=0.f;
    float maxdist = 0.f;

    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {  
      float x = *i;
      ++i;
      float y = *i;
      float dist = abs(m_a*x*x+m_b*x*y+m_c*y*y+m_d*x+m_e*y+m_f);
    
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
    return total/(float)points.size();
#endif
  }

  float Ellipse::GetErrorNakagawa(const std::vector<float>& points) const {
  
    float total=0.f;
    float max_dist =0.f;
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
    
      float d = abs((ix-xi)*(ix-xi) + (iy-yi)*(iy-yi));
      if (d > max_dist) { max_dist = d; }
      total+=d;
    }
  
#ifdef ELLIPSE_DEBUG
    PROGRESS("Total error from Nakagawa Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif

#ifdef MAXDISTANCE
    return max_dist;
#else
    return total/(float)points.size();
#endif
  }

  float Ellipse::GetErrorSafaeeRad(const std::vector<float>& points) const {
    // draw a ray between each point and the centre of the ellipse C
    // intersecting the ellipse at Ij.  The lengths of the bisected
    // portions of the ray mj and nj are determined

    float total=0.f;
    float max_dist = 0.f;
    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ) {  
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

      float dist =  abs(m*(1+ n/2/a)/(1+ n/2/m)*q);
      if (dist > max_dist) { max_dist = dist; }
      total += dist;
    }

#ifdef ELLIPSE_DEBUG
    PROGRESS("Total error from SafaeeRad Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif

#ifdef MAXDISTANCE
    return max_dist;
#else
    return total/(float)points.size();
#endif
  }

  float Ellipse::GetErrorSafaeeRad2(const std::vector<float>& points) const {
    float total=0.f;
    float max_dist = 0.f;
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

      float dist = abs(m*q);

      if (dist > max_dist) { max_dist = dist; }
      total += dist;
    }

#ifdef ELLIPSE_DEBUG
    PROGRESS("Total error from SafaeeRad2 Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif
#ifdef MAXDISTANCE
    return (float)max_dist;
#else
    return (float)total/(float)points.size();
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
    
      theta=FLT_PI/2.f-theta;
    }

    float c = sqrt(a*a-b*b);

    float f1x = x0 + c*cos(theta);
    float f1y = y0 + c*sin(theta);

    float f2x = x0 - c*cos(theta);
    float f2y = x0 - c*sin(theta);

    float modf1_f2 = sqrt( (f1x-f2x)*(f1x-f2x) + (f1y-f2y)*(f1y-f2y) );

    float total =0.0;
    float max_dist =0.0 ;
    for (std::vector<float>::const_iterator i = points.begin();
	 i != points.end();
	 ++i) {  
      float x = *i;
      ++i;
      float y = *i;

      float temp = sqrt( (x-f1x)*(x-f1x) + (y-f1y)*(y-f1y) );
      float temp2 = sqrt( (x-f2x)*(x-f2x) + (y-f2y)*(y-f2y) );

      float aest = 0.5f * ( temp + temp2 );
      float best = sqrt(aest*aest - a*a + b*b);

      float dest = 0.5f *(aest - a + best - b);

      float ctilde = sqrt( (a+dest)*(a+dest) - (b+dest)*(b+dest) );

      float f1xtilde = (f1x + f2x)/2+ctilde*(f1x-f2x)/modf1_f2;
      float f1ytilde = (f1y + f2y)/2+ctilde*(f1y-f2y)/modf1_f2;

      float f2xtilde = (f1x + f2x)/2-ctilde*(f1x-f2x)/modf1_f2;
      float f2ytilde = (f1y + f2y)/2-ctilde*(f1y-f2y)/modf1_f2;

      float temp4 = sqrt( (x-f1xtilde)*(x-f1xtilde) + (y-f1ytilde)*(y-f1ytilde) );
      float temp5 = sqrt( (x-f2xtilde)*(x-f2xtilde) + (y-f2ytilde)*(y-f2ytilde) );
      float atilde = 0.5f * ( temp4 + temp5 );

      float dist = abs(atilde - a);
    
      if (dist > max_dist) { max_dist = dist; }
      total += dist;
    }

#ifdef ELLIPSE_DEBUG
    PROGRESS("Total error from Stricker Method is "<< total/points.size()<< " maximimum distance was " << max_dist);
#endif
#ifdef MAXDISTANCE
    return max_dist;
#else
    return total/(float)points.size();
#endif
  }

  void Ellipse::Compose() {
    float x0 = m_x0;
    float y0 = m_y0;
    float alpha1sq = m_width*m_width;
    float alpha2sq = m_height*m_height;

    float angle = m_angle_radians;

    //    float c = DCOS(8,angle); // DCOS
    //    float s = DSIN(8,angle); // DSINE
    float c = (float)cos(angle); // DCOS
    float s = (float)sin(angle); // DSINE
  
    m_a = c*c/alpha1sq + s*s/alpha2sq;
    m_b = 2.f*c*s*(1.f/alpha1sq - 1.f/alpha2sq);
    m_c = s*s/alpha1sq + c*c/alpha2sq;
    m_d = -2.f*x0*m_a - y0*m_b;
    m_e = -x0*m_b - 2.f*y0*m_c;
    m_f = x0*x0*m_a + x0*y0*m_b + y0*y0*m_c - 1.f;

  }

  /**
   * This function must be implemented with double precision (64-bit) floats
   */
  void Ellipse::Decompose() {
    double a = GetA();
    double b = GetB();
    double c = GetC();
    double d = GetD();
    double e = GetE();
    double f = GetF();
    
#ifdef DECOMPOSE_DEBUG
    PROGRESS("a,b,c,d,e,f = " << a << " " << b << " " << c << " " << d << " " << e << " " << f);
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

    double disc = b*b - 4.0*a*c;

    double x0 = (2.0*c*d - b*e) / disc;
    double y0 = (2.0*a*e - b*d) / disc;

    double tmproot = sqrt( (a-c)*(a-c) + b*b );
    double lambda1 = ((a+c) - tmproot)/2.0;
    double lambda2 = ((a+c) + tmproot)/2.0;
    bool swapped = false;
    if (lambda1 > lambda2) {
      swapped = true;
      double lambda1t = lambda2;
      lambda2 = lambda1;
      lambda1 = lambda1t;
    }

#ifdef DECOMPOSE_DEBUG
    PROGRESS("tmproot= " << tmproot);
    PROGRESS("lambda1= " << lambda1);
    PROGRESS("lambda2= " << lambda2);
#endif
  
    double scale_factor = sqrt( -f + a*x0*x0 + b*x0*y0 + c*y0*y0);

#ifdef DECOMPOSE_DEBUG
    PROGRESS("scale= " << scale_factor);
    PROGRESS("dbl= " << DBL_EPSILON);
    PROGRESS("flt= " << FLT_EPSILON);
#endif
  
    m_width = scale_factor / sqrt(lambda1);
    m_height = scale_factor / sqrt(lambda2);
    m_x0 = x0;
    m_y0 = y0;

    
    if (abs(m_width - m_height) <= DBL_EPSILON) {
      // obviously the angle is undefined if we have a circle
      m_angle_radians = 0.0;
    }
    else if (abs(b) > DBL_EPSILON) { 
	m_angle_radians = atan2( 2.0*(lambda1-a), b ); // DATAN	
    }
    else {
      m_angle_radians = 0.0;
    }
#ifdef DECOMPOSE_DEBUG
    PROGRESS("width= " << m_width);
    PROGRESS("height= " <<m_height);
    PROGRESS("X= " << m_x0);
    PROGRESS("Y= " << m_y0);
    PROGRESS("angle= " << m_angle_radians << " (" << (m_angle_radians/FLT_PI*180.0) << ")");
    PROGRESS("swap = " << swapped);
#endif
  
  }

  bool Ellipse::Check(const std::vector<float>& points) const {
    return (GetError(points) < MAXFITERROR); 
  }
}
