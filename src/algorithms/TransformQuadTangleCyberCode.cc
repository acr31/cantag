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

#include <cantag/algorithms/TransformQuadTangleCyberCode.hh>

namespace Cantag {

  struct FreeMinimizer {
    gsl_multimin_fminimizer* s;
    gsl_vector* x;

    FreeMinimizer() : s(NULL), x(NULL) {}
    ~FreeMinimizer() {
      if (s) gsl_multimin_fminimizer_free (s);
      if (x) gsl_vector_free (x);
    }
  };

  bool TransformQuadTangleCyberCode::operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& dest) const {

    const QuadTangle& q = *shape.GetShape();

    size_t iter = 0;
    int status;

    const gsl_multimin_fminimizer_type *T;
    gsl_multimin_fminimizer *s;

    FreeMinimizer freeobj;  // stack allocated object to auto free allocated vectors

    // Set up a data params array
    float  p[10] = {q.GetX0(), q.GetY0(),
		    q.GetX1(), q.GetY1(),
		    q.GetX2(), q.GetY2(),
		    q.GetX3(), q.GetY3(),
		    0.0, 0.0};
  

    // Calculate the centre point
    float lambda = ( (p[1]-p[3])*(p[6]-p[2]) - (p[7]-p[3])*(p[0]-p[2]) ) /
      ( (p[4]-p[0])*(p[7]-p[3]) - (p[5]-p[1])*(p[6]-p[2]) );
  
    p[8] = p[0]+lambda*(p[4]-p[0]);
    p[9] = p[1]+lambda*(p[5]-p[1]);
  
    gsl_vector *x;
    gsl_vector *step;
    gsl_multimin_function errfunc;

    int nparam = 2;
  
    errfunc.f = &(Cantag::TransformQuadTangleCyberCode::QuadFunc);
    errfunc.n = nparam;
    errfunc.params = &p;
   
    x = gsl_vector_alloc (nparam);
    freeobj.x = x;
    gsl_vector_set (x, 0, M_PI);
    gsl_vector_set (x, 1, M_PI);

    // Characteristic steps 
    step = gsl_vector_alloc (nparam);
    gsl_vector_set (step, 0, M_PI/2.0);
    gsl_vector_set (step, 1, M_PI);
  
    T = gsl_multimin_fminimizer_nmsimplex;
    s = gsl_multimin_fminimizer_alloc (T, nparam);
    freeobj.s = s;

    gsl_multimin_fminimizer_set (s, &errfunc, x, step); 
    float pts[12] = {0.0, 0.0};
    float n[3] = {0.0};
    do {
      iter++;
      status = gsl_multimin_fminimizer_iterate (s);
      if (status) break;

      // There is a danger that theta ~ 180 and phi
      // is being moved randomly without effect
      if (gsl_vector_get(s->x, 0)/M_PI > 0.98 && 
	  QuadFunc(s->x,p) < 1e-4) break;;

      status = gsl_multimin_test_size(s->size,1e-5);
    }  while (status == GSL_CONTINUE && iter < 500);
    

    if (iter <500) {
      // Converged
      ComputeCameraPointsFromAngles(s->x,p,pts,n);
      // Choose a side as the x axis of the tag
      float xaxis[3] = { pts[6]-pts[3],
			 pts[7]-pts[4],
			 pts[8]-pts[5] };
      float m = sqrt (xaxis[0]*xaxis[0]+
		      xaxis[1]*xaxis[1]+
		      xaxis[2]*xaxis[2]);
      for (int i=0; i<3; i++) xaxis[i]/=m;
      float yaxis[] = { (xaxis[1]*n[2]-xaxis[2]*n[1]),
		      (xaxis[2]*n[0]-xaxis[0]*n[2]),
		      (xaxis[0]*n[1]-xaxis[1]*n[0]) };

      Transform* t = new Transform(1.f);
      dest.GetTransforms().push_back(t);
      Transform& transform = *t;
	
      // We want m to be 2.0
      transform[0]  = xaxis[0];
      transform[4]  = xaxis[1];
      transform[8]  = xaxis[2];
    
      transform[1]  = yaxis[0];
      transform[5]  = yaxis[1];
      transform[9]  = yaxis[2];
    
      transform[2]  = n[0];
      transform[6]  = n[1];
      transform[10] = n[2];
    
      // Add in the 4th col (offsets)
      transform[3]  = p[8]*2.f/m;
      transform[7]  = p[9]*2.f/m;
      transform[11] = 2.f/m;
    
      // Add in the 4th row (homogeneous coords)
      transform[12] = 0.f;
      transform[13] = 0.f;
      transform[14] = 0.f;
      transform[15] = 1.f;

    //   std::cout << "Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";"<< std::endl;
//       std::cout << "             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";"<< std::endl;
//       std::cout << "             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";"<< std::endl;
//       std::cout << "             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];"<< std::endl;
      return true;
    }
    return false;
  }

  double TransformQuadTangleCyberCode::QuadFunc(const gsl_vector *v, void *vp) {
    float *p = (float *)vp;
    float pts[12]={0.0};
    float n[3]={0.0};
    if (!ComputeCameraPointsFromAngles(v,p,pts,n)) return 1000.0;

    // Compute the vectors for the four sides
    float s[18]={0.0};

    for (int i=0; i<12; i+=3) {
      // vector from this to next
      s[i] = pts[i] - pts[(i+3)%12];
      s[i+1] = pts[i+1] - pts[(i+4)%12];
      s[i+2] = pts[i+2] - pts[(i+5)%12];
    }


    // Add diagonals
    s[12]  = pts[0]-pts[6];
    s[13]  = pts[1]-pts[7];
    s[14]  = pts[2]-pts[8];
    s[15]  = pts[3]-pts[9];
    s[16]  = pts[4]-pts[10];
    s[17]  = pts[5]-pts[11];
    
    float sumsq=0.0;
    for (int i=0; i<12; i+=3) {
      float dotprod = s[i]*s[(i+3)%12] + 
	s[i+1]*s[(i+4)%12] +
	s[i+2]*s[(i+5)%12];
      float m1 = sqrt(s[i]*s[i]+s[i+1]*s[i+1] + s[i+2]*s[i+2]);
      float m2 = sqrt(s[(i+3)%12]*s[(i+3)%12] + s[(i+4)%12]*s[(i+4)%12] + s[(i+5)%12]*s[(i+5)%12]);
      sumsq += dotprod*dotprod/(m1*m1*m2*m2);
    }

    float diagdp = (s[12]*s[15] +
      s[13]*s[16] +
      s[14]*s[17])/ (sqrt(s[12]*s[12]+s[13]*s[13]+s[14]*s[14])*sqrt(s[15]*s[15]+s[16]*s[16]+s[17]*s[17]));
    sumsq += diagdp*diagdp;
    //   std::cout << "S " << sumsq << std::endl;
    return sumsq;
  }

  bool TransformQuadTangleCyberCode::ComputeCameraPointsFromAngles(const gsl_vector *v, void *vp, float *pts, float *n) {
    float *p = (float *)vp;
    // v contains the theta/phi for spherical polars
    float theta = gsl_vector_get(v, 0);
    float phi   = gsl_vector_get(v, 1);
    
    // theta=0, phi=0 points along 1,0,0
    float nx = sin(theta)*cos(phi);
    float ny = sin(theta)*sin(phi);
    float nz = cos(theta);

    // std::cout << theta/M_PI*180.0 << " " << phi/M_PI*180.0 << " " << nx << " " << ny << " " << nz << std::endl;
    
    float Xc = p[8];
    float Yc = p[9];
    
    for (int i=0; i<4; i++) {
      float lambda = (Xc*nx + Yc*ny + nz) / 
	(p[i*2]*nx + p[i*2+1]*ny +nz);
      pts[3*i]   = lambda*p[2*i];
      pts[3*i+1] = lambda*p[2*i+1];
      pts[3*i+2] = lambda;
      if (lambda <= 1e-05) return false;
    }

    n[0] = nx;
    n[1] = ny;
    n[2] = nz;
    return true;
  } 

}
