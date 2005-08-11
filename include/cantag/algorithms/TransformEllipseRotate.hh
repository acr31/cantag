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

#ifndef TRANSFORM_ELLIPSE_ROTATE_GUARD
#define TRANSFORM_ELLIPSE_ROTATE_GUARD

#include <cantag/Function.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cmath>
namespace Cantag {

  /**
   * An algorithm to rotate the object co-ordinates of the transforms
   * to line up with a sector edge.  This is a helper object that may
   * be constructed using the templated function
   * TransformEllipseRotate which will infer template parameters
   * automatically
   */
  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT=5>
  class TransformEllipseRotateObj : public Function<TL1(MonochromeImage),TL1(TransformEntity)> {
  private:
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    const Camera& m_camera;
  public:
    TransformEllipseRotateObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) : m_tagspec(tagspec),m_camera(camera){}
    bool operator()(const MonochromeImage& image, TransformEntity& transform) const;
  };

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT> bool TransformEllipseRotateObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const MonochromeImage& image, TransformEntity& source) const {

    for(std::list<Transform*>::const_iterator i = source.GetTransforms().begin(); i != source.GetTransforms().end(); ++i) {
      Transform* transform = *i;
      bool left = true;
      bool right = true;
      int k=RING_COUNT-1;
      int j;
      // scan round reading a half sector width apart until we read two adjacent cells of different values
      for(j=0;j<SECTOR_COUNT*READ_COUNT;j+=READ_COUNT/2) {
	float tpt[]=  {  m_tagspec.GetXSamplePoint(j,k),
			 m_tagspec.GetYSamplePoint(j,k) };
	transform->Apply(tpt[0],tpt[1],tpt,tpt+1);
	m_camera.NPCFToImage(tpt,1);
	if (tpt[0] < 0 || tpt[0] >= image.GetWidth() ||
	    tpt[1] < 0 || tpt[1] >= image.GetHeight()) { 
	  return false;
	}
	right = image.GetPixel(tpt[0],tpt[1]);
	if ((j>0) && (left != right)) break;
	left = right;
      }
  
      if (left == right) {
#ifdef RING_TAG_DEBUG
	PROGRESS("Failed to find a sector edge!");
#endif
	continue;
      }
  
      int leftindex = j-READ_COUNT;
      int rightindex = j;
      while(rightindex - leftindex > 1) {
	int centre = (leftindex + rightindex) / 2;
	float tpt[]=  {  m_tagspec.GetXSamplePoint(centre,k),
			 m_tagspec.GetYSamplePoint(centre,k) };
	transform->Apply(tpt[0],tpt[1],tpt,tpt+1);
	m_camera.NPCFToImage(tpt,1);
	if (tpt[0] < 0 || tpt[0] >= image.GetWidth() ||
	    tpt[1] < 0 || tpt[1] >= image.GetHeight()) { 
	  return false;
	}
	bool sample = image.GetPixel(tpt[0],tpt[1]);
	if (sample) {
	  if (left && !right) leftindex = centre;
	  else if (!left && right) rightindex = centre;
	  else assert(false);
	}
	else {
	  if (left && !right) rightindex = centre;
	  else if (!left && right) leftindex = centre;
	  else assert(false);
	}
      }

      float cos,sin;
      m_tagspec.GetAngle(leftindex,cos,sin);
      transform->Rotate(cos,sin);
    }
    return true;
  }

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  inline
  TransformEllipseRotateObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> TransformEllipseRotate(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return TransformEllipseRotateObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }
}

#endif//TRANSFORM_ELLIPSE_ROTATE_GUARD
