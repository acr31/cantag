/**
 * $Header$
 */ 

#ifndef X_OUTPUT_STAGES_MECHANISM_GUARD
#define X_OUTPUT_STAGES_MECHANISM_GUARD

#include <tripover/XOutputMechanism.hh>
#include <tripover/ContourTree.hh>
#include <tripover/ShapeTree.hh>
#include <tripover/WorldState.hh>

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
}

class XOutputStagesMechanism : public XOutputMechanism {
private:
  void FromContourTree(const ContourTree::Contour* contour);
  template<class S> void FromShapeTree(Image& i, const typename ShapeTree<S>::Node* node);
public:
  
  XOutputStagesMechanism(int width,int height,const Camera& camera);
  virtual ~XOutputStagesMechanism();
  void FromImageSource(const Image& image);
  void FromThreshold(const Image& image);
  void FromContourTree(const ContourTree& contours);
  inline void FromRemoveIntrinsic(const ContourTree& contours) {};
  template<class S> void FromShapeTree(const ShapeTree<S>& shapes);
  template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);
};

template<class S> void XOutputStagesMechanism::FromShapeTree(Image& image,const typename ShapeTree<S>::Node* node) {
  node->matched.DrawChain(image,m_camera);
  for(typename std::vector<typename ShapeTree<S>::Node* >::const_iterator i = node->children.begin();
      i!=node->children.end();
      ++i) {
    FromShapeTree<S>(image,*i);
  }
};


template<class S> void XOutputStagesMechanism::FromShapeTree(const ShapeTree<S>& shapes) {
  Image image(m_width,m_height);
  FromShapeTree<S>(image,shapes.GetRootNode());

  // copy the image into the XImage
  const int midpoint = m_bytes_per_pixel * m_width/2;
  int row = 0;
  for (int y=m_height/2; y<m_height; ++y) {
    const unsigned char* pointer = image.GetRow(row);
    char* destptr = m_image->data + m_image->bytes_per_line * y + midpoint;
    row+=2;
    for (int x=0; x<m_width/2; ++x) {
      unsigned char data = *pointer;
      pointer+=2;
      for(int i=0;i<m_bytes_per_pixel;++i) {
	*destptr = data == 0 ? 0 : 0xFF;
	destptr++;
      }
    }
  }
};

template<int PAYLOADSIZE> void XOutputStagesMechanism::FromTag(const WorldState<PAYLOADSIZE>& world) {
  XShmPutImage(m_display,m_window,m_gc,m_image,0,0,0,0,m_width,m_height,false);
  for(typename std::vector<LocatedObject<PAYLOADSIZE>*>::const_iterator i = world.GetNodes().begin();
      i!=world.GetNodes().end();
      ++i) {
    LocatedObject<PAYLOADSIZE>* lobj = *i;
    float pts[] = {-1,-1,
		   -1,1,
		   1,1,
		   1,-1};
    ApplyTransform(lobj->transform,pts,4);
    m_camera.NPCFToImage(pts,4);
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[0]/2),(int)(pts[1]/2),
	      (int)(pts[2]/2),(int)(pts[3]/2));
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[2]/2),(int)(pts[3]/2),
	      (int)(pts[4]/2),(int)(pts[5]/2));
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[4]/2),(int)(pts[5]/2),
	      (int)(pts[6]/2),(int)(pts[7]/2));
    XDrawLine(m_display,m_window,m_gc,
	      (int)(pts[6]/2),(int)(pts[7]/2),
	      (int)(pts[0]/2),(int)(pts[1]/2));
    XTextItem ti;
    ti.chars=new char[PAYLOADSIZE];
    for(int i=0;i<PAYLOADSIZE;++i) {
      ti.chars[i] = (*(lobj->tag_codes[0]))[i] ? '1' : '0';
    }
    ti.nchars=PAYLOADSIZE;
    ti.delta=0;
    ti.font=None;
    XDrawText(m_display,m_window,m_gc,(int)(pts[0]/2),(int)(pts[1]/2),&ti,1);
    delete[] ti.chars;
  }
}

#endif//X_OUTPUT_STAGES_MECHANISM_GUARD
