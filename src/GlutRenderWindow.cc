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

#ifdef HAVE_GL_FREEGLUT_H

#ifdef WIN32
# include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>


#include <cantag/GlutRenderWindow.hh>

namespace Cantag {

  GlutRenderWindow::GlutRenderWindow(int width, int height) : m_width(width), m_height(height) {
    m_ratio=(float)width/(float)height;

    char argv = '\0';
    char* argvp = &argv;
    int argc = 0;
    glutInit(&argc,&argvp);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    m_window_id = glutCreateWindow("CantagGL");
    glutSetWindow(m_window_id);
    ServiceEventQueue();

  }

  

  bool GlutRenderWindow::ServiceEventQueue() {

    glutMainLoopEvent();
    return true;
  }

  GlutRenderWindow::~GlutRenderWindow() {
    glutDestroyWindow(m_window_id);
  }

  int GlutRenderWindow::FindNextTextureSize(int size) const {
    // texture size must be of the form 2^n + 2
    size -= 2;
    int count = 0;
    while(size) {
      ++count;
      size>>=1;
    }
    return (1 << count) + 2;    
  }
  
  void GlutRenderWindow::Flush() {
    glutSwapBuffers();
    if (!ServiceEventQueue()) exit(-1);
  }
  
}
#endif//HAVE_GL_GL_H
