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
class Payload {
private:
  std::bitset<BITCOUNT>* m_data;
public:
  Payload(std::bitset<BITCOUNT>& data) : m_data(&data) {};

  /**
   * Rotate the bit field so that no other rotation has a smaller
   * value.
   */
  int MinRotate() {
    std::bitset<BITCOUNT> currentMin = &m_data; // copy
    int minindex = 0;
    for(int i=1;i<BITCOUNT;i++) {
      RotateLeft(1);
      if (*this<currentMin) {
	currentMin = *this; //copy
	minindex=i;
      }
    }
    RotateLeft(minindex+1);
    return minindex;
  }

  /** 
   * Returns true if this 
   */
  bool LessThan(int rot1, int rot2) {

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

  bool operator<(const Payload<BITCOUNT>& o) const {
    for(unsigned int i=BITCOUNT;i>0;i--) {
      if ( !(*this)[i-1] && o[i-1] ) {
	return true;
      }
      else if ( (*this)[i-1] && !o[i-1] ) {
	return false;
      }
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
