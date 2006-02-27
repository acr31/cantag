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


#include <iostream>

#ifdef HAVE_GSL_MULTIMIN_H
#ifdef HAVELIB_GSLCBLAS
#ifdef HAVELIB_GSL
#include <gsl/gsl_multimin.h>
#endif
#endif
#endif

#include <cantag/Camera.hh>

namespace Cantag {
  Camera::Camera() :
    m_r2(0),m_r4(0),m_r6(0),
    m_d1(0),m_d2(0)
  {
    m_extrinsic[0] = 1;    m_extrinsic[1] = 0;    m_extrinsic[2] = 0;   m_extrinsic[3] = 0;    
    m_extrinsic[4] = 0;    m_extrinsic[5] = 1;    m_extrinsic[6] = 0;   m_extrinsic[7] = 0;
    m_extrinsic[8] = 0;    m_extrinsic[9] = 0;    m_extrinsic[10] = 1;   m_extrinsic[11] = 0;  
    m_extrinsic[12] = 0;    m_extrinsic[13] = 0;    m_extrinsic[14] = 0;   m_extrinsic[15] = 1;  

    m_intrinsic[0] = 1;    m_intrinsic[1] = 0;    m_intrinsic[2] = 0.5;  
    m_intrinsic[3] = 0;    m_intrinsic[4] = 1;    m_intrinsic[5] = 0.5;  
    m_intrinsic[6] = 0;    m_intrinsic[7] = 0;    m_intrinsic[8] = 1;  
  }

  Camera::~Camera() {}

  void Camera::SetExtrinsic(float extrinsic[16]) {
    for(int i=0;i<16;i++) {
      m_extrinsic[i] = extrinsic[i];
    }
  }

  void Camera::SetExtrinsic(const Transform& extrinsic) {
    for(int i=0;i<16;i++) {
      m_extrinsic[i] = extrinsic[i];
    }
  }

  void Camera::SetIntrinsic(float intrinsic[9]) {
    for(int i=0;i<9;i++) {
      m_intrinsic[i] = intrinsic[i];
    }
  }

  void Camera::SetIntrinsic(float xscale, float yscale, float principlex, float principley, float skew) {
    m_intrinsic[0] = xscale;          m_intrinsic[1] = skew;            m_intrinsic[2] = principlex;
    m_intrinsic[3] = 0;               m_intrinsic[4] = yscale;          m_intrinsic[5] = principley;
    m_intrinsic[6] = 0;               m_intrinsic[7] = 0;               m_intrinsic[8] = 1;  
  }

  void Camera::SetRadial(float r2,  float r4,  float r6) {
    m_r2 = r2; 
    m_r4 = r4; 
    m_r6 = r6;  
  }

  void Camera::SetTangential(float d1, float d2) {
    m_d1 = d1;
    m_d2 = d2;
  }


  void Camera::CameraToWorld(float x, float y, float z, float *rx, float *ry, float* rz) const {
    float rh = m_extrinsic[12]*x+m_extrinsic[13]*y+m_extrinsic[14]*z+m_extrinsic[15];
    *rx = (m_extrinsic[0]*x+m_extrinsic[1]*y+m_extrinsic[2]*z+m_extrinsic[3])/rh;
    *ry = (m_extrinsic[4]*x+m_extrinsic[5]*y+m_extrinsic[6]*z+m_extrinsic[7])/rh;
    *rz = (m_extrinsic[8]*x+m_extrinsic[9]*y+m_extrinsic[10]*z+m_extrinsic[11])/rh;
  }

  void Camera::CameraToWorld(float* points, int numpoints) const {
    for(int i=0;i<numpoints*3;i+=3) {
      CameraToWorld(points[i],points[i+1],points[i+2],points+i,points+i+1,points+i+2);
    }
  }


  /**
   * \todo currently ignores tangential parameters and r^6 coeff.
   * since inversion of these quantities is not straightforward
   */
  void Camera::NPCFToImage(float* points, int numpoints) const {
    for(int i=0;i<numpoints*2;i+=2) {
      float x = points[i];
      float y = points[i+1];

      // there is no need to translate these points or apply a scaling -
      // that has been done already

      // 1) Compute the distance from the principle point (now 0,0)
      float rpwr2 = x*x + y*y;
      float rpwr4 = rpwr2*rpwr2;
      //float rpwr6 = rpwr4*rpwr2;
    
      float radialcoeff = 1 + m_r2*rpwr2 + m_r4*rpwr4;// + m_r6*rpwr6;

      // 2) Compute the tangential offset
      float dxx = 0; //2*m_d1*x*y + m_d2*(rpwr2+2*x*x);
      float dxy = 0; //m_d1*(rpwr2+2*y*y)+2*m_d2*x*y;

      // 3) Compute the new values of x and y
      float xd1 = radialcoeff*x+dxx;
      float xd2 = radialcoeff*y+dxy;

      // 4) rescale and return to image co-ordinates
      points[i] = m_intrinsic[0]*(xd1+m_intrinsic[1]*xd2)+m_intrinsic[2];
      points[i+1] = m_intrinsic[4]*xd2+m_intrinsic[5];
    }      
  }


