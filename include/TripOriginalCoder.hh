/**
 * $Header$
 */

#ifndef TRIP_ORIGINAL_CODER_GUARD
#define TRIP_ORIGINAL_CODER_GUARD

#include <Config.hh>
#include <Coder.hh>
#include <CyclicBitSet.hh>
#include <cmath>
#include <BigInt.hh>

#ifdef TEXT_DEBUG
#define TRIP_ORIGINAL_CODER_DEBUG
#endif
/**
 * A generalised coding scheme based on the one used in TRIP.  This
 * scheme encodes a fixed number of bits using 4 or more symbols
 * (i.e. 2 or more bits per symbol).  One symbol (with highest value)
 * is reserved for synchronization and not used again.  The remainder
 * of the code consists of a number of checksum symbols followed by
 * the coded value.  The template should be initialised with the
 * number of bits that this tag should store, the granularity, and the
 * number of checksum symbols to use.  The granularity specifies the
 * symmetry of the tag you are encoding.  It is the minimum number of
 * bits you could rotate the code by to get another possible reading.
 */ 
template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT=2>	 
class TripOriginalCoder : public virtual Coder<BIT_COUNT> {

public:
  TripOriginalCoder();

  virtual bool IsErrorCorrecting() const;
  virtual int GetSymbolSize() const;
  virtual int GetHammingDistanceBits() const;
  virtual int GetHammingDistanceSymbols() const;

  /**
   * Take the bit pattern from the tag and decode the value stored
   */
  virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;

  /**
   * This method encodes the given value and returns the bit pattern
   * to store on the tag
   */
  virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const;

};

template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::TripOriginalCoder() 
{
  // we encode into BIT_COUNT bits
  // reserve GRANULARITY of these for the sync sector
  // then reserve CHECKSUM_COUNT*GRANULARITY of these for the checksum
  // this leaves the remaining bits for data:
  //   BIT_COUNT - GRANULARITY*(CHECKSUM_COUNT+1)
  // we get one symbol for every GRANULARITY bits
  // so this leaves
  //   (BIT_COUNT - GRANULARITY*(CHECKSUM_COUNT+1))/GRANULARITY
  // data symbols
        

  assert(BIT_COUNT >= GRANULARITY);
  // we need to ensure that the sync sector can be read uniquely
  assert(GRANULARITY >= 2);

};


template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> bool TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::IsErrorCorrecting() const { return false; }

template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> int TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::GetSymbolSize() const { return GRANULARITY; }

template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> int TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::GetHammingDistanceBits() const { return 2; }

template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> int TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::GetHammingDistanceSymbols() const { return 2; }

template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> int TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
  CyclicBitSet<BIT_COUNT> payload(data);
  data.reset();

  const CyclicBitSet<GRANULARITY> sync_sector_mask((1<<GRANULARITY)-1);
  const BigInt<BIT_COUNT> base((1<<GRANULARITY)-1);
  const BigInt<BIT_COUNT> checksum_mod( BigInt<BIT_COUNT>((1<<GRANULARITY)-1).Pwr(CHECKSUM_COUNT));

  // try all possible rotations...
  for(size_t i=0;i<BIT_COUNT;i+=GRANULARITY) {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Current rotation is "<< payload);
#endif
    // have we found sync sector
    if (payload.template Equals<GRANULARITY>(sync_sector_mask)) {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
      PROGRESS("Found sync sector");
#endif

      // accumulate the checksum
      BigInt<BIT_COUNT> checksum(0);
      BigInt<BIT_COUNT> checksum_pwr(1);
      for(unsigned int c=0;c<CHECKSUM_COUNT;c++) {
	unsigned int symbol = payload.GetSymbol(c+1,GRANULARITY);
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Read Symbol "<< symbol);
#endif
	 
	checksum += checksum_pwr * BigInt<BIT_COUNT>(symbol);
	checksum_pwr *= base;
      }

      BigInt<BIT_COUNT> checksum_count(0);

      BigInt<BIT_COUNT> pwr(1);
      BigInt<BIT_COUNT> bi(data);
	
      int data_symbol_count = (BIT_COUNT - GRANULARITY*(CHECKSUM_COUNT+1))/GRANULARITY;
	
      for(int c=0;c<data_symbol_count;c++) {
	unsigned int symbol = payload.GetSymbol(c+1+CHECKSUM_COUNT,GRANULARITY);
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Read Symbol "<< symbol);
#endif	  
	BigInt<BIT_COUNT> next(symbol);
	checksum_count+=next;
	bi += pwr * next;
	pwr *= base;
      }

#ifdef TRIP_ORIGINAL_CODER_DEBUG
      PROGRESS("Code is " << bi);
      PROGRESS("Read checksum "<< checksum);
      PROGRESS("Accumulated checksum "<< checksum_count % checksum_mod);	
#endif
      // now check the checksum
      if (checksum == checksum_count % checksum_mod) {
	return i;
      }
      else {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Failed. Checksum mismatch");
#endif
	return -1;
      }
    }
    else {
      // sync sector not found yet
      payload.RotateLeft(GRANULARITY);
    }
  }
#ifdef TRIP_ORIGINAL_CODER_DEBUG
  PROGRESS("Failed to find a sync sector");
#endif
  return -1;
}

template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> bool TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
  PROGRESS("Encode called with " << data);
#endif

  CyclicBitSet<BIT_COUNT> tag_data(data);
  data.reset();

  const BigInt<BIT_COUNT> base((1<<GRANULARITY)-1);
  const BigInt<BIT_COUNT> checksum_mod( BigInt<BIT_COUNT>((1<<GRANULARITY)-1).Pwr(CHECKSUM_COUNT));
    
  // build the code by taking the remainder of tag_data with
  // (1<<GRANULARITY)-1 to get the next symbol and then dividing
  // tag_data by (1<<GRANULARITY)-1

  BigInt<BIT_COUNT> bi(tag_data);
#ifdef TRIP_ORIGINAL_CODER_DEBUG
  PROGRESS("Value to encode is "<<bi);
#endif
  BigInt<BIT_COUNT> checksum(0);
  int data_symbol_count = (BIT_COUNT - GRANULARITY*(CHECKSUM_COUNT+1))/GRANULARITY;

  for(unsigned int i=0;i<data_symbol_count;i++) {
    PROGRESS(i);
    unsigned int symbol = bi % base;
    checksum+=BigInt<BIT_COUNT>(symbol);
    data.PutSymbol(symbol,i+1+CHECKSUM_COUNT,GRANULARITY); 
    bi/=base;
  }

  checksum %= checksum_mod;
#ifdef TRIP_ORIGINAL_CODER_DEBUG
  PROGRESS("Checksum is "<<checksum);
#endif
  for(unsigned int i=0;i<CHECKSUM_COUNT;i++) {
    unsigned int symbol = checksum % base;
    data.PutSymbol(symbol,i+1,GRANULARITY);
    checksum /= base;
  }
#ifdef TRIP_ORIGINAL_CODER_DEBUG
  PROGRESS("Adding Sync sector");
#endif
  // sync sector
  data.PutSymbol( (1<<GRANULARITY)-1 , 0, GRANULARITY);

#ifdef TRIP_ORIGINAL_CODER_DEBUG
  PROGRESS("Final bit pattern is " << data);
#endif
  return true;
}

#endif//TRIP_ORIGINAL_CODER_GUARD
