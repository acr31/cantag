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

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class TransformSelectEllipseErrorOfFitObj : public Function<TL1(TreeNode<ShapeEntity<Ellipse> >),TL1(TreeNode<TransformEntity>) > {
  private:
    const Camera& m_camera;
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    
  public:
    TransformSelectEllipseErrorOfFitObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec,const Camera& camera) : m_camera(camera), m_tagspec(tagspec) {};
    bool operator()(const TreeNode<ShapeEntity<Ellipse> >& shape, TreeNode<TransformEntity>& treenode) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool TransformSelectEllipseErrorOfFitObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const TreeNode<ShapeEntity<Ellipse> >& shape, TreeNode<TransformEntity>& node) const {
    return true;
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  TransformSelectEllipseErrorOfFitObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>
  TransformSelectEllipseErrorOfFit(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return TransformSelectEllipseErrorOfFitObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }

}
#endif//TRANSFORM_SELECT_ELLIPSE_ERROROFFIT_GUARD
