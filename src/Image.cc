#include <tripover/Image.hh>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>
#include <cassert>
#ifdef HAVE_BOOST_RANDOM
# include <boost/random.hpp>
#endif

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
#include <Magick++.h>
#endif



Image::Image(int width, int height) : m_width(width),m_height(height), m_contents(new unsigned char[width*height]), m_free_contents(true), m_width_step(m_width), m_binary(false) {
  ConvertScale(0,255);
};

Image::Image(const Image& c) : m_width(c.m_width), m_height(c.m_height), m_contents(new unsigned char[c.m_width_step*c.m_height]), m_free_contents(true), m_width_step(c.m_width_step), m_binary(c.m_binary) {
  memcpy(m_contents,c.m_contents,m_width_step*m_height);
};

Image::Image(char* filename) { 
  Load(filename);
};

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
void Image::Load(const char* filename) {
  Magick::Image i;
  try {
    i.read(filename);
    i.quantizeColorSpace(Magick::GRAYColorspace);
    i.quantizeColors(256);
    i.quantize();
    m_width = i.baseColumns();
    m_width_step = m_width;
    m_height = i.baseRows();
    m_contents = new unsigned char[m_width*m_height];
    m_free_contents = true;
    Magick::PixelPacket *pixel_cache = i.getPixels(0,0,m_width,m_height);
    int size = m_width*m_height;
    for(int ptr=0;ptr<size;++ptr) {
      m_contents[ptr] = pixel_cache->red;
      ++pixel_cache;
    }
  }
  catch(Magick::Exception& e) {
    throw e.what();
  }
}
#else
void Image::Load(const char* filename) {
  std::ifstream input(filename);
  char buffer[50];
  
  input.getline(buffer,50);

  if (strncmp(buffer,"P2",2) != 0) {
    throw "Can only load greyscale PNM images (no P2 tag)!";
  }

  input.getline(buffer,50);
  
  input >> m_width;
  m_width_step = m_width;
  input >> m_height;
  int colourdepth;
  input >> colourdepth;

  if (colourdepth != 255) {
    throw "Can only load greyscale PNM images (wrong colour depth)!";        
  }

  m_contents = new unsigned char[m_width*m_height];
  m_free_contents = true;
  int ptr = 0;
  int max = m_width*m_height;
  while(ptr < max && !input.eof()) {
    int val;
    input >> val;
    m_contents[ptr++] = val;
  }
  
  if (ptr != max) {
    throw "Incorrect number of datapoints in PNM file";
  }
}
#endif

Image::Image(int width,int height, int width_step, unsigned char* contents) : m_width(width), m_height(height), m_contents(contents),m_free_contents(false),m_width_step(width_step), m_binary(false) {}

Image::~Image() {
  if (m_free_contents) {
    delete[] m_contents;
  }    
}

void Image::GlobalThreshold(const unsigned char threshold) {
  const int width = GetWidth();
  const int height = GetHeight();
  for(int i=0;i<height;++i) {
    unsigned char* data_pointer = GetRow(i);
    for(int j=0;j<width;++j) {
      unsigned char pixel = *data_pointer;
      *data_pointer = pixel > threshold ? 0 : 1;
      data_pointer++;
    }
  }
#ifdef IMAGE_DEBUG
  Save("debug-globalthreshold.bmp");
#endif

  m_binary = true;
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
  DrawPolygon(points,numvertices,color,thickness);
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
  if (thickness == -1) {
    float angle = angle_radians + (start_angle+end_angle)/2;
    // remember that y increases down from the top of the image so we
    // do yc minus point rather than yc + point
    float cosa = cos(angle_radians);
    float sina = sin(angle_radians);
    float cost = cos(angle);
    float sint = sin(angle);
    int fx = Round(xc + width/4*cosa*cost + height/4*sina*sint);
    int fy = Round(yc + width/4*sina*cost - height/4*cosa*sint);
    SeedFill(fx,fy,color);
  }
}

void Image::DrawEllipse(float xc, float yc, 
			float width, float height, 
			float angle_radians, 
			unsigned char color, int thickness) {
  assert(thickness > 0 || thickness == -1);

  int numsteps = (int)(2*PI/STEPSIZE); 

  float points[numsteps*2];
  ellipse_polygon_approx(points,0, numsteps, xc, yc ,width, height, angle_radians, color, thickness, 0);
  
  if (thickness == -1) {
    SeedFill(Round(xc),Round(yc),color);
  }

}

Image::Image(Socket& socket) {
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

    if (dy > 0 && dy >= dx) { // NE or N
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


void Image::DrawFilledPolygon(int* points, int numpoints, unsigned char colour) {
  DrawPolygon(points,numpoints,colour,1);
  int cx = 0, cy=0;
  for (int i=0;i<2*numpoints;i+=2) {
    cx+=points[i];
    cy+=points[i+1];
  }
  cx /= numpoints;
  cy /= numpoints;

  SeedFill(cx,cy,colour);
}


void Image::DrawFilledPolygon(float* points, int numpoints, unsigned char colour) {
  DrawPolygon(points,numpoints,colour,1);
  float cx = 0, cy=0;
  for (int i=0;i<2*numpoints;i+=2) {
    cx+=points[i];
    cy+=points[i+1];
  }
  cx /= numpoints;
  cy /= numpoints;

  SeedFill(Round(cx),Round(cy),colour);
}


/*
 * Recursive fill.  OK, so it's not pretty, but
 * we don't need high performance and it works fine!
 *
 * Rob Harle <rkh23@cam.ac.uk>
 */
void Image::SeedFill(int x, int y,unsigned char colour) {
  if (x<0 || y<0 || x >=(int)m_width || y >= (int)m_height) return;
  if (Sample(x,y)==colour) return;
  else {
    DrawPixel(x,y,colour);
    SeedFill(x+1,y,colour);
    SeedFill(x-1,y,colour);
    SeedFill(x,y+1,colour);
    SeedFill(x,y-1,colour);
  }
}

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
void Image::Save(const char* filename) const {
  try {
    Magick::Image i(Magick::Geometry(this->GetWidth(),this->GetHeight()),
		    Magick::ColorRGB(1.0,1.0,1.0));
    i.modifyImage();
    for(int y=0;y<GetHeight();++y) {
      const unsigned char* row = GetRow(y);
      for(int x=0;x<GetWidth();++x) {
	Magick::ColorRGB color((double)*row/255,(double)*row/255,(double)*row/255);
	i.pixelColor(x,y,color);
	++row;
      }
    } 
    i.write(filename);
  }
  catch(Magick::Exception& e) {
    throw e.what();
  }
}
#else
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
#endif 

/**
 * \todo unimplemented
 */
void Image::Resize(Image& image) const {}
