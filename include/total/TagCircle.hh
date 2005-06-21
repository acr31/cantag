/**
 * $Header$
 */

#ifndef TAG_CIRCLE_GUARD
#define TAG_CIRCLE_GUARD

#include <cmath>
#include <total/ContourRestrictions.hh>
#include <total/ConvexHullRestrictions.hh>

namespace Total {

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT=5>
  class TagCircle  : public ContourRestrictions, public ConvexHullRestrictions {
  private:
    const float m_bullseye_inner_edge;
    const float m_bullseye_outer_edge;
    const float m_data_inner_edge;
    const float m_data_outer_edge;

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
    inline float GetXSamplePoint(int read_angle, int ring) const {
      return m_cos_read_angles[read_angle] * m_data_ring_centre_radii[ring]/m_bullseye_outer_edge;
    }
    
    inline float GetYSamplePoint(int read_angle, int ring) const {
      return m_sin_read_angles[read_angle] * m_data_ring_centre_radii[ring]/m_bullseye_outer_edge;
    }   
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>::~TagCircle() {
    delete[] m_data_ring_outer_radii;
    delete[] m_data_ring_centre_radii;
    delete[] m_sector_angles;
    delete[] m_read_angles;
    delete[] m_sin_read_angles;
    delete[] m_cos_read_angles;
  }
  
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>::TagCircle(float bullseye_inner_edge, float bullseye_outer_edge, float data_inner_edge, float data_outer_edge) :
    ContourRestrictions(30,10,10),
    ConvexHullRestrictions(100000),
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
  
    m_data_ring_centre_radii = new float[RING_COUNT];
    m_data_ring_outer_radii = new float[RING_COUNT];
  
    float ring_width = (m_data_outer_edge-m_data_inner_edge)/RING_COUNT;
    
    for(int i=0;i<RING_COUNT;i++) {
      m_data_ring_outer_radii[i] = m_data_inner_edge + ring_width*(i+1);
      m_data_ring_centre_radii[i] = m_data_ring_outer_radii[i]-ring_width/2;    
    }
    
    // now the sector angles - go one more than necessary it makes drawing easier ;-)
    
    m_sector_angles = new float[SECTOR_COUNT+1];
    for(int i=0;i<SECTOR_COUNT+1;i++) {
      m_sector_angles[i] = 2*M_PI/SECTOR_COUNT *i;
    }
    
    // when we read the tag we read a total of five times and then
    // look for three codes which are the same
    m_read_angles = new float[SECTOR_COUNT*READ_COUNT];
    m_sin_read_angles = new float[SECTOR_COUNT*READ_COUNT];
    m_cos_read_angles = new float[SECTOR_COUNT*READ_COUNT];
    for(int i=0;i<SECTOR_COUNT*READ_COUNT;i++) {
      m_read_angles[i] = 2*M_PI/SECTOR_COUNT/READ_COUNT * i;
      m_sin_read_angles[i] = sin(m_read_angles[i]);
      m_cos_read_angles[i] = cos(m_read_angles[i]);
    }
  }
  
  
}
#endif//TAG_CIRCLE_GUARD
