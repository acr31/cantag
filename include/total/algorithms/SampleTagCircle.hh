/**
 * $Header$
 */

#ifndef SAMPLE_CIRCLE_TAG
#define SAMPLE_CIRCLE_TAG

#include <total/entities/Entity.hh>
#include <total/TagCircle.hh>
#include <total/MonochromeImage.hh>
#include <total/Function.hh>

namespace Total {

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT=5>
  class SampleTagCircle : public Function2<MonochromeImage,TransformEntity,DecodeEntity<RING_COUNT*SECTOR_COUNT> > {
  private:
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    const Camera& m_camera;

  public:
    SampleTagCircle(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera);
    bool operator()(const MonochromeImage& image, const TransformEntity& source, DecodeEntity<RING_COUNT*SECTOR_COUNT>& destination) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> SampleTagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>::SampleTagCircle(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, const Camera& camera) :
    m_tagspec(tagspec),
    m_camera(camera) {}
  
  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool SampleTagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const MonochromeImage& image, const TransformEntity& source, DecodeEntity<RING_COUNT*SECTOR_COUNT>& destination) const {

    bool return_result = false;
    for(std::list<Transform*>::const_iterator i = source.m_transforms.begin(); i != source.m_transforms.end(); ++i) {
  
      bool left = true;
      bool right = true;
      int k=RING_COUNT-1;
      int j;
      // scan round reading a half sector width apart until we read two adjacent cells of different values
      for(j=0;j<SECTOR_COUNT*READ_COUNT;j+=READ_COUNT/2) {
	float tpt[]=  {  m_tagspec.GetXSamplePoint(j,k),
			 m_tagspec.GetYSamplePoint(j,k) };
	(*i)->Apply(tpt[0],tpt[1],tpt,tpt+1);
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
	(*i)->Apply(tpt[0],tpt[1],tpt,tpt+1);
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

      CyclicBitSet<RING_COUNT*SECTOR_COUNT>* payload = new CyclicBitSet<RING_COUNT*SECTOR_COUNT>();  
      destination.m_payloads.push_back(payload);
      int index = 0;
      int readindex = (leftindex + READ_COUNT/2) % (SECTOR_COUNT * READ_COUNT);
      for(int j=0;j<SECTOR_COUNT;++j) {
	// read a chunk by sampling each ring and shifting and adding
	for(int k=RING_COUNT-1;k>=0;--k) {
	  float tpt[]=  {  m_tagspec.GetXSamplePoint(readindex,k),
			   m_tagspec.GetYSamplePoint(readindex,k) };
	  (*i)->Apply(tpt[0],tpt[1],tpt,tpt+1);
	  m_camera.NPCFToImage(tpt,1);
	  if (tpt[0] < 0 || tpt[0] >= image.GetWidth() ||
	      tpt[1] < 0 || tpt[1] >= image.GetHeight()) { 
	    return false;
	  }
	  bool sample = image.GetPixel(tpt[0],tpt[1]);
	  (*payload)[index] = sample;      
	  index++;
	}
	readindex+=READ_COUNT;
	readindex %= SECTOR_COUNT * READ_COUNT;
      }      
      return_result = true;
    }

    return return_result;
  }
}
#endif//SAMPLE_CIRCLE_TAG
