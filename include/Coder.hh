/**
 * $Header$
 */

#ifndef CODER_GUARD
#define CODER_GUARD

#include <CyclicBitSet.hh>

template <int PAYLOAD_SIZE>
class Coder {
public:  

  static const int PayloadSize = PAYLOAD_SIZE;
  
  /**
   * Alters the passed payload inorder to encode it.
   *
   * Returns false if the value could not be encoded for some reason.
   */
  virtual bool EncodePayload(CyclicBitSet<PAYLOAD_SIZE>& data) const = 0;

    
  /**
   * Decode the passed value in place.
   *
   * Returns the number of rotations required to orient the code
   * correctly or -1 if there was some kind of failure
   *
   */
  virtual int DecodePayload(CyclicBitSet<PAYLOAD_SIZE>& data) const =0;

  /**
   * Return true if this code is error correcting or false if it only
   * detects errors.
   */
  virtual bool IsErrorCorrecting() const = 0;

  /**
   * Return the number of bits per symbol.  The code could be rotated
   * by any multiple of the symbol size.
   */
  virtual int GetSymbolSize() const = 0;

  /**
   * Return the codes minimum hamming distance in bits.  This is the
   * minimum number of bits that you need to change to go from one
   * code word to another one.
   */
  virtual int GetHammingDistanceBits() const = 0;

  /**
   * Return the codes minimum hamming distance in bits.  This is the
   * minimum number of symbols that you need to change to go from one
   * code word to another one.
   */
  virtual int GetHammingDistanceSymbols() const = 0;


};


#endif//CODER_GUARD
