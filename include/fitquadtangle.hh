/**
 * Fits a quadtangle to a contour
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/02/11 08:23:49  acr31
 * *** empty log message ***
 *
 */
#ifndef FITQUADTANGLE_GUARD
#define FITQUADTANGLE_GUARD

#include <Config.hh>
#include <QuadTangle2D.hh>

QuadTangle2D fitquadtangle(unsigned int* points, int numpoints);

#endif//FITQUADTANGLE_GUARD
