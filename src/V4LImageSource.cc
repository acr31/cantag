/**
 * $Header$
 */

#include <V4LImageSource.hh>

#include <sys/types.h>  // for open()
#include <sys/stat.h>   // for open()
#include <fcntl.h>      // for open()
#include <sys/ioctl.h>  // for ioctl()
#include <unistd.h>     // for mmap()
#include <sys/mman.h>   // for mmap()

#define V4L_DEBUG

/**
 * Create a video4linux image source.  Takes a device i.e. /dev/video0
 * and the channel. The channel is the index of the channel in the
 * list of channels returned by the video card.
 */
V4LImageSource::V4LImageSource(const char* device, const int channel) : m_handle(-1), m_mmap((uchar*)MAP_FAILED,-1) {
  // open the video device, store the handle in the wrapper object.
  // When this instance is free'd the video port will be closed


  if ((m_handle.SetHandle(open(device,O_RDWR))).Get() < 0) {
    throw "Failed to open video port";
  }


#ifdef V4L_DEBUG
  PROGRESS("Opened video port");
#endif

  // load the capabilities of the video device
  video_capability capability;
  if (ioctl(m_handle.Get(),VIDIOCGCAP,&capability) < 0) {
    throw "Failed to ioctl (VIDIOCGCAP) video device";
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
  if (capability.type & VID_TYPE_CAPTURE == 0) {
    throw "This device is not capable of video capture";
  }

#ifdef V4L_DEBUG
  PROGRESS("This is a capture device");
#endif

  // check that the requested channel is in range
  if (channel > capability.channels-1) {
    throw "Channel out of range";
  }
  m_channel = channel;

#ifdef V4L_DEBUG
  PROGRESS("Accepted selected channel");
#endif
  
  // find out the details of the buffer needed
  video_mbuf mbuf;
  if (ioctl(m_handle.Get(),VIDIOCGMBUF,&mbuf) < 0) {
    throw "Failed to ioctl (VIDIOCGMBUF) video device";
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
  if ((m_mmap.SetHandle((uchar*)mmap(0,mbuf.size,PROT_READ,MAP_SHARED,m_handle.Get(),0),mbuf.size)).Get() == (uchar*)MAP_FAILED) {
    throw "Failed to mmap suitable buffer size";
  }
  m_total_frames = mbuf.frames;
  
  m_image_width = capability.maxwidth;
  m_image_height = capability.maxheight;

#ifdef V4L_DEBUG
  PROGRESS("Successfully mmapped file");
#endif

  // configure the device to give us greyscale images
  struct video_picture p;
  p.palette = VIDEO_PALETTE_GREY;
  if (ioctl(m_handle.Get(),VIDIOCSPICT,&p) < 0) {
    throw "Failed to ioctl (VIDIOCSPICT) video device";
  } 

#ifdef V4L_DEBUG
  PROGRESS("Configured palette");
#endif

  // create a (scoped) array of video_mmap structs - we use these when
  // we ask the capture card to asynchronously fetch the images for
  // us.
  boost::scoped_array<video_mmap> temp(new struct video_mmap[m_total_frames]);
  m_slots.swap(temp);
  boost::scoped_array<Image> temp2(new Image[m_total_frames]) ;
  m_images.swap(temp2);

  // populate the arrays
  for(int i=0;i<m_total_frames;i++) {
    m_slots[i].format = p.palette;
    m_slots[i].frame = i;
    m_slots[i].width = m_image_width;
    m_slots[i].height = m_image_height;
    m_images[i] = Image(m_image_width,m_image_height,m_handle.Get()+(uchar*)mbuf.offsets[i]);
  }

#ifdef V4L_DEBUG
  PROGRESS("Created slots and corresponding image cache");
#endif

  m_current_frame = 0;
  // start the device asynchronously fetching the next frame
  if (ioctl(m_handle.Get(),VIDIOCMCAPTURE,&(m_slots[0])) < 0) {
    throw "Failed to ioctl (VIDIOCMCAPTURE) video device";
  }

#ifdef V4L_DEBUG
  PROGRESS("Initiated asynchronous capture");
#endif
}

V4LImageSource::~V4LImageSource() {
#ifdef V4L_DEBUG
  PROGRESS("Destroying image source");
#endif
}

Image* V4LImageSource::Next() {  
  // collect the next image - block until its there
  if (ioctl(m_handle.Get(),VIDIOCSYNC,&(m_slots[m_current_frame].frame)) < 0) {
    throw "Failed to ioctl (VIDIOCSYNC) video device";
  }  

  Image* result = &m_images[m_current_frame];

  m_current_frame++;
  m_current_frame%=m_total_frames;

  // start the device collecting the next one
  if (ioctl(m_handle.Get(),VIDIOCMCAPTURE,&(m_slots[m_current_frame])) < 0) {
    throw "Failed to ioctl (VIDIOCMCAPTURE) video device";
  }
  return result;
}



V4LImageSource::VideoDevHandle::VideoDevHandle(int f_handle) : m_f_handle(f_handle) {};
V4LImageSource::VideoDevHandle& V4LImageSource::VideoDevHandle::SetHandle(int f_handle) { m_f_handle = f_handle; return *this;}
V4LImageSource::VideoDevHandle::~VideoDevHandle() { 
  if(m_f_handle >=0) {
#ifdef V4L_DEBUG
    PROGRESS("Closing handle");
#endif
    close(m_f_handle); 
  }
}

int V4LImageSource::VideoDevHandle::Get() const { 
  return m_f_handle; 
}

V4LImageSource::MMapHandle::MMapHandle(uchar* mmap_start,int size) : m_mmap_start(mmap_start),m_size(size) {};
V4LImageSource::MMapHandle& V4LImageSource::MMapHandle::SetHandle(uchar* f_handle, int size) { m_mmap_start = f_handle; m_size=size; return *this; }
V4LImageSource::MMapHandle::~MMapHandle() { 
  if (m_mmap_start != (uchar*)MAP_FAILED) { 
#ifdef V4L_DEBUG
    PROGRESS("Unmapping memory");
#endif
    munmap(m_mmap_start,m_size); 
  }
}

uchar* V4LImageSource::MMapHandle::Get() { 
  return m_mmap_start; 
}
