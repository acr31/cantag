/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/02/06 21:11:44  acr31
 * adding ellipse fitting
 *
 * Revision 1.1  2004/01/25 14:53:35  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.2  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#include <adaptivethreshold.hh>
#include <opencv/cv.h>

#undef FILENAME
#define FILENAME "adaptivethreshold.cc"

void AdaptiveThreshold(Image *image) {
  int window_size = image->width/8;
  window_size+= 1-(window_size %2); /* window size must be an odd number */
  PROGRESS("Thresholding image with window size "<<window_size);
  cvAdaptiveThreshold(image,image,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,window_size,40);
}
