/**
 * $Header$
 */

#ifndef CODER_GUARD
#define CODER_GUARD

#include <Payload.hh>

template <int PAYLOAD_SIZE>
class Coder {
public:  
  
  /**
   * Alters the passed payload inorder to encode it.
   *
   * Returns false if the value could not be encoded for some reason.
   */
  virtual bool EncodePayload(Payload<PAYLOAD_SIZE>& payload) = 0;

    
  /**
   * Decode the passed value in place.
   *
   * Returns the number of rotations required to orient the code
   * correctly or -1 if there was some kind of failure
   *
   */
  virtual int DecodeTag(Payload<PAYLOAD_SIZE>& payload)  =0;

};


#endif//CODER_GUARD
