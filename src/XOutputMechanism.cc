/**
 * $Header$
 */

#include <tripover/XOutputMechanism.hh>
extern "C" {
#include <sys/ipc.h>
#include <sys/shm.h>
}
#include <iostream>
XOutputMechanism::XOutputMechanism(int width, int height, const Camera& camera) :
  m_image(NULL),
  m_width(width),
  m_height(height),
  m_xattached(false),
  m_createdpixmap(false),
  m_shmat(false),
  m_shmgot(false),
  m_camera(camera)
{
 
  if ((m_display = XOpenDisplay(NULL)) == NULL) {
    throw "Failed to open display.\n";
  }

    // Get default colo
  int blackColour = BlackPixel(m_display, DefaultScreen(m_display));
  int whiteColour = WhitePixel(m_display, DefaultScreen(m_display));

  m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, m_width, m_height, 0, blackColour, blackColour);
  XStoreName(m_display, m_window, "TripOver");
  XGetWindowAttributes(m_display, m_window, &m_windowAttributes);
  int depth;
  depth = m_windowAttributes.depth;
  XSelectInput(m_display, m_window, StructureNotifyMask);
  XMapWindow(m_display, m_window);
  m_gc = XCreateGC(m_display, m_window, 0, 0);
  XSetForeground(m_display, m_gc, 0xFF);
  Visual* visual = DefaultVisualOfScreen(DefaultScreenOfDisplay(m_display));
  // Xshm stuff
  Status found = XShmQueryExtension(m_display);
  if (!found) {
    throw "No XShm Extension available";
  }
  
  m_image = XShmCreateImage(m_display,visual,depth,ZPixmap,NULL,&m_shminfo,m_width,m_height);
  if (!m_image) {
    throw "Failed to create shared image!";
  }

  m_shminfo.shmid = shmget(IPC_PRIVATE,m_image->bytes_per_line * m_image->height, IPC_CREAT|0777);
  if (m_shminfo.shmid == -1) {
    throw "Failed to create shared memory segment";
  }
  m_shmgot = true;

  void* addr = shmat(m_shminfo.shmid,0,0);
  if ((int)addr == -1) {
    throw "Failed to attach shared memory segment";
  }
  m_shminfo.shmaddr = m_image->data = (char*)addr;
  m_shmat = true;
  m_shminfo.readOnly = False;

  Status s = XShmAttach(m_display,&m_shminfo);
  if (s==0) {
    throw "Failed to attach shared memory to X server";
  }
  m_xattached = true;


  // Wait for MapNotify
  while (1) {
    XEvent e;
    XNextEvent(m_display, &e);
    if (e.type == MapNotify) {
      break;
    }
  }
}

XOutputMechanism::~XOutputMechanism() {
  if (m_xattached) XShmDetach(m_display,&m_shminfo);
  if (m_image) XDestroyImage(m_image);
  if (m_shmat) shmdt(m_shminfo.shmaddr);
  if (m_shmgot) shmctl(m_shminfo.shmid,IPC_RMID,0);
}

void XOutputMechanism::FromImageSource(const Image& image) {
  if (!m_image) { throw "Noimage"; }
  for (int y=0; y<m_height/2; ++y) {
    const unsigned char* pointer = image.GetRow(2*y);
    for (int x=0; x<m_width/2; ++x) {
      unsigned char data = *pointer;
      pointer+=2;
      unsigned char rPart = data >> 3;
      unsigned char gPart = data >> 2;
      unsigned char bPart = data >> 3;
      
      unsigned long colourPixel = rPart<<11 | gPart<<5 | bPart;
      XPutPixel(m_image,x,y,colourPixel);
    }
  }
}

void XOutputMechanism::FromThreshold(const Image& image) {
  if (!m_image) { throw "Noimage"; }
  for (int y=0; y<m_height/2; ++y) {
    const unsigned char* pointer = image.GetRow(2*y);
    for (int x=m_width/2; x<m_width; ++x) {
      unsigned char data = *pointer;
      pointer+=2;
      unsigned long colourPixel = data == 0 ? 0 : ((1<<16)-1);
      XPutPixel(m_image,x,y,colourPixel);
    }
  }
}

void XOutputMechanism::FromContourTree(const ContourTree& contours) {
  if (!m_image) { throw "Noimage"; }
  for(int x=0;x<m_width/2;++x) {
    for(int y=m_height/2;y<m_height;++y) {
      XPutPixel(m_image,x,y,(1<<16)-1);
    }
  }
  FromContourTree(contours.GetRootContour());
}

void XOutputMechanism::FromContourTree(const ContourTree::Contour* contour) {
  if (!contour->weeded) {
    for(std::vector<float>::const_iterator i = contour->points.begin();
	i != contour->points.end();
	++i) {
      const float x = *i;
      ++i;
      const float y = *i;
      XPutPixel(m_image,(int)(x/2),(int)(y/2+m_height/2),0);
    }
  }
  for(std::vector<ContourTree::Contour*>::const_iterator i = contour->children.begin();
      i != contour->children.end();
      ++i) {
    FromContourTree(*i);
  }
}


void XOutputMechanism::Flush() {
  //  XShmPutImage(m_display,m_window,m_gc,m_image,0,0,0,0,m_width,m_height,false);
  XFlush(m_display);
}
