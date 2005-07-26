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

#include <cantag/algorithms/TransformQuadTangleSpaceSearch.hh>
#include <cantag/gaussianelimination.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {

  bool TransformQuadTangleSpaceSearch::operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& dest) const {
    
    const QuadTangle& q = *(shape.m_shapeDetails);
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

    q.EstimatePoseQuadrant(n);

    // Store where the z' points
    float ezx = n[0]; //t[2];
    float ezy = n[1]; //t[6];
    float ezz = n[2]; //t[10];
    float mag = sqrt(ezx*ezx+ezy*ezy+ezz*ezz);
    ezx/=mag; ezy/=mag; ezz/=mag;

    float zest = 10.0;
    float alpha=0.0;
    float beta = 0.0;
    float gamma  = 0.0;

    x = gsl_vector_alloc (nparam);
    gsl_vector_set (x, 0, zest);
    gsl_vector_set (x, 1, alpha);
    gsl_vector_set (x, 2, beta);
    gsl_vector_set (x, 3, gamma);

    // Characteristic steps differ 
    // for distances and angles
    step = gsl_vector_alloc (nparam);
    gsl_vector_set (step, 0, zest/2.0);
    gsl_vector_set (step, 1, 1.5);
    gsl_vector_set (step, 2, 1.5);
    gsl_vector_set (step, 3, 1.5);

  
    T = gsl_multimin_fminimizer_nmsimplex;
    s = gsl_multimin_fminimizer_alloc (T, nparam);
  
    gsl_multimin_fminimizer_set (s, &errfunc, x, step); 
  
    do {
      iter++;
      status = gsl_multimin_fminimizer_iterate (s);
      if (status)
	break;      
      status = gsl_multimin_test_size(s->size,1e-4);

    }  while (status == GSL_CONTINUE && iter < 500);

    if (iter <500) {
      // Converged
      float z = gsl_vector_get(s->x, 0);
      float alpha = gsl_vector_get(s->x, 1);
      float beta  = gsl_vector_get(s->x, 2);
      float gamma = gsl_vector_get(s->x, 3);

      // Wrap the angles around
      while (alpha >  M_PI) alpha -=  2*M_PI;
      while (alpha < -M_PI) alpha +=  2*M_PI;
      while (beta  >  M_PI) beta  -=  2*M_PI;
      while (beta  < -M_PI) beta  +=  2*M_PI;
      while (gamma >  M_PI) gamma -=  2*M_PI;
      while (gamma < -M_PI) gamma +=  2*M_PI;
      

      // Now figure out the transform
      float ca = DCOS(8,alpha); 
      float sa = DSIN(8,alpha);
      float cb = DCOS(8,beta); 
      float sb = DSIN(8,beta);
      float cg = DCOS(8,gamma);
      float sg = DSIN(8,gamma);
      
      Transform* t = new Transform(1.f);
      dest.GetTransforms().push_back(t);
      
      Transform& transform = *t;
      transform[0] = ca*cg - cb*sa*sg;
      transform[1] = sa*cg + cb*ca*sg;
      transform[2] = sg*sb;
    
      transform[4] = -sg*ca - cb*sa*cg;
      transform[5] = -sg*sa + cb*ca*cg;
      transform[6] = cg*sb;
    
      transform[8] = sb*sa;
      transform[9] = -sb*ca;
      transform[10] = cb;
    
      // Add in the 4th col (offsets)
      transform[3] = p[8]*z;
      transform[7] = p[9]*z;
      transform[11] = z;
    
      // Add in the 4th row (homogeneous coords)
      transform[12] = 0.0;
      transform[13] = 0.0;
      transform[14] = 0.0;
      transform[15] = 1.0;

    
      gsl_multimin_fminimizer_free (s);
      gsl_vector_free (x);
      return true;
    }

    gsl_multimin_fminimizer_free (s);
    gsl_vector_free (x);
    return false;
  }

  double TransformQuadTangleSpaceSearch::SpaceSearchQuadFunc(const gsl_vector *v, void *params) {
    float *p = (float *) params;
    float X0, Y0, X1, Y1, X2, Y2, X3, Y3, XC, YC;

    float z = gsl_vector_get(v, 0);
    float alpha = gsl_vector_get(v, 1);
    float beta  = gsl_vector_get(v, 2);
    float gamma = gsl_vector_get(v, 3);
    
    // Wrap the angles around
    while (alpha >  M_PI) alpha -=  2*M_PI;
    while (alpha < -M_PI) alpha +=  2*M_PI;
    while (beta  >  M_PI) beta  -=  2*M_PI;
    while (beta  < -M_PI) beta  +=  2*M_PI;
    while (gamma >  M_PI) gamma -=  2*M_PI;
    while (gamma < -M_PI) gamma +=  2*M_PI;
    
    
    // Now figure out the transform
    float ca = DCOS(8,alpha); 
    float sa = DSIN(8,alpha);
    float cb = DCOS(8,beta); 
    float sb = DSIN(8,beta);
    float cg = DCOS(8,gamma);
    float sg = DSIN(8,gamma);
          
    float x = p[8] * z;
    float y = p[9] * z;


    // Projections of what we currently have
    X0 = ((ca*cg - cb*sa*sg)*(-1.0) + (sa*cg + cb*ca*sg)*(1.0) + x) /
      (sb*sa*(-1.0) - sb*ca*(1.0) + z);
    Y0 = ((-sg*ca - cb*sa*cg)*(-1.0) + (-sg*sa + cb*ca*cg)*(1.0)+ y) /
      (sb*sa*(-1.0) - sb*ca*(1.0) + z);

    X1 = ((ca*cg - cb*sa*sg)*(-1.0) + (sa*cg + cb*ca*sg)*(-1.0) + x) /
      (sb*sa*(-1.0) - sb*ca*(-1.0) + z);
    Y1 = ((-sg*ca - cb*sa*cg)*(-1.0) + (-sg*sa + cb*ca*cg)*(-1.0) + y) /
      (sb*sa*(-1.0) - sb*ca*(-1.0) + z);

    X2 = ((ca*cg - cb*sa*sg)*(1.0) + (sa*cg + cb*ca*sg)*(-1.0) + x) /
      (sb*sa*(1.0) - sb*ca*(-1.0) + z);
    Y2 = ((-sg*ca - cb*sa*cg)*(1.0) + (-sg*sa + cb*ca*cg)*(-1.0) + y) /
      (sb*sa*(1.0) - sb*ca*(-1.0) + z);

    X3 = ((ca*cg - cb*sa*sg)*(1.0) + (sa*cg + cb*ca*sg)*(1.0) + x) /
      (sb*sa*(1.0) - sb*ca*(1.0) + z);
    Y3 = ((-sg*ca - cb*sa*cg)*(1.0) + (-sg*sa + cb*ca*cg)*(1.0) + y) /
      (sb*sa*(1.0) - sb*ca*(1.0) + z);

    // Centre point of current
    float lambda = ( (Y0-Y1)*(X3-X1) - (Y3-Y1)*(X0-X1) ) /
      ( (X2-X0)*(Y3-Y1) - (Y2-Y0)*(X3-X1) );
  
    XC = X0+lambda*(X2-X0);
    YC = Y0+lambda*(Y2-Y0);

  
 //    // Vectors of projected sides
//     float s1x = X1-X0; float s1y = Y1-Y0;
//     float s2x = X2-X1; float s2y = Y2-Y1;
//     float s3x = X3-X2; float s3y = Y3-Y2;
//     float s4x = X0-X3; float s4y = Y0-Y3;

//     // Actual vectors of sides
//     float p1x = p[2]-p[0]; float p1y = p[3]-p[1];
//     float p2x = p[4]-p[2]; float p2y = p[5]-p[3];
//     float p3x = p[6]-p[4]; float p3y = p[7]-p[5];
//     float p4x = p[0]-p[6]; float p4y = p[1]-p[7];
    
//     // Angle betweeen actual and current
//     float ca1 = (p1x*s1x + p1y*s1y) / (sqrt(p1x*p1x+p1y*p1y)*sqrt(s1x*s1x+s1y*s1y));
//     float ca2 = (p2x*s2x + p2y*s2y) / (sqrt(p2x*p2x+p2y*p2y)*sqrt(s2x*s2x+s2y*s2y));
//     float ca3 = (p3x*s3x + p3y*s3y) / (sqrt(p3x*p3x+p3y*p3y)*sqrt(s3x*s3x+s3y*s3y));
//     float ca4 = (p4x*s4x + p4y*s4y) / (sqrt(p4x*p4x+p4y*p4y)*sqrt(s4x*s4x+s4y*s4y));

//     // Scaling/weighting factors for angles and distances
//     float xmin = p[0];
//     float xmax = p[0];
//     float ymin = p[1];
//     float ymax = p[1];
//     for (int i=2; i<8; i+=2) {
//       if (p[i] < xmin) xmin=p[i];
//       if (p[i] > xmax) xmax=p[i];
//       if (p[i+1] < ymin) ymin=p[i+1];
//       if (p[i+1] > ymax) ymax=p[i+1];
//     }
//     float scale = ((ymax-ymin)>(xmax-xmin)) ? (ymax-ymin):(xmax-xmin);

//     float f= (scale/5)/(cos(20.0/180.0*M_PI)-1);
//     f*=f;

    // Sum of squares
    return (X0-p[0])*(X0-p[0]) + (Y0-p[1])*(Y0-p[1]) +
      (X1-p[2])*(X1-p[2]) + (Y1-p[3])*(Y1-p[3]) +
      (X2-p[4])*(X2-p[4]) + (Y2-p[5])*(Y2-p[5]) +
      (X3-p[6])*(X3-p[6]) + (Y3-p[7])*(Y3-p[7])  +
      (XC-p[8])*(XC-p[8]) + (YC-p[8])*(YC-p[9]);
  }



}
