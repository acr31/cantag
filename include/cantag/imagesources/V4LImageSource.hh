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

/**
 * $Header$
 */

#ifndef V4L_IMAGE_SOURCE_GUARD
#define V4L_IMAGE_SOURCE_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_LINUX_VIDEODEV_H
#error This version has been configured without Video4Linux support
#endif

#include <cantag/imagesources/ImageSource.hh>

extern "C" {
#include <linux/videodev.h> // for Video4Linux
#include <sys/types.h>  // for open()
#include <sys/stat.h>   // for open()
#include <fcntl.h>      // for open()
#include <sys/ioctl.h>  // for ioctl()
#include <unistd.h>     // for mmap()
#include <sys/mman.h>   // for mmap()
}

#include <cerrno>

#define V4L_DEBUG


namespace Cantag {

  template <Pix::Sze::Bpp size,Pix::Fmt::Layout layout> class V4LImageSource : public ImageSource<size,layout> {
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
      MMapHandle(unsigned char* mmap_start, int mmap_size);
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
    Image<size,layout>** m_images;

  public:

    V4LImageSource(const char* deviceName, int channel);
    virtual ~V4LImageSource();
    virtual Image<size,layout>* Next();
    inline int GetWidth() const { return m_image_width; }
    inline int GetHeight() const {  return m_image_height; }

  };

  /**
   * Create a video4linux image source.  Takes a device i.e. /dev/video0
   * and the channel. The channel is the index of the channel in the
   * list of channels returned by the video card.
   *
   * \todo need someway of saying composite input
   *
   * Added Colour support: Alastair R. Beresford.
   */
  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> V4LImageSource<size,layout>::V4LImageSource(const char* device, int channel) : 
    m_handle(-1), m_mmap((unsigned char*)MAP_FAILED,-1),
    m_slots(NULL),m_images(NULL)
  {
    // open the video device, store the handle in the wrapper object.
    // When this instance is free'd the video port will be closed

    if ((m_handle.SetHandle(open(device,O_RDWR))).Get() < 0) {
      throw "V4L: Failed to open video device";
    }


#ifdef V4L_DEBUG
    PROGRESS("Opened video port");
#endif

    // load the capabilities of the video device
    video_capability capability;
    if (ioctl(m_handle.Get(),VIDIOCGCAP,&capability) < 0) {
      throw "V4L: Failed to ioctl (VIDIOCGCAP) video device";
    }

#ifdef V4L_DEBUG
    PROGRESS("Read video capability");
    PROGRESS(" name: " << capability.name);
    PROGRESS(" type: " << capability.type);
    PROGRESS(" channels: " << capability.channels);
    PROGRESS(" audios: " << capability.audios);
    PROGRESS(" maxwidth: " << capability.maxwidth);
    PROGRESS(" maxheight: " << capability.maxheight);
    PROGRESS(" minwidth: " << capability.minwidth);
    PROGRESS(" minheight: " << capability.minheight);
#endif
  
    // check if this is a capture device
    if ((capability.type & VID_TYPE_CAPTURE) == 0) {
      throw "V4L: This device is not capable of video capture";
    }

#ifdef V4L_DEBUG
    PROGRESS("This is a capture device");
#endif

    // check that the requested channel is in range
    if (channel > capability.channels-1) {
      throw "V4L: Channel out of range";
    }
    m_channel = channel;

#ifdef V4L_DEBUG
    PROGRESS("Accepted selected channel");
#endif
  
    // find out the details of the buffer needed
    video_mbuf mbuf;
    if (ioctl(m_handle.Get(),VIDIOCGMBUF,&mbuf) < 0) {
      throw "V4L: Failed to ioctl (VIDIOCGMBUF) video device";
    }

#ifdef V4L_DEBUG
    PROGRESS("Found buffer details");
    PROGRESS(" size: "<<mbuf.size);
    PROGRESS(" frames: " << mbuf.frames);
    for(int i=0;i<mbuf.frames;i++) {
      PROGRESS(" offset"<<i<<": " << mbuf.offsets[i]);
    }
#endif
  
    // memory map the right buffer size
    // the video device will read into n different frames sequentially
    // however the _total_ amount of memory needed for all of these
    // frames if given by mbuf.size
    if ((m_mmap.SetHandle((unsigned char*)mmap(0,mbuf.size,PROT_READ | PROT_WRITE,MAP_SHARED,m_handle.Get(),0),mbuf.size)).Get() == (unsigned char*)MAP_FAILED) {
      throw "V4L: Failed to mmap suitable buffer size";
    }
    m_total_frames = mbuf.frames;
  
    m_image_width = capability.maxwidth;
    m_image_height = capability.maxheight;

#ifdef V4L_DEBUG
    PROGRESS("Successfully mmapped file");
#endif

    // configure the device to give us greyscale images
    struct video_picture p;
    if (ioctl(m_handle.Get(),VIDIOCGPICT,&p) < 0) {
      throw "V4L: Failed to ioctl (VIDIOCGPICT) video device";
    }
  
    if (size==Pix::Sze::Byte1 && layout==Pix::Fmt::Grey8) {
#ifdef V4L_DEBUG
      PROGRESS("Selecting Grey palatte support");
#endif
      p.palette = VIDEO_PALETTE_GREY;
      p.depth = 8;
      if (ioctl(m_handle.Get(),VIDIOCSPICT,&p) < 0) {
	throw "V4L: Failed to ioctl (VIDIOCSPICT) video device for GREY palette";
      } 
    }
    if (size==Pix::Sze::Byte3 && layout==Pix::Fmt::BGR24) {
#ifdef V4L_DEBUG
      PROGRESS("Selecting RGB24 palatte support");
#endif
      p.palette = VIDEO_PALETTE_RGB24; //on big-endian machines we want to reverse bytes
      p.depth = 24;
      if (ioctl(m_handle.Get(),VIDIOCSPICT,&p) < 0) {
	throw "V4L: Failed to ioctl (VIDIOCSPICT) video device for RGB24 palette";
      } 
    }

    if ( !(size==Pix::Sze::Byte3 && layout==Pix::Fmt::BGR24) &&
	 !(size==Pix::Sze::Byte1 && layout==Pix::Fmt::Grey8)) {
      //\todo: this can be (and should) be detected at compile time
      throw "V4L: Failed: unsupported template type";
    }
      
#ifdef V4L_DEBUG
    PROGRESS("Configured palette");
#endif

    // create a (scoped) array of video_mmap structs - we use these when
    // we ask the capture card to asynchronously fetch the images for
    // us.
    m_slots = new struct video_mmap[m_total_frames];
    m_images = new Image<size,layout>*[m_total_frames];

    for(int i=0;i<m_total_frames;i++) {
      m_images[i] = NULL;
    }

    // populate the arrays
    for(int i=0;i<m_total_frames;i++) {
      m_slots[i].format = p.palette;
      m_slots[i].frame = i;
      m_slots[i].width = m_image_width;
      m_slots[i].height = m_image_height;
      switch(size) {
      case Pix::Sze::Byte1:
	m_images[i] = new Image<size,layout>(m_image_width,m_image_height,m_image_width,m_mmap.Get()+mbuf.offsets[i]);
	break;
      case Pix::Sze::Byte3:
	m_images[i] = new Image<size,layout>(m_image_width,m_image_height,m_image_width*3,m_mmap.Get()+mbuf.offsets[i]);
	break;
      default:
	throw "V4L: Failed: unsupported template type";
      }

      // start the device asynchronously fetching the frame
      if (i>0) { // dont start capturing for this one because we'll
	// start it when we first call next
	if (ioctl(m_handle.Get(),VIDIOCMCAPTURE,&(m_slots[i])) < 0) {
	  throw "V4L: Failed to ioctl (VIDIOCMCAPTURE) video device";
	}
      }
    }

#ifdef V4L_DEBUG
    PROGRESS("Created slots and corresponding image cache");
#endif

    m_current_frame = 0;
  
#ifdef V4L_DEBUG
    PROGRESS("Initiated asynchronous capture");
#endif
  }

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> V4LImageSource<size,layout>::~V4LImageSource() {
#ifdef V4L_DEBUG
    PROGRESS("Destroying image source");
#endif
    if (m_images) {
      for (int i=0;i<m_total_frames;i++)
	delete m_images[i];
      delete[] m_images;
    }
    if (m_slots) delete[] m_slots;
  }

