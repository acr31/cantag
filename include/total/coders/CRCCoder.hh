/**
 * $Header$
 */

#ifndef CRC_CODER_GUARD
#define CRC_CODER_GUARD

#include <tripover/Config.hh>
#include <tripover/Coder.hh>
#include <boost/crc.hpp>

/**
 * A CRC based coding scheme.  The generator polynomials used are:
 *
 * CRC-32 = x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 * CRC-24 = x^24+x^23+x^14+x^12+x^8+1
 * CRC-16 = x^16+x^15+x^2+1
 * CRC-12 = x^12+x^11+x^3+x^2+x+1
 * CRC-8  = x^8+x^7+x^6+x^4+x^2+1
 * CRC-4  = x^4+x^3+x^2+x+1
 *
 * \todo finish this one off.  
 *
 * \todo include in the coder regression testing
 */
template<int PAYLOAD_SIZE, int CRC_SIZE,  
	 typename boost::crc_basic<CRC_SIZE>::value_type TRUNC_POLY, 
	 typename boost::crc_basic<CRC_SIZE>::value_type INIT_REM, 
	 typename boost::crc_basic<CRC_SIZE>::value_type FINAL_XOR, 
	 bool  REFLECT_IN, 
	 bool REFLECT_REM>
class CRCCoder : public Coder<PAYLOAD_SIZE> {
public:
  CRCCoder() {};
  
  virtual bool EncodePayload(const std::bitset<PAYLOAD_SIZE>& tag_data, Payload<PAYLOAD_SIZE>& payload) const;

  virtual int DecodePayload(std::bitset<PAYLOAD_SIZE>& data, Payload<PAYLOAD_SIZE>& payload) const;
};

template<int PAYLOAD_SIZE, int CRC_SIZE> bool CRCCoder<PAYLOAD_SIZE,CRC_SIZE>::EncodePayload(const std::bitset<PAYLOAD_SIZE>& tag_data, Payload<PAYLOAD_SIZE>& payload) const {
  boost::crc_basic<CRC_SIZE> crc(TRUNC_POLY, INIT_REM, FINAL_XOR, REFLECT_IN, REFLECT_REM);
  for(unsigned int i=0;i<PAYLOAD_SIZE-CRC_SIZE;i++) {
    crc.process_bit(tag_data[i]);
    payload[CRC_SIZE+i] = tag_data[i];
  }
  for(unsigned int i=0;i<CRC_SIZE;i++) {
    crc.process_bit(0);
  }
  payload |= crc.checksum();
  payload.MinRotate();
};


template<int PAYLOAD_SIZE, int CRC_SIZE> bool CRCCoder<PAYLOAD_SIZE,CRC_SIZE>::DecodePayload(std::bitset<PAYLOAD_SIZE>& data, Payload<PAYLOAD_SIZE>& payload) const {
  int rotation = payload.MinRotate();
  boost::crc_basic<CRC_SIZE> crc(TRUNC_POLY, INIT_REM, FINAL_XOR, REFLECT_IN, REFLECT_REM);
  for(unsigned int i=0;i<PAYLOAD_SIZE;i++) {
    crc.process_bit(payload[i]);
  }
  if (crc.checksum() == 0) {
    data = (payload>>CRC_SIZE);    
    return rotation;
  }
  else {
    return -1;
  }
};

template<int PAYLOAD_SIZE> 
struct CRCCoder32
{ 
  typedef CRCCoder<PAYLOAD_SIZE,32,0x4C11DB7,0xFFFFFFFF,0xFFFFFFFF,true,true> Type;
};

template<int PAYLOAD_SIZE> 
struct CRCCoder24
{ 
  typedef CRCCoder<PAYLOAD_SIZE,24,0x805101,0x0,0x0,false,false> Type;
};

template<int PAYLOAD_SIZE> 
struct CRCCoder16
{ 
  typedef CRCCoder<PAYLOAD_SIZE,16,0x8005,0x0,0x0,true,true> Type;
};

template<int PAYLOAD_SIZE>
struct CRCCoder12
{
  typedef CRCCoder<PAYLOAD_SIZE,12,0x407,0x0,0x0,true,true> Type;
};

template<int PAYLOAD_SIZE>
struct CRCCoder8
{
  typedef CRCCoder<PAYLOAD_SIZE,8,0x6B,0x0,0x0,false,false> Type;
};

template<int PAYLOAD_SIZE>
struct CRCCoder4
{
  typedef CRCCoder<PAYLOAD_SIZE,4,0xF,0x0,0x0,false,false> Type;
};

#endif//CRC_CODER_GUARD
