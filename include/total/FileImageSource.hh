/**
 * $Header$
 *
 */
#ifndef FILE_IMAGE_SOURCE
#define FILE_IMAGE_SOURCE

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/ImageSource.hh>

namespace Total {
  /**
   * An image source that provides an image loaded from disk
   */
  class FileImageSource : public ImageSource {
  private:
    Image* m_original;
    Image* m_buffer;

  public:
    /**
     * Create the image source, displaying the file given (jpg or bmp)
     */ 
    FileImageSource(char* filename);
    virtual ~FileImageSource();
    virtual Image* Next();
  };
}
#endif//FILE_IMAGE_SOURCE
