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

#ifndef SIMULATE_MIN_DISTANCE_GUARD
#define SIMULATE_MIN_DISTANCE_GUARD

#include <cmath>

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/TagCircle.hh>
#include <cantag/TagSquare.hh>
#include <cantag/Camera.hh>
#include <cantag/SpeedMath.hh>
#include <cantag/entities/TransformEntity.hh>

namespace Cantag {

  namespace Simulate {
    double ComputeDistance(double xc, double yc, double x0, double y0, double x1, double y1);

    /**
     * Take corner_count corners from the array corners and compute the
     * minimum distance between the centre point and the line made between
     * each sequential pair of corners.  Return the most minimal distance.
     */   
    double ComputeAll(double xc, double yc, double* corners, int corner_count);
  }
  
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class SimulateMinDistanceCircleObj : Function<TL0,TL1(TransformEntity)> {
  private:
    enum { TOP_APPROX_COUNT = 100,
	   BOTTOM_APPROX_COUNT = 100 };

    Minima& m_minimum;    
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    const Camera& m_camera;    
    void InitPoints(std::vector<float>& points) const;
  public:
    SimulateMinDistanceCircleObj(Minima& minima, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) : m_minimum(minima), m_tagspec(tagspec), m_camera(camera) {};
    bool operator()(TransformEntity& te) const;
  };

  template<int EDGE_CELLS>
  class SimulateMinDistanceSquareObj : Function<TL0,TL1(TransformEntity)> {
  private:
    Minima& m_minimum;
    const TagSquare<EDGE_CELLS>& m_tagspec;
    const Camera& m_camera;
    void InitPoints(std::vector<float>& points) const;
  public:
    SimulateMinDistanceSquareObj(Minima& minima, const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) : m_minimum(minima), m_tagspec(tagspec), m_camera(camera) {};
    bool operator()(TransformEntity& te) const;
  };

  template<int EDGE_CELLS>
  inline
  SimulateMinDistanceSquareObj<EDGE_CELLS> SimulateMinDistance(Minima& minima, const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) {
    return SimulateMinDistanceSquareObj<EDGE_CELLS>(minima,tagspec,camera);
  }

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  inline
  SimulateMinDistanceCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> SimulateMinDistance(Minima& minima, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return SimulateMinDistanceCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(minima,tagspec,camera);
  }
      
  template<int EDGE_CELLS>
  bool SimulateMinDistanceSquareObj<EDGE_CELLS>::operator()(TransformEntity& te) const {
    std::vector<float> points;
    InitPoints(points);
    te.GetPreferredTransform()->Apply(points);
    m_camera.NPCFToImage(points);
    for(std::vector<float>::const_iterator i = points.begin(); i!=points.end();++i) {
      double pointsa[8] = { *(i++),*(i++), // x0,y0
			    *(i++),*(i++), // x1,y1
			    *(i++),*(i++), // x2,y2
			    *(i++),*(i++)}; // x3,y3
      float xc = *(i++);
      float yc = *i;
    
      double min = Simulate::ComputeAll(xc,yc,pointsa,4);
      m_minimum.UpdateMinima(min);
    }
    return true;
  }

