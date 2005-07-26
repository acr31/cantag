/**
 * $Header$
 */ 

#ifndef X_OUTPUT_MECHANISM_GUARD
#define X_OUTPUT_MECHANISM_GUARD

#include <total/Config.hh>

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

#include <total/entities/Entity.hh>
#include <total/Function.hh>
#include <total/MonochromeImage.hh>
#include <total/Camera.hh>

namespace Total {

  template<class Shape, int PAYLOAD_SIZE>
  class XOutputMechanism {
  private:
    Colormap m_colormap;

    bool m_xattached;
    
    bool m_gcgot;
    bool m_mapped;
    bool m_windowgot;
    bool m_displaygot;
    bool m_colormapgot;
    const Camera& m_camera;
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
  public:

    class ImageAlgorithm : public Function0<Image<Colour::Grey> > {
      friend class XOutputMechanism;
    public:
      bool operator()(Image<Colour::Grey>& dest);
      
    private:
      XOutputMechanism& m_output;
      ImageAlgorithm(XOutputMechanism& output) : m_output(output) {}
    };
    
    class ThresholdAlgorithm : public Function0<MonochromeImage>{
      friend class XOutputMechanism;
    public:
      bool operator()(MonochromeImage& dest);
    private:
      XOutputMechanism& m_output;
      ThresholdAlgorithm(XOutputMechanism& output) : m_output(output) {}
    };

    class ContourAlgorithm : public Function0<ContourEntity>{
      friend class XOutputMechanism;
    public:
      bool operator()(ContourEntity& dest);
    private:
      XOutputMechanism& m_output;
      ContourAlgorithm(XOutputMechanism& output) : m_output(output) {}
    };
    
    class ShapeAlgorithm : public Function0<ShapeEntity<Shape> > {
      friend class XOutputMechanism;
    public:
      bool operator()(ShapeEntity<Shape>& dest);
    private:
      XOutputMechanism& m_output;
      ShapeAlgorithm(XOutputMechanism& output) : m_output(output) {}
    };
    
    class TransformAlgorithm : public Function2<TransformEntity,DecodeEntity<PAYLOAD_SIZE>,DecodeEntity<PAYLOAD_SIZE> > {
      friend class XOutputMechanism;     
    public:
      bool operator()(const TransformEntity& transform, const DecodeEntity<PAYLOAD_SIZE>& decode, DecodeEntity<PAYLOAD_SIZE>& dest) const;
    private:
      XOutputMechanism& m_output;
      TransformAlgorithm(XOutputMechanism& output) : m_output(output) {}  
    };
    
    ImageAlgorithm m_ImageAlgorithm;
    ThresholdAlgorithm m_ThresholdAlgorithm;
    ContourAlgorithm m_ContourAlgorithm;
    ShapeAlgorithm m_ShapeAlgorithm;
    TransformAlgorithm m_TransformAlgorithm;

    XOutputMechanism(int width,int height,const Camera& camera);
    virtual ~XOutputMechanism();

    void Flush();
  };

  template<class Shape, int PAYLOAD_SIZE> XOutputMechanism<Shape,PAYLOAD_SIZE>::XOutputMechanism(int width,int height, const Camera& camera) : 
    m_mapped(false),
    m_windowgot(false),
    m_displaygot(false),
    m_colormapgot(false),
    m_camera(camera),
    m_width(width),
    m_height(height),
    m_needPut(false),
    m_ImageAlgorithm(*this),
    m_ThresholdAlgorithm(*this),
    m_ContourAlgorithm(*this),
    m_ShapeAlgorithm(*this),
    m_TransformAlgorithm(*this)
  {
    m_image[0] = NULL;
    m_image[1] = NULL;
    if ((m_display = XOpenDisplay(NULL)) == NULL) {
      throw "Failed to open display.\n";
    }
    m_displaygot = true;

    int blackColour = BlackPixel(m_display, DefaultScreen(m_display));
    m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, m_width, m_height, 0, blackColour, blackColour);
    m_windowgot = true;
    XStoreName(m_display, m_window, "Total");
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

  template<class Shape, int PAYLOAD_SIZE> XOutputMechanism<Shape,PAYLOAD_SIZE>::~XOutputMechanism() {
    for(int i=0;i<2;++i) {
      if (m_image[i]) delete m_image[i];
    }
    if (m_colormapgot) XFreeColormap(m_display,m_colormap);
    if (m_gcgot) XFreeGC(m_display,m_gc);
    if (m_mapped) XUnmapWindow(m_display,m_window);
    if (m_windowgot) XDestroyWindow(m_display,m_window);
    if (m_displaygot) XCloseDisplay(m_display);
  }

