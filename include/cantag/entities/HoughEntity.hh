/*
  Copyright (C) 2007 Tom Craig

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

  Email: jatc2@cam.ac.uk
*/

/**
 * $Header$
 */

#ifndef HOUGH_ENTITY_GUARD
#define HOUGH_ENTITY_GUARD

#include <cantag/entities/Entity.hh>

namespace Cantag
{
  class CANTAG_EXPORT HoughEntity : public Entity
  {
  private:
    float m_r;
    float m_theta;
    int m_acc;
    HoughEntity(const HoughEntity& rhs);
  public:
    HoughEntity() : m_r(0), m_theta(0), m_acc(0) {}
    HoughEntity(float r, float theta) : m_r(r), m_theta(theta), m_acc(0) {}
    float GetPerpendicularDistance() { return m_r; }
    void SetPerpendicularDistance(float r) { m_r = r; }
    float GetAngle() { return m_theta; }
    void SetAngle(float theta) { m_theta = theta; }
    float GetAccumulator() { return m_acc; }
    void IncrementAccumulator() { ++m_acc; }
    void SetAccumulator(int acc) { m_acc = acc; }
  };

}

#endif//HOUGH_ENTITY_GUARD
