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

#include <cantag/entities/ContourEntity.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {

  ContourEntity::ContourEntity(const std::vector<float>& points) : 
    m_points(),
    m_length(0),
    m_minX(int_max()),
    m_maxX(int_min()),
    m_minY(int_max()),
    m_maxY(int_min()),
    m_concave(false),
    m_centralX(0),
    m_centralY(0),
    m_numPoints(0) {    
    AddPoints(points);
  }

  void ContourEntity::AddPoints(const std::vector<float>& points) {
    m_centralX *= m_numPoints;
    m_centralY *= m_numPoints;

    for(std::vector<float>::const_iterator i = points.begin(); i!= points.end();++i) {
      float x = *(i++);
      float y = *i;

      m_centralX += Round(x);
      m_centralY += Round(y);
      ++m_numPoints;

      if (x < m_minX) m_minX = Round(x);
      if (x > m_maxX) m_maxX = Round(x);
      if (y < m_minY) m_minY = Round(y);
      if (y > m_maxY) m_maxY = Round(y);      
      
      m_points.push_back(x);
      m_points.push_back(y);
    }

    m_centralX /= m_numPoints;
    m_centralY /= m_numPoints;
  }

  void ContourEntity::SetStart(int x, int y) {
    m_points.clear();
    m_currentX = m_centralX = m_minX = m_maxX = x;
    m_currentY = m_centralX = m_minY = m_maxY = y;
    m_numPoints = 1;
    m_length=32;
    m_points.push_back(x);
    m_points.push_back(y);
  }

  void ContourEntity::AddPoint(int freeman) {
    assert(freeman >= -1 && freeman < 8);

    if (freeman == -1) return;

    static int offset_x[] = {-1,-1,0,1,1,1,0,-1};
    static int offset_y[] = {0,1,1,1,0,-1,-1,-1};
    bool is4connected = freeman == 0 || freeman == 6 || freeman == 4 || freeman == 2;
    int newX = m_currentX += offset_x[freeman];
    int newY = m_currentY += offset_y[freeman];
    
    m_length += is4connected ? 32 : 45;
    if (newX < m_minX) m_minX = newX;
    if (newX > m_maxX) m_maxX = newX;
    if (newY < m_minY) m_minY = newY;
    if (newY > m_maxY) m_maxY = newY;
    m_centralX += newX;
    m_centralY += newY;
    m_currentX = newX;
    m_currentY = newY;
    ++m_numPoints;
    m_points.push_back(newX);
    m_points.push_back(newY);
  }

}