 template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> Image<size,layout>* V4LImageSource<size,layout>::Next() {  
    // start the device collecting the one we just used
    int rs;
    int retry = 0;
    while( (rs = ioctl(m_handle.Get(),VIDIOCMCAPTURE,&(m_slots[m_current_frame]))) == EBUSY && ++retry<=5);
    if (rs < 0) {
      throw "V4L: Failed to ioctl (VIDIOCMCAPTURE) video device";
    }

    m_current_frame++;
    m_current_frame%=m_total_frames;

    // collect the next image - block until its there
    while( (rs = ioctl(m_handle.Get(),VIDIOCSYNC,&(m_slots[m_current_frame].frame))) == EINTR);
    if (rs < 0) {
      throw "V4L: Failed to ioctl (VIDIOCSYNC) video device";
    }  

    Image<size,layout>* result = m_images[m_current_frame];
    result->SetValid(true);

    return result;
  }



  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> V4LImageSource<size,layout>::VideoDevHandle::VideoDevHandle(int f_handle) : m_f_handle(f_handle) {};
  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> typename V4LImageSource<size,layout>::VideoDevHandle& V4LImageSource<size,layout>::VideoDevHandle::SetHandle(int f_handle) { m_f_handle = f_handle; return *this;}
  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> V4LImageSource<size,layout>::VideoDevHandle::~VideoDevHandle() { 
    if(m_f_handle >=0) {
#ifdef V4L_DEBUG
      PROGRESS("Closing handle");
#endif
      close(m_f_handle); 
    }
  }

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> int V4LImageSource<size,layout>::VideoDevHandle::Get() const { 
    return m_f_handle; 
  }

  template<Pix::Sze::Bpp _size,Pix::Fmt::Layout layout> V4LImageSource<_size,layout>::MMapHandle::MMapHandle(unsigned char* mmap_start,int mmap_size) : m_mmap_start(mmap_start),m_size(mmap_size) {};
  
template<Pix::Sze::Bpp _size,Pix::Fmt::Layout layout> typename V4LImageSource<_size,layout>::MMapHandle& V4LImageSource<_size,layout>::MMapHandle::SetHandle(unsigned char* f_handle, int size) { m_mmap_start = f_handle; m_size=size; return *this; }

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> V4LImageSource<size,layout>::MMapHandle::~MMapHandle() { 
    if (m_mmap_start != (unsigned char*)MAP_FAILED) { 
#ifdef V4L_DEBUG
      PROGRESS("Unmapping memory");
#endif
      munmap(m_mmap_start,m_size); 
    }
  }

  template<Pix::Sze::Bpp size,Pix::Fmt::Layout layout> unsigned char* V4LImageSource<size,layout>::MMapHandle::Get() { 
    return m_mmap_start; 
  }

}
#endif//V4L_IMAGE_SOURCE_GUARD
