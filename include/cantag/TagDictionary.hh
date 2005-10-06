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
    float rho;
    float theta;
    float phi;  
  };

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
  class TagDictionaryInternal {};

  template<int PAYLOAD_SIZE, class Tail> 
  struct TagDictionaryInternal<PAYLOAD_SIZE,TypeList<LocationElement,Tail> > : public TagLocationDictionary<PAYLOAD_SIZE>, TagDictionaryInternal<PAYLOAD_SIZE,Tail> {
    struct Element : public LocationElement, public TagDictionaryInternal<PAYLOAD_SIZE,Tail>::Element {};
  };

  template<int PAYLOAD_SIZE, class Tail> 
  struct TagDictionaryInternal<PAYLOAD_SIZE,TypeList<PoseElement,Tail> > : public TagPoseDictionary<PAYLOAD_SIZE>, TagDictionaryInternal<PAYLOAD_SIZE,Tail> {
    struct Element : public PoseElement, public TagDictionaryInternal<PAYLOAD_SIZE,Tail>::Element {};
  };

  template<int PAYLOAD_SIZE, class Tail> 
  struct TagDictionaryInternal<PAYLOAD_SIZE,TypeList<SizeElement,Tail> > : public TagSizeDictionary<PAYLOAD_SIZE>, TagDictionaryInternal<PAYLOAD_SIZE,Tail> {
    struct Element : public SizeElement, public TagDictionaryInternal<PAYLOAD_SIZE,Tail>::Element {};
  };

  template<int PAYLOAD_SIZE>
  struct TagDictionaryInternal<PAYLOAD_SIZE,TypeListEOL> {
    struct Element {};
  };

  
  template<int PAYLOAD_SIZE, class TraitList> 
  struct TagDictionary : public TagDictionaryInternal<PAYLOAD_SIZE,typename Reorder<TraitList,TL3(LocationElement,PoseElement,SizeElement)>::value > {    
    typedef typename TagDictionaryInternal<PAYLOAD_SIZE,typename Reorder<TraitList,TL3(LocationElement,PoseElement,SizeElement)>::value>::Element Element;
  };

  template<int PAYLOAD_SIZE>
  class LocationDirectory : public TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)> {
  private:
    std::map<CyclicBitSet<PAYLOAD_SIZE>, typename TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>::Element*> m_map;
  public:
    LocationDirectory() : m_map() {};
    ~LocationDirectory();
    virtual const typename TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>::Element* GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const;
    void StoreInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code, float x, float y, float z);
  };

  template<int PAYLOAD_SIZE> void LocationDirectory<PAYLOAD_SIZE>::StoreInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code, float x, float y, float z) {
    typename TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>::Element* e = new typename TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>::Element();
    e->x = x;
    e->y = y;
    e->z = z;
    m_map[tag_code] = e;
  };

  template<int PAYLOAD_SIZE> LocationDirectory<PAYLOAD_SIZE>::~LocationDirectory() {
    for( typename std::map<CyclicBitSet<PAYLOAD_SIZE>,typename TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>::Element* >::iterator i = m_map.begin();i!= m_map.end();++i) {
      delete (*i).second;
    }
  };

  template<int PAYLOAD_SIZE> const typename TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>::Element* LocationDirectory<PAYLOAD_SIZE>::GetInformation(const CyclicBitSet<PAYLOAD_SIZE>& tag_code) const {
    typename std::map<CyclicBitSet<PAYLOAD_SIZE>,typename TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>::Element*>::const_iterator i = m_map.find(tag_code);
    if (i == m_map.end()) {
      return NULL;
    }
    else {
      return (*i).second;
    }
  }
}
#endif//TAGDICTIONARY_GUARD
