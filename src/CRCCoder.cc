/**
 * This will be a CRC encoder but for now its just binary with no check
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:53:34  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.2  2004/01/23 22:35:04  acr31
 * changed coder to use unsigned long long
 *
 * Revision 1.1  2004/01/23 18:18:11  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 */

#include <CRCCoder.hh>

#undef FILENAME
#define FILENAME "CRCCoder.cc"


CRCCoder::CRCCoder(int symbol_range,  int symbol_count) : 
  m_symbol_range(symbol_range),
  m_symbol_count(symbol_count),
  m_encoded(0),
  m_counter(0)
{
  assert(symbol_range >=2 );
}

void CRCCoder::Set(unsigned long long value) {
  if (value > pow(m_symbol_range,m_symbol_count) -1) {
    throw ValueTooLarge();
  }
  m_encoded = value;
  PROGRESS("Encoded value is "<<m_encoded);

}

unsigned int CRCCoder::NextChunk() {
  unsigned int result = m_encoded % m_symbol_range;
  m_encoded /= m_symbol_range;

  PROGRESS("NextChunk returns "<<result);
  return result;
}

void CRCCoder::Reset() {
  PROGRESS("Decoder Reset");
  m_encoded = 0;
  m_counter = 0;
};

bool CRCCoder::LoadChunk(unsigned int chunk) { 
  PROGRESS("LoadChunk value "<<chunk);
  m_encoded+=(unsigned long)(pow(m_symbol_range,m_symbol_count -1 - m_counter++)*chunk);
}

unsigned long long CRCCoder::Decode()  { 
  PROGRESS("Encoded value = "<<m_encoded);
  return m_encoded;
}
