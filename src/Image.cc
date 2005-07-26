/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

#include <total/Image.hh>
#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
#include <Magick++.h>
#endif

namespace Total {

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
    void ImageSpecialise<Colour::RGB>::Save(const char* filename) const {
      try {
	Magick::Image i(Magick::Geometry(m_width,m_height),
			Magick::ColorRGB(1.0,1.0,1.0));
	i.type(Magick::TrueColorType);
	i.colorSpace(Magick::RGBColorspace);
	i.depth(8);
	i.modifyImage();
	for(unsigned int y=0;y<m_height;++y) {
	  for(unsigned int x=0;x<m_width;++x) {	
	    int offset = Bpp*(y*m_width_step+x);
	    Magick::ColorRGB color((double)m_contents[offset]/255,
				   (double)m_contents[offset+1]/255,
				   (double)m_contents[offset+2]/255);
	    i.pixelColor(x,y,color);
	  }
	} 
	i.write(filename);
      }
      catch(Magick::Exception& e) {
	throw e.what();
      }
    }
#else
  void ImageSpecialise<Colour::RGB>::Save(const char* filename) const {
      std::ofstream output(filename);
      output << "P3" << std::endl;
      output << "# CREATOR: TOTAL" << std::endl;
      output << m_width << " " << m_height << std::endl;
      output << 255 << std::endl;
      for(unsigned int i=0;i<m_height;++i) {
	const unsigned char* row_pointer = GetRow(i);
	for(unsigned int j=0;j<Bpp*m_width;++j) {
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

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
    void ImageSpecialise<Colour::RGB>::Load(const char* filename) {
      
      Magick::Image i;
      
      if (m_free_contents) {
	delete[] m_contents;
      }    
      
      try {
	i.read(filename);
	m_width = i.baseColumns();
	m_width_step = m_width;
	m_height = i.baseRows();
	int size = m_width*m_height*Bpp;
	m_contents = new unsigned char[size];
	m_free_contents = true;
	m_binary = false;
	Magick::Pixels view(i);
	Magick::PixelPacket *pixel_cache = view.get(0,0,m_width,m_height);
	i.quantizeColorSpace(Magick::RGBColorspace);
	i.quantize();
	for(int ptr=0;ptr<size;ptr+=3) {
	  m_contents[ptr] = pixel_cache->red;
	  m_contents[ptr+1] = pixel_cache->green;
	  m_contents[ptr+2] = pixel_cache->blue;
	  ++pixel_cache;
	}
      }
      catch(Magick::Exception& e) {
	throw e.what();
      }
    }
#endif

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
   void ImageSpecialise<Colour::Grey>::Save(const char* filename) const {
    try {
      Magick::Image i(Magick::Geometry(m_width,m_height),
		      Magick::ColorRGB(1.0,1.0,1.0));
      i.type(Magick::GrayscaleType);
      i.colorSpace(Magick::GRAYColorspace);
      i.depth(8);
      i.modifyImage();
      for(unsigned int y=0;y<m_height;++y) {
	const unsigned char* row = GetRow(y);
	for(unsigned int x=0;x<m_width;++x) {	
	  Magick::ColorGray color(m_binary ? ((*row & 1) ? 1.0 : 0.0) : (double)*row/255);
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
    void ImageSpecialise<Colour::Grey>::Save(const char* filename) const {
      std::ofstream output(filename);
      output << "P2" << std::endl;
      output << "# CREATOR: TOTAL" << std::endl;
      output << m_width << " " << m_height << std::endl;
      output << 255 << std::endl;
      for(unsigned int i=0;i<m_height;++i) {
	const unsigned char* row_pointer = GetRow(i);
	for(unsigned int j=0;j<m_width;++j) {
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

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
    void ImageSpecialise<Colour::Grey>::Load(const char* filename) {

      Magick::Image i;
      
      if (m_free_contents) {
	delete[] m_contents;
      }    
      
      try {
	
	i.read(filename);
	m_width = i.baseColumns();
	m_width_step = m_width;
	m_height = i.baseRows();
	int size = m_width*m_height*Bpp;
	m_contents = new unsigned char[size];
	m_free_contents = true;
	m_binary = false;
	Magick::Pixels view(i);
	Magick::PixelPacket *pixel_cache = view.get(0,0,m_width,m_height);
	i.quantizeColorSpace(Magick::GRAYColorspace);
	i.quantizeColors(256);
	i.quantize();
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
    void ImageSpecialise<Colour::Grey>::Load(const char* filename) {

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
      
      m_contents = new unsigned char[m_width*m_height*Bpp];
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
      m_binary = false;
    }
#endif

}

  /*
   Image::ImageBase(Socket& socket) {
    m_width = socket.RecvInt();
    m_height = socket.RecvInt();
    m_width_step = socket.RecvInt();
    m_binary = socket.RecvInt() == 1;
    m_contents = new unsigned char[m_width_step*m_height];
    m_free_contents = true;
    socket.Recv(m_contents,m_height*m_width_step);
  }
  */
  /*
   int Image::Save(Socket& socket) const {
    int count = socket.Send(m_width);
    count += socket.Send(m_height);
    count += socket.Send(m_width_step);
    count += socket.Send(m_binary ? 1 : 0);
    count += socket.Send(m_contents,m_height*m_width_step);
    return count;
  }
  */

  /*
   void Image::GlobalThreshold(const unsigned char threshold) {
    const int width = GetWidth();
    const int height = GetHeight();
    for(int i=0;i<height;++i) {
      unsigned char* data_pointer = GetRow(i);
      for(int j=0;j<width;++j) {
	unsigned char pixel = *data_pointer;
	*data_pointer = pixel > threshold ? (pixel & 0xFC) : ((pixel & 0xFC) | 1);
	data_pointer++;
      }
    }
    m_binary = true;

#ifdef IMAGE_DEBUG
    Save("debug-globalthreshold.pnm");
#endif

  }
  */
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


  void Image::AdaptiveThreshold(const unsigned int window_size, const unsigned char offset) {
    int moving_average = 127;
    const int image_width = GetWidth();
    const int image_height = GetHeight();
    const int useoffset = 255-offset;

    int previous_line[image_width];
    // intentionally uninitialised
    for(int i=0;i<image_width;++i) { previous_line[i] = 127; }

    for(int i=0;i<image_height-1;) { // use height-1 so we dont overrun the image if its height is an odd number
      unsigned char* data_pointer = GetRow(i);
      for(int j=0;j<image_width;++j) {
	int pixel = *data_pointer;
	moving_average = pixel + moving_average - (moving_average >> window_size);
	int current_thresh = (moving_average + previous_line[j])>>1;
	previous_line[j] = moving_average;      
	*data_pointer = (pixel << window_size+8) < (current_thresh * useoffset) ? ((*data_pointer & 0xFC) | 1) : (*data_pointer & 0xFC);
	++data_pointer;
      }

      ++i;
      data_pointer = GetRow(i) + image_width-1;
      for(int j=image_width-1;j>=0;--j) {
	int pixel = *data_pointer;
	moving_average = pixel + moving_average - (moving_average >> window_size);
	int current_thresh = (moving_average + previous_line[j])>>1;
	previous_line[j] = moving_average;
	*data_pointer = (pixel << window_size+8) < (current_thresh * useoffset)  ? ((*data_pointer & 0xFC) | 1) : (*data_pointer & 0xFC);
	--data_pointer;
      }
      ++i;
    }  
    m_binary = true;

#ifdef IMAGE_DEBUG
    Save("debug-adaptivethreshold.pnm");
#endif
  }
   */


  /**
   * apply canny edge detector

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
    Save("debug-homogenoustransform.pnm");
#endif
  }
   */

  /**
   * Add noise to the image in an attempt to simulate that of a real
   * camera.

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
   */


  /**
   * \todo unimplemented
   void Image::Resize(Image& image) const {}
   */
