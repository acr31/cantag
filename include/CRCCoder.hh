/**
 * $Header$
 */

#ifndef CRC_CODER_GUARD
#define CRC_CODER_GUARD

#include <Config.hh>
#include <Coder.hh>

/**
 * A CRC based coding scheme.  The generator polynomials used are:
 *
 * CRC-32 = x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 * CRC-24 = x^24+x^23+x^14+x^12+x^8+1
 * CRC-16 = x^16+x^15+x^2+1
 * CRC-8  = x^8+x^7+x^6+x^4+x^2+1
 * CRC-4  = x^4+x^3+x^2+x+1
 */
template<int PAYLOAD_SIZE, int CRC_SIZE>
class CRCCoder : public Coder<PAYLOAD_SIZE> {
private:

public:
  CRCCoder();
  
  virtual bool EncodePayload(const std::bitset<PAYLOAD_SIZE>& tag_data, Payload<PAYLOAD_SIZE>& payload) const {
    payload = tag_data<<CRC_SIZE;
    
    
  }
  virtual int DecodePayload(std::bitset<PAYLOAD_SIZE>& data, Payload<PAYLOAD_SIZE>& payload) const;

private:
  void ShiftRegister(Payload<PAYLOAD_SIZE>& payload) const {
    
  }
};


#endif//CRC_CODER_GUARD
