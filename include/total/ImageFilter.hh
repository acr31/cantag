#ifndef IMAGE_FILTER_GUARD
#define IMAGE_FILTER_GUARD

class ImageFilter {

public:
  /**
   * Load an image and prepare it for contour following. An image
   * suitable for contour following will be returned by the method.
   * Callers of this method lose ownership of the image passed as the
   * parameter and gain ownership of the returned value.
   */
  virtual Image* LoadImage(Image* image) = 0;

  /**
   * Perform a possibly expensive binarization of a portion of the
   * loaded image to allow the tag to be read
   */
  virtual Image* BinarizeRegion(int x, int y, int width , int height) = 0;
  
};

#endif//IMAGE_FILTER_GUARD
