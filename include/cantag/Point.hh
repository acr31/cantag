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

#ifndef POINT_GUARD
#define POINT_GUARD

#include <utility>
using std::pair;

namespace Cantag
{
  class CANTAG_EXPORT Point
  {
  private:
    int m_x;
    int m_y;
  public:
    Point(int x, int y) : m_x(x), m_y(y) {}
    Point(const pair<int, int>& p) : m_x(p.first), m_y(p.second) {}
    inline bool operator<(const Point& rhs) const;
    int x() const { return m_x; }
    int y() const { return m_y; }
    void x(int x) { m_x = x; }
    void y(int y) { m_y = y; }
  };

  /** Lexicographic compare with x major and y minor (an arbitrary decision to satisfy the demands of ordered STL containers). */
  bool Point::operator<(const Point& rhs) const {
    return m_x < rhs.m_x || (m_x == rhs.m_x && m_y < rhs.m_y);
  }
}

#endif // POINT_GUARD
