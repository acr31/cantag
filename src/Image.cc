#include <Image.hh>
#include <iostream>

#ifdef HAVE_BOOST_RANDOM
# include <boost/random.hpp>
#endif

Image::Image() : m_from_header(false), m_free_contents(false), m_image(NULL) {};


Image::Image(int width, int height) : m_from_header(false), m_free_contents(false), m_image(cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1)) {
  cvConvertScale(m_image,m_image,0,255);
  m_contents = (uchar*)m_image->imageData;
};
Image::Image(const Image& c) : m_from_header(false), m_free_contents(false), m_image(cvCloneImage(c.m_image)) {
  m_contents = (uchar*)m_image->imageData;
};
Image::Image(char* filename) : m_from_header(false), m_free_contents(false), m_image(cvLoadImage(filename)) {
  if (m_image->nChannels == 3) {
    IplImage* image2 = cvCreateImage(cvSize(m_image->width,m_image->height),IPL_DEPTH_8U,1);
    cvCvtColor(m_image,image2,CV_RGB2GRAY);
    cvReleaseImage(&m_image);
    m_image = image2;
  }  
  m_contents = (uchar*)m_image->imageData;
};

Image::Image(int width,int height, uchar* contents) : m_from_header(true),m_free_contents(false) {
  m_image = cvCreateImageHeader(cvSize(width,height),
				IPL_DEPTH_8U, 1);
  m_image->imageData = m_image->imageDataOrigin = (char*)contents;
  m_contents = contents;
}

Image::~Image() {
  if (m_image != NULL) {
    if (m_from_header) {
      /*
       * \todo work out why this results in unknown error code -49 in opencv
       */
      //      cvSetImageROI(m_image,cvRect(0,0,0,0));
      //cvReleaseImageHeader(&m_image);
      
    }
    else {
      cvReleaseImage(&m_image);
    }    
  }
  
  if (m_free_contents) {
    delete[] m_contents;
  }
}

