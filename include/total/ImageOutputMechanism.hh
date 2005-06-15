/**
 * $Header$
 */ 

#ifndef IMAGE_OUTPUT_MECHANISM_GUARD
#define IMAGE_OUTPUT_MECHANISM_GUARD

#include <total/Config.hh>
#include <total/LocatedObject.hh>
#include <total/WorldState.hh>
#include <total/Camera.hh>
#include <total/entities/Entity.hh>
#include <total/ComposeEntity.hh>
#include <total/CyclicBitSet.hh>
#include <list>

namespace Total {
  class ImageOutputMechanism {
  private:    
    const Camera& m_camera;
    Image* m_saved_originalimage;
    Image* m_saved_thresholdimage;
    
    void FromContourTree(Image& dest, const ContourTree::Contour* contour);
    template<class ShapeType> void FromShapeTree(Image& image, const typename ShapeTree<ShapeType>::Node* node);

    class ContourAlgorithm {
    private:
      Image& m_image;
    public:
      ContourAlgorithm(Image& output_image);
      void operator()(const ContourEntity& contour) const;
    };

    template<class Shape>
    class ShapeAlgorithm {
    private:
      const Camera& m_camera;
      Image& m_image;
    public:
      ShapeAlgorithm(const Camera& camera, Image& output_image);
      void operator()(const ShapeEntity<Shape>& shape) const;
    };

    template<class Shape,int PAYLOADSIZE>
    class DecodeAlgorithm {
    private:
      const Camera& m_camera;
      Image& m_image;
    public:
      DecodeAlgorithm(const Camera& camera, Image& output_image);
      void operator()(const ShapeEntity<Shape>& shape, const DecodeEntity<PAYLOADSIZE>& decode) const;
    };
    


    template<class Entity, class Dummy>
    struct Helper {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {
	Helper<Entity,typename Dummy::Tail>::Output(root_element,original_image,camera);
      };
    };

    template<class Entity>
    struct Helper<Entity,TypeListEOL> {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {}
    };
    
    
    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<ContourEntity,Tail> > {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {
	Image i(original_image->GetWidth(),original_image->GetHeight());
	root_element.Apply(ContourAlgorithm(i));
	i.Save("debug-fromcontourtree.pnm");
	Helper<Entity,Tail>::Output(root_element,original_image,camera);
      }
    };

    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<ShapeEntity<typename Entity::ShapeType>,Tail> > {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {
	Image i(original_image->GetWidth(),original_image->GetHeight());
	root_element.Apply(ShapeAlgorithm<typename Entity::ShapeType>(camera,i));
	i.Save("debug-fromshapetree.pnm");
	Helper<Entity,Tail>::Output(root_element,original_image,camera);
      }
    };

    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<DecodeEntity<Entity::PayloadSize>,Tail> > {
      static void Output(const Entity& root_element, const Image* original_image, const Camera& camera) {
	Image i(original_image->GetWidth(),original_image->GetHeight());
	DecodeAlgorithm<typename Entity::ShapeType, Entity::PayloadSize> d(camera,i);
	root_element.Apply2(d);
	i.Save("debug-fromdecode.pnm");
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

  template<class Shape> ImageOutputMechanism::ShapeAlgorithm<Shape>::ShapeAlgorithm(const Camera& camera, Image& output_image) : m_camera(camera), m_image(output_image) {};
  template<class Shape> void ImageOutputMechanism::ShapeAlgorithm<Shape>::operator()(const ShapeEntity<Shape>& shape) const {
    if (shape.m_shapeFitted) {
      m_camera.Draw(m_image,*shape.m_shapeDetails);
    }
  }

  
  template<class Shape,int PAYLOADSIZE> ImageOutputMechanism::DecodeAlgorithm<Shape,PAYLOADSIZE>::DecodeAlgorithm(const Camera& camera, Image& output_image) : m_camera(camera), m_image(output_image) {};
  template<class Shape,int PAYLOADSIZE> void ImageOutputMechanism::DecodeAlgorithm<Shape,PAYLOADSIZE>::operator()(const ShapeEntity<Shape>& shape, const DecodeEntity<PAYLOADSIZE>& decode) const {
    if (shape.m_shapeFitted) {
      for(typename std::list<CyclicBitSet<PAYLOADSIZE>*>::const_iterator i = decode.m_payloads.begin(); i != decode.m_payloads.end(); ++i) {
	if (!(*i)->IsInvalid()) {
	  m_camera.Draw(m_image,*(shape.m_shapeDetails));
	  return;
	}
      }
    }
  }
    
}
#endif//IMAGE_OUTPUT_MECHANISM_GUARD
