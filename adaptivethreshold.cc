#include "adaptivethreshold.hh"
#include "cv.h"

#undef FILENAME
#define FILENAME "adaptivethreshold.cc"

void AdaptiveThreshold(Image *image) {
  int window_size = image->width/8;
  window_size+= 1-(window_size %2); /* window size must be an odd number */
  PROGRESS("Thresholding image with window size "<<window_size);
  cvAdaptiveThreshold(image,image,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,window_size,40);
}
