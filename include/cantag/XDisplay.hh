#ifndef X_DISPLAY
#define X_DISPLAY

#define TEXT_DEBUG
#include <cantag/Config.hh>

#ifndef HAVE_X11_XLIB_H
# error This version has been configured without Xlib support
#endif
#ifndef HAVE_X11_XUTIL_H
# error This version has been configured without XUtil support
#endif
#ifndef HAVE_X11_EXTENSIONS_XSHM_H
# error This version has been configured without MIT XSHM support
#endif

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
}

#include <Cantag.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/Function.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Camera.hh>

#define XDISPLAY_DEBUG

namespace Cantag {

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> class XDisplay {
  private:
    Colormap m_colormap;

    bool m_xattached;
    
    bool m_gcgot;
    bool m_mapped;
    bool m_windowgot;
    bool m_displaygot;
    bool m_colormapgot;
    Display* m_display;
    GC m_gc;
    Window m_window;
    int m_width;
    int m_height;
    bool m_needPut; 

    class XSHMAttachedImage {
    public:
      bool m_xattached;
      bool m_shmat;
      bool m_shmgot;
      XImage* m_image;
      Display* m_display;
      XShmSegmentInfo m_shminfo;
      int m_redbits;
      int m_bluebits;
      int m_greenbits;
      int m_redshift;
      int m_blueshift;
      int m_greenshift;
      int m_bytes_per_pixel; 

      XSHMAttachedImage(Display* display, Visual* visual, int depth, int width, int height) : m_xattached(false),m_shmat(false),m_shmgot(false),m_image(NULL),m_display(display) {
	m_image = XShmCreateImage(m_display,visual,depth,ZPixmap,NULL,&m_shminfo,width,height);
	if (!m_image) throw "Failed to create shared image!";

	m_shminfo.shmid = shmget(IPC_PRIVATE,m_image->bytes_per_line * m_image->height, IPC_CREAT|0777);
	if (m_shminfo.shmid == -1) throw "Failed to create shared memory segment";
	m_shmgot = true;
	
	void* addr = shmat(m_shminfo.shmid,0,0);
	if ((int)addr == -1) throw "Failed to attach shared memory segment";
	m_shminfo.shmaddr = m_image->data = (char*)addr;
	m_shmat = true;
	m_shminfo.readOnly = False;
	
	Status s = XShmAttach(m_display,&m_shminfo);
	if (s==0) throw "Failed to attach shared memory to X server";
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
	
#ifdef XDISPLAY_DEBUG
	PROGRESS("Red mask is " << m_image->red_mask << " = " << m_redbits << " shifted by " << m_redshift);
	PROGRESS("Green mask is " << m_image->green_mask << " = " << m_greenbits << " shifted by " << m_greenshift);
	PROGRESS("Blue mask is " << m_image->blue_mask << " = " << m_bluebits << " shifted by " << m_blueshift);
	PROGRESS("Bits per pixel is " << m_image->bits_per_pixel);
#endif
	m_bytes_per_pixel = m_image->bits_per_pixel/8;

      }

      ~XSHMAttachedImage() {
	if (m_xattached) XShmDetach(m_display,&m_shminfo);
	if (m_image) XDestroyImage(m_image);
	if (m_shmat) shmdt(m_shminfo.shmaddr);
	if (m_shmgot) shmctl(m_shminfo.shmid,IPC_RMID,0);
      }
    };

    XSHMAttachedImage* m_image[2];
    int m_displayed_image;

    //template the image algorithm on size, so we can specialise the display fn
    //and call the appropriate functions  on the PixIterator
    template<Pix::Sze::Bpp s, Pix::Fmt::Layout l> class ImageAlgorithm 
      : public  Function<TL0,TypeList<Image<size,l> > > {};
    
    template<Pix::Fmt::Layout l> class ImageAlgorithm<Pix::Sze::Byte1,l> {
      friend class XDisplay<Pix::Sze::Byte1,l>;
    public:
      bool operator()(const Image<Pix::Sze::Byte1,l>& dest);
      
    private:
      XDisplay<Pix::Sze::Byte1,l>& m_output;
      ImageAlgorithm<Pix::Sze::Byte1,l>(XDisplay<Pix::Sze::Byte1,l>& output) 
	: m_output(output) {}
    };
  
    template<Pix::Fmt::Layout l> class ImageAlgorithm<Pix::Sze::Byte3,l> {
      friend class XDisplay<Pix::Sze::Byte3,l>;
    public:
      bool operator()(const Image<Pix::Sze::Byte3,l>& dest);
      
    private:
      XDisplay<Pix::Sze::Byte3,l>& m_output;
      ImageAlgorithm<Pix::Sze::Byte3,l>(XDisplay<Pix::Sze::Byte3,l>& output) 
	: m_output(output) {}
    };
    
    ImageAlgorithm<size,layout> m_ImageAlgorithm;
  public:

    XDisplay(int width,int height);
    virtual ~XDisplay();

    void Output(const Image<size,layout>& image);
  };

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> XDisplay<size,layout>::XDisplay(int width,int height) : 
    m_mapped(false),
    m_windowgot(false),
    m_displaygot(false),
    m_colormapgot(false),
    m_width(width),
    m_height(height),
    m_needPut(false),
    m_ImageAlgorithm(*this)
  {
    m_image[0] = NULL;
    m_image[1] = NULL;
    if ((m_display = XOpenDisplay(NULL)) == NULL) throw "Failed to open display.\n";
    m_displaygot = true;

    int blackColour = BlackPixel(m_display, DefaultScreen(m_display));
    m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, m_width, m_height, 0, blackColour, blackColour);
    m_windowgot = true;
    XStoreName(m_display, m_window, "Cantag");
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
  
