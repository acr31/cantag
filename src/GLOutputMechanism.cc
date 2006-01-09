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

extern "C" {
# include <sys/timeb.h>
}
#include <cantag/GLOutputMechanism.hh>

namespace Cantag {

  GLOutputMechanism::GLOutputMechanism(int width, int height, const Camera& camera) : GLRenderWindow(width,height) {
    for(int i=0;i<9;++i) {
      m_displayListInitialised[i] = false;
    }
    SetupCamera(camera);
    InitialiseScene(camera);
  };

  void GLOutputMechanism::SetupCamera(const Camera& camera) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // fov in the y direction
    float fov = 2.f * atan(camera.GetYScale()/ (float)m_height / 2.f) / M_PI * 180.f;
    //    gluPerspective((GLfloat)fov, (GLfloat)camera.GetXScale()/(GLfloat)camera.GetYScale(),0,1000.0);
    gluPerspective((GLfloat)fov,1,1,1000.0);

    // select the modelview matrix - transforms object co-ordinates to eye co-ordinates
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0,0.0,
	      0.0, 0.0,1.0,
	      0.0,-1.0,0.0);
    
    GLfloat ambient[] = { 0.25,0.25,0.25, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat position[] = { 0.0,1.0,1.0,1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);  
    glDepthFunc(GL_LESS);
    glDepthRange(0,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
  }

  void GLOutputMechanism::InitialiseScene(const Camera& camera) {}

  void GLOutputMechanism::RenderModel(int display_list) {
    if (m_displayListInitialised[display_list]) {
      glCallList(display_list+1);
    }
    else {
      glNewList(display_list+1, GL_COMPILE_AND_EXECUTE);
      switch(display_list) {
      case 0: {
	GLfloat ambient[] = {0.33, 0.22, 0.03, 1.0};
	GLfloat diffuse[] = {0.78, 0.57, 0.11, 1.0};
	GLfloat specular[] = {0.f,0.f,0.f, 1.0};
	GLfloat shininess = 0.f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glRotatef(-90,1.f,0.f,0.f);
	glutSolidTeapot(1);
	break;
      }
      case 1: {
	GLfloat ambient[] = {0.0, 0.0, 0.0,1.0};
	GLfloat diffuse[] = {0.5, 0.0, 0.0,1.0};
	GLfloat specular[] = {0.7, 0.6, 0.6,1.0};
	GLfloat shininess = 32.0;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glRotatef(-90,1.f,0.f,0.f);
	glutSolidTorus(0.4,0.7,20,20);
	break;
      }
      case 2: {
	GLfloat ambient[] = {0.0215, 0.1745, 0.0215,1.0};
	GLfloat diffuse[] = {0.07568, 0.61424, 0.07568,1.0};
	GLfloat specular[] = {0.633, 0.727811, 0.633,1.0};
	GLfloat shininess = 76.8;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glutSolidTetrahedron();
	break;
      }
      case 3: {
	GLfloat ambient[] = {0.02, 0.02, 0.02,1.0};
	GLfloat diffuse[] = {0.02, 0.01, 0.01,1.0};
	GLfloat specular[] = {0.4, 0.4, 0.4,1.0};
	GLfloat shininess = .78125;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glRotatef(180,1.f,0.f,0.f);
	glutSolidCone(1,1.5,20,5);
	break;
      }
      case 4: {
	GLfloat ambient[] = {0.1745 ,0.01175 ,0.01175,1.0};
	GLfloat diffuse[] = {0.61424 ,0.04136 ,0.04136,1.0};
	GLfloat specular[] = {0.727811 ,0.626959 ,0.626959,1.0};
	GLfloat shininess = 0.6;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glTranslatef(0,0,-0.5);
	glutSolidSphere(0.8,20,20);
	break;
      }
      case 5: {
	GLfloat ambient[] = {0.1 ,0.18725 ,0.1745,1.0};
	GLfloat diffuse[] = {0.396 ,0.74151 ,0.69102,1.0};
	GLfloat specular[] = {0.297254 ,0.30829 ,0.306678,1.0};
	GLfloat shininess = 0.1;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glTranslatef(0,0,-0.5);
	glutSolidCube(1.f);
	break;
      }
      case 6: {
	GLfloat ambient[] = {0.24725 ,0.1995 ,0.0745,1.0};
	GLfloat diffuse[] = {0.75164 ,0.60648 ,0.22648,1.0};
	GLfloat specular[] = {0.628281 ,0.555802 ,0.366065,1.0};
	GLfloat shininess = 0.4;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glScalef(0.7,0.7,0.7);
	glutSolidIcosahedron();
	break;
      }
      case 7: {
	GLfloat ambient[] = {0.0 ,0.05 ,0.05,1.0};
	GLfloat diffuse[] = {0.4 ,0.5 ,0.5,1.0};
	GLfloat specular[] = {0.04 ,0.7 ,0.7,1.0};
	GLfloat shininess = .078125;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glScalef(0.5,0.5,0.5);
	glutSolidDodecahedron();
	break;
      }
      case 8: {
	GLfloat ambient[] = {0.0 ,0.05 ,0.05,1.0};
	GLfloat diffuse[] = {0.4 ,0.5 ,0.5,1.0};
	GLfloat specular[] = {0.04 ,0.7 ,0.7,1.0};
	GLfloat shininess = .078125;
	glMaterialfv (GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glBegin(GL_QUADS);
	glVertex3f(-1, -1, 0.0);
	glVertex3f(1, -1, 0.0);
	glVertex3f(1, 1, 0.0);
	glVertex3f(-1, 1, 0.0); 
	glEnd();
	break;
      }
      };
      glEndList();
      m_displayListInitialised[display_list] = true;
    }
  }

  void GLOutputMechanism::Draw(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) {
    glRasterPos3f(-0.9999999,-0.9999999,2);
    glPixelZoom( 1.0, -1.0 );
    glDrawPixels(m_width,m_height,GL_LUMINANCE,GL_UNSIGNED_BYTE,image.GetContents());
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  void GLOutputMechanism::Draw(const Transform& t, int display_list) {
    /*
      timeb current_time;
      ftime(&current_time);
      int current = ((current_time.time * 1000 + current_time.millitm) / 10) % 100;
      float intensity = (float)current / 100.f;
      GLfloat diffuse[] = { 1.0, intensity, intensity, 1.0 };
      glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    GLfloat matrix[] = { t[0],t[4],t[8],t[12],
    			 t[1],t[5],t[9],t[13],
			 t[2],t[6],t[10],t[14],
			 t[3],t[7],t[11],t[15] };
    glMultMatrixf(matrix);    
    //glRotatef(angle,0,0,1);
    RenderModel(display_list);
    glPopMatrix();
  }

  void GLOutputMechanism::DrawText(float x, float y, const char* s,int r, int g, int b) {
    glDisable(GL_LIGHTING);
    GLfloat mat[] = {(float)r/255.f,(float)g/255.f,(float)b/255.f,1.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,mat);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0.f);
    glColor4f((float)r/255.f,(float)g/255.f,(float)b/255.f,1.f);
    glRasterPos3f(x,y,1);
    size_t len = strlen(s);
    for (size_t i = 0; i < len; ++i) {
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
    }
    glEnable(GL_LIGHTING);
  }

  void GLOutputMechanism::Flush() {
    glXSwapBuffers(m_display, m_window); /* buffer swap does implicit glFlush */  
  }
};


