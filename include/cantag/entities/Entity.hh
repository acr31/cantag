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

#ifndef ENTITY_GUARD
#define ENTITY_GUARD

#include <cantag/Config.hh>

#include <list>
#include <vector>
#include <iostream>
namespace Cantag {
  class Entity {
  protected:
    bool m_valid;
    Entity() : m_valid(false) {};
  public:
    void SetValid(bool valid) { m_valid = valid;}
    bool IsValid() const { return m_valid; }
  };

  
  class LocatedEntity : public Entity {
  protected:    
    float normal[3];
    float location[3];
    float angle;

  public:
    LocatedEntity() {};
    ~LocatedEntity() {};

  private:
    LocatedEntity(const LocatedEntity& copyme) {}
  };

}

#endif//ENTITY_GUARD
