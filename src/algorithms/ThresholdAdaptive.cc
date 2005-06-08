/**
 * $Header$
 */

#include <total/algorithms/ThresholdAdaptive.hh>
namespace Total {
  ThresholdAdaptive::ThresholdAdaptive(int window_size, int offset) : m_window_size(window_size), m_offset(offset) {};

  bool ThresholdAdaptive::operator()(const Image& image, MonochromeImage& dest) const {
    int moving_average = 127;
    const int image_width = image.GetWidth();
    const int image_height = image.GetHeight();
    const int useoffset = 255-m_offset;
    
    int previous_line[image_width];
    for(int i=0;i<image_width;++i) { previous_line[i] = 127; }
    
    for(int i=0;i<image_height-1;) { // use height-1 so we dont overrun the image if its height is an odd number
      const unsigned char* data_pointer = image.GetRow(i);
      for(int j=0;j<image_width;++j) {
	const int pixel = *data_pointer;
	moving_average = pixel + moving_average - (moving_average >> m_window_size);
	int current_thresh = (moving_average + previous_line[j])>>1;
	previous_line[j] = moving_average;      
	dest.SetPixel(j,i, (pixel << m_window_size+8) < (current_thresh * useoffset) );
	++data_pointer;
      }

      ++i;
      data_pointer = image.GetRow(i) + image_width-1;
      for(int j=image_width-1;j>=0;--j) {
	const int pixel = *data_pointer;
	moving_average = pixel + moving_average - (moving_average >> m_window_size);
	int current_thresh = (moving_average + previous_line[j])>>1;
	previous_line[j] = moving_average;
	dest.SetPixel(j,i, (pixel << m_window_size+8) < (current_thresh * useoffset) );
	--data_pointer;
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
