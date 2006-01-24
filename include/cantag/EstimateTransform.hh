/*
  Copyright (C) 2005 Robert Harle

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

#ifndef ESTIMATETRANSFORM_GUARD
#define ESTIMATETRANSFORM_GUARD

#include <list>

#include <cantag/Config.hh>

#ifdef HAVE_GSL_MULTIMIN_H
#ifdef HAVELIB_GSLCBLAS
#ifdef HAVELIB_GSL
#else
#error This version has not been configured with GNU Scientific Library support!
#endif
#endif
#endif

#include <cantag/Correspondence.hh>
#include <cantag/TagDictionary.hh>
#include <cantag/Transform.hh>

namespace Cantag {
  
 /**
   * Iteratively solves for the camera position
   * given a series of correspondences between
   * real world points and their NPCF equivalents
   *
   * The operator takes a list of correspondences
   * and a guess for the camera transform. The
   * AccumulateCorrespondences function produces 
   * a good guess for this
   * 
   * Note that there are 6 parameters to determine
   * and so we must have at least 6 correspondences
   * in the list!
   * 
   * Exceptions are thrown if there are fewer than 6
   * independent correspondences or the minimisation
   * fails to converge
   */
  class EstimateTransform {    
  public:

    /**
     * Initialise with a model fit error in
     * (distorted) pixels. If the estimate is unable to
     * match it, an exception is thrown
     */
    EstimateTransform(const float maxResidual) : mFitError(maxResidual), mMaxResidual(-1.0) {}
    EstimateTransform() : mFitError(1.0), mMaxResidual(-1.0) {}


    /**
     * Perform the minimisation based on a supplied
     * set of correspondences, a guess for the correct 
     * transform, a camera with the distortion model,
     * and (optionally) a hint about the scale of the
     * (x,y,z) space to search about the guess point
     */
    Transform operator()(std::list<Correspondence>& correspondences,
			 const Transform &guess,
			 const Camera &c,
			 float x_min_scale=-1.0,
			 float y_min_scale=-1.0,
			 float z_min_scale=-1.0);

    /**
     * Get the maximum residual for the fit
     */
    float GetMaxResidual() const { return mMaxResidual; };

  private:


    static float EvaluateResidualSq(const Transform &t,
				    const Correspondence &c,
				    const Camera &cam);

    /**
     * This static function is required for the 
     * libgsl minimiser
     */
    static double _MinFunc(const gsl_vector *v, void *params);

    float mFitError;
    float mMaxResidual;

    struct MinData_t {
      const Camera *c;
      std::list<Correspondence> *corr;
    };

  };
}

#endif//ESTIMATETRANSFORM_GUARD
