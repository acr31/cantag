/**
 * Galois Field(4) coder.  
 *
 * Contains code by Ian Caulkin
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/22 12:02:10  acr31
 * added parts of Ian Caulkins GF4 coding.  Fixed a bug in the weight function (should be +=2 not ++) Need to finish off GF4Coder so it actually does something
 *
 *
 */

#include "GF4Coder.hh"

/**
 * Initialise genpoly to 404429. This is
 *
 * X^9+x*X^8+x*X^6+x*X^5+(x+1)X^4+(x+1)X^3+(x+1)X^1+1
 */
GF4Coder::GF4Coder(int symbol_range,  int symbol_count) : m_symbol_range(symbol_range), m_symbol_count(0), m_genpoly(404429) {
  assert(symbol_range == 4);
  assert(symbol_count < 21);
}

void GF4Coder::Set(unsigned long value) {
  m_encoded = GF4Poly(value)*genpoly;  
}

unsigned int NextChunk() {
  return 0;
}

void Reset() {};
bool LoadChunk(unsigned int chunk) { return false; }
unsigned long Decode() const { return 0; }
