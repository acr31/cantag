#include "GrayScaleFileImageSource.hh"

#include <cv.h>
#include <highgui.h>


#undef FILENAME
#define FILENAME "GrayScaleFileImageSource.cc"

GrayScaleFileImageSource::GrayScaleFileImageSource(char *fileName) : m_buffer(NULL) {
  PROGRESS("Loading image "<<fileName);
  Image* image = cvLoadImage(fileName);
  if (image->nChannels==3) {
    PROGRESS("Image is RGB. Converting to greyscale");
    m_original = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U,1);
    cvCvtColor(image,m_original,CV_RGB2GRAY);
    cvReleaseImage(&image);
  }
  else {
    PROGRESS("Leaving image in original format");
    m_original = image;
  }    
}

GrayScaleFileImageSource::~GrayScaleFileImageSource() {
  cvReleaseImage(&m_original);
  if (m_buffer != NULL) {
    cvReleaseImage(&m_buffer);
  }
}

void GrayScaleFileImageSource::Next() {
  PROGRESS("Cloning original image into buffer");
  cvCopy(m_original,m_buffer);
}

Image* GrayScaleFileImageSource::GetBuffer() {
  if (m_buffer == NULL) {
    PROGRESS("Creating image buffer");
    m_buffer = cvCreateImage(cvSize(m_original->width,m_original->height),IPL_DEPTH_8U,1);
  }
  return m_buffer;
}

