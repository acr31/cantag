/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.2  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#ifndef IMAGE_SOURCE_GUARD
#define IMAGE_SOURCE_GUARD

#include "Drawing.hh"

class ImageSource {

public:
  virtual void Next() = 0;
  virtual Image* GetBuffer() = 0;
};

#endif//IMAGE_SOURCE_GUARD
