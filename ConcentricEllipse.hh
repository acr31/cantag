#ifndef FIND_TAGS_GUARD
#define FIND_TAGS_GUARD

#include <cv.h>
#include <vector>
#include <cmath>

#include "Tag.hh"

void findTags(IplImage *image, IplImage *jam,std::vector<Tag> *concentrics);

#endif//FIND_TAGS_GUARD
