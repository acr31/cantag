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

#ifndef ESTIMATE_MAX_SAMPLE_ERROR_GUARD
#define ESTIMATE_MAX_SAMPLE_ERROR_GUARD

#include <cantag/entities/Entity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/entities/MaxSampleStrengthEntity.hh>
#include <cantag/TagCircle.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>
#include <cantag/Camera.hh>

namespace Cantag {

  static bool CheckPoints(const MonochromeImage& image, bool sought, int startx, int starty, int radius) {
    const int numpoints = 2000;
    for(int count = 0; count < numpoints; ++count) {
      float angle = (float)count / (float)numpoints * 2.f*FLT_PI;
      int pollx = Round((float)startx + (float)radius * sin(angle));
      int polly = Round((float)starty + (float)radius * cos(angle));
      
      if (pollx >= 0 && pollx < image.GetWidth() &&
	  polly >= 0 && polly < image.GetHeight() &&
	  image.GetPixel(pollx,polly) == sought) {
	return true;
      }
    }
    return false;
  }

  static float FindDistance(const MonochromeImage& image, bool sought, int startx, int starty, float current) {
    (void) current;
    int radius = 1;
    while(radius<50 && !CheckPoints(image,sought,startx,starty,radius)) ++radius;
    return radius;
  }
    
  /**
   * This algorithm estimates the sample points on a circular tag and searches
   * outwards from that point to the edge of the sector.  The minimum of these
   * distances for the tag is the estimate of the sample strength.  This is a
   * helper object you can create using the inline template function
   * EstimateMaxSampleStrength which will infer the template parameters
   */
    template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
    class EstimateMaxSampleStrengthCircleObj : public Function<TL2(MonochromeImage,TransformEntity),TL1(MaxSampleStrengthEntity) > {
    private:
	const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
	const Camera& m_camera;

    public:
	EstimateMaxSampleStrengthCircleObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) : m_tagspec(tagspec),m_camera(camera) {};
	bool operator()(const MonochromeImage& image, const TransformEntity& source, MaxSampleStrengthEntity& strength) const;
    };
    
    template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool EstimateMaxSampleStrengthCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const MonochromeImage& image, const TransformEntity& source, MaxSampleStrengthEntity& strength) const {

    bool return_result = false;
    const Transform* i = source.GetPreferredTransform();

    if (i) {
	AggregateMin<float> maxfn;
      int readindex = READ_COUNT/2;
      for(int j=0;j<SECTOR_COUNT;++j) {
	// read a chunk by sampling each ring and shifting and adding
	for(int k=0;k<RING_COUNT;++k) {
	  float tpt[]=  {  m_tagspec.GetXSamplePoint(readindex,RING_COUNT - 1 - k),
			   m_tagspec.GetYSamplePoint(readindex,RING_COUNT - 1 - k) };
	  i->Apply(tpt[0],tpt[1],tpt,tpt+1);
	  m_camera.NPCFToImage(tpt,1);
	  int samplex = Round(tpt[0]);
	  int sampley = Round(tpt[1]);

	  if (samplex < 0 || samplex >= image.GetWidth() ||
	      sampley < 0 || sampley >= image.GetHeight()) { 
	    return false;
	  }

	  float strength = FindDistance(image,!image.GetPixel(samplex,sampley),samplex,sampley, 0.f);
	  assert(strength <= 1e9);
	  maxfn(strength);

	}
	readindex+=READ_COUNT;
	readindex %= SECTOR_COUNT * READ_COUNT;
      }      
      strength.SetSampleStrength(maxfn());
      return_result = true;
    }
    return return_result;
  }


  /**
   * This algorithm estimates the sample points on a square tag and searches
   * outwards from that point to the edge of the sector.  The minimum of these
   * distances for the tag is the estimate of the sample strength.  This is a
   * helper object you can create using the inline template function
   * EstimateMaxSampleStrength which will infer the template parameters
   */
  template<int EDGE_COUNT>
    class EstimateMaxSampleStrengthSquareObj : public Function<TL2(MonochromeImage,TransformEntity),TL1(MaxSampleStrengthEntity) > {
    private:
	const TagSquare<EDGE_COUNT>& m_tagspec;
	const Camera& m_camera;

    public:
	EstimateMaxSampleStrengthSquareObj(const TagSquare<EDGE_COUNT>& tagspec, const Camera& camera) : m_tagspec(tagspec),m_camera(camera) {};
	bool operator()(const MonochromeImage& image, const TransformEntity& source, MaxSampleStrengthEntity& strength) const;
    };
    
    template<int EDGE_COUNT> bool EstimateMaxSampleStrengthSquareObj<EDGE_COUNT>::operator()(const MonochromeImage& image, const TransformEntity& source, MaxSampleStrengthEntity& strength) const {

      bool return_result = false;
      const Transform* i = source.GetPreferredTransform();
      
      if (i) {
	AggregateMin<float> maxfn;
	
	for(int j=0;j<TagSquare<EDGE_COUNT>::PayloadSize;j++) {
	  float pts[] = { m_tagspec.GetXSamplePoint(j),
			  m_tagspec.GetYSamplePoint(j) };
	  i->Apply(pts[0],pts[1],pts,pts+1);
	  m_camera.NPCFToImage(pts,1);
	  
	  int samplex = Round(pts[0]);
	  int sampley = Round(pts[1]);

	  if (samplex < 0 || samplex >= image.GetWidth() ||
	      sampley < 0 || sampley >= image.GetHeight()) { 
	    return false;
	  }

	  float strength = FindDistance(image,!image.GetPixel(samplex,sampley),samplex,sampley, 0.f);
	  assert(strength <= 1e9);
	  maxfn(strength);

	}

	strength.SetSampleStrength(maxfn());
	return_result = true;
      }
      return return_result;
    }

  
/**
 * the estimate should always be more conservative or equal to the actual
 * value because we might miss the worst case due to there not being a data
 * transition on the worst cell

 */ 

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  EstimateMaxSampleStrengthCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> EstimateMaxSampleStrength(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return EstimateMaxSampleStrengthCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }


  template<int EDGE_COUNT>
  inline
  EstimateMaxSampleStrengthSquareObj<EDGE_COUNT> EstimateMaxSampleStrength(const TagSquare<EDGE_COUNT>& tagspec, const Camera& camera) {
    return EstimateMaxSampleStrengthSquareObj<EDGE_COUNT>(tagspec,camera);
  }
}
#endif//ESTIMATE_MAX_SAMPLE_ERROR_GUARD
