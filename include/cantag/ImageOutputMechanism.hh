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

#ifndef IMAGE_OUTPUT_MECHANISM_GUARD
#define IMAGE_OUTPUT_MECHANISM_GUARD

#include <cantag/Config.hh>
#include <cantag/LocatedObject.hh>
#include <cantag/Camera.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/ComposeEntity.hh>
#include <cantag/CyclicBitSet.hh>
#include <list>

namespace Cantag {
  class ImageOutputMechanism {
  public:

    class ContourAlgorithm : Function<TL0,TL1(ContourEntity)> {
    private:
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    public:
      ContourAlgorithm(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& output_image);
      bool operator()(const ContourEntity& contour) const;
    };

    template<class Shape>
    class ShapeAlgorithm : Function<TL0,TL1(ShapeEntity<Shape>)> {
    private:
      const Camera& m_camera;
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    public:
      ShapeAlgorithm(const Camera& camera, Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& output_image);
      bool operator()(const ShapeEntity<Shape>& shape) const;
    };

    template<class Shape,int PAYLOADSIZE>
    class DecodeAlgorithm : Function<TL1(ShapeEntity<Shape>),TL1(DecodeEntity<PAYLOADSIZE>)>{
    private:
      const Camera& m_camera;
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    public:
      DecodeAlgorithm(const Camera& camera, Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& output_image);
      bool operator()(const ShapeEntity<Shape>& shape, const DecodeEntity<PAYLOADSIZE>& decode) const;
    };
    


    template<class Entity, class Dummy>
    struct Helper {
      static void Output(const Entity& root_element, const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* original_image, const Camera& camera) {
	Helper<Entity,typename Dummy::Tail>::Output(root_element,original_image,camera);
      };
    };

    template<class Entity>
    struct Helper<Entity,TypeListEOL> {
      static void Output(const Entity& root_element, const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* original_image, const Camera& camera) {}
    };
    
    
    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<ContourEntity,Tail> > {
      static void Output(const Entity& root_element, const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* original_image, const Camera& camera) {
	Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> i(original_image->GetWidth(),original_image->GetHeight());
	root_element.Apply(ContourAlgorithm(i));
	i.Save("debug-fromcontourtree.pnm");
	Helper<Entity,Tail>::Output(root_element,original_image,camera);
      }
    };

    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<ShapeEntity<typename Entity::ShapeType>,Tail> > {
      static void Output(const Entity& root_element, const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* original_image, const Camera& camera) {
	Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> i(original_image->GetWidth(),original_image->GetHeight());
	root_element.Apply(ShapeAlgorithm<typename Entity::ShapeType>(camera,i));
	i.Save("debug-fromshapetree.pnm");
	Helper<Entity,Tail>::Output(root_element,original_image,camera);
      }
    };

    template<class Entity, class Tail>
    struct Helper<Entity,TypeList<DecodeEntity<Entity::PayloadSize>,Tail> > {
      static void Output(const Entity& root_element, const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* original_image, const Camera& camera) {
	Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> i(original_image->GetWidth(),original_image->GetHeight());
	DecodeAlgorithm<typename Entity::ShapeType, Entity::PayloadSize> d(camera,i);
	root_element.Apply2(d);
	i.Save("debug-fromdecode.pnm");
	Helper<Entity,Tail>::Output(root_element,original_image,camera);
      }
    };


  public:
    //    ImageOutputMechanism(const Camera& camera) : m_camera(camera), m_saved_originalimage(NULL), m_saved_thresholdimage(NULL) {};
    ~ImageOutputMechanism();

    //    template<class Entity> void FromEntity(const Entity& root_entity) {
    //      Helper<Entity,typename Entity::Typelist>::Output(root_entity,m_saved_originalimage,m_camera );
    //    }


  };
  
  template<class Shape> ImageOutputMechanism::ShapeAlgorithm<Shape>::ShapeAlgorithm(const Camera& camera, Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& output_image) : m_camera(camera), m_image(output_image) {};

  template<class Shape> bool ImageOutputMechanism::ShapeAlgorithm<Shape>::operator()(const ShapeEntity<Shape>& shape) const {
    m_camera.Draw(m_image,*shape.GetShape());
    return true;
  }

  
  template<class Shape,int PAYLOADSIZE> ImageOutputMechanism::DecodeAlgorithm<Shape,PAYLOADSIZE>::DecodeAlgorithm(const Camera& camera, Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& output_image) : m_camera(camera), m_image(output_image) {};
  template<class Shape,int PAYLOADSIZE> bool ImageOutputMechanism::DecodeAlgorithm<Shape,PAYLOADSIZE>::operator()(const ShapeEntity<Shape>& shape, const DecodeEntity<PAYLOADSIZE>& decode) const {
    for(typename std::list<CyclicBitSet<PAYLOADSIZE>*>::const_iterator i = decode.m_payloads.begin(); i != decode.m_payloads.end(); ++i) {
      if (!(*i)->IsInvalid()) {
	m_camera.Draw(m_image,*(shape.GetShape()));
	return;
      }
    }
    return true;
  }
    
}
#endif//IMAGE_OUTPUT_MECHANISM_GUARD
