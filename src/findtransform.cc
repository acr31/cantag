/**
 * $Header$
 */

#include <total/findtransform.hh>
#include <total/polysolve.hh>
#include <total/gaussianelimination.hh>
#include <cmath>

#undef SQUARE_TRANSFORM_DEBUG
#undef APPLY_TRANSFORM_DEBUG

namespace Total {

  void ApplyTransform(const float transform[16], float x, float y, std::vector<float>& points) {
    float projX = transform[0]*x + transform[1]*y + transform[3];
    float projY = transform[4]*x + transform[5]*y + transform[7];
    float projZ = transform[8]*x + transform[9]*y + transform[11];
    float projH = transform[12]*x + transform[13]*y + transform[15];

#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Transformed ("<<x<<","<<y<<","<<"0) on to ("<<projX<<","<<projY<<","<<projZ<<","<<projH<<")");
#endif
  
    projX /= projH;
    projY /= projH;
    projZ /= projH;
  
    projX /= projZ;
    projY /= projZ;

    points.push_back(projX);
    points.push_back(projY);
#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Projected ("<<x<<","<<y<<","<<"0) on to ("<<projX<<","<<projY<<")");
#endif
  }

  void ApplyTransform(const float transform[16], float x, float y, float z, float* projX, float* projY) {
    *projX = transform[0]*x + transform[1]*y + transform[2]*z + transform[3];
    *projY = transform[4]*x + transform[5]*y + transform[6]*z + transform[7];
    float projZ = transform[8]*x + transform[9]*y + transform[10]*z + transform[11];
    float projH = transform[12]*x + transform[13]*y + transform[14]*z + transform[15];

#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Transformed ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<","<<projZ<<","<<projH<<")");
#endif
  
    *projX /= projH;
    *projY /= projH;
    projZ /= projH;
  
    *projX /= projZ;
    *projY /= projZ;

#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Projected ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<")");
#endif

  }


  void ApplyTransform(const float transform[16], float x, float y, float* projX, float* projY) {
    ApplyTransform(transform,x,y,0,projX,projY);
  }


  void ApplyTransform(const float transform[16], std::vector<float>& points) {
    int numpoints = points.size();
    for(int i=0;i<numpoints;i+=2) {
      float x = points[i];
      float y = points[i+1];

      float projX = transform[0]*x + transform[1]*y + transform[3];
      float projY = transform[4]*x + transform[5]*y + transform[7];
      float projZ = transform[8]*x + transform[9]*y + transform[11];
      float projH = transform[12]*x + transform[13]*y + transform[15];
    
#ifdef APPLY_TRANSFORM_DEBUG
      PROGRESS("Transformed ("<<x<<","<<y<<","<<"0) on to ("<<projX<<","<<projY<<","<<projZ<<","<<projH<<")");
#endif
  
      projX /= projH;
      projY /= projH;
      projZ /= projH;
    
      projX /= projZ;
      projY /= projZ;
    
      points[i] = projX;
      points[i] = projY;
    }
  }

  void ApplyTransform(const float transform[16], float* points, int numpoints) {
    for(int i=0;i<numpoints*2;i+=2) {
      ApplyTransform(transform,points[i],points[i+1],0,points+i,points+i+1);
    }
  }

  void ApplyTransform3D(const float transform[16], float* points, int numpoints) {
    for(int i=0;i<numpoints*3;i+=3) {
      ApplyTransform(transform,points[i],points[i+1],points[i+2],points+i,points+i+1);
    }
  }

  void ApplyCameraTransform(const float transform[16], float* point) {
    float x = point[0];
    float y = point[1];
    float z = point[2];
    point[0] = transform[0]*x + transform[1]*y + transform[2]*z + transform[3];
    point[1] = transform[4]*x + transform[5]*y + transform[6]*z + transform[7];
    point[2] = transform[8]*x + transform[9]*y + transform[10]*z + transform[11];
    float projH = transform[12]*x + transform[13]*y + transform[14]*z + transform[15];
  
    point[0] /= projH;
    point[1] /= projH;
    point[2] /= projH;
  }

  void GetLocation(const float transform[16], float location[3], float tag_size) {
    location[0] = tag_size*transform[3]/transform[15];
    location[1] = tag_size*transform[7]/transform[15];
    location[2] = tag_size*transform[11]/transform[15];
    PROGRESS("Found location ("<<location[0]<<","<<location[1]<<","<<location[2]<<")");
  }


  void GetNormalVector(const float transform[16], const Camera& cam, float normal[3]) {
    // project (0,0,0) and (0,0,1).  Take the difference between them and normalize it

    // (0,0,0)
    float proj0x = transform[3];
    float proj0y = transform[7];
    float proj0z = transform[11];
    float proj0h = transform[15];
    cam.CameraToWorld(proj0x/proj0h,proj0y/proj0h,proj0z/proj0h,&proj0x,&proj0y,&proj0z);

    // (0,0,1)
    float proj1x = transform[2] + transform[3];
    float proj1y = transform[6] + transform[7];
    float proj1z = transform[10] + transform[11];
    float proj1h = transform[14] + transform[15];
    cam.CameraToWorld(proj1x/proj1h,proj1y/proj1h,proj1z/proj1h,&proj1x,&proj1y,&proj1z);

    // (1,0,0)
    float proj2x = transform[0] + transform[3];
    float proj2y = transform[4] + transform[7];
    float proj2z = transform[8] + transform[11];
    float proj2h = transform[12] + transform[15];
    cam.CameraToWorld(proj2x/proj2h,proj2y/proj2h,proj2z/proj2h,&proj2x,&proj2y,&proj2z);

    // (0,1,0)
    float proj3x = transform[1] + transform[3];
    float proj3y = transform[5] + transform[7];
    float proj3z = transform[9] + transform[11];
    float proj3h = transform[13] + transform[15];
    cam.CameraToWorld(proj3x/proj3h,proj3y/proj3h,proj3z/proj3h,&proj3x,&proj3y,&proj3z);


    float v1x = proj2x - proj0x;
    float v1y = proj2y - proj0y;
    float v1z = proj2z - proj0z;

    float v2x = proj3x - proj0x;
    float v2y = proj3y - proj0y;
    float v2z = proj3z - proj0z;


    /*  
	normal[0] = proj1x/proj1h - proj0x/proj0h;
	normal[1] = proj1y/proj1h - proj0y/proj0h;
	normal[2] = proj1z/proj1h - proj0z/proj0h;
    */
  
  
    // normal vector is the cross product
    normal[0] = v1y * v2z - v1z * v2y;
    normal[1] = v1z * v2x - v1x * v2z;
    normal[2] = v1x * v2y - v1y * v2x;

    float modulus = -sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
  
    normal[0]/=modulus;
    normal[1]/=modulus;
    normal[2]/=modulus;

#ifdef APPLY_TRANSFORM_DEBUG
    PROGRESS("Found normal vector ("<<normal[0]<<","<<normal[1]<<","<<normal[2]<<")");
#endif

  }

}
