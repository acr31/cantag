/**
 * $Header$
 */

#ifndef SYMBOL_CHUNK_CODER
#define SYMBOL_CHUNK_CODER

#include <total/Config.hh>
#include <total/Coder.hh>

#define SYMBOL_CHUNK_DEBUG

namespace Total {
  /**
   * Encodes the data into a number of symbols containing granularity
   * bits.  The first bit of each symbol is the orientation bit - 1 for
   * the first symbol, the last bit of each symbol is a parity bit.
   */
  template<int BIT_COUNT, int GRANULARITY>
  class SymbolChunkCoder : public virtual Coder<BIT_COUNT> {
  public:
    virtual bool IsErrorCorrecting() const;
    virtual int GetSymbolSize() const;
    virtual int GetHammingDistanceBits() const;
    virtual int GetHammingDistanceSymbols() const;


    virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  
    virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  };


  template<int BIT_COUNT,int GRANULARITY> bool SymbolChunkCoder<BIT_COUNT,GRANULARITY>::IsErrorCorrecting() const { return false; }

  template<int BIT_COUNT,int GRANULARITY> int SymbolChunkCoder<BIT_COUNT,GRANULARITY>::GetSymbolSize() const { return GRANULARITY; }

  template<int BIT_COUNT,int GRANULARITY> int SymbolChunkCoder<BIT_COUNT,GRANULARITY>::GetHammingDistanceBits() const { return 2; }

  template<int BIT_COUNT,int GRANULARITY> int SymbolChunkCoder<BIT_COUNT,GRANULARITY>::GetHammingDistanceSymbols() const { return 0; }


  template<int BIT_COUNT,int GRANULARITY> int SymbolChunkCoder<BIT_COUNT,GRANULARITY>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    // rotate the code by increments of GRANULARITY until the first bit is a 1
    int rotation = 0;
    while(!data[0] && rotation < BIT_COUNT) {
      data.RotateLeft(GRANULARITY);
      rotation+=GRANULARITY;
    }

    if (data[0]) {
      CyclicBitSet<BIT_COUNT> data_copy(data);
      data.reset();
      int payload_pointer = 0;
      // we've found the first symbol
      for(int i=0;i<BIT_COUNT/GRANULARITY * (GRANULARITY-2);i+=GRANULARITY-2) {
	bool parity = data_copy[payload_pointer++]; //read off the orientation bit
	if ((payload_pointer > 1) && (parity)) {
#ifdef SYMBOL_CHUNK_DEBUG
	  PROGRESS("Orientation bit at position " << (payload_pointer-1) << " is corrupted");
#endif
	  // orientation bit is corrupted
	  return -1;
	}					      
	for(int j=0;j<GRANULARITY-2;j++) {
	  data[j+i] = data_copy[payload_pointer];
	  if (data_copy[payload_pointer++]) { parity = !parity; }
	}
	if (data_copy[payload_pointer++]) { parity = !parity; }
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

  template<int BIT_COUNT,int GRANULARITY> bool SymbolChunkCoder<BIT_COUNT,GRANULARITY>::EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    CyclicBitSet<BIT_COUNT> data_copy(data);
    data.reset();
    // we encode BIT_COUNT/GRANULARITY symbols, each one containing GRANULARITY-2 bits of payload
    int payload_pointer = 0;
    for(int i=0;i<BIT_COUNT/GRANULARITY * (GRANULARITY-2);i+=GRANULARITY-2) {
      data[payload_pointer++] = i==0;  // encode a 1 if this is the first symbol
      bool parity = i==0;
      for(int j=0;j<GRANULARITY-2;j++) {
	data[payload_pointer++] = data_copy[i+j];
	if (data_copy[i+j]) { parity = !parity; }
      }
      data[payload_pointer++] = !parity; // add the parity bit
    }
    return true;
  }
}
#endif//SYMBOL_CHUNK_CODER
