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
  virtual bool EncodePayload(const std::bitset<BIT_COUNT>& tag_data, Payload<BIT_COUNT>& payload) const {
    bool parity = true;
    for(int i=0;i<BIT_COUNT-1;i++) {
      payload[i]=tag_data[i];
      if (tag_data[i]) { 
	parity = !parity;
      }
    }

#ifdef PARITY_DEBUG
    PROGRESS("Parity is "<<parity);
#endif 

    payload[BIT_COUNT-1] = !parity;

    payload.MinRotate();
  }

  virtual int DecodePayload(std::bitset<BIT_COUNT>& data, Payload<BIT_COUNT>& payload) const {
    int rotation = payload.MinRotate();
    bool parity = true;
    for(int i=0;i<BIT_COUNT-1;i++) {
      data[i] = payload[i];
      if (payload[i]) {
	parity = !parity;
      }
    }

    if (payload[BIT_COUNT-1]) {
      parity = !parity;
    }

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
