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

#ifndef LINEAR_ELLIPSE_TRANSFORM_GUARD
#define LINEAR_ELLIPSE_TRANSFORM_GUARD

#include <cantag/entities/Entity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Ellipse.hh>
#include <cantag/Function.hh>

namespace Cantag {

  class TransformEllipseLinear : public Function1<ShapeEntity<Ellipse>,TransformEntity> {
  public:
    typedef ShapeEntity<Ellipse> SourceType;
    typedef TransformEntity DestinationType;

    bool operator()(const ShapeEntity<Ellipse>& source, TransformEntity& dest) const;
  };

}

#endif//LINEAR_ELLIPSE_TRANSFORM_GUARD
