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

#ifndef GL_IMAGE_SOURCE_GUARD
#define GL_IMAGE_SOURCE_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_GL_GL_H
#error "This version has been configured without OpenGL support"
#endif

#ifndef HAVE_GL_GLU_H
#error "This version has been configured without OpenGL GLU support"
#endif

#ifndef HAVE_GL_OSMESA_H
#error "This version has been configured without MESA Off-Screen rendering support"
#endif

#ifndef HAVELIB_GL
#error "libGL.so is required for this component"
#endif

#ifndef HAVELIB_OSMESA
#error "libOSMesa.so is required for this component"
#endif

#ifndef HAVELIB_GLU
#error "libGLU.so is required for this component"
#endif

#include <cantag/ImageSource.hh>
#include <cantag/Image.hh>
#include <cantag/Camera.hh>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/osmesa.h>

#include <cmath>
#include <list>
#include <vector>
#define GLIMAGESOURCE_DEBUG

namespace Cantag {

  /**
   * An image source that synthesizes the view based on an OpenGL
   * rendering of the tag.
   */
  class GLImageSource : public ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> {
  private:
    int m_width;
    int m_height;
    int m_supersample;
    unsigned char* m_buffer;
    float m_fov;
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> m_glimage;
    OSMesaContext m_ctx;
    std::vector<GLubyte*> m_tmap;
    std::vector<GLuint> m_textureid;
    float m_tagrotation;

    void InitTexture(const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& texture_source);

    void InitTexture(const std::list<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >&  texture_source);

  public:
    /**
     * Create the image source.  It will create images of the given
     * size, with the texture source mapped on the tag
     */
    GLImageSource(int width, int height, float fov, const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& texture_source);
    GLImageSource(int width, int height, float fov, const std::list<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >& texture_source);

    /**
     * Create the image source.  You must call Init before attempting to use the image source if created this way
     */
    GLImageSource(int width, int height, float fov);


    /**
     * Initialise the texture source and gl renderer
     */
    void Init(const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& texture_source);

    void Init(const std::list<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> >&  texture_source);

    
    virtual ~GLImageSource();


    /**
     * Set the parameters for this camera to correspond to the opengl configuration
     */
    void SetCameraParameters(Camera& cam);
  
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* Next();

    inline int GetWidth() const { return m_width; }
    inline int GetHeight() const { return m_height; }

    /**
     * Update the buffer to contain a tag with the given rotations
     * about the x,y, and z axes and the given central point.  The
     * optional overlay parameter is used to get the image source to
     * render on top of the provided image rather than to a new image
     *
     * \todo find some way of incorporating the lighting and occlusion
     * options.
     */
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* Next(float n_x, float n_y, float n_z, float centre_x, float centre_y, float centre_z, int texture_index = 0, Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* overlay = NULL);
  
    inline Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* Next(float theta, float phi, float x0, float y0, float z0, int texture_index=0,  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* overlay = NULL) {
      float nx = sin(theta/180*M_PI)*sin(phi/180*M_PI);
      float ny = sin(theta/180*M_PI)*cos(phi/180*M_PI);
      float nz = cos(theta/180*M_PI);
      return Next(nx,ny,nz,x0,y0,z0,texture_index,overlay);
    };

    inline void SetTagRotation(float degrees) {
      m_tagrotation = degrees;
    }
    inline void SetSuperSample(int mult) {
      m_supersample = mult;
    }

  };
}
#endif//GL_IMAGE_SOURCE_GUARD
