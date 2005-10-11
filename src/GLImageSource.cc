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

#include <cantag/GLImageSource.hh>

namespace Cantag {

  void GLImageSource::SetCameraParameters(Camera& cam) {
    // focal length
    
    // if we point the camera at a tag of size 1 and we want it to
    // appear as size 1 we must place it at a particular distance
    // determined by the field of view

    // tan(fov/2) = (size/2)/distance

    // d = 0.5/tan(fov/2)

    // however, we'd actually like the system to report the distance as 1
    // so set the focabl length to d

    float invd = tan(m_fov/2/180*M_PI)*2;

    cam.SetIntrinsic(m_height/invd,m_height/invd,m_width/2,m_height/2,0);

  
    // we want the tag to have size 1 at distance 1 in the camera co-ordinates.  However its actual size will be
    // 2*tan(fov/2) - therefore we need to translate the camera so its z-origin is at 2*tan(fov/2) so that when we do a perspective 

    float extrinsic[] = {
      1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      0,0,0,1
    };

    cam.SetExtrinsic(extrinsic);
  }
  
    

  GLImageSource::GLImageSource(int width, int height, float fov, const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& source) :
    m_width(width),
    m_height(height),
    m_fov(fov),
    m_glimage(width,height),
    m_supersample(1),
    m_tagrotation(0.f)
  {
    Init(source);
  }

  GLImageSource::GLImageSource(int width, int height, float fov, const std::list<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >& source) :
    m_width(width),
    m_height(height),
    m_fov(fov),
    m_glimage(width,height),
    m_supersample(1),
    m_tagrotation(0.f)
  {
    Init(source);
  }

  GLImageSource::GLImageSource(int width, int height, float fov) :
    m_width(width),
    m_height(height),
    m_fov(fov),
    m_glimage(width,height),
    m_supersample(1),
    m_tagrotation(0.f)
  {}

  
  void GLImageSource::Init(const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& source) {
    m_ctx = OSMesaCreateContext( GL_RGBA, NULL );
    m_buffer = (unsigned char*)malloc(m_width*m_height*4*m_supersample*m_supersample);     
    OSMesaMakeCurrent( m_ctx, m_buffer, GL_UNSIGNED_BYTE, m_width*m_supersample,m_height*m_supersample);
    InitTexture(source);
  }

  void GLImageSource::Init(const std::list<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >& source) {
    m_ctx = OSMesaCreateContext( GL_RGBA, NULL );
    m_buffer = (unsigned char*)malloc(m_width*m_height*4*m_supersample*m_supersample);     
    OSMesaMakeCurrent( m_ctx, m_buffer, GL_UNSIGNED_BYTE, m_width*m_supersample,m_height*m_supersample);
    InitTexture(source);
  }

  void GLImageSource::InitTexture(const std::list<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >& source) {
    for(std::list<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >::const_iterator i = source.begin(); i!=source.end();++i) {
      InitTexture(*i);
    }
  }

