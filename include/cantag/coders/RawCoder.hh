/**
 * $Header$
 */

#ifndef RAW_CODER_GUARD
#define RAW_CODER_GUARD

#include <cantag/Config.hh>
#include <cantag/coders/Coder.hh>
#include <cantag/CyclicBitSet.hh>

namespace Cantag {

  /**
   * A raw coding scheme.  Lays out the code given - no error detection.
   */
  template<int BIT_COUNT, int SYMBOLSIZE>
  class RawCoder : public virtual Coder<BIT_COUNT> {
  public:
    virtual bool IsErrorCorrecting() const { return false; }
    virtual int GetSymbolSize() const { return 1; }
    virtual int GetHammingDistanceBits() const { return 1; }
    virtual int GetHammingDistanceSymbols() const { return 1; }

    virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const { return true; }

    virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  };

  template<int BIT_COUNT,int SYMBOLSIZE> int RawCoder<BIT_COUNT,SYMBOLSIZE>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    int rotation = data.MinRotate(SYMBOLSIZE) * SYMBOLSIZE;
    return rotation;
  }
}
#endif//RAW_CODER_GUARD
