/**
 * Galois Field(4) coder.  
 *
 * Contains code by Ian Caulkin
 *
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/23 09:08:40  acr31
 * More work integrating the GF4 stuff with tripover
 *
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
GF4Coder::GF4Coder(int symbol_range,  int symbol_count) : m_symbol_range(symbol_range), m_symbol_count(0), m_genpoly(404429), m_encoded(0), m_maxerrors(3) {
  assert(symbol_range == 4);
  assert(symbol_count < 21);
}

void GF4Coder::Set(unsigned long value) {
  m_encoded = GF4Poly(value)*m_genpoly;
}

unsigned int GF4Coder::NextChunk() {
  unsigned int chunk = m_encoded.ConstantTerm();
  m_encoded >>= 1;
}

void GF4Coder::Reset() {
  m_encoded = GF4Poly(0);
};

bool GF4Coder::LoadChunk(unsigned int chunk) { 
  m_encoded <<=1;
  if (chunk&~3) {
    throw Coder::InvalidSymbol();
  }
  m_encoded+=chunk;

}

unsigned long GF4Coder::Decode() const { 
  GF4Poly syndrome = m_encoded % m_genpoly;
  GF4Poly error;
  
  unsigned int minweight = m_symbol_count;
  
  if (syndrome != 0) {
    //    cout << "Error: " << code << endl;
    
    for (unsigned int i = 0; i < m_symbol_count; i++) {
      if (syndrome.Weight() < minweight) {
        error = syndrome;
        minweight = error.Weight();
        error.Rotate(m_symbol_count - i, m_symbol_count);
        //      cout << (string) error << endl;
      }
      syndrome.Rotate(1, m_symbol_count);
      syndrome %= m_genpoly;
    }
                                                                               
    //    cout << "e(x) is " << (string) error << endl;
    if (minweight > m_maxerrors)
      throw InvalidCheckSum();
      
  }
                                                                               
  return (m_encoded + error) / m_genpoly;  
}
