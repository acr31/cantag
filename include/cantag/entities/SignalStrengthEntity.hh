/*
  Copyright (C) 2005 Andrew C. Rice

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

#ifndef SIGNAL_STRENGTH_ENTITY_GUARD
#define SIGNAL_STRENGTH_ENTITY_GUARD

namespace Cantag {

  /**
   * Contains the computed signal strength value.  This is a single
   * floating point number between 0 and 1.
   */
  class SignalStrengthEntity : public Entity {
  private:
    float m_signal_strength;
    int m_min;

  public:
    SignalStrengthEntity() : m_signal_strength(0.f) {}
    inline void SetSignalStrength(float val,int v2) { m_signal_strength = val; m_min=v2;}
    inline float GetSignalStrength() const { return m_signal_strength; }
    inline float GetMin() const { return m_min; }
  };

}

#endif//SIGNAL_STRENGTH_ENTITY_GUARD
