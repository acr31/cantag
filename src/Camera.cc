#include <Config.hh>
#include <Camera.hh>

Camera::Camera() :
  m_r2(0),m_r4(0),m_r6(0),
  m_d1(0),m_d2(0)
{
  m_extrinsic[0] = 1;    m_extrinsic[1] = 0;    m_extrinsic[2] = 0;  
  m_extrinsic[3] = 0;    m_extrinsic[4] = 1;    m_extrinsic[5] = 0;  
  m_extrinsic[6] = 0;    m_extrinsic[7] = 0;    m_extrinsic[8] = 1;  

  m_intrinsic[0] = 1;    m_intrinsic[1] = 0;    m_intrinsic[2] = 0.5;  
  m_intrinsic[3] = 0;    m_intrinsic[4] = 1;    m_intrinsic[5] = 0.5;  
  m_intrinsic[6] = 0;    m_intrinsic[7] = 0;    m_intrinsic[8] = 1;  
};

Camera::~Camera() {};

void Camera::SetExtrinsic(float extrinsic[9]) {
  m_extrinsic[0] = extrinsic[0];    m_extrinsic[1] = extrinsic[1];    m_extrinsic[2] = extrinsic[2];  
  m_extrinsic[3] = extrinsic[3];    m_extrinsic[4] = extrinsic[4];    m_extrinsic[5] = extrinsic[5];  
  m_extrinsic[6] = extrinsic[6];    m_extrinsic[7] = extrinsic[7];    m_extrinsic[8] = extrinsic[8];  
}

void Camera::SetIntrinsic(float intrinsic[9]) {
  m_intrinsic[0] = intrinsic[0];    m_intrinsic[1] = intrinsic[1];    m_intrinsic[2] = intrinsic[2];  
  m_intrinsic[3] = intrinsic[3];    m_intrinsic[4] = intrinsic[4];    m_intrinsic[5] = intrinsic[5];  
  m_intrinsic[6] = intrinsic[6];    m_intrinsic[7] = intrinsic[7];    m_intrinsic[8] = intrinsic[8];  
}

void Camera::SetIntrinsic(float xscale, float yscale, float principlex, float principley, float skew) {
  m_intrinsic[0] = xscale;          m_intrinsic[1] = skew;            m_intrinsic[2] = principlex;
  m_intrinsic[3] = 0;               m_intrinsic[4] = yscale;          m_intrinsic[5] = principley;
  m_intrinsic[6] = 0;               m_intrinsic[7] = 0;               m_intrinsic[8] = 1;  
}

void Camera::SetRadial(float r2,  float r4,  float r6) {
  m_r2 = r2; 
  m_r4 = r4; 
  m_r6 = r6;  
}

void Camera::SetTangential(float d1, float d2) {
  m_d1 = d1;
  m_d2 = d2;
}

void Camera::DistortPoints(float* points, int numpoints) {
  for(int i=0;i<numpoints*2;i+=2) {
    float x = points[i];
    float y = points[i+1];

    // 1) translate the points back to the principle point
    x -= m_intrinsic[2];
    y -= m_intrinsic[5];

    // 2) remove the x and y scaling
    x /= m_intrinsic[0];
    y /= m_intrinsic[4];

    // 3) Compute the distance from the principle point (now 0,0)
    double rpwr2 = x*x + y*y;
    double rpwr4 = rpwr2*rpwr2;
    double rpwr6 = rpwr4*rpwr2;
    
    double radialcoeff = 1 + m_r2*rpwr2 + m_r4*rpwr4 + m_r6*rpwr6;

    // 4) Compute the tangential offset
    double dxx = 2*m_d1*x*y + m_d2*(rpwr2+2*x*x);
    double dxy = m_d1*(rpwr2+2*y*y)+2*m_d2*x*y;

    // 5) Compute the new values of x and y
    double xd1 = radialcoeff*x+dxx;
    double xd2 = radialcoeff*y+dxy;

    // 6) rescale and return to image co-ordinates
    points[i] = m_intrinsic[0]*(xd1+m_intrinsic[1]*xd2)+m_intrinsic[2];
    points[i+1] = m_intrinsic[4]*xd2+m_intrinsic[5];
  }      
}

void Camera::UnDistortPoints(float* points, int numpoints) {
  for(int i=0;i<numpoints*2 ;i+=2) {
    float x = points[i];
    float y = points[i+1];

    DistortPoints(points+i,1);

    points[i] = x-points[i];
    points[i+1] = y-points[i+1];

    DistortPoints(points+i,1);

    points[i] = x-points[i];
    points[i+1] = y-points[i+1];    
  }
}

void Camera::UnDistortImage(Image* image) {
  Image* source = cvCloneImage(image);
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width;j++) {
      float points[] = {i,j};
      DistortPoints(points,1);
      DrawPixel(image,i,j,SampleImage(source,points[0],points[1]));
    }
  }
  cvReleaseImage(&source);
}