  void GLImageSource::InitTexture(const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& source) {
    m_tmap.push_back(new GLubyte[source.GetHeight()*source.GetWidth()*4]);
    GLubyte* tmap = *(m_tmap.rbegin());
    unsigned int pos=0;
    for(unsigned int j=0;j<source.GetWidth();++j) {
      for(unsigned int i=0;i<source.GetHeight();++i) {
	tmap[pos++] = (GLubyte)source.Sample(i,j).intensity();
	tmap[pos++] = (GLubyte)source.Sample(i,j).intensity();
	tmap[pos++] = (GLubyte)source.Sample(i,j).intensity();
	tmap[pos++] = (GLubyte)255;	
      }
    }
     
    // indicate that our data does not have each row of texels aligned
    // on a word boundary
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
     
    // create an unused name for this texture and store it in textureid
    GLuint textureid;
    glGenTextures(1,&textureid);
    m_textureid.push_back(textureid);

    // create a new texture object (this textureid is guarenteed to be
    // new) and assign it this textureid.  Set the dimensionality to 2D.
    glBindTexture(GL_TEXTURE_2D,textureid);
     
    // set our texture to repeat in both S and T directions
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     
    // which texel to use when magnifying or minifying this texture
    // GL_LINEAR which use a weighted sum of 2x2 array of nearest texels
    // - slower but smoother 
    // GL_NEAREST just uses the texel which is closest to the centre
    // co-ordinates of the pixel - quicker
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     
    // create our 2D texture, with only 1 resolution (no minimapping),
    // we'd like OpenGL to store it as an RGBA texture (though its free
    // to do what it likes), of width SIZE and height SIZE, the size of
    // the border is 0, the texture information is RGBA format, with
    // unsigned bytes, and held in tmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, source.GetHeight(),source.GetWidth(),
		 0, GL_RGBA, GL_UNSIGNED_BYTE, tmap);    
     
  };

  GLImageSource::~GLImageSource() {    
    for(std::vector<GLuint>::const_iterator i = m_textureid.begin();i!=m_textureid.end();++i) {
      GLuint ids[] = {*i};
      glDeleteTextures(1,ids);
    }
    for(std::vector<GLubyte*>::iterator i = m_tmap.begin();i!=m_tmap.end();++i) {
      delete[] *i;
    }
    OSMesaDestroyContext(m_ctx);
    free(m_buffer);
  };

  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* GLImageSource::Next() {
    return Next(0,0,-1,0,0,2);
  };


  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* GLImageSource::Next(float nx, float ny, float nz, 
					   float centre_x, float centre_y, float centre_z, int texture_id, Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* overlay ) {
    
    glClearColor(1.0,1.0,1.0,overlay ? 0.0 : 1.0);
    glShadeModel(GL_SMOOTH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0,1.0,1.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLfloat)m_fov, (GLfloat)m_width/(GLfloat)m_height,0,100.0);
    //    gluLookAt(-centre_x*m_supersample,-centre_y*m_supersample,-centre_z,-centre_x*m_supersample,-centre_y*m_supersample,-centre_z+1,0.0,-1.0,0.0);
    gluLookAt(-centre_x,-centre_y,-centre_z,-centre_x,-centre_y,-centre_z+1,0.0,-1.0,0.0);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //     glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    //     glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //     glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    //     glEnable(GL_POINT_SMOOTH);
    //     glEnable(GL_LINE_SMOOTH);
    //     glEnable(GL_POLYGON_SMOOTH); 
    
    // select the modelview matrix - transforms object co-ordinates to eye co-ordinates
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLfloat spot_position[] = {1,0,1,1};
    GLfloat spot_direction[] = {0,0,1};
    GLfloat ambient_light[] = {0.2,0.2,0.2,1.0};
    glLightfv(GL_LIGHT0, GL_POSITION,spot_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,spot_direction);
    //    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,2.0);
    //    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,1.0);
    //    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION,0.5);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF,45);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    
    // turn this on to do spot light stuff
    //    glEnable(GL_LIGHTING);
    //    glEnable(GL_LIGHT0);

    //    float factor = sqrt(nz*nz+ny*ny);
    
    //    float rotation[] = { -nz,  ny*nx/factor,                  nx, centre_x,
    //			 0,    -nz*nz/factor - nx*nx/factor,  ny, centre_y,
    //			 nx,   ny*nz/factor,                  nz, centre_z,
    //			 0,    0,                             0,  1 };

    // OpenGL requires column major representation
    //   float rotation[] = { -nz,0,nx,0,
    //			 ny*nx/factor,-nz*nz/factor-nx*nx/factor,ny*nz/factor,0,
    //			 nx,ny,nz,0,
    //			 0,0,0,1};

    float norm = sqrt(nx*nx+ny*ny+nz*nz);
    nx/=norm;
    ny/=norm;
    nz/=norm;

    //float factor = -sqrt(1-ny*ny);
    float factor = -sqrt(nx*nx+nz*nz);
    // column major representation 
    float rotation[] = { nz/factor, 0, -nx/factor, 0, 
			 -ny*nx/factor, factor, -ny*nz/factor, 0,
			 nx,ny,nz,0,
			 0,0,0,1};
    float tagsizescale =1;

    glScalef(tagsizescale,tagsizescale,1);
    glMultMatrixf(rotation);
    if (m_tagrotation != 0.f) {
      glRotatef(m_tagrotation,0,0,1);
    }
    

