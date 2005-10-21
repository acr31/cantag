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
    EstimateTransform(const float maxError) : mMaxResidual(maxError) {}
    EstimateTransform() : mMaxResidual(1.0) {}

    Transform operator()(std::list<Correspondence>& correspondences,
			 const Transform &guess,
			 const Camera &c);
  private:


    float EvaluateResidual(const Transform &t,
			   const Correspondence &c,
			   const Camera &c2);
    /**
     * This static function is required for the 
     * libgsl minimiser
     */
    static double _MinFunc(const gsl_vector *v, void *params);

    float mMaxResidual;

  };
}

#endif//ESTIMATETRANSFORM_GUARD
