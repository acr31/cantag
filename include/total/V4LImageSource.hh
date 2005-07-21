/**
 * $Header$
 */

#ifndef V4L_IMAGE_SOURCE_GUARD
#define V4L_IMAGE_SOURCE_GUARD

#include <total/Config.hh>

#ifndef HAVE_LINUX_VIDEODEV_H
#error This version has been configured without Video4Linux support
#endif

#include <total/ImageSource.hh>

extern "C" {
# include <linux/videodev.h> // for Video4Linux
};

namespace Total {

  template <Colour::Type IMTYPE> class V4LImageSource : public ImageSource<IMTYPE> {
  private:
  
    class VideoDevHandle {
    private:
      int m_f_handle;
    public:
      VideoDevHandle(int f_handle);
      ~VideoDevHandle();
      int Get() const;
      VideoDevHandle& SetHandle(int);
    };

    class MMapHandle {
    private:
      unsigned char* m_mmap_start;
      int m_size;
    public:
      MMapHandle(unsigned char* mmap_start, int size);
      ~MMapHandle();
      unsigned char* Get();
      MMapHandle& SetHandle(unsigned char*,int);
    };

    VideoDevHandle m_handle;
    MMapHandle  m_mmap;
    int m_channel;
    int m_current_frame;
    int m_total_frames;
    int m_image_width;
    int m_image_height;
    video_mmap* m_slots;
    Image<Colour::Grey> * m_images;

  public:

    V4LImageSource(char* deviceName, int channel);
    virtual ~V4LImageSource();
    virtual Image<IMTYPE>* Next();
    inline int GetWidth() const { return m_image_width; }
    inline int GetHeight() const {  return m_image_height; }

  };
}
#endif//V4L_IMAGE_SOURCE_GUARD
