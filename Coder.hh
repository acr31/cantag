/**
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/01/23 11:49:53  acr31
 * Finished integrating the GF4 coder - I've ripped out the error correcting stuff for now and its just looking for a match but it doesn't seem to work.  I need to spend some time to understand the division and mod operations in the GF4Poly to progress
 *
 * Revision 1.3  2004/01/21 11:55:07  acr31
 * added keywords for substitution
 *
 */
#ifndef CODER_GUARD
#define CODER_GUARD

#include <iostream>
#include "Exception.hh"

class Coder {
public:  
  
  /**
   * Set the encoder to encode this value
   *
   * Throws ValueToLarge if this value it too big to be 
   */
  virtual void Set(unsigned long value) = 0;
  
  /**
   * Read the next chunk from the encoder
   */
  virtual unsigned int NextChunk() = 0;  
  
  class ValueTooLarge : Exception {
  public:
    ValueTooLarge();
    std::ostream& Print(std::ostream& c) const;
  };
    
  /**
   * Load the next chunk of data into the decoder.  Returns true if
   * its valid or false otherwise.  The false data are dropped from
   * the accumulated code but the code is not reset
   *
   * Throws InvalidSymbol exception if the chunk is invalid.
   */
  virtual bool LoadChunk(unsigned int chunk) =0;
  
  /**
   * Decode the loaded value
   *
   * Throws InvalidCheckSum if the code doesn't checksum
   */
  virtual unsigned long Decode()  =0;
  
  /**
   * Reset the accumulated code and start again
   */
  virtual void Reset()=0;  
  
  class InvalidCheckSum: Exception {
  public:
    InvalidCheckSum();
    std::ostream& Print(std::ostream& c) const;
  };
  
  class InvalidSymbol: Exception {
  public:
    InvalidSymbol();
    std::ostream& Print(std::ostream& c) const;
  };

  class InvalidCode :Exception {
  public:
    InvalidCode();
    std::ostream& Print(std::ostream& c) const;
  };
};


#endif//CODER_GUARD
