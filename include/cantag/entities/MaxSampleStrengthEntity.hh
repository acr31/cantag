/*
  Copyright (C) 2006 Andrew C. Rice

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

#ifndef MAX_SAMPLE_STRENGTH_ENTITY_GUARD
#define MAX_SAMPLE_STRENGTH_ENTITY_GUARD

namespace Cantag {

  /**
   * Contains the computed signal strength value.  This is a single
   * floating point number between 0 and 1.
   */
  class CANTAG_EXPORT MaxSampleStrengthEntity : public Entity {
  private:
    float m_sample_strength;

  public:
    MaxSampleStrengthEntity() : m_sample_strength(0.f) {}
    inline void SetSampleStrength(float val) { m_sample_strength = val;}
    inline float GetSampleStrength() const { return m_sample_strength; }
  };

}

#endif//MAX_SAMPLE_STRENGTH_ENTITY_GUARD
