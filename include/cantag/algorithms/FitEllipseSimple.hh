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

#ifndef SIMPLE_ELLIPSE_FIT_GUARD
#define SIMPLE_ELLIPSE_FIT_GUARD

#include <cantag/Ellipse.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/Function.hh>

namespace Cantag {
  class CANTAG_EXPORT FitEllipseSimple : public Function<TL1(ContourEntity),TL1(ShapeEntity<Ellipse>) > {
  public:
    bool operator()(const ContourEntity& contour, ShapeEntity<Ellipse>& shape) const;
  };
}
#endif//SIMPLE_ELLIPSE_FIT_GUARD
