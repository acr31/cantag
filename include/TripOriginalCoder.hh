/**
 * $Header$
 *
 * $Log$
 * Revision 1.5  2004/02/01 14:25:33  acr31
 * moved Rectangle2D to QuadTangle2D and refactored implementations around
 * the place
 *
 * Revision 1.4  2004/01/30 16:54:17  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.3  2004/01/27 18:06:59  acr31
 * changed inheriting classes to inherit publicly from their parents
 *
 * Revision 1.2  2004/01/25 15:13:25  acr31
 * added check for code out of range
 *
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.7  2004/01/24 19:29:24  acr31
 * removed ellipsetoxy and put the project method in Ellipse2D objects
 *
 * Revision 1.6  2004/01/24 17:53:22  acr31
 * Extended TripOriginalCoder to deal with base 2 encodings.  MatrixTag
 * implementation now works.
 *
 * Revision 1.5  2004/01/23 22:35:04  acr31
 * changed coder to use unsigned long long
 *
 * Revision 1.4  2004/01/23 18:18:12  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 * Revision 1.3  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#ifndef TRIP_ORIGINAL_CODER_GUARD
#define TRIP_ORIGINAL_CODER_GUARD

#include "Config.hh"
#include "Coder.hh"
#include <cmath>

#undef FILENAME
#define FILENAME "TripOriginalCoder.hh"

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
      PROGRESS(value  << " " <<m_mask << " " <<(value &m_mask));
      if ((value & m_mask) == m_mask) {
	PROGRESS("Found sync sector");
	// yes
	value >>= m_granularity; // shift off the sync sector
	unsigned long checksum = 0;
	for(int i=0;i<CHECKSUM_COUNT;i++) {
	  unsigned long pwr = (unsigned long)pow(m_codingbase,i);
	  checksum += (value & m_mask)*pwr;
	  value >>= m_granularity;
	}
	PROGRESS("Read checksum "<< checksum);
	PROGRESS("Remaining code is "<<value);
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
	  PROGRESS("Failed. Checksum on tag was "<<checksum<< " and we have "<< (checksum_check % (unsigned long)pow(m_codingbase,CHECKSUM_COUNT)));
	  throw InvalidCheckSum();
	}
      }
      else {
	// sync sector not found yet
	value = (value << m_granularity) & (((unsigned long long)1<<m_bitcount)-1) | (value >> (m_bitcount-m_granularity));
	PROGRESS("Rotated value to "<< value);
      }
    }
    PROGRESS("Failed to find a sync sector");
    throw InvalidCode();
  }

  /**
   * This method encodes the given value and returns the bit pattern
   * to store on the tag
   */
  virtual unsigned long long EncodeTag(unsigned long long value) {
    // check to see if the value is too large
    PROGRESS("Encode called with " << value);
    PROGRESS("Maximum value is "<< (unsigned long long)pow(m_codingbase,m_symbol_count) -1 );
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
    PROGRESS("Coded value is "<<result);
    checksum %= (unsigned long)pow(m_codingbase,CHECKSUM_COUNT);
    PROGRESS("Now storing checksum " << checksum);

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

    PROGRESS("Final bit pattern is "<<result);
    return result;
  }

};

#endif//TRIP_ORIGINAL_CODER_GUARD
