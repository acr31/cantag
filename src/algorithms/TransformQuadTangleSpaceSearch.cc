/*
  Copyright (C) 2004 Robert K. Harle

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

  Email: rkh23@cam.ac.uk
*/

/**
 * $Header$
 */

#include <cantag/algorithms/TransformQuadTangleSpaceSearch.hh>
#include <cantag/gaussianelimination.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {

  bool TransformQuadTangleSpaceSearch::operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& dest) const {
   
    const QuadTangle& q = *(shape.GetShape());
    size_t iter = 0;
    int status;
  
    const gsl_multimin_fminimizer_type *T;
    gsl_multimin_fminimizer *s;
    

    // Set up a data params array
    float  p[10] = {q.GetX0(), q.GetY0(),
		    q.GetX1(), q.GetY1(),
		    q.GetX2(), q.GetY2(),
		    q.GetX3(), q.GetY3(),
		    0.0, 0.0};

    // We assume that they are ordered
    // Check that the order is the right way round
    float v1x = p[0]-p[2];
    float v1y = p[1]-p[3];
    float v2x = p[4]-p[2];
    float v2y = p[5]-p[3];
    // v1 cross v2 should be -ve
    float cp = (v1x*v2y-v1y*v2x);
    if (cp>0.0) {
      //switch the order!
      p[2]=q.GetX3();
      p[3]=q.GetY3();
      p[6]=q.GetX0();
      p[7]=q.GetY0();

    }
    
    // Calculate the centre point
    float lambda = ( (p[1]-p[3])*(p[6]-p[2]) - (p[7]-p[3])*(p[0]-p[2]) ) /
      ( (p[4]-p[0])*(p[7]-p[3]) - (p[5]-p[1])*(p[6]-p[2]) );
  
    p[8] = p[0]+lambda*(p[4]-p[0]);
    p[9] = p[1]+lambda*(p[5]-p[1]);
  
    gsl_vector *x;
    gsl_vector *step;
    gsl_multimin_function errfunc;

    int nparam = 4;
  
    errfunc.f = &(Cantag::TransformQuadTangleSpaceSearch::SpaceSearchQuadFunc);
    errfunc.n = nparam;
    errfunc.params = &p;

    //  Start by getting an estimate to help with ambiguities
    // I'm too lazy to compute the same starting conditions as I 
    // did with the Cyber algorithm so I'll just use that
    // for the moment: it should make no difference other
    // than to slow things down!
 
    float n[3]={0.0};

    // Store where the z' points
    float ezx = n[0]; //t[2];
    float ezy = n[1]; //t[6];
    float ezz = n[2]; //t[10];
    float mag = sqrt(ezx*ezx+ezy*ezy+ezz*ezz);
    ezx/=mag; ezy/=mag; ezz/=mag;


    x = gsl_vector_alloc (nparam);
    gsl_vector_set (x, 0, 10.0);
    gsl_vector_set (x, 1, 0.0); // theta
    gsl_vector_set (x, 2, 0.0);   // phi
    gsl_vector_set (x, 3, 0.0);   // psi

    // Characteristic steps differ 
    // for distances and angles
    step = gsl_vector_alloc (nparam);
    gsl_vector_set (step, 0, 10);
    gsl_vector_set (step, 1, M_PI);
    gsl_vector_set (step, 2, M_PI);
    gsl_vector_set (step, 3, M_PI);

  
    T = gsl_multimin_fminimizer_nmsimplex;
    s = gsl_multimin_fminimizer_alloc (T, nparam);
  
    gsl_multimin_fminimizer_set (s, &errfunc, x, step); 
  
    do {
      iter++;
      status = gsl_multimin_fminimizer_iterate (s);
      if (status)
	break;      
      status = gsl_multimin_test_size(s->size,1e-4);

    }  while (status == GSL_CONTINUE && iter < 1000);

    if (iter <1000) {
      // Converged
      float z = gsl_vector_get(s->x, 0);
      float theta = gsl_vector_get(s->x, 1);
      float phi  = gsl_vector_get(s->x, 2);
      float psi = gsl_vector_get(s->x, 3);
      
      Transform* t = new Transform(p[8]*z, p[9]*z,z,
				   theta, phi, psi, 1.0);
      dest.GetTransforms().push_back(t);  
      gsl_multimin_fminimizer_free (s);
      gsl_vector_free (x);
      gsl_vector_free (step);
      return true;
    }
    gsl_multimin_fminimizer_free (s);
    gsl_vector_free (x);
    gsl_vector_free (step);
    return false;
  }

  double TransformQuadTangleSpaceSearch::SpaceSearchQuadFunc(const gsl_vector *v, void *params) {
    float *p = (float *) params;

    float z = gsl_vector_get(v, 0);
    float theta = gsl_vector_get(v, 1);
    float phi  = gsl_vector_get(v, 2);
    float psi = gsl_vector_get(v, 3);


    Transform t(p[8]*z, p[9]*z,z,
		theta, phi, psi, 1.0);
    
    float X0,Y0;
    t.Apply(-1.0, -1.0, 0.0, &X0, &Y0);

    float X1,Y1;
    t.Apply(1.0, -1.0, 0.0, &X1, &Y1);

    float X2,Y2;
    t.Apply(1.0, 1.0, 0.0, &X2, &Y2);

    float X3,Y3;
    t.Apply(-1.0, 1.0, 0.0, &X3, &Y3);

    // Centre point of current
    float lambda = ( (Y0-Y1)*(X3-X1) - (Y3-Y1)*(X0-X1) ) /
      ( (X2-X0)*(Y3-Y1) - (Y2-Y0)*(X3-X1) );
  
    float XC = X0+lambda*(X2-X0);
    float YC = Y0+lambda*(Y2-Y0);

    // Sum of squares
    return (X0-p[0])*(X0-p[0]) + (Y0-p[1])*(Y0-p[1]) +
      (X1-p[2])*(X1-p[2]) + (Y1-p[3])*(Y1-p[3]) +
      (X2-p[4])*(X2-p[4]) + (Y2-p[5])*(Y2-p[5]) +
      (X3-p[6])*(X3-p[6]) + (Y3-p[7])*(Y3-p[7])  +
      (XC-p[8])*(XC-p[8]) + (YC-p[8])*(YC-p[9]);
  }



}
