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

#ifndef DISTORTION_CORRECTION_ITERATIVE_GUARD
#define DISTORTION_CORRECTION_ITERATIVE_GUARD

#include <cantag/Config.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/Camera.hh>
#include <cantag/Function.hh>

#ifndef HAVE_GSL
#error DistortionCorrectionIterative requires the GNU Scientific Library
#endif

namespace Cantag {
  class CANTAG_EXPORT DistortionCorrectionIterative : public Function<TL0,TL1(ContourEntity)> {
  private:
    const Camera& m_camera;
    const bool    m_cache;
  public:
    DistortionCorrectionIterative(const Camera& camera, const bool useCache) : m_camera(camera), m_cache(useCache) {};
    bool operator()(ContourEntity& dest) const;
  };
}

#endif//DISTORTION_CORRECTION_ITERATIVE_GUARD
