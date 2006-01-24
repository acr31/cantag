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


#ifndef TRANSFORMQUADTANGLECYBERCODE_GUARD
#define TRANSFORMQUADTANGLECYBERCODE_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_GSL
#error "TransformQuadTangleCyberCode requires the GNU Scientific Library!"
#endif

#include <gsl/gsl_multimin.h>

#include <cantag/Function.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/TransformEntity.hh>


namespace Cantag {

  class TransformQuadTangleCyberCode : public Function<TL1(ShapeEntity<QuadTangle>),TL1(TransformEntity)> {

  public:
    /*
     * Static function so it can be passed as a function pointer
     * to GNU Scientific Library
     */
    static double QuadFunc(const gsl_vector *v, void *params);
    static bool ComputeCameraPointsFromAngles(const gsl_vector *v, void *p, float *pts, float *n);

    /*
     * An implementation of the CyberCode algorithm
     * The details are scant in the paper and the maths
     * that is quoted is not actually correct/optimal
     *
     * The idea is to separate out determination of the
     * normal from determination of distance since
     * this is simply a scaling factor
     *
     * Thus we select a normal vector n and place a plane
     * with this normal at the point (Xc, Yc, 1) which is 
     * where the quad diagonals intersect. The normal is
     * represented by two spherical polar angles, theta and
     * phi
     *
     * We then look at the points P where the rays passing
     * through the corners {X,Y} intersect the plane. We compute
     * the dot products of the sides defined by P and add on
     * the dot product of the diagonals. If P is a square all
     * these angles are 90 degrees and the sum evaluates as zero.
     * Hence we minimise the sum to estimate the normal.
     *
     * To calculate the distance, we simply measure the length
     * of one of the sides defined by P and find the factor that
     * makes it 2.0
     */
    bool operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& transform) const;
  };

};

#endif//TRANSFORMQUADTANGLECYBERCODE_GUARD
