#include <Config.hh>
#include <Template.hh>

#undef FILENAME
#define FILENAME "Template.cc"

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
  Image* image = LoadImage(filename);
  
  assert(image->width == image->height);

  if (image->nChannels==3) {
    Image* t = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U,1);
    cvCvtColor(image,t,CV_RGB2GRAY);
    FreeImage(&image);
    image = t;
  }
  
  Image* dest = cvCreateImage(cvSize(size,size),IPL_DEPTH_8U,1);
  cvResize(image,dest);
  cvReleaseImage(&image);

  m_values = new unsigned char[size*size];
  for(int i=0;i<size*size;i++) {
    m_values[i] = *((uchar*)(dest->imageData + i));
  }
  cvReleaseImage(&dest);

  calculate_mean_stddev(m_values,&m_stddev,&m_average);

}

bool Template::operator<(const Template& t) {
  return strcmp(m_filename,t.m_filename);
}

Template::~Template() {
  delete[] m_values;
}

/**
 * Project each one of our points onto the quadtangle in the image to
 * sample the image value for that point.  Then calculate the
 * correlation coefficient
 */
float Template::Correlate(Image* image, QuadTangle2D* q) {
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
      readvalues[i*j]=(unsigned char)currentvalue;
    }
  }

  float stddev;
  float mean;
  calculate_mean_stddev(readvalues,&stddev,&mean);
  
  float cross = 0;
  for(int i=0;i<m_size*m_size;i++) {
    cross += (m_values[i] - m_average)*(readvalues[i]-mean);
  }
  
  cross /= m_stddev * stddev;

  return cross;

}

void Template::calculate_mean_stddev(unsigned char* values, float* stddev, float* mean) {
  *mean = 0;
  for(int i=0;i<m_size*m_size;i++) {
    *mean += (float)values[i];
  }
  *mean /= m_size*m_size;

  float diffsq = 0;
  float diff = 0;
  for(int i=0;i<m_size*m_size;i++) {
    diffsq += (values[i]- *mean) * (values[i]- *mean);
    diff   += (values[i]- *mean);
  }
  
  *stddev = sqrt(( diffsq - ( diff * diff / (float)m_size / (float)m_size ) ) / (float)(m_size*m_size-1));
}

