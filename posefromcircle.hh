/**
 * $Header$
 *
 * $Log$
 * Revision 1.3  2004/01/21 13:41:37  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#ifndef POSEFROMCIRCLE_GUARD
#define POSEFROMCIRCLE_GUARD

#include <cmath>

#include "Config.hh"
#include "Location2D.hh"
#include "Location3D.hh"

Location3D* PoseFromCircle(const Location2D *l, double radius);

#endif//POSEFROMCIRCLE_GUARD
