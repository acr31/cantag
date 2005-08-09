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

#ifndef ELLIPSE_RESTRICTIONS_GUARD
#define ELLIPSE_RESTRICTIONS_GUARD

class EllipseRestrictions {
private:
  float m_maxFitError;
  float m_maxConcentricDistance;

public:

  EllipseRestrictions(float maxFitError,float maxConcentricDistance) : m_maxFitError(maxFitError), m_maxConcentricDistance(maxConcentricDistance) {}
  
  inline float GetMaxFitError() const { return m_maxFitError; }
  inline float GetMaxConcentricDistance() const { return m_maxConcentricDistance; }

  inline void SetMaxFitError(float maxFitError) { m_maxFitError = maxFitError; }
  inline void SetMaxConcentricDistance(float maxDist) { m_maxConcentricDistance = maxDist; }

  inline bool CheckFit(float error) const {
    return m_maxFitError > error;
  }

  inline bool CheckDistance(float distance) const {
    return m_maxConcentricDistance > distance; 
  }

  inline bool CheckDistanceSquared(float distancesq) const {
    return m_maxConcentricDistance*m_maxConcentricDistance > distancesq; 
  }
};

#endif//ELLIPSE_RESTRICTIONS_GUARD
