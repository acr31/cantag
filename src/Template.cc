#include <Config.hh>
#include <Template.hh>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstring>

/**
 * Load the image file given from disk.  Subsample it to produce a
 * matrix of greyscale values with the size given.  Subsample is the
 * number of pixels to combine (linearly) into a pixel reading on the
 * target image i.e. the target image will be split into an image of
 * width and height size*subsample and subsample pixels will be
 * combined to give the reading for correlation
 */ 
Template::Template(char* filename, int size=16,int subsample=4) : 
  m_average(0), 
  m_size(size), 
  m_subsample(subsample) {
  m_filename = new char[strlen(filename)];
  strncpy(m_filename,filename,strlen(filename));
  PROGRESS("Filename "<<filename);
  Image *loaded = LoadImage(filename);  

  assert(loaded->width == loaded->height);

  m_original = cvCreateImage(cvSize(loaded->width,loaded->height),IPL_DEPTH_8U,1);
  if (loaded->nChannels==3) {
    cvCvtColor(loaded,m_original,CV_RGB2GRAY);
  }
  else {
    cvCopy(loaded,m_original,NULL);
  }
  cvReleaseImage(&loaded);

  Image* dest = cvCreateImage(cvSize(size,size),IPL_DEPTH_8U,1);
  cvResize(m_original,dest);

  m_values = new unsigned char[size*size];
  for(int i=0;i<size*size;i++) {
    m_values[i] = *((uchar*)(dest->imageData + i));
  }
  cvReleaseImage(&dest);

  calculate_mean_sigma(m_values,&m_sigma,&m_average);

}

bool Template::operator<(const Template& t) {
  return strcmp(m_filename,t.m_filename);
}

Template::~Template() {
  delete[] m_values;
  cvReleaseImage(&m_original);
}

/**
 * Project each one of our points onto the quadtangle in the image to
 * sample the image value for that point.  Then calculate the
 * correlation coefficient.  We need to try it against all four
 * orientations of the template.
 */
float Template::Correlate(Image* image, const QuadTangle2D* q) const {
  float scalefactor = 1/(float)m_size;
  float subscalefactor = 1/(float)m_size/(float)m_subsample;
  
  uchar readvalues[m_size*m_size];

  for(int i=0;i<m_size;i++) {
    for(int j=0;j<m_size;j++) {
      float currentvalue  = 0;
      for(int k=0;k<m_subsample;k++) {
	for(int l=0;l<m_subsample;l++) {
	  float projX;
	  float projY;
	  q->ProjectPoint(i*scalefactor+k*subscalefactor,
			  j*scalefactor+l*subscalefactor,
			  &projX,
			  &projY);

	  currentvalue+=SampleImage(image,projX,projY);	    
	}
      }         
      currentvalue/=m_subsample*m_subsample;
      readvalues[i*m_size+j]=(unsigned char)currentvalue;
    }
  }

  Image *d = cvCreateImage(cvSize(m_size,m_size),IPL_DEPTH_8U,1);
  for(int i=0;i<m_size;i++) {
    for(int j=0;j<m_size;j++) {
      DrawPixel(d,i,j,readvalues[i*m_size+j]);
    }
  }
  cvReleaseImage(&d);

  float sigma;
  float mean;
  calculate_mean_sigma(readvalues,&sigma,&mean);
  
  // we compare with all four orientations of the template
  /**
   *   +-----+   +-----+   +-----+   +-----+
   *   | --> |   |   | |   |     |   | ^   | 
   *   |     |   |   | |   |     |   | |   |
   *   |     |   |   V |   | <-- |   | |   |
   *   +-----+   +-----+   +-----+   +-----+
   */
  float cross0 = 0; 
  float cross1 = 0;
  float cross2 = 0;
  float cross3 = 0;
  for(int i=0;i<m_size;i++) {
    for(int j=0;j<m_size;j++) {
      cross0 += (m_values[i*m_size+j] - m_average)*(readvalues[i*m_size+j]-mean);
      cross1 += (m_values[j*m_size+(m_size-i)] - m_average)*(readvalues[i*m_size+j]-mean);
      cross2 += (m_values[(m_size-i)*m_size+(m_size-j)] - m_average)*(readvalues[i*m_size+j]-mean);
      cross3 += (m_values[(m_size-j)*m_size+i] - m_average)*(readvalues[i*m_size+j]-mean);
    }
  }
  PROGRESS("Orientation 1: "<< cross0);
  PROGRESS("Orientation 2: "<< cross1);
  PROGRESS("Orientation 3: "<< cross2);
  PROGRESS("Orientation 4: "<< cross3);
  
  cross0 = fabs(cross0);
  cross1 = fabs(cross1);
  cross2 = fabs(cross2);
  cross3 = fabs(cross3);

  float cross;
  if ((cross0 > cross1) &&
      (cross0 > cross2) &&
      (cross0 > cross3)) {
    cross = cross0;
  }
  else if ((cross1 > cross0) &&
	   (cross1 > cross2) &&
	   (cross1 > cross3)) {
    cross = cross1;
  }
  else if ((cross2 > cross0) &&
	   (cross2 > cross1) &&
	   (cross2 > cross3)) {
    cross = cross2;
  }
  else {
    cross = cross3;
  }
  cross /= m_sigma * sigma;

  return cross;

}

void Template::calculate_mean_sigma(unsigned char* values, float* sigma, float* mean) const {
  *mean = 0;
  for(int i=0;i<m_size*m_size;i++) {
    *mean += (float)values[i];
  }
  *mean /= m_size*m_size;

  float diffsq = 0;
  for(int i=0;i<m_size*m_size;i++) {
    diffsq += (values[i]- *mean) * (values[i]- *mean);
  }
  
  *sigma = sqrt(diffsq);
}

void Template::Draw2D(Image *image, int white, int black) {

  int starty = 0;
  int endy = image->height;
  
  int startx = 0;
  int endx = image->width;
  

  for(int y=starty;y<endy;y++) {
    float v = m_original->height- m_original->height*((float)y -starty)/endy;
    for(int x=startx;x<endx;x++) {
      float u = m_original->width- m_original->width*((float)x -startx)/endx;
      DrawPixel(image,x,y,SampleImage(m_original,u,v));
    }
  }
}
