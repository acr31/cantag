/**
 * $Header$
 */

#ifndef SYMBOL_CHUNK_CODER
#define SYMBOL_CHUNK_CODER

#include <Config.hh>
#include <Coder.hh>

#define SYMBOL_CHUNK_DEBUG
/**
 * Encodes the data into a number of symbols containing granularity
 * bits.  The first bit of each symbol is the orientation bit - 1 for
 * the first symbol, the last bit of each symbol is a parity bit.
 */
template<int BIT_COUNT, int GRANULARITY>
class SymbolChunkCoder : public virtual Coder<BIT_COUNT> {
public:
  SymbolChunkCoder() {};

  virtual int DecodePayload(std::bitset<BIT_COUNT>& tag_data, Payload<BIT_COUNT>& payload) const {
    // rotate the code by increments of GRANULARITY until the first bit is a 1
    int rotation = 0;
    while(!payload[0] && rotation < BIT_COUNT) {
      payload.RotateLeft(GRANULARITY);
      rotation+=GRANULARITY;
    }

    if (payload[0]) {
      int payload_pointer = 0;
      // we've found the first symbol
      for(int i=0;i<BIT_COUNT/GRANULARITY * (GRANULARITY-2);i+=GRANULARITY-2) {
	bool parity = payload[payload_pointer++]; //read off the orientation bit
	if ((payload_pointer > 1) && (parity)) {
#ifdef SYMBOL_CHUNK_DEBUG
	  PROGRESS("Orientation bit at position " << (payload_pointer-1) << " is corrupted");
#endif
	  // orientation bit is corrupted
	  return -1;
	}					      
	for(int j=0;j<GRANULARITY-2;j++) {
	  tag_data[j+i] = payload[payload_pointer];
	  if (payload[payload_pointer++]) { parity = !parity; }
	}
	if (payload[payload_pointer++]) { parity = !parity; }
	if (!parity) { 
#ifdef SYMBOL_CHUNK_DEBUG
	  PROGRESS("Parity check bit at position " << (payload_pointer-1) << " is invalid");
#endif
	  return -1; 
	}
      }
      return rotation;
    }
    else {
#ifdef SYMBOL_CHUNK_DEBUG
      PROGRESS("Failed to find orientation start bit");
#endif
      return -1;
    }
  }
  
  virtual bool EncodePayload(const std::bitset<BIT_COUNT>& tag_data, Payload<BIT_COUNT>& payload) const {
    // we encode BIT_COUNT/GRANULARITY symbols, each one containing GRANULARITY-2 bits of payload
    int payload_pointer = 0;
    for(int i=0;i<BIT_COUNT/GRANULARITY * (GRANULARITY-2);i+=GRANULARITY-2) {
      payload[payload_pointer++] = i==0;  // encode a 1 if this is the first symbol
      bool parity = i==0;
      for(int j=0;j<GRANULARITY-2;j++) {
	payload[payload_pointer++] = tag_data[i+j];
	if (tag_data[i+j]) { parity = !parity; }
      }
      payload[payload_pointer++] = !parity; // add the parity bit
    }
    return true;
  }
};

#endif//SYMBOL_CHUNK_CODER
