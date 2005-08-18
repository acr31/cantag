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


#ifndef TRANSFORM_SELECT_ELLIPSE_GUARD
#define TRANSFORM_SELECT_ELLIPSE_GUARD

#include <cantag/Config.hh>
#include <cantag/Camera.hh>
#include <cantag/Function.hh>

namespace Cantag { 

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class TransformSelectEllipseObj : public Function<TL0,TL1(TreeNode<TransformEntity>) > {
  private:
    const Camera& m_camera;
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    
    inline float normalVectorConfidence(float normal1[3], float normal2[3]) const;
    inline float locationConfidence(float normal1[3], float normal2[3]) const;
  public:
    TransformSelectEllipseObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec,const Camera& camera) : m_camera(camera), m_tagspec(tagspec) {};
    bool operator()(TreeNode<TransformEntity>& treenode) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool TransformSelectEllipseObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(TreeNode<TransformEntity>& node) const {
    // need a Transform entity
    TransformEntity& treenode = *(node.GetNode());
    for(std::list<Transform*>::iterator i = treenode.GetTransforms().begin(); i != treenode.GetTransforms().end(); ++i) {
      if ((*i)->GetConfidence() > 0.f) {
	float normal[3];
	(*i)->GetNormalVector(m_camera,normal);
	float location[3];
	(*i)->GetLocation(location,m_tagspec.GetBullseyeOuterEdge());
      
	float maxConfidenceInChild = 0.f;
	node.Reset();
	while(node.HasNext()) {
	  TreeNode<TransformEntity>* child = node.NextChild();
	  if (child->IsValid()) {
	    TransformEntity& childentity = *(child->GetNode());
	    for(std::list<Transform*>::iterator k = childentity.GetTransforms().begin(); k != childentity.GetTransforms().end(); ++k) {
	      if ((*k)->GetConfidence() > 0.f) {
		float childnormal[3];
		(*k)->GetNormalVector(m_camera,childnormal);
		float childlocation[3];
		(*k)->GetLocation(childlocation,m_tagspec.GetBullseyeInnerEdge());
	      		
		float angleconfidence = normalVectorConfidence(normal,childnormal);
		float locationconfidence = locationConfidence(location,childlocation);
		float confidence = angleconfidence * locationconfidence;
		if (confidence > maxConfidenceInChild) maxConfidenceInChild = confidence;
	      }
	    }
	  }
	}
      
	(*i)->AccrueConfidence(maxConfidenceInChild);
      }
    }
    return true;
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> float TransformSelectEllipseObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::locationConfidence(float location1[3], float location2[3]) const {

    float distance = 
      (location1[0]-location2[0])*(location1[0]-location2[0])+
      (location1[1]-location2[1])*(location1[1]-location2[1])+
      (location1[2]-location2[2])*(location1[2]-location2[2]);
    if (distance > 50.f) return 0.f;
    else return 1.f-distance/50.f;

  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> float TransformSelectEllipseObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::normalVectorConfidence(float normal1[3], float normal2[3]) const {
  
    float cosangle = normal1[0]*normal2[0]+normal1[1]*normal2[1]+normal1[2]*normal2[2];
  
    float angle = acos(cosangle);
    //    if (angle > 0.17453293) return 0.f; else return 1.f;
    return angle / 2.f / M_PI;

    // turn angle into probability
    // might want to accept a few degrees variance as not affecting the probability
    // also 90 degrees is impossible
    
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  TransformSelectEllipseObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>
  TransformSelectEllipse(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return TransformSelectEllipseObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }

}
#endif//TRANSFORM_SELECT_ELLIPSE_GUARD
