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

#ifndef TAG_SPEC_GUARD
#define TAG_SPEC_GUARD

#include <cantag/Config.hh>
#include <cantag/SpeedMath.hh>
#include <cantag/coders/Coder.hh>
#include <cmath>
namespace Cantag {

  /**
   * Concrete superclass for tag specifications.
   */ 
  template<int PAYLOAD_SIZE>
  class TagSpec : public virtual Coder<PAYLOAD_SIZE> {
  private:
    /**
     * Number of degrees of rotational symmetry
     */
    int m_symmetry;

    /**
     * The number of data cells we go past when we rotate by one degree of symmetry
     */
    int m_cells_per_rotation;

  public:
    TagSpec(int symmetry, int cells_per_rotation) : m_symmetry(symmetry),m_cells_per_rotation(cells_per_rotation) {}

    /**
     * Set vcos and vsin to the angle required to rotate the tag by the number of cells given
     */
    inline void GetCellRotation(int cells, float& vcos, float& vsin) const {
      float angle = (float)(cells / m_cells_per_rotation) / (float)m_symmetry * 2*FLT_PI;
      //      if (angle > FLT_PI) angle = FLT_PI - angle;
      //      vcos = DCOS(8,angle);
      //      vsin = DSIN(8,angle);
      vcos = cos(angle);
      vsin = sin(angle);
    }

    /**
     * Return the number of cells that would go past if we rotated by this angle
     */
    inline int GetPayloadRotation(float angle) const {
      if (angle < 0) angle += 2*FLT_PI;
      // each rotation of 2 * FLT_PI / m_symmetry is a rotation of m_cells_per_rotation
      return (m_cells_per_rotation * Round((float)m_symmetry * (angle / 2.f / FLT_PI))) % PAYLOAD_SIZE;
      //      return (int)floorf(m_symmetry * (angle / 2.f * FLT_PI));
    }
  };

};

#endif//TAG_SPEC_GUARD
