/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#include "ellipsetoxy.hh"
#include <cmath>

#undef FILENAME
#define FILENAME "ellipsetoxy.cc"

void EllipseToXY(float ellipse_centre_x, float ellipse_centre_y, float ellipse_width, float ellipse_height, float ellipse_angle, float sample_angle, float sample_dist, int *rx, int *ry) {
  float x = sample_dist*cos(sample_angle)*ellipse_width*0.5;
  float y = sample_dist*sin(sample_angle)*ellipse_height*0.5;
  float cost = cos(ellipse_angle);
  float sint = sin(ellipse_angle);
  float ix = x*cost-y*sint + ellipse_centre_x;
  float iy = x*sint+y*cost + ellipse_centre_y;
  *rx = (int)ix;
  *ry = (int)iy;
  PROGRESS("Ellipse centre ("<<ellipse_centre_x<<","<<ellipse_centre_y<<"), size ("<<ellipse_width<<","<<ellipse_height<<"), angle "<<ellipse_angle<<". Sampling angle "<<sample_angle<<" distance "<<sample_dist<<". Gives co-ordinates ("<<*rx<<","<<*ry<<")");
}
