/**
 * $Header$
 */

#ifndef PAYLOAD_GUARD
#define PAYLOAD_GUARD

#include <bitset>
#include <string>
/**
 * A class for storing the data or identifier to be placed on the tag.
 */
template <int BITCOUNT>
class Payload : public std::bitset<BITCOUNT> {
public:
  Payload() : std::bitset<BITCOUNT>() {};

  Payload(unsigned long value) : std::bitset<BITCOUNT>(value) {};

  Payload(char* value) : std::bitset<BITCOUNT>() {
    unsigned int pointer = 0;
    while(value) { 
      char current = *value;
      do {
	*this[pointer] = current & 0x1;
	current >>= 1;
      }
      while(current);
      value++;
    }
  }
  
  /**
   * Rotate the bit field so that the bit at position i goes to
   * position (i + count) % BITCOUNT.
   */
  void RotateLeft(int count) {
    std::bitset<BITCOUNT> t = *this>>(BITCOUNT-count);
    *this <<= count;
    *this |= t;
  }

  /**
   * Rotate the bit field so that the bit at position
   * (i+count)%BITCOUNT goes to i.
   */
  void RotateRight(int count) {
    std::bitset<BITCOUNT> t = *this<<(BITCOUNT-count);
    *this >>= count;
    *this |= t;
  }

  /**
   * Returns true if the lower bits of this object match the passed
   * object or vice versa if the passed object is longer.
   *
   * \todo Doesn't deal with the passed object being longer
   */ 
  template<int SIZE> bool Match(const std::bitset<SIZE>& target) const {
    for(int i=0;i<SIZE;i++) {
      if (target[i] != (*this)[i]) {
	return false;
      }
    }
    return true;
  }

  /**
   * Get the index'th symbol (of size count bits) from the payload.
   */
  unsigned int Get(unsigned int index, unsigned int count) const {
    unsigned int result = 0;
    for(unsigned int i=count;i>0;i--) {
      result <<= 1;
      result |= (*this)[index*count+i-1];
    }
    return result;
  }

  /**
   * Store the given symbol as the index'th symbol of count bits
   */
  void Put(unsigned int symbol, unsigned int index,unsigned int count) {
    for(unsigned int i=0;i<count;i++) {
      bool value = (symbol & 0x1 == 0x1);
      (*this)[index*count+i] = value;
      symbol>>=1;
    }
  }

  bool operator==(const Payload<BITCOUNT>& o) const {
    for(unsigned int i=0;i<BITCOUNT;i++) {
      if ( (*this)[i] != o[i]) { 
	return false;
      }
    }
    return true;
  }

  std::string to_string() const {
    std::string result;
    for(unsigned int i=BITCOUNT;i>0;i--) {
      result.append( (*this)[i-1] ? "1" : "0" );
    }
    return result;

  }
};

#endif//PAYLOAD_GUARD