  template<class Shape, int PAYLOAD_SIZE> bool XOutputMechanism<Shape,PAYLOAD_SIZE>::ImageAlgorithm::operator()(Image<Colour::Grey>& image) {
    XSHMAttachedImage& attached = *m_output.m_image[m_output.m_displayed_image ^ 0x1];
    XImage* ximage = attached.m_image;

    for (int y=0; y<m_output.m_height/2; ++y) {
      const unsigned char* pointer = image.GetRow(2*y);
      char* destptr = ximage->data + ximage->bytes_per_line * y;
      for (int x=0; x<m_output.m_width/2; ++x) {
	unsigned char data = *pointer;
	pointer+=2;
      
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


  template<class Shape, int PAYLOAD_SIZE> bool XOutputMechanism<Shape,PAYLOAD_SIZE>::ThresholdAlgorithm::operator()(MonochromeImage& image) {
    XSHMAttachedImage& attached = *m_output.m_image[m_output.m_displayed_image ^ 0x1];
    XImage* ximage = attached.m_image;

    const int midpoint = attached.m_bytes_per_pixel * m_output.m_width/2;

    int sampley=0;
    for (int y=0; y<m_output.m_height/2; ++y) {
      int samplex=0;
      char* destptr = ximage->data + ximage->bytes_per_line * y + midpoint;
      for (int x=0; x<m_output.m_width/2; ++x) {
	for(int i=0;i<attached.m_bytes_per_pixel;++i) {
	  *destptr = image.GetPixel(samplex,sampley) ? 0 : 0xFF;
	  destptr++;
	}
	samplex+=2;
      }
      sampley+=2;
    }
    m_output.m_needPut = true;
    return true;
  }

  template<class Shape, int PAYLOAD_SIZE> bool XOutputMechanism<Shape,PAYLOAD_SIZE>::ContourAlgorithm::operator()(ContourEntity& contour) {
    XImage* ximage = m_output.m_image[m_output.m_displayed_image ^ 0x1]->m_image;

    for(std::vector<float>::const_iterator i = contour.GetPoints().begin();
	i != contour.GetPoints().end();
	++i) {
      const float x = *i;
      ++i;
      const float y = *i;
      XPutPixel(ximage,(int)(x/2),(int)(y/2+m_output.m_height/2),0);
    }    
    m_output.m_needPut = true;
    return true;
  }

  template<class Shape, int PAYLOAD_SIZE> bool XOutputMechanism<Shape,PAYLOAD_SIZE>::ShapeAlgorithm::operator()(ShapeEntity<Shape>& shape) {
    XImage* ximage = m_output.m_image[m_output.m_displayed_image ^ 0x1]->m_image;

    std::vector<int> points;
    shape.m_shapeDetails->Draw(points,m_output.m_camera);
    for(std::vector<int>::const_iterator i = points.begin();
	i != points.end();
	++i) {
      const int x = *i;
      ++i;
      const int y = *i;
      
      int a = x / 2 + m_output.m_width / 2 ;
      int b = y /2 + m_output.m_height / 2;
      if (a >= m_output.m_width/2 && a < m_output.m_width &&
	  b >= m_output.m_height/2 && b < m_output.m_height) {      
	XPutPixel(ximage,a,b,0);
      }
    }  
    m_output.m_needPut = true;
    return true;    
  }

  template<class Shape, int PAYLOAD_SIZE> bool XOutputMechanism<Shape,PAYLOAD_SIZE>::TransformAlgorithm::operator()(const TransformEntity& transform, const DecodeEntity<PAYLOAD_SIZE>& decode, DecodeEntity<PAYLOAD_SIZE>& dest) const {
    XImage* ximage = m_output.m_image[m_output.m_displayed_image ^ 0x1]->m_image;
    XShmPutImage(m_output.m_display,m_output.m_window,m_output.m_gc,ximage,0,0,0,0,m_output.m_width,m_output.m_height,false);
    m_output.m_needPut = false;
    float pts[] = {-1,-1,
		   -1,1,
		   1,1,
		   1,-1};
    transform.GetTransform()->Apply(pts,4);
    m_output.m_camera.NPCFToImage(pts,4);
    XDrawLine(m_output.m_display,m_output.m_window,m_output.m_gc,
	      (int)(pts[0]/2),(int)(pts[1]/2),
	      (int)(pts[2]/2),(int)(pts[3]/2));
    XDrawLine(m_output.m_display,m_output.m_window,m_output.m_gc,
	      (int)(pts[2]/2),(int)(pts[3]/2),
	      (int)(pts[4]/2),(int)(pts[5]/2));
    XDrawLine(m_output.m_display,m_output.m_window,m_output.m_gc,
	      (int)(pts[4]/2),(int)(pts[5]/2),
	      (int)(pts[6]/2),(int)(pts[7]/2));
    XDrawLine(m_output.m_display,m_output.m_window,m_output.m_gc,
	      (int)(pts[6]/2),(int)(pts[7]/2),
	      (int)(pts[0]/2),(int)(pts[1]/2));
    XTextItem ti;
    ti.chars=new char[PAYLOAD_SIZE];
    typename DecodeEntity<PAYLOAD_SIZE>::Data* data = *(decode.GetPayloads().begin());
    for(int i=0;i<PAYLOAD_SIZE;++i) {
      ti.chars[i] = (data->payload)[i] ? '1' : '0';
    }
    ti.nchars=PAYLOAD_SIZE;
    ti.delta=0;
    ti.font=None;
    XDrawText(m_output.m_display,m_output.m_window,m_output.m_gc,(int)(pts[0]),(int)(pts[1]),&ti,1);
    //    std::cout << (data->payload) << std::endl;
    delete[] ti.chars;
    return true;    
  }

  template<class Shape, int PAYLOAD_SIZE> void XOutputMechanism<Shape,PAYLOAD_SIZE>::Flush() {
    m_displayed_image ^= 0x1;
    if (m_needPut) {
      XShmPutImage(m_display,m_window,m_gc,m_image[m_displayed_image]->m_image,0,0,0,0,m_width,m_height,false);	
      m_needPut = false;
    }
    XFlush(m_display);
    XImage* ximage = m_image[m_displayed_image ^ 0x1]->m_image;
    memset(ximage->data,0xFF,ximage->bytes_per_line*ximage->height);
  }
}
#endif//X_OUTPUT_MECHANISM_GUARD
