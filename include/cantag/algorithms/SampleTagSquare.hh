/**
 * $Header$
 */

#ifndef SAMPLE_SQUARE_TAG
#define SAMPLE_SQUARE_TAG

#include <cantag/entities/Entity.hh>
#include <cantag/TagSquare.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>

namespace Cantag {

  template<int EDGE_CELLS>
  class SampleTagSquare : public Function2<MonochromeImage,TransformEntity, DecodeEntity<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)> > {
  private:
    const TagSquare<EDGE_CELLS>& m_tagspec;
    const Camera& m_camera;
  public:
    SampleTagSquare(const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera);
    bool operator()(const MonochromeImage& image,const TransformEntity& source, DecodeEntity<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)>& destination) const;
  };

  
  template<int EDGE_CELLS> SampleTagSquare<EDGE_CELLS>::SampleTagSquare(const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) :
    m_tagspec(tagspec),
    m_camera(camera) {}
  
  template<int EDGE_CELLS> bool SampleTagSquare<EDGE_CELLS>::operator()(const MonochromeImage& image, const TransformEntity& source, DecodeEntity<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)>& destination) const {
    bool return_result = false;
    for(std::list<Transform*>::const_iterator i = source.m_transforms.begin(); i != source.m_transforms.end(); ++i) {
      CyclicBitSet<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)>* read_code = new CyclicBitSet<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)>();
      destination.m_payloads.push_back(read_code);
      // iterate over the tag reading each section
      for(int j=0;j<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2);j++) {
	 float pts[] = { m_tagspec.GetXSamplePoint(j),
			 m_tagspec.GetYSamplePoint(j) };
	 (*i)->Apply(pts[0],pts[1],pts,pts+1);
	 m_camera.NPCFToImage(pts,1);

	 if (pts[0] < 0 || pts[0] >= image.GetWidth() ||
	     pts[1] < 0 || pts[1] >= image.GetHeight()) { 
	   return false;
	 }
	 bool sample = image.GetPixel(pts[0],pts[1]);
	 (*read_code)[j] = sample;
      }
      return_result = true;
    }
    return return_result;
  }
}
#endif//SAMPLE_SQUARE_TAG
