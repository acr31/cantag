/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#ifndef SAMPLE_ELLIPSE_GUARD
#define SAMPLE_ELLIPSE_GUARD

#include "Config.hh"
#include "Drawing.hh"

#undef FILENAME
#define FILENAME "ellipsetoxy.hh"

void EllipseToXY(float ellipse_centre_x, float ellipse_centre_y, float ellipse_width, float ellipse_height, float ellipse_angle, float sample_angle, float sample_dist, int* resx, int* resyk);

#endif//SAMPLE_ELLIPSE_GUARD