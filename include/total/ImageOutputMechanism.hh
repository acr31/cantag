/**
 * $Header$
 */ 

#ifndef IMAGE_OUTPUT_MECHANISM_GUARD
#define IMAGE_OUTPUT_MECHANISM_GUARD

#include <total/Config.hh>
#include <total/LocatedObject.hh>

namespace Total {
  class ImageOutputMechanism {
  private:
    const Camera& m_camera;
    Image* m_savedimage;
  public:
  
    ImageOutputMechanism(const Camera& camera) : m_camera(camera), m_savedimage(NULL) {};
  
    inline void FromImageSource(const Image& image) {
      image.Save("debug-fromimagesource.pnm");

      m_savedimage = new Image(image);
    };
    inline void FromThreshold(const Image& image) {
      image.Save("debug-fromthreshold.pnm");
    };
    inline void FromContourTree(const ContourTree& contours) {};
    inline void FromRemoveIntrinsic(const ContourTree& contours) {};
    template<class ShapeType> inline void FromShapeTree(const ShapeTree<ShapeType>& shapes) {};
    template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);
  };
  
  template<int PAYLOADSIZE> void ImageOutputMechanism::FromTag(const WorldState<PAYLOADSIZE>& world) {
    if (!m_savedimage) throw "Must call FromImageSource before calling FromTag";

    m_savedimage->ConvertScale(0.25,0);
    m_savedimage->ConvertScale(1,128);      
    for(typename std::vector<LocatedObject<PAYLOADSIZE>*>::const_iterator i = world.GetNodes().begin();
	i != world.GetNodes().end();
	++i) {
      LocatedObject<PAYLOADSIZE>* loc = *i;
      if (loc->tag_codes.size() >0) {
	float pts[] = {-1,-1,
		       -1,1,
		       1,1,
		       1,-1};
	ApplyTransform(loc->transform,pts,4);
	m_camera.NPCFToImage(pts,4);
	m_savedimage->DrawPolygon(pts,4,COLOUR_BLACK,1);
      }
    }
    m_savedimage->Save("debug-fromtag.pnm");
    delete m_savedimage;
  }
}
#endif//IMAGE_OUTPUT_MECHANISM_GUARD
