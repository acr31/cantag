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

#ifndef CONTOUR_RESTRICTIONS_GUARD
#define CONTOUR_RESTRICTIONS_GUARD

class ContourRestrictions {
private:
  int m_minContourLength;
  int m_minContourWidth;
  int m_minContourHeight;    

public:

  ContourRestrictions(int minlength, int minwidth, int minheight) : m_minContourLength(minlength),m_minContourWidth(minwidth),m_minContourHeight(minheight) {}
  
  inline int GetMinContourLength() const { return m_minContourLength; }
  inline int GetMinContourWidth() const { return m_minContourWidth; }
  inline int GetMinContourHeight() const { return m_minContourHeight; }

  inline void SetContourRestrictions(int minlength, int minwidth, int minheight) {
    m_minContourLength = minlength;
    m_minContourWidth = minwidth;
    m_minContourHeight = minheight;
  }

  inline bool CheckDimensions(int length, int width, int height) const {
    return length > m_minContourLength && width > m_minContourWidth && height > m_minContourHeight;
  }
};

#endif//CONTOUR_RESTRICTIONS_GUARD
