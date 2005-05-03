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
#include <total/Entity.hh>

namespace Total {
  class ImageOutputMechanism {
  private:    
    const Camera& m_camera;
    Image* m_saved_originalimage;
    Image* m_saved_thresholdimage;
    
    void FromContourTree(Image& dest, const ContourTree::Contour* contour);
    template<class ShapeType> void FromShapeTree(Image& image, const typename ShapeTree<ShapeType>::Node* node);

    class ContourTreeAlgorithm {
    private:
      Image& m_image;
    public:
      ContourTreeAlgorithm(Image& output_image) : m_image(output_image) {};
      void operator()(const ContourEntity* contour) {
	if (!contour->weeded) {
	  for(std::vector<float>::const_iterator i = contour->points.begin();
	      i!=contour->points.end();
	      ++i) {
	    const float x = *i;
	    ++i;
	    const float y = *i;
	    m_image.DrawPixel(x,y,COLOUR_BLACK);
	  }
	}
      }
    };

    template<class Shape>
    class ShapeTreeAlgorithm {
    private:
      const Camera& m_camera;
      Image& m_image;
    public:
      ShapeTreeAlgorithm(const Camera& camera, Image& output_image) : m_camera(camera), m_image(output_image) {};
      void operator()(const ShapeEntity<Shape>* shape) {
	if (shape->m_shapeFitted) {
	  shape->m_shapeDetails->Draw(m_image,m_camera);
	}
      }
    };

    template<class Entity, class Dummy>
    struct Helper {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {};
    };

    template<class Entity, class Tail>
    struct Helper<Entity,EntityList<ContourEntity,Tail> > {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {
	Image i(original_image->GetWidth(),original_image->GetHeight());
	ContourTreeAlgorithm alg(i);
	root_element.Apply(alg);
	i.Save("debug-fromcontourtree.pnm");
	Helper<Entity,Tail>::Output(root_element,original_image,camera);
      }
    };

    template<class Entity, class Tail>
    struct Helper<Entity,EntityList<ShapeEntity<typename Entity::ShapeType>,Tail> > {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {
	Image i(original_image->GetWidth(),original_image->GetHeight());
	ShapeTreeAlgorithm<typename Entity::ShapeType> alg(camera,i);
	root_element.Apply(alg);
	i.Save("debug-fromshapetree.pnm");
	Helper<Entity,Tail>::Output(root_element,original_image,camera);
      }
    };


  public:
    ImageOutputMechanism(const Camera& camera) : m_camera(camera), m_saved_originalimage(NULL), m_saved_thresholdimage(NULL) {};
    ~ImageOutputMechanism();

    template<class Entity> void FromEntity(const Entity& root_entity) {
      Helper<Entity,typename Entity::Typelist>::Output(root_entity,m_saved_originalimage,m_camera );
    }

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
