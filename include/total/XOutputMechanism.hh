/**
 * $Header$
 *
 * Contains code contributed by Jon Davies <jjd27@cam.ac.uk>
 */ 

#ifndef X_OUTPUT_MECHANISM_GUARD
#define X_OUTPUT_MECHANISM_GUARD

#include <ostream>
#include <tripover/LocatedObject.hh>
#include <X11/Xlib.h>
template<int PAYLOAD_SIZE> 
class XOutputMechanism {
private:
  Display* m_display;
  Pixmap m_pixmap;
  GC m_gc;
  Window m_window;
  XWindowAttributes m_windowAttributes;
  int m_width;
  int m_height;

public:
  
  XOutputMechanism(int width,int height);
  void NewData(const Image* newdata);
  template<class C> void Output(SceneGraph<C,PAYLOAD_SIZE>& sg);

};

template<int PAYLOAD_SIZE> XOutputMechanism<PAYLOAD_SIZE>::XOutputMechanism(int width, int height) :
  m_width(width),
  m_height(height) {
 
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
  XSetForeground(m_display, m_gc, whiteColour);
  // Wait for MapNotify
  while (1) {
    XEvent e;
    XNextEvent(m_display, &e);
    if (e.type == MapNotify) {
      break;
    }
  }

  m_pixmap = XCreatePixmap(m_display, m_window, width, height, depth);
}


template<int PAYLOAD_SIZE> template<class C> void XOutputMechanism<PAYLOAD_SIZE>::Output(SceneGraph<C,PAYLOAD_SIZE>& sg) {
  XCopyArea(m_display, m_pixmap, m_window, m_gc, 0, 0, m_width, m_height, 0, 0);
  XFlush(m_display);
}

template<int PAYLOAD_SIZE> void XOutputMechanism<PAYLOAD_SIZE>::NewData(const Image* newdata) {
  const unsigned char* pointer = newdata->GetDataPointer();
  for (int y=0; y<m_height; ++y) {
      for (int x=0; x<m_width; ++x) {
	unsigned char data = *(pointer++);
	unsigned char rPart = data >> 3;
	unsigned char gPart = data >> 2;
	unsigned char bPart = data >> 3;

	unsigned long colourPixel = rPart<<11 | gPart<<5 | bPart;
	XSetForeground(m_display, m_gc, colourPixel); // in RGB565 format
	XDrawPoint(m_display, m_pixmap, m_gc, x, y);
    }
  }
}

#endif//X_OUTPUT_MECHANISM_GUARD
