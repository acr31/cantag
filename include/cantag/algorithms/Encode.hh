/**
 * $Header$
 */

#ifndef ENCODE_GUARD
#define ENCODE_GUARD

#include <list>

#include <total/entities/DecodeEntity.hh>
#include <total/coders/Coder.hh>
#include <total/CyclicBitSet.hh>
#include <total/Function.hh>

namespace Total {

  /**
   * Algorithm for encoding tag data
   */
  template<class Coder>
  class Encode : public Function0<DecodeEntity<Coder::PayloadSize> >, private Coder {
  private:
    typedef typename DecodeEntity<Coder::PayloadSize>::Data DecodeData; // needed to convince the compiler to parse this file
  public:
    Encode() : Coder() {};

    bool operator()(DecodeEntity<Coder::PayloadSize>& destination) const {
      bool return_result = false;
      for(typename std::vector<DecodeData*>::iterator i = destination.GetPayloads().begin(); 
	  i != destination.GetPayloads().end(); 
	  ++i) {
	DecodeData* data = *i;
	bool result = EncodePayload(data->payload);

	if (result) {
	  data->bits_rotation = 0;
	  data->confidence = 1.f;
	  return_result = true;
	}
	else {
	  data->bits_rotation = -1;
	  data->confidence = 0.f;
	}
      }
      return return_result;
    }    
  };
  
}

#endif//ENCODE_GUARD
