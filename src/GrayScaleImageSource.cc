/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/29 17:34:20  acr31
 * another image source
 *
 *
 */
#include <GrayScaleImageSource.hh>

#include <opencv/cv.h>
#include <opencv/highgui.h>


#undef FILENAME
#define FILENAME "GrayScaleImageSource.cc"

GrayScaleImageSource::GrayScaleImageSource(Image *image) : m_buffer(NULL) {
  if (image->nChannels==3) {
    PROGRESS("Image is RGB. Converting to greyscale");
    m_original = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U,1);
    cvCvtColor(image,m_original,CV_RGB2GRAY);
  }
  else {
    PROGRESS("Leaving image in original format");
    m_original = cvCloneImage(image);
  }    
}

GrayScaleImageSource::~GrayScaleImageSource() {
  cvReleaseImage(&m_original);
  if (m_buffer != NULL) {
    cvReleaseImage(&m_buffer);
  }
}

void GrayScaleImageSource::Next() {
  PROGRESS("Cloning original image into buffer");
  cvCopy(m_original,m_buffer);
}

Image* GrayScaleImageSource::GetBuffer() {
  if (m_buffer == NULL) {
    PROGRESS("Creating image buffer");
    m_buffer = cvCreateImage(cvSize(m_original->width,m_original->height),IPL_DEPTH_8U,1);
  }
  return m_buffer;
}

