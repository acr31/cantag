/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/30 16:54:17  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.1  2004/01/25 14:54:36  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.5  2004/01/23 22:35:04  acr31
 * changed coder to use unsigned long long
 *
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
  virtual unsigned long long  Encode(unsigned long long value) = 0;
  
  class ValueTooLarge : Exception {
  public:
    ValueTooLarge();
    virtual std::ostream& Print(std::ostream& c) const;
  };
    
  /**
   * Decode value
   *
   * Throws InvalidCheckSum if the code doesn't checksum
   * Throws InvalidSymbol if the code contains invalid symbols
   * Throws InvalidCode if the code is invalid for some other reason
   */
  virtual unsigned long long Decode(unsigned long long value)  =0;
    
  class InvalidCheckSum: Exception {
  public:
    InvalidCheckSum();
    virtual std::ostream& Print(std::ostream& c) const;
  };
  
  class InvalidSymbol: Exception {
  public:
    InvalidSymbol();
    virtual std::ostream& Print(std::ostream& c) const;
  };

  class InvalidCode :Exception {
  public:
    InvalidCode();
    virtual std::ostream& Print(std::ostream& c) const;
  };
};


#endif//CODER_GUARD
