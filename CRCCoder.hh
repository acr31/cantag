/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/23 18:18:11  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 *
 */

#ifndef CRC_CODER_GUARD
#define CRC_CODER_GUARD

#include "Config.hh"
#include "Coder.hh"


#undef FILENAME
#define FILENAME "CRCCoder.hh"

class CRCCoder : Coder {
private:
  int m_symbol_range;
  int m_symbol_count;
  unsigned long m_encoded;
  int m_counter;

public:
  CRCCoder(int symbol_range,int symbol_count);
  
  virtual void Set(unsigned long value);
  virtual unsigned int NextChunk();
  virtual void Reset();
  virtual bool LoadChunk(unsigned int chunk);
  virtual unsigned long Decode();
};


#endif//CRC_CODER_GUARD
