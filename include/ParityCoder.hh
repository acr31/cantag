/**
 * $Header$
 */

#ifndef PARITY_CODER_GUARD
#define PARITY_CODER_GUARD
#include <Config.hh>
#include <Coder.hh>

#define PARITY_DEBUG

template<int BIT_COUNT>
class ParityCoder : public virtual Coder<BIT_COUNT> {
public:
  virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    bool parity = true;
    for(int i=0;i<BIT_COUNT-1;i++) {
      if (data[i]) { 
	parity = !parity;
      }
    }

#ifdef PARITY_DEBUG
    PROGRESS("Parity is "<<parity);
#endif 

    data[BIT_COUNT-1] = !parity;

    data.MinRotate();
    return true;
  }

  virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    int rotation = data.MinRotate();
    bool parity = true;
    for(int i=0;i<BIT_COUNT;i++) {
      if (data[i]) {
	parity = !parity;
      }
    }
    data[BIT_COUNT] = false;

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
};

#endif//PARITY_CODER_GUARD
