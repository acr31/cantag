/**
 * $Header$
 */
#ifndef IMAGE_SOURCE_GUARD
#define IMAGE_SOURCE_GUARD

#include <Image.hh>

/**
 * Encapsulate a source of input images
 */
class ImageSource {
public:
  /**
   * Overwrite the contents of the image buffer with the next frame
   */
  virtual void Next() = 0;

  /**
   * Get the image buffer for this image source.  Calls to Next() will
   * destructivly alter this buffer so you dont need to keep calling
   * this method.
   */
  virtual Image& GetBuffer() = 0;
};

#endif//IMAGE_SOURCE_GUARD
