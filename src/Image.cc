#include <tripover/Image.hh>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include <cassert>
#ifdef HAVE_BOOST_RANDOM
# include <boost/random.hpp>
#endif

//Image::Image() : m_from_header(false), m_free_contents(false), m_image(NULL) {};


Image::Image(int width, int height) : m_width(width),m_height(height), m_contents(new unsigned char[width*height]), m_free_contents(true), m_width_step(m_width), m_binary(false) {
  ConvertScale(0,255);
};

Image::Image(const Image& c) : m_width(c.m_width), m_height(c.m_height), m_contents(new unsigned char[c.m_width_step*c.m_height]), m_free_contents(true), m_width_step(c.m_width_step), m_binary(c.m_binary) {
  memcpy(m_contents,c.m_contents,m_width_step*m_height);
};

/**
 * \todo implement this!
 */
Image::Image(char* filename) { 
  assert(false);
};

Image::Image(int width,int height, int width_step, unsigned char* contents) : m_width(width), m_height(height), m_contents(contents),m_free_contents(false),m_width_step(width_step), m_binary(false) {}

Image::~Image() {
  if (m_free_contents) {
    delete[] m_contents;
  }    
}

unsigned char Image::GlobalThreshold(const unsigned char threshold) {
  unsigned char histogram[128] = {0};
  unsigned int total = 0;
  const int width = GetWidth();
  const int height = GetHeight();
  for(int i=0;i<height;++i) {
    unsigned char* data_pointer = GetRow(i);
    for(int j=0;j<width;++j) {
      unsigned char pixel = *data_pointer;
      //      histogram[pixel]++;
      total+=pixel;
      *data_pointer = pixel > threshold ? 0 : 1;
      data_pointer++;
    }
  }
#ifdef IMAGE_DEBUG
  Save("debug-globalthreshold.bmp");
#endif

  total/=width*height;
  m_binary = true;
  return total;

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
 * Adapted to use a more efficient calculation for the moving
 * average. the window used is now 2^window_size
 *
 */

void Image::AdaptiveThreshold(const unsigned int window_size, const unsigned char offset) {
  int moving_average = 127;
  const int image_width = GetWidth();
  const int image_height = GetHeight();
 const int useoffset = 255-offset;

  int previous_line[image_width];
  // intentionally uninitialised
  //  for(int i=0;i<image_width;++i) { previous_line[i] = 127; }

  for(int i=0;i<image_height-1;) { // use height-1 so we dont overrun the image if its height is an odd number
    unsigned char* data_pointer = GetRow(i);
    for(int j=0;j<image_width;++j) {
      int pixel = *data_pointer;
      moving_average = pixel + moving_average - (moving_average >> window_size);
      int current_thresh = (moving_average + previous_line[j])>>1;
      previous_line[j] = moving_average;
      *data_pointer = (pixel << window_size+8) < (current_thresh * useoffset) ? 1 : 0;
      ++data_pointer;
    }

    ++i;
    data_pointer = GetRow(i) + image_width-1;
    for(int j=image_width-1;j>=0;--j) {
      int pixel = *data_pointer;
      moving_average = pixel + moving_average - (moving_average >> window_size);
      int current_thresh = (moving_average + previous_line[j])>>1;
      previous_line[j] = moving_average;
      *data_pointer = (pixel << window_size+8) < (current_thresh * useoffset)  ? 1 : 0;
      --data_pointer;
    }
    ++i;
  }  
  m_binary = true;

#ifdef IMAGE_DEBUG
  Save("debug-adaptivethreshold.bmp");
#endif
}

/**
 * apply canny edge detector
 */
void Image::HomogenousTransform() {
  // take the log of each pixel in the image
  const int image_width = GetWidth();
  const int image_height = GetHeight();
  
  for(int i=0;i<image_height;++i) {
    unsigned char* row_pointer = GetRow(i);
    for(int j=0;j<image_width;++j) {
      unsigned char* ptr = row_pointer+j;
      double result = 45.985904 * log((double)*ptr + 1);
      *ptr = (unsigned char)result;
    }
  }
  // and apply a small kernel edge detector
  //  cvCanny(m_image,m_image,128,128,3);
  m_binary = true;

#ifdef IMAGE_DEBUG
  Save("debug-homogenoustransform.jpg");
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
  for(int i=0;i<GetHeight();++i) {
    unsigned char* row_pointer = GetRow(i);
    for(int j=0;j<GetWidth();++j) {
      float randomval = normal();
      int sample = row_pointer[j];
      sample += Round(normal());
      if (sample < 0) { sample = 0; }
      else if (sample > 255) { sample = 255; }
      row_pointer[j] = sample;
    }
  }
}
#else 
void Image::AddNoise(float mean, float stddev) {
  
}
#endif

#define STEPSIZE 0.1f
void Image::ellipse_polygon_approx(float* points, int startindex, int length, float xc, float yc, float width, float height,  float angle_radians, unsigned char color, int thickness, float start_angle) {
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
  float cosa = cos(angle_radians);
  float sina = sin(angle_radians);
  float a = width/2;
  float b = height/2;

  float currentAngle = start_angle;
  for(int i=2*startindex;i<2*(length+startindex);i+=2) {
    float cost = cos(currentAngle);
    float sint = sin(currentAngle);
    // remember that y increases down from the top of the image so we
    // do yc minus point rather than yc + point
    points[i] = Round(xc + a*cosa*cost + b*sina*sint);
    points[i+1] = Round(yc + a*sina*cost - b*cosa*sint);
    currentAngle += STEPSIZE;
  }

  int numvertices = startindex+length;
  if (thickness > 0) {
    DrawPolygon(points,numvertices,color,thickness);
  }
  else {
    DrawFilledPolygon(points,numvertices,color);
  }
}

void Image::DrawEllipseArc(int xc, int yc, 
			   int  width, int height, 
			   float angle_radians, 
			   float start_angle, float end_angle, unsigned char color, int thickness) {
  DrawEllipseArc((float)xc,(float)yc,(float)width,(float)height,angle_radians,start_angle,end_angle,color,thickness);
}

void Image::DrawEllipseArc(float xc, float yc, 
			   float width, float height, 
			   float angle_radians, 
			   float start_angle, float end_angle, unsigned char color, int thickness) {
  assert(thickness > 0 || thickness == -1);

  int numsteps = (int)(((float)(end_angle - start_angle))/STEPSIZE)+1; // add one to get to the edge of the sector
  // add in an extra point (the centre) if we are drawing a slice
  float points[numsteps*2+2];
  points[0] = Round(xc);
  points[1] = Round(yc);
  ellipse_polygon_approx(points, 1, numsteps, xc, yc ,width, height, angle_radians, color, thickness, start_angle); 
}

void Image::DrawEllipse(float xc, float yc, 
			float width, float height, 
			float angle_radians, 
			unsigned char color, int thickness) {
  assert(thickness > 0 || thickness == -1);

  int numsteps = (int)(2*PI/STEPSIZE); 

  float points[numsteps*2];
  ellipse_polygon_approx(points,0, numsteps, xc, yc ,width, height, angle_radians, color, thickness, 0);
}

Image::Image(Socket& socket) {
  int count;
  m_width = socket.RecvInt();
  m_height = socket.RecvInt();
  m_width_step = socket.RecvInt();
  m_binary = socket.RecvInt() == 1;
  m_contents = new unsigned char[m_width_step*m_height];
  m_free_contents = true;
  socket.Recv(m_contents,m_height*m_width_step);
}

int Image::Save(Socket& socket) const {
  int count = socket.Send(m_width);
  count += socket.Send(m_height);
  count += socket.Send(m_width_step);
  count += socket.Send(m_binary ? 1 : 0);
  count += socket.Send(m_contents,m_height*m_width_step);
  return count;
}

void Image::ConvertScale(float scalefactor, int offset) {
  for(int i=0;i<GetHeight();++i) {
    unsigned char* ptr = GetRow(i);
    for(int j=0;j<GetWidth();++j) {
      unsigned char value = *ptr;
      *ptr = Round(value*scalefactor)+offset;
      ptr++;
    }
  }
}

void Image::DrawLine(int x0,int y0, int x1,int y1, unsigned char colour, unsigned int thickness) {
  colour = COLOUR_BLACK;
  if (x1 < x0 || (x0 == x1 && y1 < y0)) {
    DrawLine(x1,y1,x0,y0,colour,thickness);
  }
  else {
    int dy = y1-y0;
    int dx = x1-x0;
    int a = y1-y0;
    int b = -(x1-x0);
    int c = x1*y0-x0*y1;
    int x = x0;
    int y = y0;
    DrawPixel(x,y,colour);

    if (dy > 0 && dy > dx) { // NE or N
      int d = (int)((a*(x+0.5)+b*(y+1)+c));
      while (y < y1) {
	++y;
	if (d>0) {
	  d+=b;
	}
	else {
	  d+=a+b;	  
	  ++x;
	}
	DrawPixel(x,y,colour);
      }
    }
    else if (dy > 0 && dx > dy) { // E or NE
      int d = (int)((a*(x+1)+b*(y+0.5)+c));
      while (x < x1) {
	++x;
	if (d<0) {
	  d+= a;
	}
	else {
	  d+=a+b;
	  ++y;
	}
	DrawPixel(x,y,colour);
      }
    }
    else if (dy <= 0 && dx > -dy) { // E or SE
      int d = (int)((a*(x+1)+b*(y-0.5)+c));
      while (x<x1) {
	++x;
	if (d>0) {
	  d+=a;
	}
	else {
	  d+=a-b;
	  --y;
	}
	DrawPixel(x,y,colour);	  
      }
    }
    else { // SE or S
      int d = (int)((a*(x+0.5)+b*(y-1)+c));
      while (y > y1) {
	--y;
	if (d<0) {
	  d-=b;	  
	}
	else {
	  d+=a-b;
	  ++x;
	}
	DrawPixel(x,y,colour);
      }
    }
  }
}

void Image::DrawPolygon(int* points, int numpoints, unsigned char colour, unsigned int thickness) {
  for(int i=2;i<2*numpoints;i+=2) {
    DrawLine(points[i-2],points[i-1],points[i],points[i+1],colour,thickness);
  }
  DrawLine(points[2*numpoints-2],points[2*numpoints-1],points[0],points[1],colour,thickness);
}

void Image::DrawPolygon(float* points, int numpoints, unsigned char colour, unsigned int thickness) {
  for(int i=2;i<2*numpoints;i+=2) {
    DrawLine(points[i-2],points[i-1],points[i],points[i+1],colour,thickness);
  }  
  DrawLine(points[2*numpoints-2],points[2*numpoints-1],points[0],points[1],colour,thickness);

}


/**
 * \todo currently unfilled
 */
void Image::DrawFilledPolygon(int* points, int numpoints, unsigned char colour) {
  DrawPolygon(points,numpoints,colour,1);
}


/**
 * \todo currently unfilled
 */
void Image::DrawFilledPolygon(float* points, int numpoints, unsigned char colour) {
  DrawPolygon(points,numpoints,colour,1);
}

/**
 * \todo only does pnm
 */
void Image::Save(const char* filename) const {
  std::ofstream output(filename);
  output << "P2" << std::endl;
  output << "# CREATOR: TOTAL" << std::endl;
  output << GetWidth() << " " << GetHeight() << std::endl;
  output << 255 << std::endl;
  for(int i=0;i<GetHeight();++i) {
    const unsigned char* row_pointer = GetRow(i);
    for(int j=0;j<GetWidth();++j) {
      int data = (int)row_pointer[j];
      if (m_binary) {
	output << (data == 0 ? 0 : 255) << std::endl;
      }
      else {
	output << data << std::endl;
      }
    }
  }
  output.flush();
  output.close();
}

/**
 * \todo unimplemented
 */
void Image::Resize(Image& image) const {}