#ifdef GLIMAGESOURCE_DEBUG
    PROGRESS("Rotation is [ " << rotation[0] << " " << rotation[4] << " " << rotation[8] << " " << rotation[12]);
    PROGRESS("              " << rotation[1] << " " << rotation[5] << " " << rotation[9] << " " << rotation[13]);
    PROGRESS("              " << rotation[2] << " " << rotation[6] << " " << rotation[10] << " " << rotation[14]);
    PROGRESS("              " << rotation[3] << " " << rotation[7] << " " << rotation[11] << " " << rotation[15]);
#endif

    // enable texturing mode
    glEnable(GL_TEXTURE_2D);
    
    // turn on culling of back faces - this means the back of our tag
    // wont be drawn so we draw a back facing polygon (later on) to
    // ensure that it is drawn as solid white
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    // set the texturing to be DECAL - use directly as colours and
    // painted on the surface
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // activate the texture object for the tag
    glBindTexture(GL_TEXTURE_2D, m_textureid[texture_id]);
    GLfloat mat_colour[] = {1.0,1.0,1.0,1.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_colour);
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_colour);
    
    // define our shape - QUADS => quadruples of vertices interpreted as four-sided polygons
    // this is a front facing polygon that we map our texture to
    glBegin(GL_QUADS);
    // set the current texture co-ordinate and then the vertex that we want it to map to 
    glTexCoord2f(0.0, 0.0); glVertex3f(-1, -1, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(1, -1, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(1, 1, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-1, 1, 0.0); 

    glEnd();
    glFlush();
    
    // this is a back facing polygon that exactly lies over the
    // previous polygon that is plain white
    glBegin(GL_QUADS);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(-1, 1, 0.0); 
    glVertex3f(1, 1, 0.0);
    glVertex3f(1, -1, 0.0);
    glVertex3f(-1, -1, 0.0);
    glEnd();
    glFlush();
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    
    // add some occlusion
    /*
      glLoadIdentity();
      glBegin(GL_QUADS);
      glColor3f(1.0,1.0,1.0);
      glVertex3f(0.5, -1, 1.0); 
      glVertex3f(0.3, -1, 1.0);
      glVertex3f(0.3, 1,1.0);
      glVertex3f(0.5, 1,1.0);
      glEnd();
      glFlush();
    */

    int yout = 0;
    for(int ygl = 0; ygl < m_height; ++ygl) {
      int xout = 0;
      for(int xgl = 0; xgl < m_width; ++xgl) {
	int base = (ygl * m_width * m_supersample + xgl) * m_supersample * 4;
	unsigned long accumulator = 0;
	unsigned long alpha = 0;
	for (int yave = 0; yave < m_supersample; ++yave) {
	  for (int xave = 0; xave < m_supersample; ++ xave) {
	    int pointer = base + yave * 4 * m_width * m_supersample + xave *4;
	    accumulator += (0.3*(float)m_buffer[pointer] +
			    0.59*(float)m_buffer[pointer+1] +
			    0.11*(float)m_buffer[pointer+2]);
	    alpha += m_buffer[pointer+3];
	  }
	}
	accumulator /= (m_supersample*m_supersample);
	alpha /= (m_supersample*m_supersample);

	if (overlay) {
	  int value = overlay->SampleNoCheck(xout,yout).intensity();

	  overlay->DrawPixelNoCheck(xout,yout,Pixel<Pix::Fmt::Grey8>(accumulator * alpha / 255 + value * (255-alpha) / 255));
	}
	else {
	  m_glimage.DrawPixelNoCheck(xout,yout,Pixel<Pix::Fmt::Grey8>(accumulator));	
	}
	++xout;
      }
      ++yout;
    }
    /*
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(m_width*m_supersample,m_height*m_supersample);
    int pointer =0;
    for(int u=m_height*m_supersample-1;u>=0;--u) {
      for(int v=0;v<m_width*m_supersample;++v) {
	unsigned char value = (0.3*(float)m_buffer[pointer++] +
			       0.59*(float)m_buffer[pointer++] +
			       0.11*(float)m_buffer[pointer++]);
	output.DrawPixel(u,v,value);
      }
    }
    output.Save("gloutput.pnm");
    exit(-1);
    */
    return overlay ? overlay : &m_glimage;
  };


}
