/**
 * $Header$
 */
#ifndef IMAGE_SOURCE_GUARD
#define IMAGE_SOURCE_GUARD

#include <total/Config.hh>
#include <total/Image.hh>

namespace Total {
  /**
   * Encapsulate a source of input images
   */
  class ImageSource {
  public:
    /**
     * Get the next frame.  This method may or may not invalidate the
     * current pointer to the buffer.
     */
    virtual Image* Next() = 0;
  };

}
#endif//IMAGE_SOURCE_GUARD
