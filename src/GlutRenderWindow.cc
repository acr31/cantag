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
    m_self = this;
    char argv = '\0';
    char* argvp = &argv;
    int argc = 0;
    glutInit(&argc,&argvp);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    m_window_id = glutCreateWindow("CantagGL");
    glutSetWindow(m_window_id);
    glClearColor(1.f,1.f,1.f,1.f);
    ServiceEventQueue();
    glutKeyboardFunc(&GlutRenderWindow::KeyboardCallback);
    glutSpecialFunc(&GlutRenderWindow::SpecialCallback);
  }

  GlutRenderWindow* GlutRenderWindow::m_self = NULL;

    void GlutRenderWindow::SpecialCallback(int key, int x,int y) {
	switch(key) {
	    case GLUT_KEY_UP:
		GlutRenderWindow::m_self->m_presses.push_back(Key::CURSOR_UP);
		break;
	    case GLUT_KEY_DOWN:
		GlutRenderWindow::m_self->m_presses.push_back(Key::CURSOR_DOWN);
		break;
	    case GLUT_KEY_LEFT:
		GlutRenderWindow::m_self->m_presses.push_back(Key::CURSOR_LEFT);
		break;
	    case GLUT_KEY_RIGHT:
		GlutRenderWindow::m_self->m_presses.push_back(Key::CURSOR_RIGHT);
	}
    }

  void GlutRenderWindow::KeyboardCallback(unsigned char key,int x,int y) {
    switch(key) {
    case 27:
      GlutRenderWindow::m_self->m_presses.push_back(Key::ESC);
      break;
    case 'A':
    case 'a':
      GlutRenderWindow::m_self->m_presses.push_back(Key::A);
      break;
    case 'B':
    case 'b':
      GlutRenderWindow::m_self->m_presses.push_back(Key::B);
      break;
    case 'C':
    case 'c':
      GlutRenderWindow::m_self->m_presses.push_back(Key::C);
      break;
    case 'D':
    case 'd':
      GlutRenderWindow::m_self->m_presses.push_back(Key::D);
      break;
    case 'E':
    case 'e':
      GlutRenderWindow::m_self->m_presses.push_back(Key::E);
      break;
    case 'F':
    case 'f':
      GlutRenderWindow::m_self->m_presses.push_back(Key::F);
      break;
    case 'G':
    case 'g':
      GlutRenderWindow::m_self->m_presses.push_back(Key::G);
      break;
    case 'H':
    case 'h':
      GlutRenderWindow::m_self->m_presses.push_back(Key::H);
      break;
    case 'I':
    case 'i':
      GlutRenderWindow::m_self->m_presses.push_back(Key::I);
      break;
    case 'J':
    case 'j':
      GlutRenderWindow::m_self->m_presses.push_back(Key::J);
      break;
    case 'K':
    case 'k':
      GlutRenderWindow::m_self->m_presses.push_back(Key::K);
      break;
    case 'L':
    case 'l':
      GlutRenderWindow::m_self->m_presses.push_back(Key::L);
      break;
    case 'M':
    case 'm':
      GlutRenderWindow::m_self->m_presses.push_back(Key::M);
      break;
    case 'N':
    case 'n':
      GlutRenderWindow::m_self->m_presses.push_back(Key::N);
      break;
    case 'O':
    case 'o':
      GlutRenderWindow::m_self->m_presses.push_back(Key::O);
      break;
    case 'P':
    case 'p':
      GlutRenderWindow::m_self->m_presses.push_back(Key::P);
      break;
    case 'Q':
    case 'q':
      GlutRenderWindow::m_self->m_presses.push_back(Key::Q);
      break;
    case 'R':
    case 'r':
      GlutRenderWindow::m_self->m_presses.push_back(Key::R);
      break;
    case 'S':
    case 's':
      GlutRenderWindow::m_self->m_presses.push_back(Key::S);
      break;
    case 'T':
    case 't':
      GlutRenderWindow::m_self->m_presses.push_back(Key::T);
      break;
    case 'U':
    case 'u':
      GlutRenderWindow::m_self->m_presses.push_back(Key::U);
      break;
    case 'V':
    case 'v':
      GlutRenderWindow::m_self->m_presses.push_back(Key::V);
      break;
    case 'W':
    case 'w':
      GlutRenderWindow::m_self->m_presses.push_back(Key::W);
      break;
    case 'X':
    case 'x':
      GlutRenderWindow::m_self->m_presses.push_back(Key::X);
      break;
    case 'Y':
    case 'y':
      GlutRenderWindow::m_self->m_presses.push_back(Key::Y);
      break;
    case 'Z':
    case 'z':
      GlutRenderWindow::m_self->m_presses.push_back(Key::Z);
      break;
    }
  }

  void GlutRenderWindow::ServiceEventQueue() {
    glutMainLoopEvent();
  }

  GlutRenderWindow::~GlutRenderWindow() {
    glutDestroyWindow(m_window_id);
  }

  int GlutRenderWindow::FindNextTextureSize(int size) const {
    // texture size must be of the form 2^n 
    int count = 0;
    while(size) {
      ++count;
      size>>=1;
    }
    return (1 << count);
  }
  
  void GlutRenderWindow::Flush() {
    m_presses.clear();
    glutSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ServiceEventQueue();
  }
  
}
#endif//HAVE_GL_GL_H