    if (!XAllocColor(m_display,m_colormap,&xc)) throw "Failed to allocate colour for highlighting!";

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
  
    m_image[0] = new XSHMAttachedImage(m_display,visual,depth,m_width,m_height);
    m_image[1] = new XSHMAttachedImage(m_display,visual,depth,m_width,m_height);
    m_displayed_image = 1;
    // Wait for MapNotify
    while (1) {
      XEvent e;
      XNextEvent(m_display, &e);
      if (e.type == MapNotify) {
	break;
      }
    }
  }

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> XDisplay<size,layout>::~XDisplay() {
    for(int i=0;i<2;++i) if (m_image[i]) delete m_image[i];
    if (m_colormapgot) XFreeColormap(m_display,m_colormap);
    if (m_gcgot) XFreeGC(m_display,m_gc);
    if (m_mapped) XUnmapWindow(m_display,m_window);
    if (m_windowgot) XDestroyWindow(m_display,m_window);
    if (m_displaygot) XCloseDisplay(m_display);
  }

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> 
  template<Pix::Fmt::Layout l> bool 
  XDisplay<size,layout>::ImageAlgorithm<Pix::Sze::Byte1,l>::operator()
    (const Image<Pix::Sze::Byte1,l>& image) {

    XSHMAttachedImage& attached = *m_output.m_image[m_output.m_displayed_image ^ 0x1];
    XImage* ximage = attached.m_image;

    for (int y=0; y<m_output.m_height; ++y) {
      const PixRow<l> row = image.GetRow(y);
      typename PixRow<l>::const_iterator pixel=row.begin();
      char* destptr = ximage->data + ximage->bytes_per_line * y;
      for(int x=0; x<m_output.m_width; ++x) {
	unsigned char data = pixel.v();
	++pixel;

	unsigned long rPart = data;
	int diff = attached.m_redbits - 8;
	rPart = diff > 0 ? (rPart << diff) : (rPart >> (-diff));
	rPart <<= attached.m_redshift;

	unsigned long gPart = data;
	diff = attached.m_greenbits - 8;
	gPart = diff > 0 ? (gPart << diff) : (gPart >> (-diff));
	gPart <<= attached.m_greenshift;

	unsigned long bPart = data;
	diff = attached.m_bluebits - 8;
	bPart = diff > 0 ? (bPart << diff) : (bPart >> (-diff));
	bPart <<= attached.m_blueshift;      

	unsigned long value = rPart | gPart | bPart;
	for(int i=0;i<attached.m_bytes_per_pixel;++i) {
	  *destptr = value & 0xFF;
	  destptr++;
	  value>>=8;
	}
      }
    }
    m_output.m_needPut = true;
    return true;
  }

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> 
  template<Pix::Fmt::Layout l> bool 
  XDisplay<size,layout>::ImageAlgorithm<Pix::Sze::Byte3,l>::operator()
    (const Image<Pix::Sze::Byte3,l>& image) {

    XSHMAttachedImage& attached = *m_output.m_image[m_output.m_displayed_image ^ 0x1];
    XImage* ximage = attached.m_image;

    for (int y=0; y<m_output.m_height; ++y) {
      const PixRow<l> row = image.GetRow(y);
      typename PixRow<l>::const_iterator pixel=row.begin();
      char* destptr = ximage->data + ximage->bytes_per_line * y;
      for(int x=0; x<m_output.m_width; ++x) {
	unsigned char red = pixel.r();
	unsigned char green = pixel.g();
	unsigned char blue = pixel.b();
	++pixel;

	unsigned long rPart = red;
	int diff = attached.m_redbits - 8;
	rPart = diff > 0 ? (rPart << diff) : (rPart >> (-diff));
	rPart <<= attached.m_redshift;
	
	unsigned long gPart = green;
	diff = attached.m_greenbits - 8;
	gPart = diff > 0 ? (gPart << diff) : (gPart >> (-diff));
	gPart <<= attached.m_greenshift;
	
	unsigned long bPart = blue;
	diff = attached.m_bluebits - 8;
	bPart = diff > 0 ? (bPart << diff) : (bPart >> (-diff));
	bPart <<= attached.m_blueshift;
	
	unsigned long value = rPart | gPart | bPart;
	for(int i=0;i<attached.m_bytes_per_pixel;++i) {
	  *destptr = value & 0xFF;
	  destptr++;
	  value>>=8;
	}
      }
    }
    m_output.m_needPut = true;
    return true;
  }

  template<Pix::Sze::Bpp size, Pix::Fmt::Layout layout> void XDisplay<size,layout>::Output(const Image<size,layout>& image) {
    m_ImageAlgorithm(image);
    m_displayed_image ^= 0x1;
    XShmPutImage(m_display,m_window,m_gc,m_image[m_displayed_image]->m_image,0,0,0,0,m_width,m_height,false);	
    XFlush(m_display);
    XImage* ximage = m_image[m_displayed_image ^ 0x1]->m_image;
    memset(ximage->data,0xFF,ximage->bytes_per_line*ximage->height);
  }
}
#endif//X_DISPLAY
