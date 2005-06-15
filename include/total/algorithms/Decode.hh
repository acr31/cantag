/**
 * $Header$
 */

#ifndef DECODE_GUARD
#define DECODE_GUARD

#include <list>

#include <total/entities/Entity.hh>
#include <total/CyclicBitSet.hh>
#include <total/Function.hh>

namespace Total {

  template<class Coder>
  class Decode : public Function0<DecodeEntity<Coder::PayloadSize> >, private Coder {
  public:
    Decode() : Coder() {};

    bool operator()(DecodeEntity<Coder::PayloadSize>& destination) const {
      bool return_result = false;
      for(typename std::list<CyclicBitSet<Coder::PayloadSize>*>::iterator i = destination.m_payloads.begin(); 
	  i != destination.m_payloads.end(); 
	  ++i) {
	int rotation = DecodePayload(**i);
	if (rotation >= 0) return_result = true;
      }
      return return_result;
    }

  };

}

#endif//DECODE_GUARD