  /**
   * \todo currently ignores tangential parameters and r^6 coeff.
   * since inversion of these quantities is not straightforward
   */
  void Camera::NPCFToImage(std::vector<float>& points) const {
    int numpoints = points.size();
    for(int i=0;i<numpoints;i+=2) {
      float x = points[i];
      float y = points[i+1];

      // there is no need to translate these points or apply a scaling -
      // that has been done already

      // 1) Compute the distance from the principle point (now 0,0)
      float rpwr2 = x*x + y*y;
      float rpwr4 = rpwr2*rpwr2;
      //    float rpwr6 = rpwr4*rpwr2;
    
      float radialcoeff = 1 + m_r2*rpwr2 + m_r4*rpwr4; // + m_r6*rpwr6;

      // 2) Compute the tangential offset
      float dxx = 0; //2*m_d1*x*y + m_d2*(rpwr2+2*x*x);
      float dxy = 0; //m_d1*(rpwr2+2*y*y)+2*m_d2*x*y;

      // 3) Compute the new values of x and y
      float xd1 = radialcoeff*x+dxx;
      float xd2 = radialcoeff*y+dxy;

      //  xd1 = x;
      //  xd2 = y;

      // 4) rescale and return to image co-ordinates
      points[i] = m_intrinsic[0]*(xd1+m_intrinsic[1]*xd2)+m_intrinsic[2];
      points[i+1] = m_intrinsic[4]*xd2+m_intrinsic[5];
    }      
  }

  /**
   * \todo currently ignores  tangential parameters
   */
 
  void Camera::ImageToNPCF(float* points, int numpoints) const {
    for(int i=0;i<numpoints*2 ;i+=2) {
      // 1) translate the points back to the principle point
      points[i] -= m_intrinsic[2];
      points[i+1] -= m_intrinsic[5];

      // 2) remove the x and y scaling
      points[i] /= m_intrinsic[0];
      points[i+1] /= m_intrinsic[4];
  
      float x = points[i];
      float y = points[i+1];

      // 1) Compute the distance from the principle point (now 0,0)
      float rd2 = x*x + y*y;
    
      float factor_num = m_r2*rd2 + m_r4*rd2*rd2 + m_r2*m_r2*rd2*rd2 + m_r4*m_r4*rd2*rd2*rd2*rd2 +2*m_r2*m_r4*rd2*rd2*rd2;
      float factor_denom = 1.f + 4*m_r2*rd2 + 6*m_r4*rd2*rd2;
    
    
      // 3) Compute the new values of x and y
      float xd1 = x- x*factor_num/factor_denom;
      float xd2 = y- y*factor_num/factor_denom;
    
      points[i] = xd1;
      points[i+1] = xd2;
    }
  }

  /**
   * \todo currently ignores  tangential parameters
   */
  void Camera::ImageToNPCF(std::vector<float>& points) const {  
    int numpoints = points.size();
    for(int i=0;i<numpoints;i+=2) {
      // 1) translate the points back to the principle point
      points[i] -= m_intrinsic[2];
      points[i+1] -= m_intrinsic[5];

      // 2) remove the x and y scaling
      points[i] /= m_intrinsic[0];
      points[i+1] /= m_intrinsic[4];

      float x = points[i];
      float y = points[i+1];
    
      // 1) Compute the distance from the principle point (now 0,0)
      float rd2 = x*x + y*y;
    
      float factor_num = m_r2*rd2 + m_r4*rd2*rd2 + m_r2*m_r2*rd2*rd2 + m_r4*m_r4*rd2*rd2*rd2*rd2 +2*m_r2*m_r4*rd2*rd2*rd2;
      float factor_denom = 1.f + 4*m_r2*rd2 + 6*m_r4*rd2*rd2;
    
    
      // 3) Compute the new values of x and y
      float xd1 = x- x*factor_num/factor_denom;
      float xd2 = y- y*factor_num/factor_denom;

      points[i] = xd1;
      points[i+1] = xd2;
    }
  }

   struct ParamsArray {
     float k1, k2, x, y;
  };

#ifdef HAVE_GSL_MULTIMIN_H
#ifdef HAVELIB_GSLCBLAS
#ifdef HAVELIB_GSL

   static double _undistortfunc(const gsl_vector *v, void *params) {
	struct ParamsArray *p = (struct ParamsArray *) params;

    float l = float(gsl_vector_get(v,0));
    float x = p->x*l;
    float y = p->y*l;
    float r2 = x*x+y*y;

    float radialcoeff = 1 + p->k1*r2 + p->k2*r2*r2;
    x*=radialcoeff;
    y*=radialcoeff;
    return (x-p->x)*(x-p->x)+(y-p->y)*(y-p->y);
   }

