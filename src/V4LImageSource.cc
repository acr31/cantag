/**
 * $Header$
 */

#include <total/V4LImageSource.hh>

extern "C" {
#include <sys/types.h>  // for open()
#include <sys/stat.h>   // for open()
#include <fcntl.h>      // for open()
#include <sys/ioctl.h>  // for ioctl()
#include <unistd.h>     // for mmap()
#include <sys/mman.h>   // for mmap()
}

#include <cerrno>

#undef V4L_DEBUG

namespace Total {

  /**
   * Create a video4linux image source.  Takes a device i.e. /dev/video0
   * and the channel. The channel is the index of the channel in the
   * list of channels returned by the video card.
   *
   * \todo need someway of saying composite input
   */
  template<> V4LImageSource<Colour::Grey>::V4LImageSource(char* device, int channel) : 
    m_handle(-1), m_mmap((unsigned char*)MAP_FAILED,-1),
    m_slots(NULL),m_images(NULL)
  {
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
    if ((m_mmap.SetHandle((unsigned char*)mmap(0,mbuf.size,PROT_READ | PROT_WRITE,MAP_SHARED,m_handle.Get(),0),mbuf.size)).Get() == (unsigned char*)MAP_FAILED) {
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
    if (ioctl(m_handle.Get(),VIDIOCGPICT,&p) < 0) {
      throw "Failed to ioctl (VIDIOCGPICT) video device";
    }
  
    p.palette = VIDEO_PALETTE_GREY;
    p.depth = 8;
    if (ioctl(m_handle.Get(),VIDIOCSPICT,&p) < 0) {
      throw "Failed to ioctl (VIDIOCSPICT) video device";
    } 

#ifdef V4L_DEBUG
    PROGRESS("Configured palette");
#endif

    // create a (scoped) array of video_mmap structs - we use these when
    // we ask the capture card to asynchronously fetch the images for
    // us.
    m_slots = new struct video_mmap[m_total_frames];
    m_images = new Image<Colour::Grey>[m_total_frames];

    // populate the arrays
    for(int i=0;i<m_total_frames;i++) {
      m_slots[i].format = p.palette;
      m_slots[i].frame = i;
      m_slots[i].width = m_image_width;
      m_slots[i].height = m_image_height;
      m_images[i] = Image<Colour::Grey>(m_image_width,m_image_height,m_image_width,m_mmap.Get()+mbuf.offsets[i]);
      // start the device asynchronously fetching the frame
      if (i>0) { // dont start capturing for this one because we'll
	// start it when we first call next
	if (ioctl(m_handle.Get(),VIDIOCMCAPTURE,&(m_slots[i])) < 0) {
	  throw "Failed to ioctl (VIDIOCMCAPTURE) video device";
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

  template<> V4LImageSource<Colour::Grey>::~V4LImageSource() {
#ifdef V4L_DEBUG
    PROGRESS("Destroying image source");
#endif
    if (m_images) delete[] m_images;
    if (m_slots) delete[] m_slots;
  }

 template<> Image<Colour::Grey>* V4LImageSource<Colour::Grey>::Next() {  
    // start the device collecting the one we just used
    int rs;
    int retry = 0;
    while( (rs = ioctl(m_handle.Get(),VIDIOCMCAPTURE,&(m_slots[m_current_frame]))) == EBUSY && ++retry<=5);
    if (rs < 0) {
      throw "Failed to ioctl (VIDIOCMCAPTURE) video device";
    }

    m_current_frame++;
    m_current_frame%=m_total_frames;

    // collect the next image - block until its there
    while( (rs = ioctl(m_handle.Get(),VIDIOCSYNC,&(m_slots[m_current_frame].frame))) == EINTR);
    if (rs < 0) {
      throw "Failed to ioctl (VIDIOCSYNC) video device";
    }  

    Image<Colour::Grey>* result = &m_images[m_current_frame];
    result->SetValid(true);

    return result;
  }



  template<Colour::Type IMTYPE> V4LImageSource<IMTYPE>::VideoDevHandle::VideoDevHandle(int f_handle) : m_f_handle(f_handle) {};
  template<Colour::Type IMTYPE> typename V4LImageSource<IMTYPE>::VideoDevHandle& V4LImageSource<IMTYPE>::VideoDevHandle::SetHandle(int f_handle) { m_f_handle = f_handle; return *this;}
  template<Colour::Type IMTYPE> V4LImageSource<IMTYPE>::VideoDevHandle::~VideoDevHandle() { 
    if(m_f_handle >=0) {
#ifdef V4L_DEBUG
      PROGRESS("Closing handle");
#endif
      close(m_f_handle); 
    }
  }

  template<Colour::Type IMTYPE> int V4LImageSource<IMTYPE>::VideoDevHandle::Get() const { 
    return m_f_handle; 
  }

  template<Colour::Type IMTYPE> V4LImageSource<IMTYPE>::MMapHandle::MMapHandle(unsigned char* mmap_start,int size) : m_mmap_start(mmap_start),m_size(size) {};
  
template<Colour::Type IMTYPE> typename V4LImageSource<IMTYPE>::MMapHandle& V4LImageSource<IMTYPE>::MMapHandle::SetHandle(unsigned char* f_handle, int size) { m_mmap_start = f_handle; m_size=size; return *this; }

  template<Colour::Type IMTYPE> V4LImageSource<IMTYPE>::MMapHandle::~MMapHandle() { 
    if (m_mmap_start != (unsigned char*)MAP_FAILED) { 
#ifdef V4L_DEBUG
      PROGRESS("Unmapping memory");
#endif
      munmap(m_mmap_start,m_size); 
    }
  }

  template<Colour::Type IMTYPE> unsigned char* V4LImageSource<IMTYPE>::MMapHandle::Get() { 
    return m_mmap_start; 
  }
}
