#ifndef CONCENTRIC_ELLIPSE_GUARD
#define CONCENTRIC_ELLIPSE_GUARD

#include <vector>
#include <cv.h>

int FindConcentricEllipses(CvSeq *contourTree, 
			   std::vector<CvBox2D> *result, 
			   int maxDepth=10,
			   double maxXDiff = 6,
			   double maxYDiff = 6,
			   double maxRatioDiff = 0.1,
			   double maxFitError = 0.01);

#endif//CONCENTRIC_ELLIPSE_GUARD
