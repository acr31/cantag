/**
 * $Header$
 *
 * $Log$
 * Revision 1.11  2004/01/23 22:37:02  acr31
 * made codes an unsigned long long
 *
 * Revision 1.10  2004/01/23 16:03:52  acr31
 * moved CircularTag back to Tag - but its now templated on the type of tag - ellipse or rectangle
 *
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
#ifndef TAG_GUARD
#define TAG_GUARD

#include "Config.hh"
#include "Drawing.hh"

template<class C>
class Tag {
public:
  virtual void Draw2D(Image *image, const C *location, unsigned long long code, int black, int white)= 0;
  virtual unsigned long long Decode(Image *image, const C *location) =0;
};

#endif//TAG_GUARD
