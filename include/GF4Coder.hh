/**
 * Galois Field(4) coder.  
 *
 * $Header$
 *
 * \todo update this for the general coding framework.  Reincorporate ian's error correction and mindecode stuff.
 */
#ifndef GF4_CODER_GUARD
#define GF4_CODER_GUARD

#include "Config.hh"
#include "Coder.hh"
#include "GF4Poly.hh"
#include <cmath>

#undef FILENAME
#define FILENAME "GF4Coder.hh"

class GF4Coder : public Coder {
private:
  int m_symbol_range;
  int m_symbol_count;
  unsigned long long m_accum_code;
  int m_current_chunk;
  int m_base;
  
  GF4Poly m_genpoly;
  GF4Poly m_encoded;

  int m_maxerrors;

public:
  GF4Coder(int symbol_range,int symbol_count);
  
  virtual unsigned long long Encode(unsigned long long value);
  virtual unsigned long long Decode(unsigned long long value);
};

#endif//GF4_CODER_GUARD
