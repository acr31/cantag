#ifndef IDENTIFY_TAG_GUARD
#define IDENTIFY_TAG_GUARD

#include <cv.h>
#include <vector>
#include <cmath>

#include "Tag.hh"

bool identifyTag(IplImage *image, CvBox2D *ellipse, std::vector<Tag> *result);

#endif//IDENTIFY_TAG_GUARD
