/**
 * Galois Field(4) coder.  
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
#ifndef GF4_CODER_GUARD
#define GF4_CODER_GUARD

#include "Config.hh"
#include "Coder.hh"
#include "GF4Poly.hh"
#include <cmath>

#undef FILENAME
#define FILENAME "GF4Coder.hh"

class GF4Coder : Coder {
private:
  int m_symbol_range;
  int m_symbol_count;
  unsigned long m_accum_code;
  int m_current_chunk;
  int m_base;
  
  GF4Poly m_genpoly;
  GF4Poly m_encoded;

  int m_maxerrors;

public:
  GF4Coder(int symbol_range,int symbol_count);
  
  virtual void Set(unsigned long value);
  virtual unsigned int NextChunk();
  virtual void Reset();
  virtual bool LoadChunk(unsigned int chunk);
  virtual unsigned long Decode();
};

#endif//GF4_CODER_GUARD