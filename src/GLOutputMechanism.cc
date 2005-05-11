/**
 * $Header$
 */

#include <total/GLOutputMechanism.hh>

namespace Total {

  GLOutputMechanism::GLOutputMechanism(int argc, char* argv[], int width, int height) {

    /**
     * Follows the template laid out at:
     * http://www.sgi.com/software/opengl/glandx/intro/section3_7.html#SECTION0007000000000000000
     */

    m_ratio=(float)width/(float)height;
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
    XSetStandardProperties(m_display, m_window, "Total", "Total", None, argv, argc, NULL);
  
    /* Bind the rendering context to the window */
    glXMakeCurrent(m_display, m_window, m_context);

    /* map the window to the screen */
    XMapWindow(m_display, m_window);
  
     
    /* clear the event queue - which we ignore */
    XEvent event;
    while (XPending(m_display)) { 
      XNextEvent(m_display, &event);
    }

  };





  GLOutputMechanism::~GLOutputMechanism() {
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

};
