#include <GlobalThreshold.hh>
#include <cv/opencv.h>

GlobalThreshold::GlobalThreshold(int threshold=128): m_image(NULL), m_threshold(threshold) {};

GlobalThreshold::~GlobalThreshold() {};

Image* GlobalThreshold::LoadImage(Image* image) {
  m_image=image;
  cvThreshold(image,image,m_threshold,255,CV_THRESH_BINARY);
  return image;
}

Image* GlobalThreshold::BinarizeRegion(int x, int y, int width , int height) {
  return m_image;
}
