#ifndef CONCENTRIC_ELLIPSE_GUARD
#define CONCENTRIC_ELLIPSE_GUARD
#define MAX_FIT_ERROR 0.01
#define MAX_X_OFFSET 4
#define MAX_Y_OFFSET 4
#define MAX_RATIO_DIFF 0.1
#define MAX_ITERATOR_DEPTH 10

#include <cv.h>
#include <vector>
#include <cmath>

void findTags(CvArr *image, std::vector<CvBox2D> *concentrics);

#endif//CONCENTRIC_ELLIPSE_GUARD
