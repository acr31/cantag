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

#ifndef TRANSFORMQUADTANGLESPACE_GUARD
#define TRANSFORMQUADTANGLESPACE_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_GSL
#error "TransformQuadTangleSpaceSearch requires the GNU Scientific Library!"
#endif



#include <cantag/QuadTangle.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Function.hh>



namespace Cantag {
  /**
   * Perform transform based on assertion that tag is exactly
   * square in real life.  Solves for four parameters: z, alpha,
   * beta, gamma, where the three angles are standarad euler angles
   */
  class CANTAG_EXPORT TransformQuadTangleSpaceSearch : public Function<TL1(ShapeEntity<QuadTangle>),TL1(TransformEntity)> {
  public:
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


  };
}


#endif//TRANSFORMQUADTANGLESPACE_GUARD
