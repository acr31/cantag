/**
 * $Header$
 */

#include <tripover/XOutputMechanism.hh>

extern "C" {
#include <sys/ipc.h>
#include <sys/shm.h>
}

#undef XOUTPUT_DEBUG

XOutputMechanism::XOutputMechanism(int width, int height, const Camera& camera) :
  m_xattached(false),
  m_createdpixmap(false),
  m_shmat(false),
  m_shmgot(false),
  m_gcgot(false),
  m_mapped(false),
  m_windowgot(false),
  m_displaygot(false),
  m_colormapgot(false),
  m_camera(camera),
  m_image(NULL),
  m_width(width),
  m_height(height)
{
 
  if ((m_display = XOpenDisplay(NULL)) == NULL) {
    throw "Failed to open display.\n";
  }
  m_displaygot = true;

  int blackColour = BlackPixel(m_display, DefaultScreen(m_display));
  m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, m_width, m_height, 0, blackColour, blackColour);
  m_windowgot = true;
  XStoreName(m_display, m_window, "TripOver");
  XWindowAttributes windowAttributes;
  XGetWindowAttributes(m_display, m_window, &windowAttributes);
  int depth;
  depth = windowAttributes.depth;
  XSelectInput(m_display, m_window, StructureNotifyMask);
  XMapWindow(m_display, m_window);
  m_mapped = true;

  Visual* visual = DefaultVisualOfScreen(DefaultScreenOfDisplay(m_display));
  
  m_colormap = XCreateColormap(m_display,m_window,visual,0);
  m_colormapgot = true;

  XColor xc;
  xc.red=0;
  xc.green=65535;
  xc.blue=0;
  
  if (!XAllocColor(m_display,m_colormap,&xc)) {
    throw "Failed to allocate colour for highlighting!";
  }


  XGCValues values;
  values.foreground = xc.pixel;
  values.background = blackColour;
  values.line_width = 2;

  m_gc = XCreateGC(m_display, m_window, (GCForeground | GCBackground | GCLineWidth), &values);
  m_gcgot = true;

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

  // work out the number of bits for each of rgb and where to shift them to
  unsigned long red = m_image->red_mask;
  m_redbits = 0;
  m_redshift = 0;
  while(! (red & 0x1) ) { m_redshift++; red >>= 1; }
  while( red & 0x1 ) { m_redbits++; red >>= 1; }
  unsigned long green = m_image->green_mask;
  m_greenbits = 0;
  m_greenshift = 0;
  while(! (green & 0x1) ) { m_greenshift++; green >>= 1; }
  while( green & 0x1 ) { m_greenbits++; green >>= 1; }
  unsigned long blue = m_image->blue_mask;
  m_bluebits = 0;
  m_blueshift = 0;
  while(! (blue & 0x1) ) { m_blueshift++; blue >>= 1; }
  while( blue & 0x1 ) { m_bluebits++; blue >>= 1; }

#ifdef XOUTPUT_DEBUG
  PROGRESS("Red mask is " << m_image->red_mask << " = " << m_redbits << " shifted by " << m_redshift);
  PROGRESS("Green mask is " << m_image->green_mask << " = " << m_greenbits << " shifted by " << m_greenshift);
  PROGRESS("Blue mask is " << m_image->blue_mask << " = " << m_bluebits << " shifted by " << m_blueshift);
#endif

  m_bytes_per_pixel = m_image->bits_per_pixel/8;

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
  if (m_colormapgot) XFreeColormap(m_display,m_colormap);
  if (m_gcgot) XFreeGC(m_display,m_gc);
  if (m_mapped) XUnmapWindow(m_display,m_window);
  if (m_windowgot) XDestroyWindow(m_display,m_window);
  if (m_displaygot) XCloseDisplay(m_display);
}

void XOutputMechanism::FromImageSource(const Image& image) {
  for (int y=0; y<m_height; ++y) {
    const unsigned char* pointer = image.GetRow(y);
    char* destptr = m_image->data + m_image->bytes_per_line * y;
    for (int x=0; x<m_width; ++x) {
      unsigned char data = *pointer;
      pointer++;
      unsigned long rPart = data;
      int diff = m_redbits - 8;
      rPart = diff > 0 ? (rPart << diff) : (rPart >> (-diff));
      rPart <<= m_redshift;

      unsigned long gPart = data;
      diff = m_greenbits - 8;
      gPart = diff > 0 ? (gPart << diff) : (gPart >> (-diff));
      gPart <<= m_greenshift;

      unsigned long bPart = data;
      diff = m_bluebits - 8;
      bPart = diff > 0 ? (bPart << diff) : (bPart >> (-diff));
      bPart <<= m_blueshift;      

      unsigned long value = rPart | gPart | bPart;
      for(int i=0;i<m_bytes_per_pixel;++i) {
	*destptr = value & 0xFF;
	destptr++;
	value>>=8;
      }
    }
  }
}

void XOutputMechanism::Flush() {
  XFlush(m_display);
}
