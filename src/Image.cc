#include <Image.hh>
#include <iostream>

#include <boost/random.hpp>

Image::Image() : m_from_header(false), m_image(NULL) {};


Image::Image(int width, int height) : m_from_header(false), m_image(cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1)) {
  cvConvertScale(m_image,m_image,0,255);
};
Image::Image(const Image& c) : m_from_header(false), m_image(cvCloneImage(c.m_image)) {};
Image::Image(char* filename) : m_from_header(false), m_image(cvLoadImage(filename)) {
  if (m_image->nChannels == 3) {
    IplImage* image2 = cvCreateImage(cvSize(m_image->width,m_image->height),IPL_DEPTH_8U,1);
    cvCvtColor(m_image,image2,CV_RGB2GRAY);
    cvReleaseImage(&m_image);
    m_image = image2;
  }  
};

Image::Image(int width,int height, uchar* contents) : m_from_header(true) {
  m_image = cvCreateImageHeader(cvSize(width,height),
				IPL_DEPTH_8U, 1);
  m_image->imageData = m_image->imageDataOrigin = (char*)contents;
}

Image::~Image() {
  if (m_from_header) {
    /*
     * \todo work out why this results in unknown error code -49 in opencv
     */
    //    cvReleaseImageHeader(&m_image);
  }
  else {
    if (m_image != NULL) {
      cvReleaseImage(&m_image);
    }
  }
}

void Image::GlobalThreshold(unsigned char threshold) {
  cvThreshold(m_image,m_image,threshold,255,CV_THRESH_BINARY_INV);
#ifdef IMAGE_DEBUG
  cvSaveImage("debug-globalthreshold.jpg",m_image);
#endif
}

/**
 * An implementation of Pierre Wellner's Adaptive Thresholding
 *  @TechReport{t:europarc93:wellner,
 *     author       = "Pierre Wellner",
 *     title        = "Adaptive Thresholding for the {D}igital{D}esk",
 *     institution  = "EuroPARC",
 *     year         = "1993",
 *     number       = "EPC-93-110",
 *     comment      = "Nice introduction to global and adaptive thresholding.  Presents an efficient and effective adaptive thresholding technique and demonstrates on lots of example images.",
 *     file         = "ar/ddesk-threshold.pdf"
 *   }
 *
 *
 * \todo Doesnt work properly on noisy images
 */

void Image::AdaptiveThreshold(unsigned int window_size, unsigned char offset) {
  int moving_average = 127;
  int previous_line[m_image->width];
  for(unsigned int i=0;i<m_image->height-1;i+=2) {
    for(unsigned int j=0;j<m_image->width;j++) {
      moving_average = AdaptiveWidthStep(moving_average,previous_line,i,j,window_size,offset);
    }
    for(int j=m_image->width-1;j>=0;j--) {
      moving_average = AdaptiveWidthStep(moving_average,previous_line,i+1,j,window_size,offset);
    }
  }  
#ifdef IMAGE_DEBUG
  cvSaveImage("debug-adaptivethreshold.jpg",m_image);
#endif
}

/*
void Image::AdaptiveThreshold(unsigned int window_size, unsigned char offset) {
  assert(window_size%2==1);
  cvAdaptiveThreshold(m_image,m_image,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,window_size,offset);
}
*/
inline int Image::AdaptiveWidthStep(int moving_average, int* previous_line, unsigned int i, unsigned int j, unsigned int s, int offset) {
  unsigned char pixel = SampleNoCheck(j,i);
  //  moving_average = pixel + moving_average - moving_average/s;
  moving_average = (pixel + (moving_average << s) - moving_average) >> s;
  int current_thresh = (moving_average + (i==0 ? 0 : previous_line[j]))/2;
  //int current_thresh = moving_average;
  previous_line[j] = moving_average;
  //  if (pixel*s*255 < current_thresh*(255-offset)) {
  if (pixel*255 < current_thresh*(255-offset)) {
    DrawPixelNoCheck(j,i,COLOUR_WHITE);
  }
  else {
    DrawPixelNoCheck(j,i,COLOUR_BLACK);
  }
  return moving_average;
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


/**
 * Add noise to the image in an attempt to simulate that of a real
 * camera.
 */
void Image::AddNoise(float mean, float stddev) {
  assert(mean >= 0 && mean < 256);
  assert(stddev >= 0);
  // the first source of noise is dark noise and low level CCD noise sources
  // this manifests itself as gaussian noise with some non-zero mean

  // the second source of noise is shot noise

  boost::normal_distribution<float> normal_dist(mean,stddev);
  boost::rand48 rand_generator((unsigned long long)time(0));
  boost::variate_generator<boost::rand48&, boost::normal_distribution<float> > normal(rand_generator,normal_dist);
  for(int i=0;i<m_image->height;i++) {
    for(int j=0;j<m_image->width;j++) {
      float randomval = normal();
      int sample = ((uchar*)(m_image->imageData + i * m_image->widthStep))[j];
      sample += cvRound(normal());
      if (sample < 0) { sample = 0; }
      else if (sample > 255) { sample = 255; }
      ((uchar*)(m_image->imageData + i * m_image->widthStep))[j] = sample;
    }
  }
}
