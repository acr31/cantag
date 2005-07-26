/**
 * $Header$
 */ 

#ifndef TRANSFORMQUADTANGLESPACE_GUARD
#define TRANSFORMQUADTANGLESPACE_GUARD

#include <total/QuadTangle.hh>
#include <total/entities/Entity.hh>
#include <total/entities/TransformEntity.hh>
#include <total/Function.hh>

#if defined(HAVE_GSL_MULTIMIN_H) and defined(HAVELIB_GSLCBLAS) and defined(HAVELIB_GSL)
#else
#error This version has not been configured with GNU Scientific Library support!
#endif

#include <gsl/gsl_multimin.h>


namespace Total {
  /**
   * Perform transform based on assertion that tag is exactly
   * square in real life.  Solves for four parameters: z, alpha,
   * beta, gamma, where the three angles are standarad euler angles
   */
  class TransformQuadTangleSpaceSearch : public Function1<ShapeEntity<QuadTangle>,TransformEntity> {

    /**
     * Calculate the transform by least squares minimising the 
     * euclidean distance for each point AND the angle between the
     * sides.
     *
     * This transform assumes the tag is square (the correct thing to do)
     * and moves it in space to find a best fit to the input corner points.
     * 
     * Position is fully defined by the depth distance, z, whilst pose is
     * completely defined by the Euler angles alpha, beta, gamma --
     * see http://mathworld.wolfram.com/EulerAngles.html
     * 
     * Therefore this is a minimisation wrt (z,alpha,beta,gamma).
     *
     * This version uses the downhill simplex minimisation
     * found in the GNU Scientific Library because that does
     * not require derivatives, and more importantly, can have 
     * the search space easily restricted
     */
    bool operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& transform) const;

    /**
     * Static function so it can be passed as a function pointer
     * to GNU Scientific Library
     */
    static double SpaceSearchQuadFunc(const gsl_vector *v, void *params);

  };
}

#endif//TRANSFORMQUADTANGLESPACE_GUARD
