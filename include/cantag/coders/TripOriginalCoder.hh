/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */

#ifndef TRIP_ORIGINAL_CODER_GUARD
#define TRIP_ORIGINAL_CODER_GUARD

#include <cantag/Config.hh>
#include <cantag/coders/Coder.hh>
#include <cantag/CyclicBitSet.hh>
#include <cantag/BigInt.hh>

#include <cmath>
#include <cassert>

#ifdef TEXT_DEBUG
# define TRIP_ORIGINAL_CODER_DEBUG
#endif

namespace Cantag {
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
    typedef TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT> CoderType;
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
    const BigInt base((1<<GRANULARITY)-1);
    const BigInt checksum_mod( BigInt((1<<GRANULARITY)-1).Pwr(CHECKSUM_COUNT));

    // try all possible rotations...
    for(size_t i=0;i<BIT_COUNT;i+=GRANULARITY) {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
      PROGRESS("Current rotation (" << i << ") is "<< payload);
#endif
      // have we found sync sector
      if (payload.template Equals<GRANULARITY>(sync_sector_mask)) {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Found sync sector");
#endif

	// accumulate the checksum
	BigInt checksum(0);
	BigInt checksum_pwr(1);
	for(unsigned int c=0;c<CHECKSUM_COUNT;c++) {
	  unsigned int symbol = payload.GetSymbol(c+1,GRANULARITY);
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	  PROGRESS("Read Symbol "<< symbol);
#endif
	 
	  checksum += checksum_pwr * BigInt(symbol);
	  checksum_pwr *= base;
	}

	BigInt checksum_count(0);

	BigInt pwr(1);
	BigInt bi(0);
	
	int data_symbol_count = (BIT_COUNT - GRANULARITY*(CHECKSUM_COUNT+1))/GRANULARITY;
	
	for(int c=0;c<data_symbol_count;c++) {
	  unsigned int symbol = payload.GetSymbol(c+1+CHECKSUM_COUNT,GRANULARITY);
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	  PROGRESS("Read Symbol "<< symbol);
#endif	  
	  BigInt next(symbol);
	  checksum_count+=next;
	  bi += pwr * next;
	  pwr *= base;
	}

	bi.ToCyclicBitSetDestructive(data);

#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Code is " << bi);
	PROGRESS("Read checksum "<< checksum);
	PROGRESS("Accumulated checksum "<< checksum_count % checksum_mod);	
#endif
	// now check the checksum
	if (checksum == (checksum_count % checksum_mod)) {
	  return i;
	}
	else {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	  PROGRESS("Failed. Checksum mismatch");
#endif
	  payload.SetInvalid();
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
    payload.SetInvalid();
    return -1;
  }

  template<int BIT_COUNT, int GRANULARITY, int CHECKSUM_COUNT> bool TripOriginalCoder<BIT_COUNT,GRANULARITY,CHECKSUM_COUNT>::EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Encode called with " << data);
#endif

    CyclicBitSet<BIT_COUNT> tag_data(data);
    data.reset();

    const BigInt base((1<<GRANULARITY)-1);
    const BigInt checksum_mod( BigInt((1<<GRANULARITY)-1).Pwr(CHECKSUM_COUNT));
    
    // build the code by taking the remainder of tag_data with
    // (1<<GRANULARITY)-1 to get the next symbol and then dividing
    // tag_data by (1<<GRANULARITY)-1

    BigInt bi(tag_data);
#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Value to encode is "<<bi);
#endif
    BigInt checksum(0);
    int data_symbol_count = (BIT_COUNT - GRANULARITY*(CHECKSUM_COUNT+1))/GRANULARITY;

    for(int i=0;i<data_symbol_count;i++) {
      unsigned int symbol = bi % base;
      checksum+=BigInt(symbol);
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
}
#endif//TRIP_ORIGINAL_CODER_GUARD
