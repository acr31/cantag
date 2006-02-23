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

#ifndef CONVEXHULL_GUARD
#define CONVEXHULL_GUARD

#include <cantag/Config.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ConvexHullEntity.hh>
#include <cantag/Function.hh>
#include <cantag/ConvexHullRestrictions.hh>

namespace Cantag {
  class CANTAG_EXPORT ConvexHull : public Function<TL1(ContourEntity),TL1(ConvexHullEntity)> {
  private:
    const ConvexHullRestrictions& m_restriction;
  public:
    ConvexHull(const ConvexHullRestrictions& restrict) : m_restriction(restrict) {};
    bool operator()(const ContourEntity& source, ConvexHullEntity& dest) const;
  };
}

#endif//CONVEXHULL_GUARD
