#ifndef TRIP_ORIGINAL_CODER_GUARD
#define TRIP_ORIGINAL_CODER_GUARD

#include "Coder.hh"
#include <cmath>

#undef FILENAME
#define FILENAME "TripOriginalCoder.hh"
enum state_t {SYNC, CHECKSUM, CODE};

template<int RING_COUNT=2,
	 int SECTOR_COUNT=16,
	 int CHECKSUM_COUNT=2>	 
class TripOriginalCoder : Coder {
private:
  unsigned long m_accum_code;
  unsigned int m_accum_checksum;
  unsigned int m_accum_checksum_check;
  state_t m_state;
  unsigned int m_state_counter;

  unsigned long m_encoded;

public:
  TripOriginalCoder() : 
    m_accum_code(0),
    m_accum_checksum(0), 
    m_accum_checksum_check(0), 
    m_state(SYNC), 
    m_state_counter(0),
    m_encoded(0) {};
  
  void Set(unsigned long value) {
    PROGRESS("Encoder set with value "<<value);
    unsigned long code = 0;
    unsigned long checksum = 0;
    
    int base = (1<<RING_COUNT)-1;
    int max = (SECTOR_COUNT-CHECKSUM_COUNT-1);

    if (value >= pow(base,max)) {
      throw ValueTooLarge();
    }

    // Work backwards from the end of the code
    for(int position=max;position>=0;position--) {
      int pwr = (int)pow(base,position);
      int bit = (int)(value/pwr);
      value = value % pwr;
      code = code << RING_COUNT;
      code |= bit;
      checksum += bit;
    }
    PROGRESS("Checksum is "<<checksum);
    // Add the checksum
    checksum = checksum % (int)pow(base,CHECKSUM_COUNT);
    for(int position=CHECKSUM_COUNT-1;position>=0;position--) {
      int pwr = (int)pow(base,position);
      int bit = (int)(checksum/pwr);
      checksum = checksum % pwr;
      code = code << RING_COUNT;
      code |= bit;      
    }

    // Add the sync sector
    code = code << RING_COUNT;
    code |= base;

    m_encoded = code;
    PROGRESS("Encoded value is "<<code);
  }

  unsigned int NextChunk() {
    unsigned int result = m_encoded & ((1<<RING_COUNT)-1);
    PROGRESS("NextChunk returns "<<result);
    m_encoded = m_encoded >> RING_COUNT;
    return result;
  }


  virtual void Reset() {
    PROGRESS("Decoder Reset");
    m_accum_code = 0;
    m_accum_checksum = 0;
    m_accum_checksum_check = 0;
    m_state = SYNC;
    m_state_counter =0;
  }

  virtual bool LoadChunk(unsigned int chunk) {
    PROGRESS("LoadChunk value "<<chunk);
    if (chunk & ~((1<<RING_COUNT)-1)) {
      PROGRESS("Throwing InvalidSymbol");
      throw InvalidSymbol();
    }

    if (m_state == SYNC) {
      // we are looking for the sync sector
      if (chunk == ((1<<RING_COUNT)-1)) {
	m_state = CHECKSUM;
	PROGRESS("Accepting sync sector");
	return true;
      }
      else {
	PROGRESS("Rejected missing sync sector");
	return false;
      }
    }
    else if (chunk == ((1<<RING_COUNT)-1)) {
      PROGRESS("Throwing InvalidSymbol - non sync sector reading all ones");
      throw InvalidSymbol();
    }
    else if (m_state == CHECKSUM) {
      PROGRESS("Accumulating checksum");
      int base = (1<<RING_COUNT)-1;
      int pwr = (int)pow(base,m_state_counter);
      m_accum_checksum+=pwr*chunk;
      if (m_state_counter == CHECKSUM_COUNT-1) {
	m_state = CODE;
	m_state_counter=0;
      }
      else {
	m_state_counter++;
      }
    }
    else { // if (m_state == CODE) {
      PROGRESS("Accumulating code");
      int base = (1<<RING_COUNT)-1;
      int pwr = (int)pow(base,m_state_counter);
      m_accum_code+=pwr*chunk;
      m_accum_checksum_check += chunk;
      m_state_counter++;
    }
  }

  virtual unsigned long Decode() const  {
    PROGRESS("Decode called");
    int base = (1<<RING_COUNT)-1;
    int check = m_accum_checksum_check % (int)pow(base,CHECKSUM_COUNT);
    if (check == m_accum_checksum) {
      PROGRESS("Checksum valid returning "<<m_accum_code);
      return m_accum_code;
    }
    else {
      PROGRESS("Throwing InvalidCheckSum.  Checksum was "<<check<<" and we expected "<<m_accum_checksum);
      throw InvalidCheckSum();
    }
  }

};

#endif//TRIP_ORIGINAL_CODER_GUARD
