/*
  Copyright (C) 2006 Andrew C. Rice

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

#ifndef UEYE_IMAGE_SOURCE_GUARD
#define UEYE_IMAGE_SOURCE_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_UEYE_H
#error "UEyeImageSource requires uEye.h from the IDS driver distribution"
#endif

extern "C" {
# include <uEye.h>
  // these #defines are to negate the min and max macros defined in the uEye header which leak out and confuse the preprocessor
  // eg. std::numeric_limits<float>::max() gets replaced with std::numeric_limits<float>::( ()>() ? () : () )
  // urgggh!
# define min(_x) min(_x)
# define max(_x) max(_x)
};

#include <cantag/Image.hh>
#include <cantag/ImageSource.hh>



#define BUFFER_SIZE 10
namespace Cantag {

  class CANTAG_EXPORT UEyeImageSource : public ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> {
  private:
    int m_width;
    int m_height;
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>  *m_image;
  
    HIDS m_hCamera;
    CAMINFO m_CamInfo;
    SENSORINFO m_SensorInfo;

    struct ImageBuffer {
      char *m_buf;
      INT m_imageID;

      ImageBuffer() : m_buf(NULL) {};
    };
    
    ImageBuffer m_ring_buffer[BUFFER_SIZE];

  public:

    UEyeImageSource(double fps = 60, double exposure = 0.0, int gain = 1);
    virtual ~UEyeImageSource();
    virtual Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* Next();
    inline int GetWidth() const { return m_width; }
    inline int GetHeight() const { return m_height; }

  };
}
#endif//UEYE_IMAGE_SOURCE_GUARD