  /**
   * Push five points on to the vector for each cell in the tag.  The
   * first four points are the corners of the square and the fifth is
   * the centre.
   */
  template<int EDGE_CELLS>
  void SimulateMinDistanceSquareObj<EDGE_CELLS>::InitPoints(std::vector<float>& points) const {
    for(int i=0;i < EDGE_CELLS; ++i) {
      for(int j=0; j < EDGE_CELLS; ++j) {
	// divide the tag into EDGE_CELLS+2 rows and columns then the
	// ith cell (counting from zero) covers the i+1th column and
	// thus starts at (i+1) / (EDGE_CELLS+2) and finishes at (i+2)/(EDGE_CELLS+2)
	float startx = 2.f * ((float)i + 1.f) / ((float)EDGE_CELLS + 2.f) - 1.f;
	float endx = 2.f * ((float)i + 2.f) / ((float)EDGE_CELLS + 2.f) - 1.f;
	float midx = 2.f * ((float)i + 1.5f) / ((float)EDGE_CELLS + 2.f) - 1.f;
	
	float starty = 2.f * ((float)j + 1.f) / ((float)EDGE_CELLS + 2.f) - 1.f;
	float endy = 2.f * ((float)j + 2.f) / ((float)EDGE_CELLS + 2.f) - 1.f;
	float midy = 2.f * ((float)j + 1.5f) / ((float)EDGE_CELLS + 2.f) - 1.f;
	
	// top left
	points.push_back(startx);
	points.push_back(starty);
	
	// top right
	points.push_back(endx);
	points.push_back(starty);
	
	// bottom right
	points.push_back(endx);
	points.push_back(endy);
	
	// bottom left
	points.push_back(startx);
	points.push_back(endy);
	
	// middle
	points.push_back(midx);
	points.push_back(midy);
      }
    }
  }
  
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  bool SimulateMinDistanceCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(TransformEntity& te) const {
    std::vector<float> points;
    InitPoints(points);

    te.GetPreferredTransform()->Apply(points);
    m_camera.NPCFToImage(points);
    for(std::vector<float>::const_iterator i = points.begin(); i!=points.end();++i) {
      double pointsa[2*(4 + TOP_APPROX_COUNT + BOTTOM_APPROX_COUNT)];
      for(int c=0;c<4+TOP_APPROX_COUNT+BOTTOM_APPROX_COUNT;++c) {
	pointsa[2*c] = *(i++);
	pointsa[2*c+1] = *(i++);
      }
      float xc = *(i++);
      float yc = *i;
      
      double min = Simulate::ComputeAll(xc,yc,pointsa,4+TOP_APPROX_COUNT+BOTTOM_APPROX_COUNT);

      m_minimum.UpdateMinima(min);
    }
    return true;
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  void SimulateMinDistanceCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::InitPoints(std::vector<float>& points) const {    
    for(int ring=0;ring<RING_COUNT;++ring) {
      float inner_proportion = m_tagspec.GetDataInnerEdge() + ring * ((m_tagspec.GetDataOuterEdge() - m_tagspec.GetDataInnerEdge()) / (float)RING_COUNT);
      float outer_proportion = m_tagspec.GetDataInnerEdge() + (ring+1) * ((m_tagspec.GetDataOuterEdge() - m_tagspec.GetDataInnerEdge()) / (float)RING_COUNT);

      for(int i=0;i<SECTOR_COUNT;++i) {
	double start_theta = 2*M_PI*(double)i/(double)SECTOR_COUNT;
	double end_theta = 2*M_PI*((double)i+1.0)/(double)SECTOR_COUNT;
	
	double cos_start_theta = cos(start_theta);
	double sin_start_theta = sin(start_theta);
	double cos_end_theta = cos(end_theta);
	double sin_end_theta = sin(end_theta);
	      
	points.push_back(cos_start_theta * inner_proportion);
	points.push_back(sin_start_theta * inner_proportion);
      
	for(int j=1;j<=BOTTOM_APPROX_COUNT;++j) {
	  double theta = start_theta + (end_theta-start_theta)*(double)j/(double)(BOTTOM_APPROX_COUNT+1);
	  double cosv = cos(theta)*inner_proportion;
	  double sinv = sin(theta)*inner_proportion;
	  points.push_back(cosv);
	  points.push_back(sinv);
	}
	
	points.push_back(cos_end_theta * inner_proportion);
	points.push_back(sin_end_theta * inner_proportion);
	
	points.push_back(cos_end_theta * outer_proportion);
	points.push_back(sin_end_theta * outer_proportion);
	
	for(int j=1;j<=TOP_APPROX_COUNT;++j) {
	  double theta = end_theta - (end_theta-start_theta)*(double)j/(double)(TOP_APPROX_COUNT+1);
	  double cosv = cos(theta)*outer_proportion;
	  double sinv = sin(theta)*outer_proportion;
	  points.push_back(cosv);
	  points.push_back(sinv);      
	}
	
	points.push_back(cos_start_theta * outer_proportion);
	points.push_back(sin_start_theta * outer_proportion);        
	
	double mid_theta = (end_theta + start_theta) / 2;
	double cosv = cos(mid_theta) * (inner_proportion + outer_proportion)/2;
	double sinv = sin(mid_theta) * (inner_proportion + outer_proportion)/2;
	
	points.push_back(cosv);
	points.push_back(sinv);          
      }
    }
  }
}
#endif//SIMULATE_MIN_DISTANCE_GUARD
    
