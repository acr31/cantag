/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/31 16:48:10  acr31
 * moved some arguments to #defines
 *
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.4  2004/01/23 11:57:09  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
 * Revision 1.3  2004/01/21 12:01:41  acr31
 * moved Location2DChain definition to Location2D.hh and added a destructor
 *
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#ifndef FIND_ELLIPSES_GUARD
#define FIND_ELLIPSES_GUARD

#include "Config.hh"
#include "Ellipse2D.hh"
#include "Drawing.hh"
#include <vector>

void FindEllipses(Image *image,  std::vector<Ellipse2DChain*> *results);

static inline bool compare(Ellipse2D *e1, Ellipse2D *e2 );
static inline bool calcerror(CvBox2D *ellipse, CvPoint2D32f *points, int count);

#endif//FIND_ELLIPSES_GUARD
