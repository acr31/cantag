/*
  Copyright (C) 2006 Andrew C. Rice

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


#ifndef COMPARE_BULLSEYE_GUARD
#define COMPARE_BULLSEYE_GUARD

#include <cantag/Config.hh>
#include <cantag/Camera.hh>
#include <cantag/Function.hh>

namespace Cantag { 

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class CompareBullseyeRadiiObj : public Function<TL0,TL1(TreeNode<TransformEntity>) > {
  private:
    const Camera& m_camera;
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    
  public:
    CompareBullseyeRadiiObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec,const Camera& camera) : m_camera(camera), m_tagspec(tagspec) {};
    bool operator()(TreeNode<TransformEntity>& treenode) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool CompareBullseyeRadiiObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(TreeNode<TransformEntity>& node) const {
    // need a Transform entity
    TransformEntity& treenode = *(node.GetNode());
    
    Transform* t = treenode.GetPreferredTransform();

    if (t==NULL) return false;

    float location[3];
    t->GetLocation(m_camera,location,1.f);
    float parent_distance = location[2];

    float best = 1e10;
    node.Reset();
    while(node.HasNext()) {
      TreeNode<TransformEntity>* child = node.NextChild();
      if (child->IsValid()) {
	TransformEntity& childentity = *(child->GetNode());
	Transform* child_transform = childentity.GetPreferredTransform();
	
	// this transform has fitted this ellipse as if it were the
	// outer bullseye of the tag.  This will cause it to be
	// further away than it actually is.  Work out the ratio
	// between the distance it is away and the actual distance
	// (i.e. from the parent).  Also work out the ratio between
	// the outer and inner bullseye edges.  The difference between
	// these two ratios is the error.

	float child_location[3];
	child_transform->GetLocation(m_camera,child_location,1);
	float child_distance = child_location[2];

	float location_ratio = parent_distance / child_distance;
	float size_ratio = m_tagspec.GetBullseyeInnerEdge() / m_tagspec.GetBullseyeOuterEdge();
	float diff = abs<float>(location_ratio - size_ratio);
	if (diff < best) best = diff;
      }
    }
    treenode.SetRadiusError(best);    
    return true;
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  CompareBullseyeRadiiObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>
  CompareBullseyeRadii(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return CompareBullseyeRadiiObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }

}
#endif//COMPARE_BULLSEYE_GUARD
