/**
 * $Header: /filer/acr31/unix_home/cvsroot/src/total/include/total/coders/SymbolChunkCoder.hh,v 1.3 2005/05/24 07:44:21 rkh23 Exp $
 */

#ifndef CRC_SYMBOL_CHUNK_CODER
#define CRC_SYMBOL_CHUNK_CODER

#include <cantag/Config.hh>
#include <cantag/coders/Coder.hh>
#include <cantag/coders/CRC.hh>

#include <iostream>

#define CRC_SYMBOL_CHUNK_DEBUG

namespace Cantag {
  /**
   * Encodes the data into a number of symbols each of GRANULARITY bits
   * The first bit of each symbol is the orientation bit - 1 for
   * the first symbol, CRC_BITS are used for a CRC and the rest
   * (GRANULARITY-1-CRC_BITS) make up the data in the block
   */
  template<int BIT_COUNT, int CRC_BITS, int GRANULARITY>
  class CRCSymbolChunkCoder : public virtual Coder<BIT_COUNT> {
  public:
    typedef CRCSymbolChunkCoder<BIT_COUNT,CRC_BITS,GRANULARITY> CoderType;
    virtual bool IsErrorCorrecting() const;
    virtual int GetSymbolSize() const;
    virtual int GetHammingDistanceBits() const;
    virtual int GetHammingDistanceSymbols() const;


    virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  
    virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  };


  template<int BIT_COUNT,int CRC_BITS, int GRANULARITY> bool CRCSymbolChunkCoder<BIT_COUNT,CRC_BITS,GRANULARITY>::IsErrorCorrecting() const { return false; }

  template<int BIT_COUNT,int CRC_BITS, int GRANULARITY> int CRCSymbolChunkCoder<BIT_COUNT,CRC_BITS,GRANULARITY>::GetSymbolSize() const { return GRANULARITY; }

  template<int BIT_COUNT,int CRC_BITS, int GRANULARITY> int CRCSymbolChunkCoder<BIT_COUNT,CRC_BITS,GRANULARITY>::GetHammingDistanceBits() const { return 2; }

  template<int BIT_COUNT,int CRC_BITS, int GRANULARITY> int CRCSymbolChunkCoder<BIT_COUNT,CRC_BITS,GRANULARITY>::GetHammingDistanceSymbols() const { return 0; }


  template<int BIT_COUNT, int CRC_BITS, int GRANULARITY> int CRCSymbolChunkCoder<BIT_COUNT,CRC_BITS,GRANULARITY>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
  	
  	// Check the CRC on each block
    for (int j=0; j<BIT_COUNT/GRANULARITY; j++) {
      std::bitset<GRANULARITY-CRC_BITS> pl;
      for (int i=0; i<GRANULARITY-CRC_BITS; i++) pl[i]=data[j*GRANULARITY+i];
      CRC<GRANULARITY-CRC_BITS, CRC_BITS> crc;
      std::bitset<CRC_BITS> c = crc.GetCRC(pl);
      std::bitset<CRC_BITS> c_read;
      for (int a=0; a<CRC_BITS; a++) {
	c_read[a]=data[j*GRANULARITY+GRANULARITY-CRC_BITS+a];
      }
      //      std::cout << data << " " << pl << " " << c_read << " " << c << std::endl;
      if (c_read!=c) return -1;
    }

    // rotate the code by increments of GRANULARITY until the first bit is a 1
    int rotation = 0;
    while(!data[0] && rotation < BIT_COUNT) {
      data.RotateRight(GRANULARITY);
      rotation+=GRANULARITY;
    }

    if (data[0]) {
      CyclicBitSet<BIT_COUNT> data_copy(data);
      data.reset();
      // Concatenate the payload
      unsigned int pld=0;
      for (int i=0; i<BIT_COUNT; i+=GRANULARITY) {
	for (int j=0; j<GRANULARITY-1-CRC_BITS; j++) {
	  data.Set(pld++,data_copy[i+j+1]);
	}
      }
      return rotation;
    }
    else {
#ifdef CRC_SYMBOL_CHUNK_DEBUG
      PROGRESS("Failed to find orientation start bit");
#endif
      return -1;
    }
  }

  template<int BIT_COUNT,int CRC_BITS, int GRANULARITY> bool CRCSymbolChunkCoder<BIT_COUNT,CRC_BITS,GRANULARITY>::EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    CyclicBitSet<BIT_COUNT> data_copy(data); 
    data.reset();

    int payload_pointer = 0;
    for(int i=0;i<BIT_COUNT/GRANULARITY * (GRANULARITY-1-CRC_BITS);i+=GRANULARITY-1-CRC_BITS) {

      std::bitset<GRANULARITY-CRC_BITS> toenc(0);
      int toenci=0;

      // First the marker
      data.Set(payload_pointer++,i==0);  // encode a 1 if this is the first symbol
      toenc[toenci++] = (i==0); 

      // Now the data
      for(int j=0;j<GRANULARITY-1-CRC_BITS;j++) {
	data.Set(payload_pointer++,data_copy[i+j]);
	toenc[toenci++] = data_copy[i+j];
      }

      // Now the checksum per block
      CRC<GRANULARITY-CRC_BITS, CRC_BITS> crc;
      std::bitset<CRC_BITS> c = crc.GetCRC(toenc);

      for (int k=0; k<CRC_BITS; k++) {
	data.Set(payload_pointer++,c[k]);
      }
    }
    return true;
  }

}
#endif//SYMBOL_CHUNK_CODER
