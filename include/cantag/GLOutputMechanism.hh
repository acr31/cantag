/**
 * $Header$
 */

#ifndef GLOUTPUT_MECHANISM_GUARD
#define GLOUTPUT_MECHANISM_GUARD

#include <total/Config.hh>

#ifndef HAVE_GL_GL_H
#error "This version has been configured without OpenGL support"
#endif

#ifndef HAVELIB_GL
# error "This version has been configured without OpenGL support"
#endif

#include <iostream>
#include <map>

#include <total/ContourTree.hh>
#include <total/ShapeTree.hh>
#include <total/WorldState.hh>
#include <total/Tag.hh>
#include <total/Camera.hh>

#include <total/CyclicBitSet.hh>

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/osmesa.h>

#define FRAME_TTL 1

namespace Total {

  class GLOutputMechanism {
  private:
    Display *m_display;
    XVisualInfo* m_visual;
    GLXContext m_context;
    Colormap m_colormap;
    Window m_window;
    bool displayListInited;
  
  public:
    GLOutputMechanism(int argc, char* argv[],int width,int height);
    ~GLOutputMechanism();
    inline void FromImageSource(const Image& image) {};
    inline void FromThreshold(const Image& image) {};
    inline void FromContourTree(const ContourTree& contours) {}
    inline void FromRemoveIntrinsic(const ContourTree& contours) {};
    template<class S> inline void FromShapeTree(const ShapeTree<S>& shapes) {};
    template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);
    inline void Flush() {};

  private:
    float m_ratio;
  };

  class Corners {
  public:
    float c1[3];
    float c2[3];
    float c3[3];
    float c4[3];
    int ttl;
  };


  template<int PAYLOAD_SIZE> void GLOutputMechanism::FromTag(const WorldState<PAYLOAD_SIZE>& world) { 
   
    static std::map< unsigned long, Corners *> sSeen;
    static int fnum=-1;
    fnum=(fnum+1)%FRAME_TTL;

    std::map< unsigned long, Corners *>::iterator sIt;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,m_ratio,0.0001, 100.0);
    gluLookAt(0.0,0.0,0.0,0.0,0.0,1.0,0.0,-1.0,0.0);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(1.0, 1.0, 1.0, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
    for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = world.GetNodes().begin();
	i!=world.GetNodes().end();
	++i) {
      LocatedObject<PAYLOAD_SIZE>* loc = *i;
      if (loc->tag_codes.size() > 0) {
	sIt = sSeen.find(loc->tag_codes[0]->to_ulong());
	if (sIt==sSeen.end()) {
	  // Add it
	  Corners *c = new Corners();
	  c->c1[0] = -1; c->c1[1]=1; c->c1[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c1);
	  c->c2[0] =  1; c->c2[1]=1; c->c2[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c2);
	  c->c3[0] = 1; c->c3[1]=-1; c->c3[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c3);
	  c->c4[0] = -1; c->c4[1]=-1; c->c4[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c4);
	  c->ttl=FRAME_TTL;
	  sSeen[loc->tag_codes[0]->to_ulong()]=c;
	}
	else {
	  // Update it
	  Corners *c = sIt->second;
	  c->c1[0] = -1; c->c1[1]=1; c->c1[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c1);
	  c->c2[0] = 1; c->c2[1]=1; c->c2[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c2);
	  c->c3[0] = 1; c->c3[1]=-1; c->c3[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c3);
	  c->c4[0] = -1; c->c4[1]=-1; c->c4[2]=0.0;
	  ApplyCameraTransform(loc->transform,c->c4);
	  c->ttl=FRAME_TTL;
	}
      }
    }

    for (sIt=sSeen.begin(); sIt!=sSeen.end(); ++sIt) {
      if (sIt->second->ttl!=0) {
	Corners *c = sIt->second;
	c->ttl--;
	glBegin(GL_QUADS);
	glColor3f(0.0, 0.7, 0.1);       /* green */
	glVertex3f(c->c1[0], c->c1[1], c->c1[2]);
	glVertex3f(c->c2[0], c->c2[1], c->c2[2]);
	glVertex3f(c->c3[0], c->c3[1], c->c3[2]);
	glVertex3f(c->c4[0], c->c4[1], c->c4[2]);
	glEnd();
      }
      else{
	Corners *c = sIt->second;
	delete c;
	sSeen.erase(sIt->first);
      }
    }
    glXSwapBuffers(m_display, m_window);
    GLenum errCode;
    const GLubyte *errString;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
      errString = gluErrorString(errCode);
      std::cout << errString << std::endl;
    }
  }
}

#endif//GLOUTPUT_MECHANISM_GUARD
