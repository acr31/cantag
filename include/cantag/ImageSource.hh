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
  template<Colour::Type IMTYPE> class ImageSource {
  public:
      /**
       * Get the next frame.  This method may or may not invalidate the
       * current pointer to the buffer.
       */
      virtual Image<IMTYPE>* Next() = 0;

      virtual int GetWidth() const = 0;
      virtual int GetHeight() const = 0;
  };

}
#endif//IMAGE_SOURCE_GUARD
