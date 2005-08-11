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

#ifndef SAMPLE_CIRCLE_TAG
#define SAMPLE_CIRCLE_TAG

#include <cantag/entities/Entity.hh>
#include <cantag/TagCircle.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>

namespace Cantag {

  /**
   * This algorithm samples points on a circular tag and stores the
   * raw data in the decode entity ready for decoding.  This is a
   * helper object you can create using the inline template function
   * SampleTagCircle which will infer the template parameters
   */
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class SampleTagCircleObj : public Function<TL2(MonochromeImage,TransformEntity),TL1(DecodeEntity<RING_COUNT*SECTOR_COUNT>) > {
  private:
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    const Camera& m_camera;

  public:
    SampleTagCircleObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) : m_tagspec(tagspec),m_camera(camera) {};
    bool operator()(const MonochromeImage& image, const TransformEntity& source, DecodeEntity<RING_COUNT*SECTOR_COUNT>& destination) const;
  };
  
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool SampleTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const MonochromeImage& image, const TransformEntity& source, DecodeEntity<RING_COUNT*SECTOR_COUNT>& destination) const {

    bool return_result = false;

    const Transform* i = source.GetPreferredTransform();

    if (i) {
      typename DecodeEntity<RING_COUNT*SECTOR_COUNT>::Data* payload = destination.Add();
      
      //      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(image,128);
      int index = 0;
      int readindex = READ_COUNT/2;
      for(int j=0;j<SECTOR_COUNT;++j) {
	// read a chunk by sampling each ring and shifting and adding
	for(int k=RING_COUNT-1;k>=0;--k) {
	  float tpt[]=  {  m_tagspec.GetXSamplePoint(readindex,k),
			   m_tagspec.GetYSamplePoint(readindex,k) };
	  i->Apply(tpt[0],tpt[1],tpt,tpt+1);
	  m_camera.NPCFToImage(tpt,1);
	  if (tpt[0] < 0 || tpt[0] >= image.GetWidth() ||
	      tpt[1] < 0 || tpt[1] >= image.GetHeight()) { 
	    return false;
	  }
	  bool sample = image.GetPixel(tpt[0],tpt[1]);
	  //	  output.DrawPixel(tpt[0],tpt[1],255);
	  std::cout << "Read " << sample << std::endl;
	  
	  (payload->payload)[index] = sample;      
	  index++;
	}
	readindex+=READ_COUNT;
	readindex %= SECTOR_COUNT * READ_COUNT;
      }      
      payload->confidence = 1.f;
      return_result = true;
//      output.Save("output.pnm");

    }
    return return_result;
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  SampleTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> SampleTagCircle(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) {
    return SampleTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,camera);
  }
}
#endif//SAMPLE_CIRCLE_TAG
