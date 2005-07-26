/**
 * $Header$
 */

#ifndef IEEE1394_IMAGE_SOURCE_GUARD
#define IEEE1394_IMAGE_SOURCE_GUARD

#include <total/Config.hh>

#include <total/ImageSource.hh>

extern "C" {
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h> // libdc1394 interface
};

namespace Total {

  class IEEE1394ImageSource : public ImageSource<Colour::Grey> {
  private:
    raw1394handle_t      mHandle;
    dc1394_cameracapture mCamera;
    Image<Colour::Grey>  *mImage;
  
  public:

    IEEE1394ImageSource(
			char *device_name,
			int port,
			u_int64_t guid=0,
			int framerate= FRAMERATE_30,
			int shutter=-1,
			int gain=-1);
    virtual ~IEEE1394ImageSource();
    virtual Image<Colour::Grey>* Next();
    inline int GetWidth() const { return 640; }
    inline int GetHeight() const {  return 480; }

  };
}
#endif//V4L_IMAGE_SOURCE_GUARD
