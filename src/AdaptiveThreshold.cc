#include <AdaptiveThreshold.hh>

AdaptiveThreshold::AdaptiveThreshold(int window_size=0, int offset=40): m_image(NULL), m_window_size(window_size), m_offset(offset) {};

AdaptiveThreshold::~AdaptiveThreshold() {};

Image* AdaptiveThreshold::LoadImage(Image* image) {
  m_image=image;
  int window_size;
  if (m_window_size == 0) {
    window_size = image->width/8;
    window_size+= 1-(window_size %2); /* window size must be an odd number */
  }
  else {
    window_size = m_window_size;
  }
  PROGRESS("Thresholding image with window size "<<window_size);
  cvAdaptiveThreshold(image,image,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,window_size,m_offset);
  return image;
}

Image* AdaptiveThreshold::BinarizeRegion(int x, int y, int width , int height) {
  return m_image;
}
