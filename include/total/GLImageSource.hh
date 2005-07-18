/**
 * $Header$
 */

#ifndef GL_IMAGE_SOURCE_GUARD
#define GL_IMAGE_SOURCE_GUARD

#include <total/Config.hh>

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

#include <total/ImageSource.hh>
#include <total/Image.hh>
#include <total/Camera.hh>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/osmesa.h>

#include <cmath>

#define GLIMAGESOURCE_DEBUG

namespace Total {

  /**
   * An image source that synthesizes the view based on an OpenGL
   * rendering of the tag.
   */
  class GLImageSource : public ImageSource {
  private:
    int m_width;
    int m_height;
    unsigned char* m_buffer;
    float m_fov;
    Image m_glimage;
    OSMesaContext m_ctx;
    GLubyte* m_tmap;
    GLuint m_textureid;

  public:
    /**
     * Create the image source.  It will create images of the given
     * size, with the texture source mapped on the tag
     */
    GLImageSource(int width, int height, float fov, const Image& texture_source);
    virtual ~GLImageSource();


    /**
     * Set the parameters for this camera to correspond to the opengl configuration
     */
    void SetCameraParameters(Camera& cam);
  
    Image* Next();

    /**
     * Update the buffer to contain a tag with the given rotations about
     * the x,y, and z axes and the given central point.
     *
     * \todo find some way of incorporating the lighting and occlusion
     * options.
     */
    Image* Next(float n_x, float n_y, float n_z, float centre_x, float centre_y, float centre_z);
  
  };
}
#endif//GL_IMAGE_SOURCE_GUARD
