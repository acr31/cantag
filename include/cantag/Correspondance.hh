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

/**
 * $Header$
 */

#ifndef CORRESPONDANCE_GUARD
#define CORRESPONDANCE_GUARD

#include <cantag/Config.hh>
#include <cantag/entities/TransformEntity.hh>

#include <list>

namespace Cantag {
  /**
   * The Correspondance class is used to maintain a mapping between an
   * observed image point and a real-world co-ordinate. This is used
   * when estimating the camera position etc.
   */
  class Correspondance {
  private:
    float m_imagex;
    float m_imagey;
    float m_worldx;
    float m_worldy;
    float m_worldz;

  public:
    Correspondance(float imagex,float imagey, float worldx, float worldy, float worldz) : m_imagex(imagex), m_imagey(imagey), m_worldx(worldx), m_worldy(worldy), m_worldz(worldz) {};

    float GetImageX() const { return m_imagex; }
    float GetImageY() const { return m_imagey; }
    float GetWorldX() const { return m_worldx; }
    float GetWorldY() const { return m_worldy; }
    float GetWorldZ() const { return m_worldz; }
    
  };
}
  
#endif//CORRESPONDANCES_GUARD
  
