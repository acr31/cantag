/**
 * $Header$
 */
#ifndef TRIP_ORIGINAL_CODER_GUARD
#define TRIP_ORIGINAL_CODER_GUARD

#include "Config.hh"
#include "Coder.hh"
#include <cmath>

#undef TRIP_ORIGINAL_CODER_DEBUG

template<int CHECKSUM_COUNT=2>	 
class TripOriginalCoder : public virtual Coder {
private:
  unsigned int m_bitcount;
  unsigned int m_granularity;
  unsigned int m_codingbase;
  unsigned int m_symbol_count;
  unsigned int m_mask;

public:
  /**
   * bitcount is the number of bits we can store on the tag
   * granularity is the degree of symmetry in our representation
   * - granularity 1 means that we are encoding a bit at a time and so
   *   to generate all the possible reading of the code you
   *   repeatedly rotate by 1 bit
   * - granularity 2 means that we are encoding base 4 (two bits at a
   *   time), so rotate by two bits at a time to get possible readings
   *   of the code
   *
   * The significance of this is that the sync sector must be read
   * uniquely whatever the rotation of the reading so we need to use a
   * number of bits < the granularity to encode it
   */
  TripOriginalCoder(int bitcount, int granularity) : 
    m_bitcount(bitcount),
    m_granularity(granularity),
    m_codingbase((1<<granularity) -1), // we encode base 2^n -1 (one of the values must be the sync sector)
    m_symbol_count(bitcount-granularity*(1+CHECKSUM_COUNT)),
    m_mask(( 1<<granularity) - 1)
  {

    assert(bitcount >= granularity);
    // we need to ensure that the sync sector can be read uniquely
    assert(granularity >= 2);
    assert(m_symbol_count > 0);
  };


  /**
   * Take the bit pattern from the tag and decode the value stored
   */
  virtual unsigned long long DecodeTag(unsigned long long value) {
    // try all possible rotations...
    for(int i=0;i<m_bitcount;i+=m_granularity) {
      // have we found sync sector
#ifdef TRIP_ORIGINAL_CODER_DEBUG
      PROGRESS(value  << " " <<m_mask << " " <<(value &m_mask));
#endif
      if ((value & m_mask) == m_mask) {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Found sync sector");
#endif
	// yes
	value >>= m_granularity; // shift off the sync sector
	unsigned long checksum = 0;
	for(int i=0;i<CHECKSUM_COUNT;i++) {
	  unsigned long pwr = (unsigned long)pow(m_codingbase,i);
	  checksum += (value & m_mask)*pwr;
	  value >>= m_granularity;
	}
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Read checksum "<< checksum);
	PROGRESS("Remaining code is "<<value);
#endif
	unsigned long long code = 0;
	unsigned long checksum_check = 0;
	for(int i=0;i<m_symbol_count;i++) {
	  unsigned long long pwr = (unsigned long long)pow(m_codingbase,i);
	  code += (value & m_mask)*pwr;
	  checksum_check += value & m_mask;
	  value >>= m_granularity;
	}

	// now check the checksum
	if (checksum == checksum_check % (unsigned long)pow(m_codingbase,CHECKSUM_COUNT)) {
	  return code;		  
	}
	else {
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	  PROGRESS("Failed. Checksum on tag was "<<checksum<< " and we have "<< (checksum_check % (unsigned long)pow(m_codingbase,CHECKSUM_COUNT)));
#endif
	  throw InvalidCheckSum();
	}
      }
      else {
	// sync sector not found yet
	value = (value << m_granularity) & (((unsigned long long)1<<m_bitcount)-1) | (value >> (m_bitcount-m_granularity));
#ifdef TRIP_ORIGINAL_CODER_DEBUG
	PROGRESS("Rotated value to "<< value);
#endif
      }
    }
#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Failed to find a sync sector");
#endif
    throw InvalidCode();
  }

  /**
   * This method encodes the given value and returns the bit pattern
   * to store on the tag
   */
  virtual unsigned long long EncodeTag(unsigned long long value) {
    // check to see if the value is too large
#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Encode called with " << value);
    PROGRESS("Maximum value is "<< (unsigned long long)pow(m_codingbase,m_symbol_count) -1 );
#endif
    if (value > (unsigned long long)pow(m_codingbase,m_symbol_count) -1) {
      throw ValueTooLarge();
    }

    // now build the code from the end forwards
    unsigned long checksum = 0;
    unsigned long long result = 0;
    for(int i=m_symbol_count-1;i>=0;i--) {
      unsigned long long pwr = (unsigned long long)pow(m_codingbase,i);
      unsigned int store = value / pwr;
      value %= pwr;
      result <<= m_granularity;
      result |= store;
      checksum+=store;
    }
#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Coded value is "<<result);
#endif
    checksum %= (unsigned long)pow(m_codingbase,CHECKSUM_COUNT);
#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Now storing checksum " << checksum);
#endif

    // now store the checksum
    for(int i=CHECKSUM_COUNT-1;i>=0;i--) {
      result <<= m_granularity;
      unsigned long pwr = (unsigned long)pow(m_codingbase,i);
      result |= checksum / pwr;
      checksum %= pwr;
    }
    
    // finally, add the sync sector
    result <<= m_granularity;
    result |= (1<<m_granularity)-1;

#ifdef TRIP_ORIGINAL_CODER_DEBUG
    PROGRESS("Final bit pattern is "<<result);
#endif
    return result;
  }

};

#endif//TRIP_ORIGINAL_CODER_GUARD
