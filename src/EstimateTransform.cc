/*
  Copyright (C) 2005 Robert K. Harle

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

#include <cantag/Config.hh>

#ifdef HAVE_GSL

#include <cantag/EstimateTransform.hh>
#include <gsl/gsl_multimin.h>

#include <iostream>

namespace Cantag {

	struct MinData_t {
      const Camera *c;
      std::list<Correspondence> *corr;
    };


	  static float EvaluateResidualSq(const Cantag::Transform &t,
			 const Correspondence &c,
			 const Camera &cam) {

    // World to Camera
    Cantag::Transform t2;
    for (int i=0; i<16;i++) t2[i]=t[i];
    t2.Invert();
    

    // Get image pixel of this point
    float p[2];
    t2.Apply(c.GetWorldX(),c.GetWorldY(),c.GetWorldZ(),p, p+1);
    cam.NPCFToImage(p,1);

    // Compare with image pixel in correspondence
    float p2[] ={c.GetImageX(),c.GetImageY()};
    cam.NPCFToImage(p2,1); 


    // Return residual _squared_
    return (p[0]-p2[0])*(p[0]-p2[0]) +
      (p[1]-p2[1])*(p[1]-p2[1]);
  }


/**
   * Here we calculate the transform for camera->world
   * from the parameters, then invert it to get the transform
   * world->camera. Then we supply the world parts of the
   * correspondences, apply the transform and project.
   *
   * Then we calculate the distance of the point from the
   * correspondence image equivalent. Minimising this
   * fits to the image 
   */
  static double _MinFunc(const gsl_vector *v, void *params) {
    struct MinData_t *md = (struct MinData_t *) params;
    const Camera *cam = md->c;
    std::list<Correspondence> *list = md->corr;

    float x = gsl_vector_get(v, 0);
    float y = gsl_vector_get(v, 1);
    float z = gsl_vector_get(v, 2);
    float theta = gsl_vector_get(v, 3);
    float phi = gsl_vector_get(v, 4);
    float psi = gsl_vector_get(v, 5);

    Cantag::Transform current_estimate(x, y, z, theta, phi, psi, 1.0);

    double sumsq=0.0;
    std::list<Correspondence>::const_iterator it = list->begin();
    //  float px,py;
    for(;it!=list->end(); ++it) {
      sumsq += EvaluateResidualSq(current_estimate, *it, *cam);
    }
    return sqrt(sumsq);
  }


  /**
   * Iteratively solves for the camera position
   * given a series of correspondences between
   * real world points and their NPCF equivalents
   */
  Transform EstimateTransform::operator()(std::list<Correspondence>& correspondences,
					  const Transform &guess,
					  const Camera &c,
					   float x_min_scale,
					   float y_min_scale,
					   float z_min_scale)
  {

    if (correspondences.size() < 6) {
      throw("Sorry - insufficient cor respondences to compute camera position");
    }


    // Get the pose estimate for 
    // the start point into a useful form
    // See Transform for conventions
    float theta, phi, psi;
    guess.GetAngleRepresentation(&theta, &phi, &psi);


    // Setup the GSL minimiser
    gsl_vector *r = gsl_vector_alloc (6);
    gsl_vector_set (r, 0,  guess[3]);  // x
    gsl_vector_set (r, 1,  guess[7]);  // y
    gsl_vector_set (r, 2,  guess[11]); // z
    gsl_vector_set (r, 3,  theta); 
    gsl_vector_set (r, 4,  phi);
    gsl_vector_set (r, 5,  psi);
   

    if (x_min_scale <0.0 || y_min_scale < 0.0 || z_min_scale < 0.0) {
      // We have to guess the size of the state space
      // to search around the guess point
      x_min_scale = (guess[3]>guess[7]) ? guess[3]:guess[7];
      x_min_scale = (guess[11]>x_min_scale) ? guess[11]:x_min_scale;
      y_min_scale=x_min_scale;
      z_min_scale=x_min_scale;
    }

    gsl_vector *step = gsl_vector_alloc (6);
    gsl_vector_set(step,0,x_min_scale);
    gsl_vector_set(step,1,y_min_scale);
    gsl_vector_set(step,2,z_min_scale);
    gsl_vector_set(step,3,3.141/2.0);
    gsl_vector_set(step,4,3.141/2.0);
    gsl_vector_set(step,5,3.141);

    struct MinData_t data;
    data.corr = &correspondences;
    data.c = &c;


    gsl_multimin_function errfunc;
    errfunc.f = &(_MinFunc);
    errfunc.n = 6;
    errfunc.params = (void *)&data;

    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;

    Transform transformResult;

    // Model fit error estimate
    mMaxResidual = 1e10;
    while (mMaxResidual>mFitError && correspondences.size()>=6) {
      gsl_multimin_fminimizer *s = gsl_multimin_fminimizer_alloc (T, 6);
      gsl_multimin_fminimizer_set (s, &errfunc, r, step); 

      int iter=0;
      int status=0;
      do {
	iter++;
	status = gsl_multimin_fminimizer_iterate (s);
	if (status)  break;      
	status = gsl_multimin_test_size(s->size,1e-3);
      }  while (status == GSL_CONTINUE && iter < 5000);   

      if (iter!=5000) {
	float xx = gsl_vector_get(s->x, 0);
	float yy = gsl_vector_get(s->x, 1);
	float zz = gsl_vector_get(s->x, 2);
	float aa = gsl_vector_get(s->x, 3);
	float bb = gsl_vector_get(s->x, 4);
	float gg = gsl_vector_get(s->x, 5);
  
	transformResult.SetupFromAngles(xx,
					yy,
					zz,
					aa,
					bb,
					gg,
					1.0);
      }
      else throw("Failed to converge");

      gsl_multimin_fminimizer_free(s);

      std::list<Correspondence>::iterator it = correspondences.begin();
      std::list<Correspondence>::iterator maxResidualIt;
      float maxResidualsq=-1e10;

      for(;it!=correspondences.end();++it) {
	float residualsq = EvaluateResidualSq(transformResult, *(it),c);
	if (residualsq>maxResidualsq) {
	  maxResidualsq=residualsq;
	  maxResidualIt = it;
	}
      }

      mMaxResidual = sqrt(maxResidualsq);
      // If it's too big, discard the largest residual
      if (correspondences.size()<=6) break;
      else if (mMaxResidual>mFitError) correspondences.erase(maxResidualIt);
      
    } // while loop


    if (mMaxResidual>mFitError) throw ("Unable to meet required fit error for camera localisation");

    gsl_vector_free(step);
    gsl_vector_free(r);

    return transformResult;
  }





  
}

#endif//HAVE_GSL