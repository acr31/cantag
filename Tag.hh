#ifndef TAG_GUARD
#define TAG_GUARD

#include <cv.h>

class Tag {
public:
  char* identifier;
  unsigned int code;
  CvBox2D box;
};


#endif//TAG_GUARD
