/**
 * $Header$
 */

#include <total/MonochromeImage.hh>
#include <total/Image.hh>

namespace Total {

  MonochromeImage::MonochromeImage(unsigned int width, unsigned int height) : m_width(width),m_height(height),m_data(new bool[width*height])  {}

  MonochromeImage::~MonochromeImage() {
    delete[] m_data;
  }


  void MonochromeImage::Save(const char* filename) const {
    Image image(m_width,m_height);
    for(unsigned int i=0;i<m_height;++i) {
      unsigned char* data_pointer = image.GetRow(i);
      for(unsigned int j=0;j<m_width;++j) {
	*data_pointer = GetPixel(j,i) ? 255 : 0;
	++data_pointer;
      }
    }
    image.Save(filename);
  }
}
