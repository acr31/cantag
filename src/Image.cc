/*
  Copyright (C) 2004 Andrew C. Rice
  Copyright (C) 2005 Alastair R. Beresford

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

#include <cantag/Image.hh>
#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
#include <Magick++.h>
#endif

namespace Cantag {

  void ImageSpecialise<Pix::Sze::Byte1,Pix::Fmt::Grey8>::ConvertScale(float scalefactor, int offset) {
    
    //\todo: fix up this binary flag setting---could break stuff :(
    //m_binary = false;
    for(unsigned int i=0;i<GetHeight();++i) {
      PixRow<Pix::Fmt::Grey8> row = GetRow(i);
      for(PixRow<Pix::Fmt::Grey8>::iterator x=row.begin(); x!= row.end(); ++x) {
	x.v( Round(x.v()*scalefactor)+offset );
      }
    }
  }
  
  void ImageSpecialise<Pix::Sze::Byte1,Pix::Fmt::Grey8>::Mask(unsigned char mask) {
    
    //\todo: fix up this binary flag setting---could break stuff :(
    //m_binary = mask == 1;
    for(unsigned int i=0;i<GetHeight();++i) {
      PixRow<Pix::Fmt::Grey8> row = GetRow(i);
      for(PixRow<Pix::Fmt::Grey8>::iterator x=row.begin(); x!= row.end(); ++x) {
	x.v(x.v() & mask);
      }
    }
  }



#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
    void ImageSpecialise<Pix::Sze::Byte3,Pix::Fmt::RGB24>::Save(const char* filename) const {
      try {
	Magick::Image i(Magick::Geometry(GetWidth(),GetHeight()),
			Magick::ColorRGB(1.0,1.0,1.0));
	i.type(Magick::TrueColorType);
	i.colorSpace(Magick::RGBColorspace);
	i.depth(8);
	i.modifyImage();
	for(unsigned int y=0;y<GetHeight();++y) {
	  const PixRow<Pix::Fmt::RGB24> row = GetRow(y);
	  PixRow<Pix::Fmt::RGB24>::const_iterator pixel=row.begin();
	  for(unsigned int x=0;x<GetWidth();++x) {
	    Magick::ColorRGB color((double)pixel.r()/255,
				   (double)pixel.g()/255,
				   (double)pixel.b()/255);
	    i.pixelColor(x,y,color);
	    ++pixel;
	  }
	} 
	i.write(filename);
      }
      catch(Magick::Exception& e) {
	throw e.what();
      }
    }
#else
  void ImageSpecialise<Pix::Sze::Byte3,Pix::Fmt::RGB24>::Save(const char* filename) const {
      std::ofstream output(filename);
      output << "P3" << std::endl;
      output << "# CREATOR: TOTAL" << std::endl;
      output << GetWidth() << " " << GetHeight() << std::endl;
      output << 255 << std::endl;
      for(unsigned int y=0;y<GetHeight();++y) {
	PixRow<Pix::Fmt::RGB24> row = GetRow(y);
	for(PixRow<Pix::Fmt::RGB24>::iterator x=row.begin(); 
	    x!= row.end(); ++x) {
	  output << x.r() << " " << x.g() << " " << x.b() << std::endl;
	}
      }
      output.flush();
      output.close();
    }
#endif

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
    void ImageSpecialise<Pix::Sze::Byte3,Pix::Fmt::RGB24>::Load(const char* filename) {
      
      Magick::Image i;
      i.read(filename);
      unsigned int width=i.baseColumns();
      unsigned int height=i.baseRows();
      int size = width*height*3;
      unsigned char* contents = new unsigned char[size];
      
      try {
	i.quantizeColorSpace(Magick::RGBColorspace);
	i.quantize();
	Magick::Pixels view(i);
	Magick::PixelPacket *pixel_cache = view.get(0,0,width,height);
	for(int ptr=0;ptr<size;ptr+=3) {
	  contents[ptr] = (pixel_cache->red >> (sizeof(Magick::Quantum)-1)*8) & 0xff;
	  contents[ptr+1] = (pixel_cache->green >> (sizeof(Magick::Quantum)-1)*8) & 0xff;
	  contents[ptr+2] = (pixel_cache->blue >> (sizeof(Magick::Quantum)-1)*8) & 0xff;
	  ++pixel_cache;
	}
	NewImage(width,height,3*width,3,contents,true);

	
      }
      catch(Magick::Exception& e) {
	throw e.what();
      }

      try {
	Magick::Image i(Magick::Geometry(width,height),
			Magick::ColorRGB(1.0,1.0,1.0));
	i.type(Magick::TrueColorType);
	i.colorSpace(Magick::RGBColorspace);
	i.depth(8);
	i.modifyImage();
	for(unsigned int y=0;y<height;++y) {
	  for(unsigned int x=0;x<width;++x) {
	    Magick::ColorRGB color((double)contents[3*(y*width+x)]/255,
				   (double)contents[3*(y*width+x)]/255,
				   (double)contents[3*(y*width+x)]/255);
	    i.pixelColor(x,y,color);
	  }
	} 
	i.write("tmp.ppm");
      }
      catch(Magick::Exception& e) {
	throw e.what();
      }
    }


#endif

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
   void ImageSpecialise<Pix::Sze::Byte1,Pix::Fmt::Grey8>::Save(const char* filename) const {
    try {
      Magick::Image i(Magick::Geometry(GetWidth(),GetHeight()),
		      Magick::ColorRGB(1.0,1.0,1.0));
      i.type(Magick::GrayscaleType);
      i.colorSpace(Magick::GRAYColorspace);
      i.depth(8);
      i.modifyImage();
      for(unsigned int y=0;y<GetHeight();++y) {
	const PixRow<Pix::Fmt::Grey8> row = GetRow(y);
	PixRow<Pix::Fmt::Grey8>::const_iterator pixel=row.begin();
	for(unsigned int x=0;x<GetWidth();++x) {
	  //Magick::ColorGray color(m_binary ? ((*row & 1) ? 1.0 : 0.0) : (double)*row/255);
	  unsigned char val = pixel.v();
	  Magick::ColorGray color((double)val/255.0);
	  i.pixelColor(x,y,color);
	  ++pixel;
	}
      } 
      i.write(filename);
    }
    catch(Magick::Exception& e) {
      throw e.what();
    }
   }
#else
    void ImageSpecialise<Pix::Sze::Byte1,Pix::Fmt::Grey8>::Save(const char* filename) const {
      std::ofstream output(filename);
      output << "P2" << std::endl;
      output << "# CREATOR: TOTAL" << std::endl;
      output << GetWidth() << " " << GetHeight() << std::endl;
      output << 255 << std::endl;
      for(unsigned int y=0;y<GetHeight();++y) {
	const PixRow<Pix::Fmt::Grey8> row = GetRow(y);
	for(PixRow<Pix::Fmt::Grey8>::const_iterator x=row.begin(); 
	    x!= row.end(); ++x) {
	  output << x.v() << std::endl;
	  //if (m_binary) {
	  //output << (data == 0 ? 0 : 255) << std::endl;
	  //}
	  //else {
	  //output << data << std::endl;
	  //}
	}
      }
      output.flush();
      output.close();
    }
#endif 

#if defined(HAVE_MAGICKXX) and defined(HAVELIB_MAGICKXX) and defined(HAVELIB_MAGICK)
    void ImageSpecialise<Pix::Sze::Byte1,Pix::Fmt::Grey8>::Load(const char* filename) {

      Magick::Image i;
      
      try {
	
	i.read(filename);
	unsigned int width = i.baseColumns();
	unsigned int height = i.baseRows();
	int size = width*height;
	unsigned char* contents = new unsigned char[size];
	Magick::Pixels view(i);
	Magick::PixelPacket *pixel_cache = view.get(0,0,width,height);
	i.quantizeColorSpace(Magick::GRAYColorspace);
	i.quantizeColors(256);
	i.quantize();
	for(int ptr=0;ptr<size;++ptr) {
	  contents[ptr] = (pixel_cache->red >> (sizeof(Magick::Quantum)-1)*8) & 0xff;
	  ++pixel_cache;
	}

	NewImage(width,height,width,1,contents,true);
      }
      catch(Magick::Exception& e) {
	throw e.what();
      }
    }
#else
    void ImageSpecialise<Pix::Sze::Byte1,Pix::Fmt::Grey8>::Load(const char* filename) {

      std::ifstream input(filename);
      char buffer[50];
  
      input.getline(buffer,50);
      
      if (strncmp(buffer,"P2",2) != 0) {
	throw "Can only load greyscale PNM images (no P2 tag)!";
      }
      
      input.getline(buffer,50);
      
      unsigned int width;
      input >> width;
      unsigned int height;
      input >> height;
      int colourdepth;
      input >> colourdepth;
      
      if (colourdepth != 255) {
	throw "Can only load greyscale PNM images (wrong colour depth)!";        
      }
      
      unsigned char* contents = new unsigned char[width*height];
      int ptr = 0;
      int max = width*height;
      while(ptr < max && !input.eof()) {
	int val;
	input >> val;
	contents[ptr++] = val;
      }
      
      if (ptr != max) {
	throw "Incorrect number of datapoints in PNM file";
      }

      NewImage(width,height,width,1,contents,true);
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
