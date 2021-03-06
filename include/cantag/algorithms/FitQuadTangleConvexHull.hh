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

#ifndef FITQUADTANGLECONVEXHULL_GUARD
#define FITQUADTANGLECONVEXHULL_GUARD

#include <cantag/QuadTangle.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/ConvexHullEntity.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/Function.hh>

namespace Cantag {
  class CANTAG_EXPORT FitQuadTangleConvexHull : public Function<TL2(ContourEntity,ConvexHullEntity),TL1(ShapeEntity<QuadTangle>) > {
  public:

    /**
     * Take the set of points and form a convex hull to smooth out
     * quantization 'kinks'. Filter out vertices clearly not corners
     * based on angles.  What remains should be clusters of points at
     * the true corners. Take one from each cluster as an _estimate_
     * of that corner position.
     */
    bool operator()(const ContourEntity& contour, const ConvexHullEntity& hull,ShapeEntity<QuadTangle>& shape) const;
  };
}

#endif//FITQUADTANGLECONVEXHULL_GUARD
