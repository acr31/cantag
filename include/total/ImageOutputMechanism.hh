/**
 * $Header$
 */ 

#ifndef IMAGE_OUTPUT_MECHANISM_GUARD
#define IMAGE_OUTPUT_MECHANISM_GUARD

#include <total/Config.hh>
#include <total/LocatedObject.hh>
#include <total/ContourTree.hh>
#include <total/ShapeTree.hh>
#include <total/WorldState.hh>
#include <total/Camera.hh>

namespace Total {
  class ImageOutputMechanism {
  private:    
    const Camera& m_camera;
    Image* m_saved_originalimage;
    Image* m_saved_thresholdimage;
    void FromContourTree(Image& dest, const ContourTree::Contour* contour);
    template<class ShapeType> void FromShapeTree(Image& image, const typename ShapeTree<ShapeType>::Node* node);
  public:
  
    ImageOutputMechanism(const Camera& camera) : m_camera(camera), m_saved_originalimage(NULL), m_saved_thresholdimage(NULL) {};
    ~ImageOutputMechanism();

    void FromImageSource(const Image& image);

    void FromThreshold(const Image& image);

    void FromContourTree(const ContourTree& contours);
    inline void FromRemoveIntrinsic(const ContourTree& contours) {};
    template<class ShapeType>  void FromShapeTree(const ShapeTree<ShapeType>& shapes);
    template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);
  };
  
  template<class ShapeType> void ImageOutputMechanism::FromShapeTree(Image& image,const typename ShapeTree<ShapeType>::Node* node) {
    node->matched.DrawChain(image,m_camera);
    for(typename std::vector<typename ShapeTree<ShapeType>::Node* >::const_iterator i = node->children.begin();
	i!=node->children.end();
	++i) {
      FromShapeTree<ShapeType>(image,*i);
    }
  };


  template<class ShapeType> void ImageOutputMechanism::FromShapeTree(const ShapeTree<ShapeType>& shapes) {
    Image i(m_saved_originalimage->GetWidth(),m_saved_originalimage->GetHeight());
    FromShapeTree<ShapeType>(i,shapes.GetRootNode());
    i.Save("debug-fromshapetree.pnm");
  }

  template<int PAYLOADSIZE> void ImageOutputMechanism::FromTag(const WorldState<PAYLOADSIZE>& world) {
    if (!m_saved_originalimage) throw "Must call FromImageSource before calling FromTag";

    m_saved_originalimage->ConvertScale(0.25,0);
    m_saved_originalimage->ConvertScale(1,128);      
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
	m_saved_originalimage->DrawPolygon(pts,4,COLOUR_BLACK,1);
      }
    }
    m_saved_originalimage->Save("debug-fromtag.pnm");
  }
}
#endif//IMAGE_OUTPUT_MECHANISM_GUARD
