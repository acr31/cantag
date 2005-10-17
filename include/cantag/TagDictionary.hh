/*
  Copyright (C) 2005 Andrew C. Rice

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

#ifndef TAGDICTIONARY_GUARD
#define TAGDICTIONARY_GUARD

#include <cantag/Config.hh>
#include <cantag/TemplateUtils.hh>
#include <cantag/CyclicBitSet.hh>
#include <cantag/Transform.hh>

namespace Cantag {

  /**
   * A tag dictionary is used to hold our current estimate of the world
   * state in terms of tags that we might see.  This class provides a
   * mapping from tag payload size to the world co-ordinate frame
   * location and pose of the tag and the tag size
   */
  
  struct LocationElement {
    float x;
    float y;
    float z;
  };
  
  struct SizeElement {
    float tag_size;
  };
  
  struct PoseElement {
    float theta; // Spherical polars
    float phi;   // Spherical polars
    float psi;   // Rotation about axis
  };

  template<class ElementList>
  struct ElementHelper : public ElementList::Head, public ElementHelper<typename ElementList::Tail> {};

  template<>
  struct ElementHelper<TypeListEOL> {};

  template<class ElementList>
  struct Element : public ElementHelper<typename Reorder<ElementList,TL3(LocationElement,PoseElement,SizeElement)>::value > {};
  
  /**
   * A virtual superclass for dictionaries that provide location information
   */
  template<int PAYLOAD_SIZE>
  class TagLocationDictionary {
  public:
    virtual const LocationElement* GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const = 0;
  };

  /**
   * A virtual superclass for dictionaries that provide tag size information
   */
  template<int PAYLOAD_SIZE>
  class TagSizeDictionary {
  public:
    virtual const SizeElement* GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const = 0;
  };

  /**
   * A virtual superclass for dictionaries that provide tag pose information
   */
  template<int PAYLOAD_SIZE>
  class TagPoseDictionary {
  public:
    virtual const PoseElement* GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const = 0;
  };

  template<int PAYLOAD_SIZE, class TraitList>
  class TagDictionaryHelper {};

  template<int PAYLOAD_SIZE, class Tail> 
  struct TagDictionaryHelper<PAYLOAD_SIZE,TypeList<LocationElement,Tail> > : public TagLocationDictionary<PAYLOAD_SIZE>, public TagDictionaryHelper<PAYLOAD_SIZE,Tail> {};

  template<int PAYLOAD_SIZE, class Tail> 
  struct TagDictionaryHelper<PAYLOAD_SIZE,TypeList<PoseElement,Tail> > : public TagPoseDictionary<PAYLOAD_SIZE>,  public TagDictionaryHelper<PAYLOAD_SIZE,Tail> {};

  template<int PAYLOAD_SIZE, class Tail> 
  struct TagDictionaryHelper<PAYLOAD_SIZE,TypeList<SizeElement,Tail> > : public TagSizeDictionary<PAYLOAD_SIZE>,  public TagDictionaryHelper<PAYLOAD_SIZE,Tail> {};

  template<int PAYLOAD_SIZE, class TraitList> 
  struct TagDictionary : public TagDictionaryHelper<PAYLOAD_SIZE,typename Reorder<TraitList,TL3(LocationElement,PoseElement,SizeElement)>::value > {    
    virtual const Element<typename Reorder<TraitList,TL3(LocationElement,PoseElement,SizeElement)>::value>* GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const = 0;
  };

  template<int PAYLOAD_SIZE>
  class TransformDirectory : public TagDictionary<PAYLOAD_SIZE,TL3(LocationElement,PoseElement,SizeElement)> {
  private:
     std::map<CyclicBitSet<PAYLOAD_SIZE>, Element<TL3(LocationElement,PoseElement,SizeElement)>* > m_map;
  public:
     TransformDirectory() : m_map() {};
     ~TransformDirectory();
      virtual const Element<TL3(LocationElement,PoseElement,SizeElement)>* GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const;
      void StoreInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code,float x, float y, float z, float alpha, float beta, float gamma, float size);
      void StoreInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code, 
			    const Transform& transform);
  };

  template<int PAYLOAD_SIZE> const Element<TL3(LocationElement,PoseElement,SizeElement)>* TransformDirectory<PAYLOAD_SIZE>::GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const {
    typename std::map<CyclicBitSet<PAYLOAD_SIZE>,Element<TL3(LocationElement,PoseElement,SizeElement)>*>::const_iterator i = m_map.find(tag_code);
    if (i == m_map.end()) {
      return NULL;
    }
    else {
      return (*i).second;
    }
  }

  template<int PAYLOAD_SIZE> TransformDirectory<PAYLOAD_SIZE>::~TransformDirectory() {
    for(typename std::map<CyclicBitSet<PAYLOAD_SIZE>,Element<TL3(LocationElement,PoseElement,SizeElement)>*>::const_iterator i = m_map.begin();i!= m_map.end(); ++i) {
      delete (*i).second;
    }
  }

  template<int PAYLOAD_SIZE> void TransformDirectory<PAYLOAD_SIZE>::StoreInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code,float x, float y, float z, float theta, float phi, float psi, float size) {
    Element<TL3(LocationElement,PoseElement,SizeElement)>* e = new Element<TL3(LocationElement,PoseElement,SizeElement)>();
    m_map[tag_code] = e;
    e->x = x;
    e->y = y;
    e->z = z;
    e->theta = theta;
    e->phi = phi;
    e->psi = psi;
    e->tag_size = size;
  }
  
  template<int PAYLOAD_SIZE> void TransformDirectory<PAYLOAD_SIZE>::StoreInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code, const Transform& transform) {
 Element<TL3(LocationElement,PoseElement,SizeElement)>* e = new Element<TL3(LocationElement,PoseElement,SizeElement)>();
    m_map[tag_code] = e;
    e->x = transform[3];;
    e->y = transform[7];;
    e->z = transform[11];

    float theta, phi, psi;
    transform.GetAngleRepresentation(&theta,
				     &phi,
				     &psi);
    e->theta = theta;
    e->phi = phi;
    e->psi = psi;
    e->tag_size = 1.0;
  }


}
#endif//TAGDICTIONARY_GUARD
