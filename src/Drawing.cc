#include "Config.hh"
#include "Drawing.hh"

#undef FILENAME
#define FILENAME "Drawing.cc"

#define STEPSIZE 0.001f

static void
polygon_approx(Image* image, CvPoint* points, int startindex, int length, float xc, float yc, float width, float height,  float angle_radians, int color, int thickness, float start_angle) {
  /**
   * The parametric equation for an ellipse
   *
   * x = xc + a*cos(angle_radians)*cos(t) + b*sin(angle_radians)*sin(t)
   * y = yc - a*sin(angle_radians)*cos(t) + b*cos(angle_radians)*sin(t)
   *
   * a = width/2; b=height/2
   * angle_radians is the angle between the axis given by width and the horizontal
   *
   */

  /** 
   * Plan: create a polygon approximation to the ellipse and then get
   * opencv to render it for us
   */

  CvPoint* ppoints[1];
  ppoints[0] = points;

  float cosa = cos(angle_radians);
  float sina = sin(angle_radians);
  float a = width/2;
  float b = height/2;

  float currentAngle = start_angle;
  for(int i=startindex;i<length+startindex;i++) {
    float cost = cos(currentAngle);
    float sint = sin(currentAngle);
    // remember that y increases down from the top of the image so we
    // do yc minus point rather than yc + point
    points[i] = cvPoint( cvRound(xc + a*cosa*cost + b*sina*sint) , 
			 cvRound(yc + a*sina*cost - b*cosa*sint) );
    currentAngle += STEPSIZE;
  }

  int numvertices = startindex+length;
  if (thickness > 0) {
    cvPolyLine(image,
	       ppoints,
	       &numvertices,
	       1, // number of contours
	       true, // isClosed
	       color,thickness);
  }
  else {
    cvFillPoly(image,
	       ppoints,
	       &numvertices,
	       1, // number of contours
	       color);
  }


}

/**
 * Draw an ellipse with centre x,y and given width and height - note
 * width and height are twice the size of the major and minor axes
 * they represent.  angle_radians is the angle between the horizontal
 * and the axis given by width
 */
void 
DrawEllipseArc(Image *image, 
	       float xc, float yc, 
	       float width, float height, 
	       float angle_radians, 
	       float start_angle, float end_angle, int color, int thickness) {
  assert(thickness > 0 || thickness == -1);

  int numsteps = (int)(((float)(end_angle - start_angle))/STEPSIZE)+1; // add one to get to the edge of the sector
  // add in an extra point (the centre) if we are drawing a slice
  CvPoint points[numsteps + 1]; 
  points[0] = cvPoint(cvRound(xc),cvRound(yc));
  polygon_approx(image,points, 1, numsteps, xc, yc ,width, height, angle_radians, color, thickness, start_angle); 
}

void 
DrawEllipse(Image *image, 
	    float xc, float yc, 
	    float width, float height, 
	    float angle_radians, 
	    int color, int thickness) {

  assert(thickness > 0 || thickness == -1);

  int numsteps = (int)(2*PI/STEPSIZE); 

  CvPoint points[numsteps]; 
  polygon_approx(image,points,0, numsteps, xc, yc ,width, height, angle_radians, color, thickness, 0);
}

