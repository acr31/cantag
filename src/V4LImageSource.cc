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

extern "C" {
# include <linux/videodev.h> // for Video4Linux
};


/**
 * Create a video4linux image source.  Takes a device i.e. /dev/video0
 * and the channel. The channel is the index of the channel in the
 * list of channels returned by the video card.
 */
V4LImageSource::V4LImageSource(const char* device, const int channel) : m_handle(-1) {
  // open the video device, store the handle in the wrapper object.
  // When this instance is free'd the video port will be closed
  if ((m_handle = VideoDevHandle(open((const char*)devname_,O_RDWR))) < 0) {
    throw "Failed to open video port";
  }

  // load the capabilities of the video device
  video_capability capability;
  if (ioctl(m_handle,VIDIOCGCAP,&capability_) < 0) {
    throw "Failed to ioctl (VIDIOCGCAP) video device";
  }
  
  // check if this is a capture device
  if (capability.type & VID_TYPE_CAPTURE == 0) {
    throw "This device is not capable of video capture";
  }

  // check that the requested channel is in range
  if (channel > capability.channels-1) {
    throw "Channel out of range";
  }
  m_channel = channel;
  
  // find out the details of the buffer needed
  video_mbuf mbuf;
  if (ioctl(m_handle,VIDIOCGMBUF,&mbuf) < 0) {
    throw "Failed to ioctl (VIDIOCGMBUF) video device";
  }
  
  // memory map the right buffer size
  // the video device will read into n different frames sequentially
  // however the _total_ amount of memory needed for all of these
  // frames if given by mbuf.size
  if ((m_mmap = MMapHandle((uchar*)mmap(0,mbuf.size,PROT_READ,MAP_SHARED,m_handle,0),mbuf.size)) == MMAP_FAILED) {
    throw "Failed to mmap suitable buffer size";
  }
  m_total_frames = mbuf.frames;
  
  m_image_width = capability.maxwidth;
  m_image_height = capapbility.maxheight;

  // configure the device to give us greyscale images
  struct video_picture p;
  p.palette = VIDEO_PALETTE_GREY;
  if (ioctl(m_handle,VIDIOCSPICT,&p) < 0) {
    throw "Failed to ioctl (VIDIOCSPICT) video device";
  } 

  // create a (scoped) array of video_mmap structs - we use these when
  // we ask the capture card to asynchronously fetch the images for
  // us.
  m_slots = boost::scoped_array<video_mmap>(new struct video_mmap[m_total_frames]);
  m_images = boost::scoped_array<Image>(new Image[m_total_frames]);
  // populate the arrays
  for(int i=0;i<m_total_frames;i++) {
    m_slots[i].format = p.palette;
    m_slots[i].frame = i;
    m_slots[i].width = m_image_width;
    m_slots[i].height = m_image_height;
    m_images[i] = Image(m_image_width,m_image_height,mbuf.offsets[i]);
  }

  m_current_frame = 0;
  // start the device asynchronously fetching the next frame
  if (ioctl(m_handle,VIDIOCMCAPTURE,&(m_slots[0])) < 0) {
    throw "Failed to ioctl (VIDIOCMCAPTURE) video device";
  }
}

virtual Image* Next() {  
  // collect the next image - block until its there
  if (ioctl(m_handle,VIDIOCSYNC,m_slots[m_current_frame].frame) < 0) {
    throw "Failed to ioctl (VIDIOCSYNC) video device";
  }  

  Image* result = &m_images[m_current_frame];

  m_current_frame++;
  m_current_frame%=m_total_frames;

  // start the device collecting the next one
  if (ioctl(m_handle,VIDIOCMCAPTURE,&(m_slots[m_current_frame])) < 0) {
    throw "Failed to ioctl (VIDIOCMCAPTURE) video device";
  }
  return result;
}



V4LImageSource::VideoDevHandle::VideoDevHandle(int f_handle) : m_f_handle(f_handle) {};
V4LImageSource::VideoDevHandle::~VideoDevHandle() { if(m_f_handle >=0) close(m_f_handle); }
V4LImageSource::VideoDevHandle::operator int() const { return m_f_handle; }

V4LImageSource::MMapHandle::MMapHandle(uchar* mmap_start,int size) : m_mmap_start(mmap_start),m_size(size) {};
V4LImageSource::MMapHandle::~MMapHandle() { if (m_mmap_start != (uchar*)MAP_FAILED) { munmap(m_mmap_start,m_size); }
V4LImageSource::MMapHandle::operator int() const { return m_mmap_start; }
