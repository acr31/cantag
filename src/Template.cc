#include <tripover/Config.hh>
#include <tripover/Template.hh>
#include <cassert>
#include <cstring>
#include <tripover/findtransform.hh>

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

  m_original = new Image(filename);
  assert(m_original->GetWidth() == m_original->GetHeight());

  Image dest(size,size);
  m_original->Resize(dest);

  m_values = new unsigned char[size*size];
  for(int x=0;x<size;x++) {
    for(int y=0;y<size;y++) {
      m_values[x+size*y] = dest.Sample(x,y);
    }
  }

  calculate_mean_sigma(m_values,&m_sigma,&m_average);

}

bool Template::operator<(const Template& t) {
  return strcmp(m_filename,t.m_filename);
}

Template::~Template() {
  delete[] m_values;
  delete m_original;
}

/**
 * Project each one of our points onto the quadtangle in the image to
 * sample the image value for that point.  Then calculate the
 * correlation coefficient.  We need to try it against all four
 * orientations of the template.
 */
float Template::Correlate(const Image& image, const float transform[16], const Camera& camera) const {
  float scalefactor = 1/(float)m_size;
  float subscalefactor = 1/(float)m_size/(float)m_subsample;
  
  unsigned char readvalues[m_size*m_size];

  for(int i=0;i<m_size;i++) {
    for(int j=0;j<m_size;j++) {
      float currentvalue  = 0;
      for(int k=0;k<m_subsample;k++) {
	for(int l=0;l<m_subsample;l++) {
	  float points[] = { i*scalefactor+k*subscalefactor,
			     j*scalefactor+l*subscalefactor};
	  ApplyTransform(transform,points,1);
	  camera.NPCFToImage(points,1);
	  currentvalue+=(float)image.Sample(points[0],points[1]);	    
	}
      }         
      currentvalue/= m_subsample*m_subsample;
      readvalues[i*m_size+j]=(unsigned char)currentvalue;
    }
  }

  /*
  Image *d = cvCreateImage(cvSize(m_size,m_size),IPL_DEPTH_8U,1);
  for(int i=0;i<m_size;i++) {
    for(int j=0;j<m_size;j++) {
      DrawPixel(d,i,j,readvalues[i*m_size+j]);
    }
  }
  cvReleaseImage(&d);
  */

  return Correlate(readvalues);
}

float Template::Correlate(const Template& t) const {
  return Correlate(t.m_values);
}

float Template::Correlate(const unsigned char* readvalues) const {
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

void Template::calculate_mean_sigma(const unsigned char* values, float* sigma, float* mean) const {
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

void Template::Draw2D(Image& image) {

  int starty = 0;
  int endy = image.GetHeight();
  
  int startx = 0;
  int endx = image.GetWidth();
  
  for(int y=starty;y<endy;y++) {
    float v = m_original->GetHeight() - m_original->GetHeight()*((float)y -starty)/endy;
    for(int x=startx;x<endx;x++) {
      float u = m_original->GetWidth() - m_original->GetWidth()*((float)x -startx)/endx;
      image.DrawPixel(x,y,m_original->Sample(u,v));
    }
  }
}
