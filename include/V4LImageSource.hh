/**
 * $Header$
 */

#ifndef V4L_IMAGE_SOURCE_GUARD
#define V4L_IMAGE_SOURCE_GUARD

#include <Config.hh>
#include <ImageSource.hh>
#include <boost/scoped_ptr.hpp>

class V4LImageSource : public ImageSource {
private:
  VideoDevHandle m_handle;
  MMapHandle  m_mmap;
  int m_channel;
  int m_current_frame;
  int m_total_frames;
  int m_image_width;
  int m_image_height;
  boost::scoped_array<video_mmap> m_slots;
  boost::scoped_array<Image> m_images;

  class VideoDevHandle {
  private:
    int m_f_handle;
  public:
    VideoDevHandle(int f_handle);
    ~VideoDevHandle();
    operator int() const;
  }

  class MMapHandle {
  private:
    int m_mmap_start;
    int m_size;
  public:
    MMapHandle(uchar* mmap_start, int size);
    ~MMapHandle();
    operator int() const;
  }

public:

  V4LImageSource(const char* deviceName);

  virtual Image* Next();

}

#endif//V4L_IMAGE_SOURCE_GUARD
