/**
 * $Header$
 *
 * $Log$
 * Revision 1.6  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#ifndef TAG_GUARD
#define TAG_GUARD

#include <iostream>

#include "Config.hh"
#include "Drawing.hh"
#include "Location2D.hh"

class Tag {
public:
  virtual void Draw2D(Image *image, Location2D *location, unsigned long code, int black, int white)= 0;
  virtual unsigned long Decode(Image *image, Location2D *location) =0;
};

#endif//TAG_GUARD
