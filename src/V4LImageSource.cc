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
#include <cerrno>
#define V4L_DEBUG

/**
 * Create a video4linux image source.  Takes a device i.e. /dev/video0
 * and the channel. The channel is the index of the channel in the
 * list of channels returned by the video card.
 */
V4LImageSource::V4LImageSource(const char* device, const int channel) : 
  m_handle(-1), m_mmap((uchar*)MAP_FAILED,-1),
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
  v4l2_capability capability;
  if (ioctl(m_handle.Get(),VIDIOC_QUERYCAP,&capability) < 0) {
    throw "Failed to ioctl (VIDIOCGCAP) video device";
  }

#ifdef V4L_DEBUG
  PROGRESS("Read video capability");
  PROGRESS(" driver: " << capability.driver);
  PROGRESS(" name: " << capability.card);
  PROGRESS(" location: " << capability.bus_info);
  PROGRESS(" version: " << 
	   ((capability.version >> 16) & 0xFF) << "." <<
	   ((capability.version >> 8) & 0xFF) << "." <<
	   (capability.version & 0xFF));
  PROGRESS(" capabilities: " << capability.capabilities);
#endif
  
  // check if this is a capture device
  if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE == 0) {
    throw "This device is not capable of video capture";
  }

#ifdef V4L_DEBUG
  PROGRESS("This is a capture device");
#endif

  // check that the requested channel is in range
  v4l2_input channel_info;
  channel_info.index = channel;
  if (ioctl(m_handle.Get(),VIDIOC_ENUMINPUT,&channel_info) < 0) {
    throw "Channel out of range";
  }
  m_channel = channel;

#ifdef V4L_DEBUG
  PROGRESS("Accepted selected channel " << channel_info.name);
#endif
  
  v4l2_requestbuffers request;
  request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request.memory = V4L2_MEMORY_USERPTR;
  if (ioctl(m_handle.Get(),VIDIOC_REQBUFS,&request) < 0) {
    throw "Failed to ioctl (VIDIOC_REQBUFS) video device";
  }

  v4l2_format format;
  memset(&format,0,sizeof(format));
  format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(m_handle.Get(),VIDIOC_G_FMT,&format) < 0) {
    throw "Failed to ioctl (VIDIOC_G_FMT) video device";
  }

#ifdef V4L_DEBUG
  PROGRESS("Collected format information");
  PROGRESS(" width = " << format.fmt.pix.width);
  PROGRESS(" height = " << format.fmt.pix.height);
  PROGRESS(" pixelformat = " << format.fmt.pix.pixelformat);
  PROGRESS(" field = " << format.fmt.pix.field);
  PROGRESS(" bytesperline = " << format.fmt.pix.bytesperline);
  PROGRESS(" sizeimage = " << format.fmt.pix.sizeimage);
  PROGRESS(" colorspace = " << format.fmt.pix.colorspace);
#endif


  m_image_width = format.fmt.pix.width;
  m_image_height = format.fmt.pix.height;

  format.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;

  // configure the device to give us greyscale images
  if (ioctl(m_handle.Get(),VIDIOC_S_FMT,&format) < 0) {
      throw "Failed to ioctl (VIDIOC_S_FMT) video device";
  }

#ifdef V4L_DEBUG
  PROGRESS("Configured palette");
#endif

  m_total_frames = 4;
  m_slots = new v4l2_buffer[m_total_frames];
  m_images = new Image[m_total_frames];

  // populate the arrays
  for(int i=0;i<m_total_frames;i++) {
    memset(m_slots+i,0,sizeof(v4l2_buffer));
    m_images[i] = Image(m_image_width,m_image_height);
    m_slots[i].index = i;
    m_slots[i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    m_slots[i].memory = V4L2_MEMORY_USERPTR;
    m_slots[i].length = m_image_width*m_image_height;
    m_slots[i].m.userptr = (unsigned long)m_images[i].GetDataPointer();
    // start the device asynchronously fetching the frame
    if (i>0) { // dont start capturing for this one because we'll
	       // start it when we first call next
      if (ioctl(m_handle.Get(),VIDIOC_QBUF,&(m_slots[i])) < 0) {
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

V4LImageSource::~V4LImageSource() {
#ifdef V4L_DEBUG
  PROGRESS("Destroying image source");
#endif
  if (m_images) delete[] m_images;
  if (m_slots) delete[] m_slots;
}

Image* V4LImageSource::Next() {  
  // start the device collecting the one we just used
  int rs;
  int retry = 0;
  while(rs = ioctl(m_handle.Get(),VIDIOC_QBUF,&(m_slots[m_current_frame])) == EBUSY && ++retry<=5);
  if (rs < 0) {
    throw "Failed to ioctl (VIDIOC_QBUF) video device";
  }

  m_current_frame++;
  m_current_frame%=m_total_frames;

  v4l2_buffer request;
  memset(&request,0,sizeof(v4l2_buffer));
  request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  request.memory = V4L2_MEMORY_USERPTR;

  // collect the next image - block until its there
  while(rs = ioctl(m_handle.Get(),VIDIOC_DQBUF,&request) == EINTR);
  if (rs < 0) {
    throw "Failed to ioctl (VIDIOC_DQBUF) video device";
  }  

  Image* result = &m_images[m_current_frame];

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
