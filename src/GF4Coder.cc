/**
 * Galois Field(4) coder.  
 *
 * Contains code by Ian Caulkin
 *
 * $Header$
 *
 * $Log$
 * Revision 1.3  2005/02/03 11:21:30  acr31
 * moved include files into tripover subdirectory
 *
 * Revision 1.2  2004/01/30 16:54:27  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.1  2004/01/25 14:53:35  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.4  2004/01/23 22:35:04  acr31
 * changed coder to use unsigned long long
 *
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

#include <tripover/GF4Coder.hh>

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

unsigned long long GF4Coder::Encode(unsigned long long value) {
  PROGRESS("Encoder set with value "<<value);
  m_encoded = GF4Poly(value)*m_genpoly;
  PROGRESS("Encoded value is "<<m_encoded.GetValue());
  return m_encoded.GetValue();

}

unsigned long long GF4Coder::Decode(unsigned long long value)  { 
  GF4Poly enc(value);
  PROGRESS("Encoded value = "<<enc.GetValue());
  for(int i=0;i<m_symbol_count;i++) {
    GF4Poly syndrome = enc % m_genpoly;
    PROGRESS("Syndrome is " << syndrome.GetValue());
    if (syndrome == (unsigned long long int)0) {
      PROGRESS("Syndrome is 0 - we have a match");
      return (enc / m_genpoly).GetValue();
    }
    enc.Rotate(1,m_symbol_count);
  }
  PROGRESS("We failed to find a valid orientation for this code - throwing InvalidCode.");
  throw InvalidCode();
}