void Image::GlobalThreshold(unsigned char threshold) {
  cvThreshold(m_image,m_image,threshold,1,CV_THRESH_BINARY_INV);
#ifdef IMAGE_DEBUG
  cvSaveImage("debug-globalthreshold.bmp",m_image);
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
 * Adapted to use a more efficient calculation for the moving
 * average. the window used is now 2^window_size
 *
 * \todo Doesnt work properly on noisy images
 */

void Image::AdaptiveThreshold(unsigned int window_size, unsigned char offset) {
  int moving_average = 127;
  int previous_line[m_image->width];
  for(int i=0;i<m_image->width;i++) {
    previous_line[i] = 127;
  }  
  for(int i=0;i<m_image->height-1;) { // use height-1 so we dont overrun the image if its height is an odd number
    for(int j=0;j<m_image->width;j++) {
      unsigned char pixel = SampleNoCheck(j,i);
      //      moving_average = (pixel + (moving_average << window_size) - moving_average) >> window_size;
      moving_average = pixel + moving_average - moving_average/(1<<window_size);
      int current_thresh = (moving_average + previous_line[j])/2;
      previous_line[j] = moving_average;
      //      if (pixel*255 < current_thresh*(255-offset)) {
      if (pixel*(1<<window_size)*255 < current_thresh*(255-offset)) {
	DrawPixelNoCheck(j,i,1);
      }
      else {
	DrawPixelNoCheck(j,i,0);
      }
    }

    i++;

    for(int j=m_image->width-1;j>=0;j--) {
     unsigned char pixel = SampleNoCheck(j,i);
     //      moving_average = (pixel + (moving_average << window_size) - moving_average) >> window_size;
     moving_average = pixel + moving_average - moving_average/(1<<window_size);
     int current_thresh = (moving_average + previous_line[j])/2;
     previous_line[j] = moving_average;
     //     if (pixel*255 < current_thresh*(255-offset)) {
     if (pixel*(1<<window_size)*255 < current_thresh*(255-offset)) {
       DrawPixelNoCheck(j,i,1);
      }
      else {
	DrawPixelNoCheck(j,i,0);
      }
    }

    i++;

  }  

#ifdef IMAGE_DEBUG
  cvSaveImage("debug-adaptivethreshold.bmp",m_image);
#endif
}

/*
void Image::AdaptiveThreshold(unsigned int window_size, unsigned char offset) {
  assert(window_size%2==1);
  cvAdaptiveThreshold(m_image,m_image,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,window_size,offset);
}
*/
inline int Image::AdaptiveWidthStep(int moving_average,  // the current average
				    int* previous_line,  // a pointer to an array of size image_width for the averages of the previous line
				    unsigned int i,      // the y co-ordinate
				    unsigned int j,      // the x co-ordinate
				    unsigned int s,      // the window size is 2^s
				    int offset) {          // the offset
  unsigned char pixel = SampleNoCheck(j,i);
  moving_average = (pixel + (moving_average << s) - moving_average) >> s;
  int current_thresh = (moving_average + previous_line[j])/2;
  previous_line[j] = moving_average;
  if (abs(pixel - current_thresh) < offset) {
  }

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

#ifdef HAVE_BOOST_RANDOM
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
#else 
void Image::AddNoise(float mean, float stddev) {
  
}
#endif

#define STEPSIZE 0.001f
void Image::ellipse_polygon_approx(CvPoint* points, int startindex, int length, float xc, float yc, float width, float height,  float angle_radians, int color, int thickness, float start_angle) {
  /**
   * The parametric equation for an ellipse
   *
   * x = xc + a*cos(angle_radians)*cos(t) + b*sin(angle_radians)*sin(t)
   * y = yc - a*sin(angle_radians)*cos(t) + b*cos(angle_radians)*sin(t)
   *
   * a = width/2; b=height/2
   * angle_radians is the angle between the axis given by width and the horizontal
   *
   */

  /** 
   * Plan: create a polygon approximation to the ellipse and then get
   * opencv to render it for us
   */

  CvPoint* ppoints[1] = {points};

  float cosa = cos(angle_radians);
  float sina = sin(angle_radians);
  float a = width/2;
  float b = height/2;

  float currentAngle = start_angle;
  for(int i=startindex;i<length+startindex;i++) {
    float cost = cos(currentAngle);
    float sint = sin(currentAngle);
    // remember that y increases down from the top of the image so we
    // do yc minus point rather than yc + point
    points[i] = cvPoint( cvRound(xc + a*cosa*cost + b*sina*sint) , 
			 cvRound(yc + a*sina*cost - b*cosa*sint) );
    currentAngle += STEPSIZE;
  }

  int numvertices = startindex+length;
  if (thickness > 0) {
    cvPolyLine(m_image,
	       ppoints,
	       &numvertices,
	       1, // number of contours
	       true, // isClosed
	       color,thickness);
  }
  else {
    cvFillPoly(m_image,
	       ppoints,
	       &numvertices,
	       1, // number of contours
	       color);
  }
}

void Image::DrawEllipseArc(float xc, float yc, 
			   float width, float height, 
			   float angle_radians, 
			   float start_angle, float end_angle, int color, int thickness) {
  assert(thickness > 0 || thickness == -1);

  int numsteps = (int)(((float)(end_angle - start_angle))/STEPSIZE)+1; // add one to get to the edge of the sector
  // add in an extra point (the centre) if we are drawing a slice
  CvPoint points[numsteps + 1]; 
  points[0] = cvPoint(cvRound(xc),cvRound(yc));
  ellipse_polygon_approx(points, 1, numsteps, xc, yc ,width, height, angle_radians, color, thickness, start_angle); 
}

void Image::DrawEllipse(float xc, float yc, 
			float width, float height, 
			float angle_radians, 
			int color, int thickness) {

  assert(thickness > 0 || thickness == -1);

  int numsteps = (int)(2*PI/STEPSIZE); 

  CvPoint points[numsteps]; 
  ellipse_polygon_approx(points,0, numsteps, xc, yc ,width, height, angle_radians, color, thickness, 0);
}

