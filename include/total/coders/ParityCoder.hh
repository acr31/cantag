/**
 * $Header$
 */

#ifndef PARITY_CODER_GUARD
#define PARITY_CODER_GUARD

#include <total/Config.hh>
#include <total/Coder.hh>
#include <total/CyclicBitSet.hh>

#define PARITY_DEBUG

namespace Total {

  /**
   * A parity coding scheme.  Lays out the code with a parity bit at the end.
   */
  template<int BIT_COUNT>
  class ParityCoder : public virtual Coder<BIT_COUNT> {
  public:
    virtual bool IsErrorCorrecting() const;
    virtual int GetSymbolSize() const;
    virtual int GetHammingDistanceBits() const;
    virtual int GetHammingDistanceSymbols() const;

    virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const;

    virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  };

  template<int BIT_COUNT> bool ParityCoder<BIT_COUNT>::IsErrorCorrecting() const { return false; }
  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::GetSymbolSize() const { return 1; }
  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::GetHammingDistanceBits() const { return 2; }
  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::GetHammingDistanceSymbols() const { return 2; }

  template<int BIT_COUNT> bool ParityCoder<BIT_COUNT>::EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    bool parity = true;
    for(unsigned int i=0;i<BIT_COUNT-1;i++) {
      if (data[i]) { 
	parity = !parity;
      }
    }
    data[BIT_COUNT-1] = !parity;
    return true;
  }

  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    int rotation = data.MinRotate();
    bool parity = true;    
    for(unsigned int i=0;i<BIT_COUNT;i++) {
      if (data[i]) {
	parity = !parity;
      }
    }
    data[BIT_COUNT-1] = false;

#ifdef PARITY_DEBUG
    PROGRESS("Parity is "<<parity);
#endif 
    
    if (parity) {
      return rotation;
    }
    else {
      return -1;
    }
  }
}
#endif//PARITY_CODER_GUARD
