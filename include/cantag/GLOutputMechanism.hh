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

#ifndef GLOUTPUT_MECHANISM_GUARD
#define GLOUTPUT_MECHANISM_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_GL_GL_H
#error "This version has been configured without OpenGL support"
#endif

#ifndef HAVELIB_GL
# error "This version has been configured without OpenGL support"
#endif

#include <iostream>
#include <map>
#include <ctime>

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//#include <GL/osmesa.h>

#include <cantag/GLRenderWindow.hh>
#include <cantag/Transform.hh>

namespace Cantag {

  class GLOutputMechanism : protected GLRenderWindow {
  private:
    int m_image_width;
    int m_image_height;
    int m_texture_width;
    int m_texture_height;
    bool m_displayListInitialised[9];
    GLuint m_textureid;
    GLubyte* m_tmap;
    GLfloat m_texture_maxx;
    GLfloat m_texture_maxy;

    void SetupCamera();
    void InitialiseScene();
    void InitialiseTexture();
    void RenderModel(int display_list);

  public:
    GLOutputMechanism(int window_width,int window_height,
		      int image_width, int image_height);
    ~GLOutputMechanism();
    void Flush();
    void Draw(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, bool flip);
    void Draw(const Transform& t, int display_list, bool flip);

    /**
     * Draws the text given.  The co-ordinates are (x,y,1) => so 0.5 > x > -0.5, 0.5 > y > -0.5
     */
    void DrawText(float x, float y, const char* s,int r = 0, int g = 0, int b = 0);
  };
}

#endif//GLOUTPUT_MECHANISM_GUARD
