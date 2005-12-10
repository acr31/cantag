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

#ifndef SIMULATE_CONTOUR_GUARD
#define SIMULATE_CONTOUR_GUARD

#include <cmath>

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/TagCircle.hh>
#include <cantag/TagSquare.hh>
#include <cantag/Camera.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/entities/ContourEntity.hh>

namespace Cantag {
  
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class SimulateContourCircleObj : Function<TL1(TransformEntity),TL1(ContourEntity)> {
  private:
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    const Camera& m_camera;    
  public:
    SimulateContourCircleObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) :  m_tagspec(tagspec), m_camera(camera) {};
    bool operator()(const TransformEntity& te, ContourEntity& ce) const;
  };

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  inline
  SimulateContourCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> SimulateContour(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return SimulateContourCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  bool 
  SimulateContourCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const TransformEntity& te, ContourEntity& ce) const {
    Ellipse e(0.f,0.f,m_tagspec.GetBullseyeOuterEdge(),m_tagspec.GetBullseyeOuterEdge(),0.f);
    std::vector<float> points;
    e.Draw(points);
    te.GetPreferredTransform()->Apply(points);
    m_camera.NPCFToImage(points);
    ce.AddPoints(points);
    return true;
  }
  

  template<int EDGE_CELLS>
  class SimulateContourSquareObj : Function<TL1(TransformEntity),TL1(ContourEntity)> {
  private:
    const TagSquare<EDGE_CELLS>& m_tagspec;
    const Camera& m_camera;
  public:
    SimulateContourSquareObj(const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) : m_tagspec(tagspec), m_camera(camera) {};
    bool operator()(const TransformEntity& te, ContourEntity& ce) const;
  };

  template<int EDGE_CELLS>
  inline
  SimulateContourSquareObj<EDGE_CELLS> SimulateContour(const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) {
    return SimulateContourSquareObj<EDGE_CELLS>(tagspec,camera);
  }

  template<int EDGE_CELLS> 
  bool
  SimulateContourSquareObj<EDGE_CELLS>::operator()(const TransformEntity& te, ContourEntity& ce) const {
    QuadTangle q(-1,-1,
		 1,-1,
		 1,1,
		 -1,1,0,1,2,3);
    std::vector<float> points;
    q.Draw(points);
    te.GetPreferredTransform()->Apply(points);
    m_camera.NPCFToImage(points);
    ce.AddPoints(points);
    return true;
  }

}
#endif//SIMULATE_MIN_DISTANCE_GUARD
    
