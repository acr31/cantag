/**
 * $Header$
 */


#ifndef IMAGESEGMENTOR_GUARD
#define IMAGESEGMENTOR_GUARD

#include <Image.hh>


class ImageSegmentor {
public:
  int FollowContour(Image& image, int start_x, int start_y, float* point_buffer, int maxcount);


};

#endif//IMAGESEGMENTOR_GUARD
