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


#ifndef SIMULATE_MAX_SAMPLE_ERROR_GUARD
#define SIMULATE_MAX_SAMPLE_ERROR_GUARD

#include <cantag/SpeedMath.hh>
#include <cantag/entities/TransformEntity.hh>

namespace Cantag {
  
  /**
   * Take a perfect transform for a particular Circle tag and one
   * derived from the image processing system.  Work out the maximum
   * distance from a perfect sample point to a corresponding derived
   * sample point.
   */
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class SimulateMaxSampleErrorCircleObj : public Function<TL1(TransformEntity),TL1(TransformEntity)> {
    Maxima& m_maxima;
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    const Camera& m_camera;
    void InitPoints(const TransformEntity& te, std::vector<float>& points) const;
  public:
    SimulateMaxSampleErrorCircleObj(Maxima& maxima, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) : m_maxima(maxima), m_tagspec(tagspec), m_camera(camera) {};
    bool operator()(const TransformEntity& tederived, TransformEntity& teperfect) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  void SimulateMaxSampleErrorCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::InitPoints(const TransformEntity& te, std::vector<float>& points) const {
    const Transform* i = te.GetPreferredTransform();
    if (i) {
      int readindex = READ_COUNT/2;
      for(int j=0;j<SECTOR_COUNT;++j) {
	// read a chunk by sampling each ring and shifting and adding
	for(int k=0;k<RING_COUNT;++k) {
	  float tpt[]=  {  m_tagspec.GetXSamplePoint(readindex,RING_COUNT - 1 - k),
			   m_tagspec.GetYSamplePoint(readindex,RING_COUNT - 1 - k) };
	  i->Apply(tpt[0],tpt[1],tpt,tpt+1);
	  m_camera.NPCFToImage(tpt,1);
	  points.push_back(tpt[0]);
	  points.push_back(tpt[1]);
	}
	readindex+=READ_COUNT;
	readindex %= SECTOR_COUNT * READ_COUNT;
      }      
    }
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> 
  bool SimulateMaxSampleErrorCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const TransformEntity& tederived, TransformEntity& teperfect) const {
    std::vector<float> perfect_points;
    InitPoints(teperfect,perfect_points);

    std::vector<float> derived_points;
    InitPoints(tederived,derived_points);
    
    /*
    std::vector<float>::const_iterator i = perfect_points.begin();
    std::vector<float>::const_iterator j = derived_points.begin();
    for(; i!=perfect_points.end() && j!=derived_points.end();++i,++j) {
      std::cerr << *(i++) << " " << *i << " " << *(j++) << " " << *j << std::endl;
    }
    std::cerr << std::endl;
    */

    std::vector<float>::const_iterator pi = perfect_points.begin();
    std::vector<float>::const_iterator di = derived_points.begin();
    for(;pi != perfect_points.end() && di != derived_points.end(); ++pi, ++di) {
      float px = *(pi++);
      float py = *pi;
      float dx = *(di++);
      float dy = *di;

      float distance = sqrt( (px-dx)*(px-dx) + (py-dy)*(py-dy) );
      m_maxima.UpdateMaxima(distance);
    }
    
    return true;
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  SimulateMaxSampleErrorCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> SimulateMaxSampleError(Maxima& maxima, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return SimulateMaxSampleErrorCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(maxima,tagspec,camera);
  }

  template<int EDGE_CELLS>
  class SimulateMaxSampleErrorSquareObj : public Function<TL1(TransformEntity),TL1(TransformEntity)> {
    Maxima& m_maxima;
    const TagSquare<EDGE_CELLS>& m_tagspec;
    const Camera& m_camera;
    void InitPoints(const TransformEntity& te, std::vector<float>& points) const;
  public:
    SimulateMaxSampleErrorSquareObj(Maxima& maxima, const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) : m_maxima(maxima), m_tagspec(tagspec), m_camera(camera) {};
    bool operator()(const TransformEntity& tederived, TransformEntity& teperfect) const;
  };

  template<int EDGE_CELLS>
  void SimulateMaxSampleErrorSquareObj<EDGE_CELLS>::InitPoints(const TransformEntity& te, std::vector<float>& points) const {
    const Transform* i = te.GetPreferredTransform();
    const int payloadsize = EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2);
    if (i) {
      for(int j=0;j<payloadsize;j++) {
	float pts[] = { m_tagspec.GetXSamplePoint(j),
			m_tagspec.GetYSamplePoint(j) };
	i->Apply(pts[0],pts[1],pts,pts+1);
	m_camera.NPCFToImage(pts,1);
	points.push_back(pts[0]);
	points.push_back(pts[1]);
      }
    }
  }

  template<int EDGE_CELLS>
  bool SimulateMaxSampleErrorSquareObj<EDGE_CELLS>::operator()(const TransformEntity& tederived, TransformEntity& teperfect) const {
    std::vector<float> perfect_points;
    InitPoints(teperfect,perfect_points);

    std::vector<float> derived_points;
    InitPoints(tederived,derived_points);

    /*
    std::vector<float>::const_iterator j = derived_points.begin();
    std::vector<float>::const_iterator i = perfect_points.begin();
    for(; i!=perfect_points.end() && j!=derived_points.end();++i,++j) {
      std::cerr << *(i++) << " " << *i << " " << *(j++) << " " << *j << std::endl;
    }
    std::cerr << std::endl;
    */
    Maxima m[4];

    const int rot_size = (EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS % 2))/2;  //doubled because there are two entries in the vector for each co-ordinate

    for(int c=0;c<perfect_points.size();c+=2) {
      for(int rot=0;rot<4;++rot) {
	
	float px = perfect_points[(c + rot*rot_size) % perfect_points.size()];
	float py = perfect_points[(c+1 + rot*rot_size) % perfect_points.size()];

	float dx = derived_points[c];
	float dy = derived_points[c+1];
	
	float distance = sqrt( (px-dx)*(px-dx) + (py-dy)*(py-dy) );
	m[rot].UpdateMaxima(distance);
      }
    }

    int min = -1;
    float minv = 1e10;
    for(int i=0;i<4;++i) {
      if (m[i].GetMaxima() < minv || min == -1) {
	min = i;
	minv = m[i].GetMaxima();
      }
    }

    m_maxima.UpdateMaxima(m[min].GetMaxima());

    
    return true;
  }

  template<int EDGE_CELLS>
  SimulateMaxSampleErrorSquareObj<EDGE_CELLS> SimulateMaxSampleError(Maxima& maxima, const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) {
    return SimulateMaxSampleErrorSquareObj<EDGE_CELLS>(maxima,tagspec,camera);
  }


}
#endif//SIMULATE_MAX_SAMPLE_ERROR_GUARD
