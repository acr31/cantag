/**
 * Galois Field(4) coder.  
 *
 * Contains code by Ian Caulkin
 *
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/01/23 11:49:53  acr31
 * Finished integrating the GF4 coder - I've ripped out the error correcting stuff for now and its just looking for a match but it doesn't seem to work.  I need to spend some time to understand the division and mod operations in the GF4Poly to progress
 *
 * Revision 1.2  2004/01/23 09:08:40  acr31
 * More work integrating the GF4 stuff with tripover
 *
 * Revision 1.1  2004/01/22 12:02:10  acr31
 * added parts of Ian Caulkins GF4 coding.  Fixed a bug in the weight function (should be +=2 not ++) Need to finish off GF4Coder so it actually does something
 *
 *
 */

#include "GF4Coder.hh"

#undef FILENAME
#define FILENAME "GF4Coder.cc"

/**
 * Initialise genpoly to 404429. This is
 *
 * X^9+x*X^8+x*X^6+x*X^5+(x+1)X^4+(x+1)X^3+(x+1)X^1+1
 */
GF4Coder::GF4Coder(int symbol_range,  int symbol_count) :
  m_symbol_range(symbol_range),
  m_symbol_count(symbol_count), 
  m_genpoly(404429), 
  m_encoded(0), 
  m_maxerrors(0),
  m_current_chunk(0) {
  assert(symbol_range == 4);
  assert(symbol_count < 21);
}

void GF4Coder::Set(unsigned long value) {
  PROGRESS("Encoder set with value "<<value);
  m_encoded = GF4Poly(value)*m_genpoly;
  m_current_chunk = 0;
  PROGRESS("Encoded value is "<<m_encoded.GetValue());

}

unsigned int GF4Coder::NextChunk() {
  unsigned int chunk = m_encoded.GetTerm(m_symbol_count-1-(m_current_chunk++));
  PROGRESS("NextChunk returns "<<chunk);
  return chunk;
}

void GF4Coder::Reset() {
  PROGRESS("Decoder Reset");
  m_encoded = GF4Poly(0);
};

bool GF4Coder::LoadChunk(unsigned int chunk) { 
  PROGRESS("LoadChunk value "<<chunk);
  m_encoded <<=1;
  if (chunk&~3) {
    PROGRESS("Chunk out of range - throwing invalid symbol");
    throw Coder::InvalidSymbol();
  }
  m_encoded+=chunk;

}

unsigned long GF4Coder::Decode()  { 
  PROGRESS("Encoded value = "<<m_encoded.GetValue());
  for(int i=0;i<m_symbol_count;i++) {
    GF4Poly syndrome = m_encoded % m_genpoly;
    PROGRESS("Syndrome is " << syndrome.GetValue());
    if (syndrome == (unsigned long long int)0) {
      PROGRESS("Syndrome is 0 - we have a match");
      return (m_encoded / m_genpoly).GetValue();
    }
    m_encoded.Rotate(1,m_symbol_count);
  }
  PROGRESS("We failed to find a valid orientation for this code - throwing InvalidCode.");
  throw InvalidCode();
}

