/**
 * $Header$
 *
 * $Log$
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
class TripOriginalCoder : Coder {
private:
  int m_symbol_range;
  int m_symbol_count;
  int m_original_symbol_range;
  int m_original_symbol_count;
  int m_counter;
  unsigned int *m_values;


public:
  TripOriginalCoder(int symbol_range,int symbol_count) : 
    m_symbol_range(symbol_range),
    m_symbol_count(symbol_count),
    m_original_symbol_range(symbol_range),
    m_original_symbol_count(symbol_count),
    m_counter(0) {

    assert(symbol_range >= 2);

    if (symbol_range == 2) {
      m_symbol_count /= 2;
      m_symbol_range = 4;
    }

    m_values = new unsigned int[m_symbol_count];

  };

  /**
   * This method takes the value and stores internally a value which
   * when interpreted with base symbol_range has the sync digit at the
   * beginning and then two checksum digits and then the code
   */
  void Set(unsigned long long value) {
    m_values[0] = m_symbol_range-1;
    m_counter = 0;
    int num_syms = m_symbol_count - CHECKSUM_COUNT -1;
    unsigned int checksum = 0;
    for(int i=num_syms-1;i>=0;i--) {
      unsigned long long pwr = (unsigned long long)pow(m_symbol_range-1,i);
      // we need to produce a base m_symbol_range-1 encoding of the value
      m_values[i+CHECKSUM_COUNT+1] = (unsigned int)(value / pwr);
      
      // now remove this from the total
      value -= m_values[i+CHECKSUM_COUNT+1] * pwr;
      checksum += m_values[i+CHECKSUM_COUNT+1];
    }
    PROGRESS("Checksum is "<< checksum);
    
    // we now have to store the checksum in the first few values base m_symbol_range-1
    unsigned int max_checksum = (unsigned int)pow(m_symbol_range-1,CHECKSUM_COUNT);
    checksum %= max_checksum;

    for(int i=CHECKSUM_COUNT-1;i>=0;i--) {
      unsigned int pwr = (unsigned int)pow(m_symbol_range-1,i);
      m_values[i+1] = checksum/pwr;
      checksum -= m_values[i+1] * pwr;
    }
  }
  
  /**
   * Read off the encoded code in the original symbol range
   */
  unsigned int NextChunk() {
    unsigned int result;
    if (m_original_symbol_range == 2) {
      result = m_values[m_counter/2];
      result >>= m_counter % 2;
      result &= 1;
      m_counter++;
    }
    else {
      result = m_values[m_counter++];
    }
    PROGRESS("NextChunk returns "<<result);
    return result;
  }

  virtual void Reset() {
    PROGRESS("Decoder Reset");
    m_counter=0;
  }

  /**
   * Accumulate the encoded value - the tag is giving us data base
   * original_symbol_range   
   */ 
  virtual bool LoadChunk(unsigned int chunk) {
    PROGRESS("LoadChunk: "<<chunk);
    if (chunk > m_original_symbol_range-1) {
      PROGRESS("Throwing InvalidSymbol");
      throw InvalidSymbol();
    }
    
    if (m_original_symbol_range == 2) {
      if (m_counter % 2 == 0) {
	m_values[m_counter++/2] = chunk;
      }
      else {
	m_values[m_counter++/2] += chunk << 1;
      }
    }
    else {
      m_values[m_counter++] = chunk;
    }
    return true;
  }

  /**
   * We now need to decode the code
   * It should have a sync digit at the beginning with value m_symbol_range-1
   *
   * Then follows some checksum digits which is a number base
   * m_symbol_range-1 encoded in base m_symbol_range.  So we have to
   * read each digit and add it up
   *
   */
  virtual unsigned long long Decode() {
    PROGRESS("Decoding");
    for(int i=0;i<m_symbol_count;i++) {
      if (m_values[i] == m_symbol_range -1 ) {
	PROGRESS("Found SYNC sector");

	// extract the checksum
	unsigned int checksum = 0;
	unsigned int pwr = 1;
	for(int j=0;j<CHECKSUM_COUNT;j++) {
	  checksum += m_values[(i+j+1) % m_symbol_count] * pwr;
	  pwr *= m_symbol_range - 1;
	}
	PROGRESS("Checksum is "<< checksum);
	
	// extract the result is what's left       	
	unsigned long long result = 0;
	unsigned int checksum_check = 0;
	unsigned long long r_pwr = 1;
	int num_syms = m_symbol_count - CHECKSUM_COUNT -1;
	for(int j=0;j<num_syms;j++) {
	  result += m_values[(i+j+1+CHECKSUM_COUNT) % m_symbol_count] * r_pwr;
	  checksum_check += m_values[(i+j+1+CHECKSUM_COUNT) % m_symbol_count];
	  r_pwr *= m_symbol_range -1;
	}
	PROGRESS("Result is "<< result);
	PROGRESS("Checksum check is "<<checksum_check);

	checksum_check %= (unsigned int)pow(m_symbol_range-1,CHECKSUM_COUNT);

	if (checksum_check == checksum) {
	  PROGRESS("Checksum valid returning "<<result);
	  return result;
	}
	else {
	  PROGRESS("Throwing InvalidCheckSum.  Checksum was "<<checksum_check<<" and we expected "<<checksum);
	  throw InvalidCheckSum();
	}
      }
    }
    PROGRESS("Failed to find a synchronization sector");
    throw InvalidCode();
  }

};

#endif//TRIP_ORIGINAL_CODER_GUARD
