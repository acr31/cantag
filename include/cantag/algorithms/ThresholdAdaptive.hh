/*
  Copyright (C) 2004 Andrew C. Rice, Robert K. Harle

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

/**
 * $Header$
 */

#ifndef THRESHOLD_ADAPTIVE_GUARD
#define THRESHOLD_ADAPTIVE_GUARD

#include <cantag/Image.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>

namespace Cantag {

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

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> class ThresholdAdaptive
    : public Function<TypeList<Image<size,layout> >, // needed because the preprocessor can't parse the call to TL1(Image<size,layout>)
    TL1(MonochromeImage)> {
  private:
    int m_window_size;
    int m_offset;
  
  public:
    ThresholdAdaptive(int window_size, int offset);
    bool operator()(const Image<size,layout>& source, MonochromeImage& dest) const;
  };


  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> ThresholdAdaptive<size,layout>::ThresholdAdaptive(int window_size, 
														 int offset)
    : m_window_size(window_size), m_offset(offset) {}


  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> 
  bool ThresholdAdaptive<size,layout>::operator()(const Image<size,layout>& image, MonochromeImage& dest) const {
     int moving_average = 127;
    const int image_width = image.GetWidth();
    const int image_height = image.GetHeight();
    const int useoffset = 255-m_offset;
    
    int previous_line[image_width];
    for(int i=0;i<image_width;++i) { previous_line[i] = 127; }
    
    for(int i=0;i<image_height-1;) { // use height-1 so we dont overrun the image if its height is an odd number
      const PixRow<layout> row = image.GetRow(i);
      typename PixRow<layout>::const_iterator x = row.begin();
      for (int j=0; j<image_width;++j) {
	const int pixel = (*x).intensity(); ++x;

	moving_average = pixel + moving_average - (moving_average >> m_window_size);
	int current_thresh = (moving_average + previous_line[j])>>1;
	previous_line[j] = moving_average;      
	dest.SetPixel(j,i, (pixel << m_window_size+8) < (current_thresh * useoffset) );
      }

      ++i;
      x=row.end()-1; 
      for(int j=image_width-1;j>=0;--j) {
	const int pixel = (*x).intensity(); --x;

	moving_average = pixel + moving_average - (moving_average >> m_window_size);
	int current_thresh = (moving_average + previous_line[j])>>1;
	previous_line[j] = moving_average;
	dest.SetPixel(j,i, (pixel << m_window_size+8) < (current_thresh * useoffset) );
      }
      ++i;
    }  

    for(int i=0;i<image_height;++i) {
      dest.SetPixel(0,i,false);
      dest.SetPixel(image_width-1,i,false);
      if (i == 0 || i == image_height-1) {
	for(int j=0;j<image_width-1;++j) {
	  dest.SetPixel(j,i,false);
	}
      }
    }
    return true;
  }

}

#endif//THRESHOLD_ADAPTIVE_GUARD
