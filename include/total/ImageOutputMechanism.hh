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
    Image m_savedimage;
  public:
  
    ImageOutputMechanism(const Camera& camera) : m_camera(camera) {};
  
    inline void FromImageSource(const Image& image) {
      m_savedimage = image;
      image->Save("debug-fromimagesource.pnm");
    };
    inline void FromThreshold(const Image& image) {
      image->Save("debug->fromthreshold.pnm");
    };
    inline void FromContourTree(const ContourTree& contours) {};
    inline void FromRemoveIntrinsic(const ContourTree& contours) {};
    template<class ShapeType> inline void FromShapeTree(const ShapeTree<ShapeType>& shapes) {};
    template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);
  };
  
  template<int PAYLOADSIZE> void TextOutputMechanism::FromTag(const WorldState<PAYLOADSIZE>& world) {
    for(typename std::vector<LocatedObject<PAYLOADSIZE>*>::const_iterator i = world.GetNodes().begin();
	i != world.GetNodes().end();
	++i) {
      LocatedObject<PAYLOADSIZE>* loc = *i;
      if (loc->tag_codes.size() >0) {
	float pts[] = {-1,-1,
		       -1,1,
		       1,1,
		       1,-1};
	ApplyTransform(lobj->transform,pts,4);
	m_camera.NPCFToImage(pts,4);
	m_savedimage.DrawPolygon(pts,4,COLOUR_BLACK,1);
      }
    }
  }
}
#endif//IMAGE_OUTPUT_MECHANISM_GUARD
