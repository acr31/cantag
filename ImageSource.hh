#ifndef IMAGE_SOURCE_GUARD
#define IMAGE_SOURCE_GUARD

#include "Drawing.hh"

class ImageSource {

public:
  virtual void Next() = 0;
  virtual Image* GetBuffer() = 0;
};

#endif//IMAGE_SOURCE_GUARD
