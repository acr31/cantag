/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */

#include <cantag/Config.hh>

#ifdef HAVE_GL_GLX_H

#include <cantag/GLRenderWindow.hh>

namespace Cantag {

  GLRenderWindow::GLRenderWindow(int width, int height) : m_width(width), m_height(height) {

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
    swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask | KeyPressMask;
    m_window = XCreateWindow(m_display, RootWindow(m_display, m_visual->screen), 0, 0, width, height, 0, m_visual->depth,
			     InputOutput, m_visual->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
    XSetStandardProperties(m_display, m_window, "Cantag GL", "Cantag GL", None, (char**)0, 0, NULL);
  
    /* Bind the rendering context to the window */
    glXMakeCurrent(m_display, m_window, m_context);

    /* map the window to the screen */
    XMapWindow(m_display, m_window);
  
    ServiceEventQueue();

  }

  bool GLRenderWindow::ServiceEventQueue() {
    /* clear the event queue - which we ignore */
    XEvent event;
    bool returnval = true;
    while (XPending(m_display)) { 
      XNextEvent(m_display, &event);
      if (event.type == KeyPress && ((XKeyEvent*)&event)->keycode == 9) returnval = false;
    }
    return returnval;
  }

  GLRenderWindow::~GLRenderWindow() {
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

  int GLRenderWindow::FindNextTextureSize(int size) const {
    // texture size must be of the form 2^n + 2
    size -= 2;
    int count = 0;
    while(size) {
      ++count;
      size>>=1;
    }
    return (1 << count) + 2;    
  }

  void GLRenderWindow::Flush() {
    glXSwapBuffers(m_display, m_window); /* buffer swap does implicit glFlush */  
    if (!ServiceEventQueue()) exit(-1);
  }
}
#endif//HAVE_GL_GL_H
