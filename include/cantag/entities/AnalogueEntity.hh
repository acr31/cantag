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

#ifndef ANALOGUE_ENTITY_GUARD
#define ANALOGUE_ENTITY_GUARD

#include <cantag/Config.hh>
#include <cantag/CyclicBitSet.hh>
#include <cantag/entities/Entity.hh>

namespace Cantag {

  class AnalogueEntity : public Entity {
  private:
    /**
     * For the time being, only have one bar.
     */
    float m_payload;
    float m_max_error;
  public:
    AnalogueEntity() : Entity(), m_payload(0.0f), m_max_error(0.0f) {}
    float GetPayload() const { return m_payload; }
    void SetPayload(float payload) { m_payload = payload; }
    float GetMaxError() const { return m_max_error; }
    void SetMaxError(float max_error) { m_max_error = max_error; }
  private:
    AnalogueEntity(const AnalogueEntity& rhs) {}
  };
}

#endif//ANALOGUE_ENTITY_GUARD
