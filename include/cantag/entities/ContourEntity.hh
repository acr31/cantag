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

#ifndef CONTOUR_ENTITY_GUARD
#define CONTOUR_ENTITY_GUARD

#include <cassert>
#include <cantag/Config.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {

  class ContourEntity : public Entity {
  public:
    enum bordertype_t { UNKNOWN = 2, OUTER_BORDER = 1, HOLE_BORDER = 0};
  private:
    
    /**
     * A unique ID for this contour - used to index it in the image
     */
    int m_nbd;

    /**
     * The type of this border
     */
    bordertype_t m_bordertype;

    /**
     * The unique id for this contour's parent
     */ 
    int m_parent_id;

    /**
     * The points on this contour
     */
    std::vector<float> m_points;

    /**
     * The length of the contour.  If we move by 0,6,4,2 then we add 1
     * to the contour else we add sqrt(2).  We approximate this in
     * integer arithmetic by adding 32 for every 4-connected point and
     * 45 (sqrt(2)*32 = 42.255) for every other point and then dividing
     * by 32 (left shift 5) at the end
     */
    int m_length;
    
    /**
     * The minimum X value for the contour (bounding box)
     */
    int m_minX;
    
    /**
     * The maximum X value for the contour (bounding box)
     */
    int m_maxX;
    
    /**
     * The minimum Y value for the contour (bounding box)
     */
    int m_minY;
    
    /**
     * The maximum Y value for the contour (bounding box)
     */
    int m_maxY;
    
    /**
     * True if this contour is concave, false if convex
     */
    bool m_concave;
    
    /**
     * The x co-ordinate of the first central moment
     */
    int m_centralX;
    
    /**
     * The y co-ordinate of the first central moment
     */
    int m_centralY;
    
    int m_currentX;
    int m_currentY;
    int m_numPoints;
    
  public:
    ContourEntity() : m_points(),
		      m_length(0),
		      m_minX(MAXINT),
		      m_maxX(MININT),
		      m_minY(MAXINT),
		      m_maxY(MININT),
		      m_concave(false),
		      m_centralX(0),
		      m_centralY(0),
		      m_numPoints(0) {};
    ContourEntity(const std::vector<float>& points);
    ~ContourEntity() {};

    void AddPoints(const std::vector<float>& points);

    inline int GetNBD() const { return m_nbd; }
    inline void SetNBD(int nbd) { m_nbd = nbd; }
    inline bordertype_t GetBorderType() const { return m_bordertype; }
    inline void SetBorderType(bordertype_t bordertype) { m_bordertype = bordertype; }
    inline int GetParentNBD() const { return m_parent_id; }
    inline void SetParentNBD(int parentid) { m_parent_id = parentid; }

    inline int GetNumPoints() const { return m_numPoints; }

    inline int GetLength() const { return m_length >> 5; }
    inline int GetWidth() const { return m_maxX - m_minX; }
    inline int GetHeight() const { return m_maxY - m_minY; }

    inline int GetMinX() const { return m_minX; }
    inline int GetMaxX() const { return m_maxX; }
    inline int GetMinY() const { return m_minY; }
    inline int GetMaxY() const { return m_maxY; }

    inline int GetCentralX() const { return m_centralX / m_numPoints; }
    inline int GetCentralY() const { return m_centralY / m_numPoints; }    

    inline const std::vector<float>& GetPoints() const { return m_points;}
    inline std::vector<float>& GetPoints() { return m_points; }
    void SetStart(int x, int y);
    void AddPoint(int freeman_code);

  private:
    ContourEntity(const ContourEntity& copyme) : 
      m_nbd(copyme.m_nbd),
      m_bordertype(copyme.m_bordertype),
      m_parent_id(copyme.m_parent_id),
      m_points(copyme.m_points) {};
  };

}
#endif//CONTOUR_ENTITY_GUARD
