#include <Image.hh>

Image::Image(int width, int height) : m_image(cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1)) {
  cvConvertScale(m_image,m_image,0,255);
};
Image::Image(const Image& c) : m_image(cvCloneImage(c.m_image)) {};
Image::Image(char* filename) : m_image(cvLoadImage(filename)) {};

Image::~Image() {
  cvReleaseImage(&m_image);
}

void Image::GlobalThreshold(unsigned char threshold) {
  cvThreshold(m_image,m_image,threshold,255,CV_THRESH_BINARY_INV);
#ifdef IMAGE_DEBUG
  cvSaveImage("debug-globalthreshold.jpg",m_image);
#endif
}

void Image::AdaptiveThreshold(unsigned int window_size, unsigned char offset) {
  assert(window_size%2==1); // window size must be an odd number
  cvAdaptiveThreshold(m_image,m_image,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,window_size,offset);
#ifdef IMAGE_DEBUG
  cvSaveImage("debug-adaptivethreshold.jpg",m_image);
#endif
}

void Image::HomogenousTransform() {
  // take the log of each pixel in the image
  for(int i=0;i<m_image->height;i++) {
    for(int j=0;j<m_image->width;j++) {
      uchar* ptr = (uchar*)(m_image->imageData + i * m_image->widthStep + j);
      double result = 45.985904 * log((double)*ptr + 1);
      *ptr = (uchar)result;
    }
  }
  // and apply a small kernel edge detector
  cvCanny(m_image,m_image,128,128,3);
#ifdef IMAGE_DEBUG
  cvSaveImage("debug-homogenoustransform.jpg",m_image);
#endif
}
