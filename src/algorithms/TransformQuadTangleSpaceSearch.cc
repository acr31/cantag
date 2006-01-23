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
#include <cantag/algorithms/TransformQuadTangleProjective.hh>
#include <cantag/gaussianelimination.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {

  bool TransformQuadTangleSpaceSearch::operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& dest) const {
    const QuadTangle& q = *(shape.GetShape());

    TransformEntity te;
    if (!TransformQuadTangleProjective()(shape,te)) { return false; }
   
    //    const QuadTangle& q = *(shape.GetShape());
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
      p[6]=q.GetX1();
      p[7]=q.GetY1();

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
    const Transform* t = te.GetPreferredTransform();
    float initz = 40.f;
    float theta = 0.f;
    float phi = 0.f;
    float psi = 0.f;


    float steptheta=2*M_PI;
    float stepphi=2*M_PI;
    float steppsi=2*M_PI;

    int num_start=1;

    if (t != NULL) {
      initz = (*t)[11];
      t->GetAngleRepresentation(&theta,&phi,&psi);
   
      Transform check(0.0,0.0,0.0,theta,phi,psi,1.0);   

      if (check[10]<0) {
	theta=2.20741;
	phi=2.74889;
	psi=-1.21712;
	num_start=4;
      }

    }

    // Characteristic steps differ 
    // for distances and angles
    step = gsl_vector_alloc (nparam);
    gsl_vector_set (step, 0, initz);
    gsl_vector_set (step, 1, steptheta);
    gsl_vector_set (step, 2, stepphi);
    gsl_vector_set (step, 3, steppsi);

    T = gsl_multimin_fminimizer_nmsimplex;
    
    float min=FLT_INFINITY;
    float min_theta=0.0, min_phi=0.0, min_psi=0.0, min_z=0.0;

    for (int i=0;i<num_start; i++) {
    
      if (i==1) {
	theta=1.766;
 	phi=2.74889;
 	psi=-2.2643;
      }
      else if (i==2) {
	theta=0.9342;
	phi=0.3927;
	psi=-1.21712;
      }
      else if (i==3) {
	theta=1.375;
	phi=0.3927;
	psi=-2.2643;
      }

      gsl_vector_set (x, 0, initz); // z
      gsl_vector_set (x, 1, theta); // theta
      gsl_vector_set (x, 2, phi);   // phi
      gsl_vector_set (x, 3, psi); 
    
    
      s = gsl_multimin_fminimizer_alloc (T, nparam);
      
      gsl_multimin_fminimizer_set (s, &errfunc, x, step); 
      
      do {
	iter++;
	status = gsl_multimin_fminimizer_iterate (s);
	
	/*
	  float z = gsl_vector_get(s->x, 0);
	  float theta = gsl_vector_get(s->x, 1);
	  float phi  = gsl_vector_get(s->x, 2);
	  float psi = gsl_vector_get(s->x, 3);
	  
	  std::cerr << z << " " << theta << " " << phi << " " << psi << " " << 
	  SpaceSearchQuadFunc(s->x,&p) << " " << s->size << std::endl;
	*/
	if (SpaceSearchQuadFunc(s->x,&p)<1e-7) break;

	if (status)
	  break;      
	status = gsl_multimin_test_size(s->size,1e-3);
	
      }  while (status == GSL_CONTINUE && iter < 1000);
      
   
      if (iter <1000) {
	// Converged
	float z = gsl_vector_get(s->x, 0);
	float theta = gsl_vector_get(s->x, 1);
	float phi  = gsl_vector_get(s->x, 2);
	float psi = gsl_vector_get(s->x, 3);
	
	if (i==0 || (SpaceSearchQuadFunc(s->x,&p) < min)) {
	  min = SpaceSearchQuadFunc(s->x,&p);
	  min_theta=theta;
	  min_phi=phi;
	  min_psi=psi;
	  min_z=z;
	}
      }

      gsl_multimin_fminimizer_free (s);
    }

    if (min!=FLT_INFINITY) {
	Transform* t = new Transform(p[8]*min_z, p[9]*min_z,min_z,
				     min_theta, min_phi, min_psi, 1.0);
	
	dest.GetTransforms().push_back(t);  
	gsl_vector_free (x);
	gsl_vector_free (step);

	return true;
    }
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
    
    float X0,Y0, X1, Y1, X2, Y2, X3, Y3;

    t.Apply(-1.0, -1.0, 0.0, &X3, &Y3);
    t.Apply(1.0, -1.0, 0.0, &X0, &Y0);
    t.Apply(1.0, 1.0, 0.0, &X1, &Y1);
    t.Apply(-1.0, 1.0, 0.0, &X2, &Y2);


   //  std::cerr << X0 << " " << Y0 << std::endl;
//     std::cerr << X1 << " " << Y1 << std::endl;
//     std::cerr << X2 << " " << Y2 << std::endl;
//     std::cerr << X3 << " " << Y3 << std::endl;
//     std::cerr << std::endl;
//     exit(0);

    // Centre point of current
    float lambda = ( (Y0-Y1)*(X3-X1) - (Y3-Y1)*(X0-X1) ) /
      ( (X2-X0)*(Y3-Y1) - (Y2-Y0)*(X3-X1) );
  
    float XC = X0+lambda*(X2-X0);
    float YC = Y0+lambda*(Y2-Y0);

    // Sum of squares
    double s= (X0-p[0])*(X0-p[0]) + (Y0-p[1])*(Y0-p[1]) +
      (X1-p[2])*(X1-p[2]) + (Y1-p[3])*(Y1-p[3]) +
      (X2-p[4])*(X2-p[4]) + (Y2-p[5])*(Y2-p[5]) +
      (X3-p[6])*(X3-p[6]) + (Y3-p[7])*(Y3-p[7])  +
      (XC-p[8])*(XC-p[8]) + (YC-p[8])*(YC-p[9]);
    return s;
  }



}
