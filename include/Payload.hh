/**
 * $Header$
 */

#ifndef PAYLOAD_GUARD
#define PAYLOAD_GUARD

#include <bitset>

/**
 * A class for storing the data or identifier to be placed on the tag.
 */
template <int bitcount>
class Payload : public std::bitset<bitcount> {

  Payload(unsigned long value) : std::bitset(value) {};

  Payload(char* value) : std::bitset() {
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
   * position (i + count) % bitcount.
   */
  void RotateLeft(int count) {
    std::bitset<bitcount> t = *this>>(bitcount-count);
    *this <<= count;
    *this |= t;
  }

  /**
   * Rotate the bit field so that the bit at position
   * (i+count)%bitcount goes to i.
   */
  void RotateRight(int count) {
    std::bitset<bitcount> t = *this<<(bitcount-count);
    *this >>= count;
    *this |= t;
  }

  /**
   * Returns true if the lower bits of this object match the passed
   * object or vice versa if the passed object is longer.
   *
   * \todo Doesn't deal with the passed object being longer
   */ 
  bool Match(const std::bitset<SIZE>& target) const {
    for(int i=0;i<SIZE;i++) {
      if (target[i] != *this[i]) {
	return false;
      }
    }
    return true;
  }
}

#endif//PAYLOAD_GUARD
