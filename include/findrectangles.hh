/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/01/31 16:48:10  acr31
 * moved some arguments to #defines
 *
 * Revision 1.2  2004/01/26 12:04:22  acr31
 * noted that the routines search for white rather than black objects
 *
 * Revision 1.1  2004/01/25 14:54:38  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.1  2004/01/23 15:42:30  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#ifndef FIND_RECTANGLES_GUARD
#define FIND_RECTANGLES_GUARD

#include "Config.hh"
#include "Rectangle2D.hh"
#include "Drawing.hh"
#include <vector>

/**
 * Find rectangular white regions - if you are looking for black tags you must invert the image
 */
void FindRectangles(Image *image,std::vector<Rectangle2DChain*> *results);

static inline bool compare(Rectangle2D *e1, Rectangle2D *e2);

#endif//FIND_RECTANGLES_GUARD
