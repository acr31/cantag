/**
 * $Header$
 */ 

#ifndef X_OUTPUT_MECHANISM_GUARD
#define X_OUTPUT_MECHANISM_GUARD

#include <tripover/Config.hh>

#ifndef HAVE_X11_XLIB_H
# error This version has been configured without Xlib support
#endif
#ifndef HAVE_X11_XUTIL_H
# error This version has been configured without XUtil support
#endif
#ifndef HAVE_X11_EXTENSIONS_XSHM_H
# error This version has been configured without MIT XSHM support
#endif

#include <tripover/LocatedObject.hh>
#include <tripover/ContourTree.hh>
#include <tripover/ShapeTree.hh>
#include <tripover/WorldState.hh>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
}

class XOutputMechanism {
private:
  XShmSegmentInfo m_shminfo;
  Colormap m_colormap;

  bool m_xattached;
  bool m_createdpixmap;
  bool m_shmat;
  bool m_shmgot;
  bool m_gcgot;
  bool m_mapped;
  bool m_windowgot;
  bool m_displaygot;
  bool m_colormapgot;

protected:
  const Camera& m_camera;
  Display* m_display;
  GC m_gc;
  Window m_window;
  XImage* m_image;
  int m_width;
  int m_height;

  int m_redbits;
  int m_bluebits;
  int m_greenbits;
  int m_redshift;
  int m_blueshift;
  int m_greenshift;
  int m_bytes_per_pixel; 

public:
  
  XOutputMechanism(int width,int height,const Camera& camera);
  virtual ~XOutputMechanism();
  void FromImageSource(const Image& image);
  inline void FromThreshold(const Image& image) {};
  inline void FromContourTree(const ContourTree& contours) {}
  inline void FromRemoveIntrinsic(const ContourTree& contours) {};
  template<class S> inline void FromShapeTree(const ShapeTree<S>& shapes) {};
  template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);
  void Flush();
};


template<int PAYLOADSIZE> void XOutputMechanism::FromTag(const WorldState<PAYLOADSIZE>& world) {
  XShmPutImage(m_display,m_window,m_gc,m_image,0,0,0,0,m_width,m_height,false);
  for(typename std::vector<LocatedObject<PAYLOADSIZE>*>::const_iterator i = world.GetNodes().begin();
      i!=world.GetNodes().end();
      ++i) {
    LocatedObject<PAYLOADSIZE>* lobj = *i;
    float pts[] = {0,0,
		   0,1,
		   1,1,
		   1,0};
    ApplyTransform(lobj->transform,pts,4);
    m_camera.NPCFToImage(pts,4);
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[0]),(int)(pts[1]),
	      (int)(pts[2]),(int)(pts[3]));
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[2]),(int)(pts[3]),
	      (int)(pts[4]),(int)(pts[5]));
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[4]),(int)(pts[5]),
	      (int)(pts[6]),(int)(pts[7]));
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[6]),(int)(pts[7]),
	      (int)(pts[0]),(int)(pts[1]));
    XTextItem ti;
    ti.chars=new char[PAYLOADSIZE];
    for(int i=0;i<PAYLOADSIZE;++i) {
      ti.chars[i] = (*(lobj->tag_code))[i] ? '1' : '0';
    }
    ti.nchars=PAYLOADSIZE;
    ti.delta=0;
    ti.font=None;
    XDrawText(m_display,m_window,m_gc,(int)(pts[0]),(int)(pts[1]),&ti,1);
    delete[] ti.chars;
  }
}

#endif//X_OUTPUT_MECHANISM_GUARD
