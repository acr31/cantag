/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:54:36  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.2  2004/01/21 11:55:07  acr31
 * added keywords for substitution
 *
 */
#ifndef EXCEPTION_GUARD
#define EXCEPTION_GUARD

#include "Config.hh"
#include <iostream>

class Exception {
public:
  virtual std::ostream& Print(std::ostream& s) const = 0;  
};

std::ostream& operator<<(std::ostream& s, const Exception& z);

#endif//EXCEPTION_GUARD
