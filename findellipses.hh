/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#ifndef FIND_ELLIPSES_GUARD
#define FIND_ELLIPSES_GUARD

#include "Config.hh"
#include "Location2D.hh"
#include "Drawing.hh"
#include <vector>

struct Location2DChain {
  Location2D* current;
  Location2DChain* next;
};

void FindEllipses(Image *image, int maxDepth, int maxLength, float  maxXDiff, float maxYDiff, float maxRatioDiff, float maxFitError, std::vector<Location2DChain*> *results);

static inline bool compare(Location2D *e1, Location2D *e2, float maxXDiff, float maxYDiff, float maxRatioDiff);
static inline bool calcerror(CvBox2D *ellipse, CvPoint2D32f *points, int count, float maxFixError);

#endif//FIND_ELLIPSES_GUARD
