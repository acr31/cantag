/**
 * $Header$
 */
#include <Config.hh>
#include <QuadTangle.hh>
#include <opencv/cv.h>
#include <cmath>

#define COMPARE_THRESH 1

#define QUADTANGLE_DEBUG

QuadTangle::QuadTangle(float* points, int numpoints, bool prev_fitted) {
  
  if (!prev_fitted) {
    CvMemStorage* store = cvCreateMemStorage(0);
    CvSeq* contour = cvCreateSeq(CV_SEQ_POLYLINE,sizeof(CvSeq),sizeof(CvPoint),store);
    for(int i=0;i<numpoints*2;i+=2) {
      CvPoint p = cvPoint((int)points[i],(int)points[i+1]);
      cvSeqPush(contour,&p);    
    }

#ifdef QUADTANGLE_DEBUG
    PROGRESS("Copied points back into opencv sequence");
#endif

    CvSeq *result = cvApproxPoly( contour, sizeof(CvContour), store,
				  CV_POLY_APPROX_DP, 
				  cvContourPerimeter(contour)*0.02, 0 );
#ifdef QUADTANGLE_DEBUG
    PROGRESS("Applied polygon approximation");
#endif

   // Check for 4 vertices
    // Check for a convex contour
    
    if( result->total == 4 &&
	cvCheckContourConvexity(result) &&
	fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 1000) {    
#ifdef QUADTANGLE_DEBUG
      PROGRESS("Accepting polygon");
#endif
      float corners[4];
      cvCvtSeqToArray(result,corners,cvSlice(0,4));
      m_x0 = corners[0];
      m_y0 = corners[1];
      m_x1 = corners[2];
      m_y1 = corners[3];
      m_x2 = corners[4];
      m_y2 = corners[5];
      m_x3 = corners[6];
      m_y3 = corners[7];
      m_fitted = true;
    }
    else {
#ifdef QUADTANGLE_DEBUG
      PROGRESS("Rejecting polygon");
#endif
      m_fitted = false;
    }
  
    cvReleaseMemStorage(&store);
  }
  else {
    m_fitted = false;
  }
}

QuadTangle::QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3) : m_x0(x0), m_y0(y0),m_x1(x1), m_y1(y1),m_x2(x2), m_y2(y2),m_x3(x3), m_y3(y3), m_fitted(true) {}

float dist(float x0, float y0, float x1, float y1) {
  return sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) );
}

bool QuadTangle::Compare(const QuadTangle& o) const {
  return ((dist(m_x0,m_y0,o.m_x0,o.m_y0) < COMPARE_THRESH) &&
	  (dist(m_x1,m_y1,o.m_x1,o.m_y1) < COMPARE_THRESH) &&
	  (dist(m_x2,m_y2,o.m_x2,o.m_y2) < COMPARE_THRESH) &&
	  (dist(m_x3,m_y3,o.m_x3,o.m_y3) < COMPARE_THRESH));
}
