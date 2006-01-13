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

#ifndef GLRENDERWINFOW_GUARD
#define GLRENDERWINDOW_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_GL_GL_H
#error "This version has been configured without OpenGL support"
#endif

#ifndef HAVELIB_GL
# error "This version has been configured without OpenGL support"
#endif

#include <iostream>
#include <map>

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <cantag/Camera.hh>
#include <cantag/Transform.hh>

namespace Cantag {
  
  /**
   * Superclass for GL rendering 
   */
  class GLRenderWindow {
  private:
    XVisualInfo* m_visual;
    Colormap m_colormap;

  protected:
    Display *m_display;
    Window m_window;
    GLXContext m_context;
    float m_ratio;
    int m_width;
    int m_height;

  public:
    GLRenderWindow(int width,int height);
    virtual ~GLRenderWindow();
    
    /**
     * Find the next allowable texture size which is greater than the argument
     */
    int FindNextTextureSize(int size) const;
  };
}

#endif//GLRENDER_WINDOW_GUARD
