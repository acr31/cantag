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

#include <cantag/EstimateTransform.hh>
#include <gsl/gsl_multimin.h>

#include <iostream>

namespace Cantag {

  /**
   * Iteratively solves for the camera position
   * given a series of correspondences between
   * real world points and their NPCF equivalents
   */
  Transform EstimateTransform::operator()(std::list<Correspondence>& correspondences,
					  const Transform &guess,
					  const Camera &c) {

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
   
    gsl_vector *step = gsl_vector_alloc (6);
    gsl_vector_set(step,0,1.0);
    gsl_vector_set(step,1,1.0);
    gsl_vector_set(step,2,1.0);
    gsl_vector_set(step,3,3.141/2.0);
    gsl_vector_set(step,4,3.141/2.0);
    gsl_vector_set(step,5,3.141);

    struct MinData_t data;
    data.corr = &correspondences;
    data.c = &c;


    gsl_multimin_function errfunc;
    errfunc.f = &(Cantag::EstimateTransform::_MinFunc);
    errfunc.n = 6;
    errfunc.params = (void *)&data;

    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;

    Transform transformResult;

    // Model fit error estimate
    mSigma = 1e10;
    while (mSigma>mFitError && correspondences.size()>=6) {
      gsl_multimin_fminimizer *s = gsl_multimin_fminimizer_alloc (T, 6);
      gsl_multimin_fminimizer_set (s, &errfunc, r, step); 

      int iter=0;
      int status=0;
      do {
	iter++;
	status = gsl_multimin_fminimizer_iterate (s);
	if (status)  break;      
	status = gsl_multimin_test_size(s->size,1e-3);
      }  while (status == GSL_CONTINUE && iter < 50000);   

      if (iter!=50000) {
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

      
      float maxResidualsq=-1.0e10;

      float sumsq=0.0;
      std::list<Correspondence>::iterator it = correspondences.begin();
      std::list<Correspondence>::iterator maxResidualIt;
      float maxResidualSq=-1e10;
      float sumsqres = 0.f;
	float residualsq = EvaluateResidualSq(transformResult, *(it),c);
	sumsq+=residualsq;
	if (residualsq>maxResidualsq) {
	    maxResidualsq=residualsq;
	    maxResidualIt = it;
	}

      // Recompute mSigma
      mSigma=sqrt( sumsq/(correspondences.size()-2.0) );

      // If it's too big, discard the largest residual
      if (correspondences.size()==6) break;
      else if (mSigma>mFitError && correspondences.size()) correspondences.erase(maxResidualIt);
      

    } // while loop


    if (mSigma>mFitError) throw ("Unable to meet required fit error for camera localisation");


    std::list<Correspondence>::iterator cit = correspondences.begin();
    for(;cit!=correspondences.end(); ++cit) {
      Cantag::Transform t2;
      for (int i=0; i<16;i++) t2[i]=transformResult[i];
      t2.Invert();
      
      // Get image pixel of this point
      float p[2];
      float a,b;
      t2.Apply(cit->GetWorldX(),cit->GetWorldY(),cit->GetWorldZ(),p, p+1);
      c.NPCFToImage(p,1);
      
      // Compare with image pixel in correspondence
      float p2[] ={cit->GetImageX(),cit->GetImageY()};
      c.NPCFToImage(p2,1); 
    }


    gsl_vector_free(step);
    gsl_vector_free(r);

    return transformResult;
  };


  float EstimateTransform::EvaluateResidualSq(const Cantag::Transform &t,
			 const Correspondence &c,
			 const Camera &cam) {

    // World to Camera
    Cantag::Transform t2;
    for (int i=0; i<16;i++) t2[i]=t[i];
    t2.Invert();
    

    // Get image pixel of this point
    float p[2];
    float a,b;
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
  double EstimateTransform::_MinFunc(const gsl_vector *v, void *params) {
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
    // current_estimate.Invert();

    double sumsq=0.0;
    std::list<Correspondence>::const_iterator it = list->begin();
    //  float px,py;
    for(;it!=list->end(); ++it) {
      sumsq += EstimateTransform::EvaluateResidualSq(current_estimate, *it, *cam);
    }
    return sqrt(sumsq);
  }

}
