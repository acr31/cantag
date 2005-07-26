/**
 * $Header$
 */

#ifndef DECODE_GUARD
#define DECODE_GUARD

#include <list>

#include <total/entities/DecodeEntity.hh>
#include <total/coders/Coder.hh>
#include <total/CyclicBitSet.hh>
#include <total/Function.hh>

namespace Total {

  template<class Coder>
  class Decode : public Function0<DecodeEntity<Coder::PayloadSize> >, private Coder {
  private:
    typedef typename DecodeEntity<Coder::PayloadSize>::Data DecodeData; // needed to convince the compiler to parse this file

  public:
    Decode() : Coder() {};

    bool operator()(DecodeEntity<Coder::PayloadSize>& destination) const {
      bool return_result = false;
      for(typename std::vector<DecodeData*>::iterator i = destination.GetPayloads().begin(); 
	  i != destination.GetPayloads().end(); 
	  ++i) {
	DecodeData* data = *i;	
	int rotation = DecodePayload(data->payload);
	data->bits_rotation = rotation;
	if (rotation >= 0) {
	  data->confidence = 1.f;
	  return_result = true;
	}
	else {
	  data->confidence = 0.f;
	}
      }
      return return_result;
    }

  };

}

#endif//DECODE_GUARD