  void Camera::ImageToNPCFIterative(std::vector<float>& points, bool useCache) const {
    for(unsigned int i=0;i<points.size();i+=2) {
      dist_coord d;
      bool cached=false;
      if (useCache) {
	d.x=points[i];
	d.y=points[i+1];
	std::map<dist_coord, dist_coord>::const_iterator ci = m_distortion_map.find(d);
	//   std::cout << m_distortion_map.size() << std::endl;;
	if (ci!=m_distortion_map.end()) {
	  points[i] = ci->second.x;
	  points[i+1] = ci->second.y;
	  cached=true;
	}
      }

      if (!cached) {
	// If we're here we either don't use the cache
	// or we didn't have the value cached

	// 1) translate the points back to the principle point
	points[i] -= m_intrinsic[2];
	points[i+1] -= m_intrinsic[5];
      
	// 2) remove the x and y scaling
	points[i] /= m_intrinsic[0];
	points[i+1] /= m_intrinsic[4];
      
	float x = points[i];
	float y = points[i+1];
      
	struct ParamsArray paramsarray;
	paramsarray.k1 = m_r2;
	paramsarray.k2 = m_r4;
	paramsarray.x = x;
	paramsarray.y = y;
	
	// setup gsl minmisation
	gsl_vector *r = gsl_vector_alloc (1);
	gsl_vector_set (r, 0,  1.0);
	gsl_vector *step = gsl_vector_alloc (1);
	gsl_vector_set(step,0,1.0);
	gsl_multimin_function errfunc;
	errfunc.f = (&_undistortfunc);
	errfunc.n=1;
	errfunc.params=&paramsarray;
	const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
	gsl_multimin_fminimizer *s = gsl_multimin_fminimizer_alloc (T, 1);
	gsl_multimin_fminimizer_set (s, &errfunc, r, step); 
	int iter=0;
	int status=0;
	const int max_it=100;
	do {
	  iter++;
	  status = gsl_multimin_fminimizer_iterate (s);
	  if (status)
	    break;      
	  status = gsl_multimin_test_size(s->size,1e-3);
	}  while (status == GSL_CONTINUE && iter < max_it); 
	float l = float(gsl_vector_get(s->x,0));
	points[i] = x*l;
	points[i+1] = y*l;

	if (useCache) {
	  dist_coord ip;
	  ip.x = d.x;
	  ip.y = d.y;
	  dist_coord op;
	  op.x = points[i];
	  op.y = points[i+1];
	  m_distortion_map[ip]=op;
	}

	gsl_multimin_fminimizer_free (s);
	gsl_vector_free (r);
	gsl_vector_free (step);
      }
    }
  }
    
#endif
#endif
#endif


  void Camera::UnDistortImage(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) const {
    const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> source(image);
    for(short i=0;i<image.GetHeight();i++) {
      for(short j=0;j<image.GetWidth();j++) {
	float points[] = {i,j};
	points[0] -= m_intrinsic[2];
	points[1] -= m_intrinsic[5];
	points[0] /= m_intrinsic[0];
	points[1] /= m_intrinsic[4];
	NPCFToImage(points,1);
	image.DrawPixel(i,j,source.Sample(points[0],points[1]));
      }
    }
  }

  void Camera::DrawEllipse(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Ellipse& ellipse) const {
    float x0 = (ellipse.GetX0()*m_intrinsic[0]) + m_intrinsic[2];
    float y0 = (ellipse.GetY0()*m_intrinsic[4]) + m_intrinsic[5];
    float width = (ellipse.GetWidth()*m_intrinsic[0]) * 2.f;
    float height = (ellipse.GetHeight()*m_intrinsic[4]) * 2.f;
    float angle = ellipse.GetAngle();
    image.DrawEllipse(x0,y0,width,height,angle,COLOUR_BLACK,2);
  }

  void Camera::Draw(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Ellipse& ellipse) const {
    DrawEllipse(image,ellipse);
  }
  void Camera::Draw(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const QuadTangle& quadtangle) const {
    DrawQuadTangle(image,quadtangle);
  }

  void Camera::DrawQuadTangle(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const QuadTangle& quadtangle) const {
    float pts[] = { quadtangle.GetX0(),quadtangle.GetY0(),
		    quadtangle.GetX1(),quadtangle.GetY1(),
		    quadtangle.GetX2(),quadtangle.GetY2(),
		    quadtangle.GetX3(),quadtangle.GetY3() };
    for(int i=0;i<8;i+=2) {
      pts[i] = pts[i] * m_intrinsic[0] + m_intrinsic[2];
      pts[i+1] = pts[i+1] * m_intrinsic[4] + m_intrinsic[5];
    }
    image.DrawQuadTangle((int)pts[0],(int)pts[1],
			 (int)pts[2],(int)pts[3],
			 (int)pts[4],(int)pts[5],
			 (int)pts[6],(int)pts[7],
			 COLOUR_BLACK,2);
  }
}
