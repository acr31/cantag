/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#ifndef POSEFROMCIRCLE_GUARD
#define POSEFROMCIRCLE_GUARD

#include <cmath>
#include <cv.h>

void PoseFromCircle(const CvBox2D &ellipse, double radius, CvPoint3D32f* unit_normal, CvPoint3D32f* centre);


#endif//POSEFROMCIRCLE_GUARD
