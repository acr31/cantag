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
