/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/23 12:05:47  acr31
 * moved Tag to CircularTag in preparation for Squaretag
 *
 * Revision 1.8  2004/01/23 11:57:08  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
 * Revision 1.7  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.6  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#ifndef CIRCULAR_TAG_GUARD
#define CIRCULAR_TAG_GUARD

#include <iostream>

#include "Config.hh"
#include "Drawing.hh"
#include "Ellipse2D.hh"

class CircularTag {
public:
  virtual void Draw2D(Image *image, const Ellipse2D *location, unsigned long code, int black, int white)= 0;
  virtual unsigned long Decode(Image *image, const Ellipse2D *location) =0;
};

#endif//CIRCULAR_TAG_GUARD
