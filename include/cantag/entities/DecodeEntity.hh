/**
 * $Header$
 */

#ifndef DECODE_ENTITY_GUARD
#define DECODE_ENTITY_GUARD

#include <vector>

#include <cantag/Config.hh>
#include <cantag/CyclicBitSet.hh>

namespace Cantag {

  /**
   * This entity stores symbolic payload data from the tag and the
   * number of bits of rotation that the payload has undergone.  There
   * may be more than one possible payload for a tag 
   */
  template<int PAYLOAD_SIZE>
  class DecodeEntity : public Entity {
  public:
    
    struct Data {
      CyclicBitSet<PAYLOAD_SIZE> payload;
      int bits_rotation;
      float confidence;

      Data() : payload(), bits_rotation(0), confidence(0.f) {};
    };
      
  private:
    std::vector<Data*> m_payloads;

  public:
    DecodeEntity() : m_payloads() {};
    ~DecodeEntity() {
      for(typename std::vector<Data*>::const_iterator i = m_payloads.begin(); i!=m_payloads.end(); ++i) {
	delete *i;
      }
    };
    
    inline std::vector<Data*>& GetPayloads() { return m_payloads; }
    inline const std::vector<Data*>& GetPayloads() const { return m_payloads; }
    inline Data* Add() { 
      Data* newdata = new Data();
      m_payloads.push_back(newdata);
      return newdata;
    }

  private:
    DecodeEntity(const DecodeEntity<PAYLOAD_SIZE>& copyme) {}
  };
}

#endif//DECODE_ENTITY_GUARD
