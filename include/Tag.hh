/**
 * $Header$
 *
 * $Log$
 * Revision 1.5  2004/02/16 08:02:02  acr31
 * *** empty log message ***
 *
 * Revision 1.4  2004/02/08 20:30:10  acr31
 * changes to interfaces to add the ImageFilter class
 *
 * Revision 1.3  2004/02/03 16:24:56  acr31
 * various function signature changes and use of __FILE__ and __LINE__ in debug macros
 *
 * Revision 1.2  2004/02/01 14:25:33  acr31
 * moved Rectangle2D to QuadTangle2D and refactored implementations around
 * the place
 *
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
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
#include <Camera.hh>

template<class C>
class Tag {
public:
  virtual void Draw2D(Image *image, unsigned long long code, int black, int white)= 0;
  virtual unsigned long long Decode(Image *image, Camera* camera, const C *location) =0;
};

#endif//TAG_GUARD
