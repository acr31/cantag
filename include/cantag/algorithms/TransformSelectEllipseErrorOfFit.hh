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


#ifndef TRANSFORM_SELECT_ELLIPSE_ERROROFFIT_GUARD
#define TRANSFORM_SELECT_ELLIPSE_ERROROFFIT_GUARD

#include <cantag/Config.hh>
#include <cantag/Camera.hh>
#include <cantag/Function.hh>

namespace Cantag { 

  template<class ErrorAlg, int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class TransformSelectEllipseErrorOfFitObj : public Function<TL1(TreeNode<ShapeEntity<Ellipse> >),TL1(TreeNode<TransformEntity>)> {
  private:
    const Camera& m_camera;
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    
  public:
    TransformSelectEllipseErrorOfFitObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec,const Camera& camera) : m_camera(camera), m_tagspec(tagspec) {};
    bool operator()(const TreeNode<ShapeEntity<Ellipse> >& shape, TreeNode<TransformEntity>& treenode) const;
  };

  template<class ErrorAlg, int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool TransformSelectEllipseErrorOfFitObj<ErrorAlg,RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const TreeNode<ShapeEntity<Ellipse> >& shapenode,  TreeNode<TransformEntity>& transnode) const {
      // need a shape entity
      TreeNode<ShapeEntity<Ellipse> >* shapeptr = (TreeNode<ShapeEntity<Ellipse> >*)&shapenode;
      shapeptr->Reset();

      if (!transnode.GetNode()->IsValid()) return false;

    TransformEntity& t_ent = *(transnode.GetNode());
    while(shapeptr->HasNext()) {
      const TreeNode<ShapeEntity<Ellipse> >* child = shapeptr->NextChild();
      const ShapeEntity<Ellipse>& treenode = *(child->GetNode());

      if (!treenode.IsValid()) continue;

      // estimate the transforms
      const int count = 200;
      float points[count*2];
      for(int i=0;i<count;++i) {
	  float x = cos( (float)i*2.f*FLT_PI/(float)count );
	  float innerx = x * m_tagspec.GetBullseyeInnerEdge() / m_tagspec.GetBullseyeOuterEdge();
//	  float midx = x * (1-m_tagspec.GetBullseyeInnerEdge() / m_tagspec.GetBullseyeOuterEdge())/2.f;
	  float y = sin( (float)i*2.f*FLT_PI/(float)count );
	  float innery = y * m_tagspec.GetBullseyeInnerEdge() / m_tagspec.GetBullseyeOuterEdge();
//	  float midy = y * (1-m_tagspec.GetBullseyeInnerEdge() / m_tagspec.GetBullseyeOuterEdge())/2.f;
	  points[i*2] = innerx;
	  points[i*2 + 1] = innery;
      }

      for(std::list<Transform*>::iterator i = t_ent.GetTransforms().begin();
	  i != t_ent.GetTransforms().end();
	  ++i) {
	  Transform* t = *i;
	  typename ErrorAlg::Aggregator agg;
	  for(int c=0;c<count;++c) {
	      float x;
	      float y;
	      t->Apply(points[2*c],points[2*c+1],&x,&y);
	      agg(ErrorAlg::eval(*(treenode.GetShape()),x,y));
	  }

	  float result = agg();
	  assert(result>=0.f);
//	  if (result > t->GetConfidence()) {
	      t->SetConfidence(result);
//	  }
      }
    }
    return true;
  }

  template<class ErrFunc, int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  TransformSelectEllipseErrorOfFitObj<ErrFunc,RING_COUNT,SECTOR_COUNT,READ_COUNT>
  TransformSelectEllipseErrorOfFit(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return TransformSelectEllipseErrorOfFitObj<ErrFunc, RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }

}
#endif//TRANSFORM_SELECT_ELLIPSE_ERROROFFIT_GUARD
