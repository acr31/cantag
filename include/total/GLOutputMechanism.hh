/**
 * $Header$
 */

#ifndef GLOUTPUT_MECHANISM_GUARD
#define GLOUTPUT_MECHANISM_GUARD

#include <tripover/Config.hh>

#ifndef HAVE_GL_GL_H
#error "This version has been configured without OpenGL support"
#endif

#ifndef HAVELIB_GL
# error "This version has been configured without OpenGL support"
#endif

#include <tripover/ContourTree.hh>
#include <tripover/ShapeTree.hh>
#include <tripover/WorldState.hh>
#include <iostream>

#include <GL/glx.h>

class GLOutputMechanism {
private:
  Display *m_display;
  XVisualInfo* m_visual;
  GLXContext m_context;
  Colormap m_colormap;
  Window m_window;
  bool displayListInited;
  
public:
  GLOutputMechanism(int argc, char* argv[],int width,int height);
  ~GLOutputMechanism();
  inline void FromImageSource(const Image& image) {};
  inline void FromThreshold(const Image& image) {};
  inline void FromContourTree(const ContourTree& contours) {}
  inline void FromRemoveIntrinsic(const ContourTree& contours) {};
  template<class S> inline void FromShapeTree(const ShapeTree<S>& shapes) {};
  template<int PAYLOADSIZE> void FromTag(const WorldState<PAYLOADSIZE>& world);
  inline void Flush() {};

private:
  void Draw(int mode);
};

template<int PAYLOAD_SIZE> void GLOutputMechanism::FromTag(const WorldState<PAYLOAD_SIZE>& world) { 
    glMatrixMode(GL_MODELVIEW);
    /* reset modelview matrix to the identity matrix */
    glLoadIdentity();
    /* move the camera back three units */
    glTranslatef(0.0, 0.0, -3.0);
    glRasterPos2i(0,0);
    glPixelZoom(1.0, -1.0);
    //    glDrawPixels(newdata->GetWidth(),newdata->GetHeight(),GL_RED,GL_UNSIGNED_BYTE, newdata->GetDataPointer());

    for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = world.GetNodes().begin();
	i!=world.GetNodes().end();
	++i) {
      LocatedObject<PAYLOAD_SIZE>* loc = *i;
      if (loc->tag_codes.size() > 0) {
	float nx = loc->normal[0];
	float ny = loc->normal[1];
	float nz = loc->normal[2];
	std::cout << "Tag: " << loc->location[0] << "," << loc->location[1] << "," << loc->location[2] << std::endl;
	
	/* rotate by X, Y, and Z angles */
	float norm = sqrt(nx*nx+ny*ny+nz*nz);
	nx/=norm;
	ny/=norm;
	nz/=norm;
	
	float factor = -sqrt(nx*nx+nz*nz);
	float rotation[] = { nz/factor, 0, -nx/factor, 0, 
			     -ny*nx/factor, factor, -ny*nz/factor, 0,
			     nx,ny,nz,0,
			     0,0,0,1};
	float angle = -loc->angle/M_PI*180;
	
	glMatrixMode(GL_MODELVIEW);
	/* reset modelview matrix to the identity matrix */
	glLoadIdentity();
	/* move the camera back three units */
	glTranslatef(0.0, 0.0, -3.0);
	
	glMultMatrixf(loc->transform);
	/* rotate tag around z axis for angle */
	//	glRotatef(angle,0.0,0.0,1.0);
	
	float tagsizescale=(34-loc->location[2])/20;
	
	//	glScalef(tagsizescale,tagsizescale,tagsizescale);
	
	Draw(1);
	return;
      }
    }
    Draw(2);
}


#endif//GLOUTPUT_MECHANISM_GUARD
