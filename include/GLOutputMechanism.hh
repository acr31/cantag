/**
 * $Header$
 */

#ifndef GLOUTPUT_MECHANISM_GUARD
#define GLOUTPUT_MECHANISM_GUARD

#include <GL/glx.h>

template<int PAYLOAD_SIZE>
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
  void NewData(Image* newdata);
  template<class C> void Output(SceneGraph<C,PAYLOAD_SIZE>& sg);


private:
  void Draw(int mode);
};

template<int PAYLOAD_SIZE> GLOutputMechanism<PAYLOAD_SIZE>::GLOutputMechanism(int argc, char* argv[], int width, int height) 
 {
  /**
   * Follows the template laid out at:
   * http://www.sgi.com/software/opengl/glandx/intro/section3_7.html#SECTION0007000000000000000
   */

  /* open the display */
  m_display = XOpenDisplay(NULL);
  if (m_display == NULL) { throw "Failed to open display"; }
  
  /* check for GLX support */
  int dummy;
  if(!glXQueryExtension(m_display, &dummy, &dummy)) { throw "GLX Extension is unsupported on this server"; }

  /* find a suitable visual */
  int requestDoubleBuffer[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
  m_visual = glXChooseVisual(m_display, DefaultScreen(m_display), requestDoubleBuffer);
  if (m_visual == NULL) { throw "No suitable visual (RGB, DOUBLEBUFFER, DEPTH 16) available"; }
  //  if (m_visual->class != TrueColor) { throw "TrueColor visual required for this program"; }

  /* create the GL rendering context */
  m_context = glXCreateContext(m_display, 
			       m_visual, 
			       None, /* no sharing of display lists */
			       GL_TRUE /* direct rendering */);  
  if (m_context == NULL) { throw "Failed to create rendering context"; }

  /* create a new colormap (we are probably not using the default visual */
  m_colormap = XCreateColormap(m_display, RootWindow(m_display, m_visual->screen), m_visual->visual, AllocNone);

  /* create an X window for this visual */
  XSetWindowAttributes swa;
  swa.colormap = m_colormap;
  swa.border_pixel = 0;
  swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;
  m_window = XCreateWindow(m_display, RootWindow(m_display, m_visual->screen), 0, 0, width, height, 0, m_visual->depth,
			   InputOutput, m_visual->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
  XSetStandardProperties(m_display, m_window, "TripOver", "TripOver", None, argv, argc, NULL);
  
  /* Bind the rendering context to the window */
  glXMakeCurrent(m_display, m_window, m_context);

  /* map the window to the screen */
  XMapWindow(m_display, m_window);
  
  glEnable(GL_DEPTH_TEST); /* enable depth buffering */
  /* frame buffer clears should be to black */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  /* set up projection transform */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 10.0);
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  
  /* clear the event queue - which we ignore */
  XEvent event;
  while (XPending(m_display)) { 
    XNextEvent(m_display, &event);
  }

  GLuint i, j;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  displayListInited = GL_FALSE;
};

template<int PAYLOAD_SIZE> GLOutputMechanism<PAYLOAD_SIZE>::~GLOutputMechanism() {
  // unbind the current context
  glXMakeCurrent(m_display, None, NULL);

  // destroy window m_window
  XDestroyWindow(m_display,m_window);

  // destroy colormap 
  XFreeColormap(m_display,m_colormap);

  // free the visual
  XFree(m_visual);

  // close the display
  XCloseDisplay(m_display);
}

template<int PAYLOAD_SIZE> void GLOutputMechanism<PAYLOAD_SIZE>::NewData(Image* newdata) {

}

template<int PAYLOAD_SIZE> void GLOutputMechanism<PAYLOAD_SIZE>::Draw(int mode) {
  if (displayListInited) {
    /* if display list already exists, just execute it */
    glCallList(1);
  } else {
    /* otherwise compile and execute to create the display list */
    glNewList(1, GL_COMPILE_AND_EXECUTE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* front face */
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.7, 0.1);       /* green */
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(-1.0, -1.0, 1.0);
    /* back face */
    glColor3f(0.9, 1.0, 0.0);       /* yellow */
    glVertex3f(-1.0, 1.0, -1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    /* top side face */
    glColor3f(0.2, 0.2, 1.0);       /* blue */
    glVertex3f(-1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    /* bottom side face */
    glColor3f(0.7, 0.0, 0.1);       /* red */
    glVertex3f(-1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, 1.0);
    glVertex3f(1.0, -1.0, -1.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glEnd();
    glEndList();
    displayListInited = GL_TRUE;
  }
  glXSwapBuffers(m_display, m_window); /* buffer swap does implicit glFlush */  

}

template<int PAYLOAD_SIZE> template<class C> void GLOutputMechanism<PAYLOAD_SIZE>::Output(SceneGraph<C,PAYLOAD_SIZE>& sg) {
  
  LocatedObject<PAYLOAD_SIZE>* loc = sg.First();

  if (loc != NULL) {
    float nx = loc->normal[0];
    float ny = loc->normal[1];
    float nz = loc->normal[2];
    /* rotate by X, Y, and Z angles */
    float norm = sqrt(nx*nx+ny*ny+nz*nz);
    nx/=norm;
    ny/=norm;
    nz/=norm;
    
    float factor = -sqrt(nx*nx+nz*nz);
    float rotation[] = { nz/factor, 0, -nx/factor, 0, 
			 -ny*nx/factor, factor, -ny*nz/factor, 0,
			 nx,ny,nz,0,
			 0,0,0,1};
    float angle = -loc->angle/M_PI*180;
    
    glMatrixMode(GL_MODELVIEW);
    /* reset modelview matrix to the identity matrix */
    glLoadIdentity();
    /* move the camera back three units */
    glTranslatef(0.0, 0.0, -3.0);
    
    glMultMatrixf(rotation);
    /* rotate tag around z axis for angle */
    glRotatef(angle,0.0,0.0,1.0);
    
    Draw(1);
  }
  else {
    Draw(2);
  }
    
}


#endif//GLOUTPUT_MECHANISM_GUARD
