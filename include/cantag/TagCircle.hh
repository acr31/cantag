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

#ifndef TAG_CIRCLE_GUARD
#define TAG_CIRCLE_GUARD

#include <cmath>

#include <cantag/Config.hh>
#include <cantag/TagSpec.hh>
#include <cantag/ContourRestrictions.hh>
#include <cantag/ConvexHullRestrictions.hh>
#include <cantag/EllipseRestrictions.hh>
#include <cantag/Ellipse.hh>

namespace Cantag {

  template<int PARAM_RING_COUNT,int PARAM_SECTOR_COUNT,int PARAM_READ_COUNT=50>
  class TagCircle : public TagSpec<PARAM_RING_COUNT*PARAM_SECTOR_COUNT>, 
		    public ContourRestrictions, 
		    public ConvexHullRestrictions, 
		    public EllipseRestrictions {
  public:
    enum { RING_COUNT = PARAM_RING_COUNT };
    enum { SECTOR_COUNT = PARAM_SECTOR_COUNT };
    enum { READ_COUNT = PARAM_READ_COUNT };
    typedef Ellipse Shape;
  private:
    const float m_bullseye_inner_edge;
    const float m_bullseye_outer_edge;
    const float m_data_inner_edge;
    const float m_data_outer_edge;

    float *m_data_ring_inner_radii;
    float *m_data_ring_outer_radii;
    float *m_data_ring_centre_radii;
    float *m_sector_angles;
    float *m_read_angles;
  
    float *m_sin_read_angles;
    float *m_cos_read_angles;

    int m_minContourLength;
    int m_minContourWidth;
    int m_minContourHeight;    

  public:
    TagCircle(float bullseye_inner_edge, float bullseye_outer_edge, float data_inner_edge, float data_outer_edge);
    ~TagCircle();
    
    /**
     * Return the x-coordinate to sample the tag.  The sample point
     * will be the middle of the selected ring and at the angle
     * read_angle/(PARAM_RING_COUNT*PARAM_READ_COUNT) * 360
     */
    inline float GetXSamplePoint(int read_angle, int ring) const {
      return m_cos_read_angles[read_angle] * m_data_ring_centre_radii[ring];
    }
    
    /**
     * Return the y-coordinate to sample the tag.  The sample point
     * will be the middle of the selected ring and at the angle
     * read_angle/(PARAM_RING_COUNT*PARAM_READ_COUNT) * 360
     */
    inline float GetYSamplePoint(int read_angle, int ring) const {
      return m_sin_read_angles[read_angle] * m_data_ring_centre_radii[ring];
    }   

    /**
     * Return cos(read_angle/(PARAM_RING_COUNT*PARAM_READ_COUNT) * 360) and sin(read_angle/(PARAM_RING_COUNT*PARAM_READ_COUNT) * 360)
     */
    inline void GetAngle(int read_angle, float& cos, float& sin) const {
      cos = m_cos_read_angles[read_angle];
      sin = m_sin_read_angles[read_angle];
    }

    inline float GetBullseyeOuterEdge() const { return m_bullseye_outer_edge; }
    inline float GetBullseyeInnerEdge() const { return m_bullseye_inner_edge; }
    inline float GetDataOuterEdge() const { return m_data_outer_edge; }
    inline float GetDataInnerEdge() const { return m_data_inner_edge; }
    inline float GetDataRingOuterEdge(int ring) const { return m_data_ring_outer_radii[ring]; }
    inline float GetDataRingInnerEdge(int ring) const { return m_data_ring_inner_radii[ring]; }
    inline float GetReadAngle(int index) const { return m_sector_angles[index]; }
  };

  template<int PARAM_RING_COUNT,int PARAM_SECTOR_COUNT,int PARAM_READ_COUNT> TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::~TagCircle() {
    delete[] m_data_ring_inner_radii;
    delete[] m_data_ring_outer_radii;
    delete[] m_data_ring_centre_radii;
    delete[] m_sector_angles;
    delete[] m_read_angles;
    delete[] m_sin_read_angles;
    delete[] m_cos_read_angles;
  }
  
  template<int PARAM_RING_COUNT,int PARAM_SECTOR_COUNT,int PARAM_READ_COUNT> TagCircle<PARAM_RING_COUNT,PARAM_SECTOR_COUNT,PARAM_READ_COUNT>::TagCircle(float bullseye_inner_edge, float bullseye_outer_edge, float data_inner_edge, float data_outer_edge) :
    TagSpec<PARAM_SECTOR_COUNT*PARAM_RING_COUNT>(PARAM_SECTOR_COUNT,PARAM_RING_COUNT),
    ContourRestrictions(1,1,1),
    ConvexHullRestrictions(100000),
    EllipseRestrictions(1000,0.1),
    m_bullseye_inner_edge(bullseye_inner_edge),
    m_bullseye_outer_edge(bullseye_outer_edge),
    m_data_inner_edge(data_inner_edge),
    m_data_outer_edge(data_outer_edge)
  {
    assert(bullseye_inner_edge < bullseye_outer_edge);
    assert(data_inner_edge < data_outer_edge);
  
    // bullseye_inner_edge < bullseye_outer_edge < data_inner_edge < data_outer_edge
    // data_inner_edge < data_outer_edge < bullseye_inner_edge < bullseye_outer_edge
    // bullseye_inner_edge < data_inner_edge < data_outer_edge < bullseye_outer_edge
  
    // Populate the radii cache
    // Slice up the range data_inner_edge to data_outer_edge evenly.
  
    // there are two ways of doing this.
  
    // 1) give each one an equal amount
    // 2) give them different amounts so that the area of each sector is the same
  
    // Lets give each one an equal amount - first pass
  
    m_data_ring_centre_radii = new float[PARAM_RING_COUNT];
    m_data_ring_inner_radii = new float[PARAM_RING_COUNT];
    m_data_ring_outer_radii = new float[PARAM_RING_COUNT];
  
    float ring_width = (m_data_outer_edge-m_data_inner_edge)/PARAM_RING_COUNT;
    
    for(int i=0;i<PARAM_RING_COUNT;i++) {
      m_data_ring_inner_radii[i] = m_data_inner_edge + ring_width*(i);
      m_data_ring_outer_radii[i] = m_data_inner_edge + ring_width*(i+1);
      m_data_ring_centre_radii[i] = m_data_ring_outer_radii[i]-ring_width/2;    
    }
    
    // now the sector angles - go one more than necessary it makes drawing easier ;-)
    
    m_sector_angles = new float[PARAM_SECTOR_COUNT+1];
    for(int i=0;i<PARAM_SECTOR_COUNT+1;i++) {
      m_sector_angles[i] = 2*M_PI/PARAM_SECTOR_COUNT *i;
    }
    
    // when we read the tag we read a total of five times and then
    // look for three codes which are the same
    m_read_angles = new float[PARAM_SECTOR_COUNT*PARAM_READ_COUNT];
    m_sin_read_angles = new float[PARAM_SECTOR_COUNT*PARAM_READ_COUNT];
    m_cos_read_angles = new float[PARAM_SECTOR_COUNT*PARAM_READ_COUNT];
    for(int i=0;i<PARAM_SECTOR_COUNT*PARAM_READ_COUNT;i++) {
      m_read_angles[i] = 2*M_PI/PARAM_SECTOR_COUNT/PARAM_READ_COUNT * i;
      m_sin_read_angles[i] = sin(m_read_angles[i]);
      m_cos_read_angles[i] = cos(m_read_angles[i]);
    }
  }  
}
#endif//TAG_CIRCLE_GUARD
