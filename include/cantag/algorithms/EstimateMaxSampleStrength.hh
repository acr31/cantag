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
#include <cantag/TagCircle.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>
#include <cantag/Camera.hh>

namespace Cantag {
    
  /**
   * This algorithm estimates the sample points on a circular tag and searches
   * outwards from that point to the edge of the sector.  The minimum of these
   * distances for the tag is the estimate of the sample strength.  This is a
   * helper object you can create using the inline template function
   * EstimateMaxSampleStrength which will infer the template parameters
   */
    template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
    class EstimateMaxSampleStrengthObj : public Function<TL2(MonochromeImage,TransformEntity),TL1(MaxSampleStrengthEntity) > {
    private:
	const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
	const Camera& m_camera;

	float FindDistance(const MonochromeImage& image,bool sought, bool* buffer, int startx, int starty, float current) const;
    public:
	EstimateMaxSampleStrengthObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) : m_tagspec(tagspec),m_camera(camera) {};
	bool operator()(const MonochromeImage& image, const TransformEntity& source, MaxSampleStrengthEntity& strength) const;
    };
    
    template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool EstimateMaxSampleStrengthObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const MonochromeImage& image, const TransformEntity& source, MaxSampleStrengthEntity& strength) const {

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
	  if (tpt[0] < 0 || tpt[0] >= image.GetWidth() ||
	      tpt[1] < 0 || tpt[1] >= image.GetHeight()) { 
	    return false;
	  }

	  bool* buffer = new bool[image.GetWidth() * image.GetHeight()]();
	  for(size_t i = 0;i<image.GetWidth()*image.GetHeight();++i) { 
	      buffer[i] = false; 
	  }
	  int samplex = Round(tpt[0]);
	  int sampley = Round(tpt[1]);
	  float strength = FindDistance(image,!image.GetPixel(samplex,sampley),buffer,samplex,sampley, 0.f);
	  delete[] buffer;
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

    template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT> float EstimateMaxSampleStrengthObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::FindDistance(const MonochromeImage& image,bool sought, bool* buffer, int startx, int starty, float current) const {
	bool currentPix = image.GetPixel(startx,starty);
	if (currentPix == sought) return current;

	int dirx[] = {-1,0,1,1,1,0,-1,-1};
	int diry[] = {-1,-1,-1,0,1,1,1,0};

	buffer[startx + starty*image.GetWidth()] = true;
	float min = 1e10;
 	for(int c=0;c<8;++c) {
	    int nextx = startx + dirx[c];
	    int nexty = starty + diry[c];
	    if (nextx >= 0 && nextx < image.GetWidth() &&
		nexty >= 0 && nexty < image.GetHeight() &&
		!buffer[nextx + nexty*image.GetWidth()]) {
		float step = c % 2 == 0 ? sqrt(2.f) : 1.f;
		float est = FindDistance(image,sought,buffer,nextx,nexty,current+step);
		if (est < min) min = est;
	    }
	}
	return min;
    }

/**
 * the estimate should always be more conservative or equal to the actual
 * value because we might miss the worst case due to there not being a data
 * transition on the worst cell

 */ 

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  EstimateMaxSampleStrengthObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> EstimateMaxSampleStrength(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return EstimateMaxSampleStrengthObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }
}
#endif//ESTIMATE_MAX_SAMPLE_ERROR_GUARD
