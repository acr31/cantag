/**
 * $Header$
 *
 * $Log$
 * Revision 1.6  2004/01/23 09:08:20  acr31
 * added missing bracket..
 *
 * Revision 1.5  2004/01/22 12:01:03  acr31
 * added a test for a curved contour before trying to ellipse fit
 *
 * Revision 1.4  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.3  2004/01/21 12:01:41  acr31
 * moved Location2DChain definition to Location2D.hh and added a destructor
 *
 * Revision 1.2  2004/01/21 11:55:09  acr31
 * added keywords for substitution
 *
 */
#include "findellipses.hh"

#include <cv.h>
#include <cmath>

void FindEllipses(Image *image, int maxDepth, int maxLength, float  maxXDiff, float maxYDiff, float maxRatioDiff, float maxFitError, std::vector<Location2DChain*> *results) { 
  IplImage *copy = cvCloneImage(image); // the find contours process changes the image ;-(
  
#ifdef IMAGE_DEBUG
  IplImage *debug0 = cvCloneImage(image);
  cvConvertScale(debug0,debug0,0.5,128);
  IplImage *debug1 = cvCloneImage(debug0);
  IplImage *debug2 = cvCloneImage(debug0);
#endif

  CvMemStorage* store = cvCreateMemStorage(0);
  PROGRESS("Initializing contour scanner");
  CvContourScanner scanner = cvStartFindContours(copy,store,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

  CvPoint points[maxLength];
  CvPoint2D32f fpoints[maxLength];
  CvBox2D current;
  CvSeq* c;
  while ((c = cvFindNextContour(scanner)) != NULL) {
    int count = c->total;
#ifdef IMAGE_DEBUG
    cvDrawContours(debug0,c,0,0,0,3,8);
#endif
    if ((CV_IS_SEQ_CURVE(c)) && (count > 6)) {
      PROGRESS("Found contour with "<<count<<" (>6) vertices");
      if (count > maxLength) { count = maxLength; }
      /* Copy the points into floating point versions for the
	 ellipse fitter */
      cvCvtSeqToArray( c, points ,cvSlice(0,count));
      for( int pt = 0; pt < count; pt++ ) {
	fpoints[pt].x = (float)points[pt].x;
	fpoints[pt].y = (float)points[pt].y;
      }

      cvFitEllipse( fpoints, count,&current);

      if (calcerror(&current,fpoints,count,maxFitError)) {
#ifdef IMAGE_DEBUG
	cvEllipseBox(debug1,current,0,3);
#endif
	/*
	  The contour followed is the outside edge of the circle so
	  our estimate is out by 2 pixels.
	*/
	//current.size.width -=2;
	//	current.size.height -=2;
	
	/* We accept this ellipse as a good fit */
	/* If our ellipse shares aspect ratio and center points
	   with the parent ellipse then we accept them as
	   concentric */

	Location2D *newbox = new Location2D(current.center.x,
					    current.center.y,
					    current.size.width,
					    current.size.height,
					    current.angle);

	for(std::vector<Location2DChain*>::const_iterator i = results->begin();i!= results->end();i++) {
	  if (compare(newbox,(*i)->current,maxXDiff,maxYDiff,maxRatioDiff)) {
	    PROGRESS("Found concentric partner");
	    Location2DChain *toadd = *i;
	    while(toadd->nextchain != NULL) {
	      toadd = toadd->nextchain;
	    }
	    toadd->nextchain = new Location2DChain(newbox);
	    newbox = NULL;
	    break;
	  }
	}

	if (newbox != NULL) {
	  PROGRESS("No concentric partner found");
	  results->push_back(new Location2DChain(newbox));
	}
      }
    }
  }
#ifdef IMAGE_DEBUG
  for(std::vector<Location2DChain*>::const_iterator i = results->begin();i!=results->end();i++) {
    Location2DChain *tocheck = *i;
    if (tocheck->nextchain != NULL) {
      do {
	cvEllipse(debug2,
		  cvPoint((int)tocheck->current->m_x,(int)tocheck->current->m_y),
		  cvSize((int)(tocheck->current->m_width*0.5),(int)(tocheck->current->m_height*0.5)),
		  tocheck->current->m_angle_radians/CV_PI*180,
		  0,
		  360,
		  0,
		  3);		  
	tocheck = tocheck->nextchain;
      } while (tocheck !=NULL);
    }
  }
  cvSaveImage("debug-findellipses0.jpg",debug0);
  cvSaveImage("debug-findellipses1.jpg",debug1);
  cvSaveImage("debug-findellipses2.jpg",debug2);
  cvReleaseImage(&debug0);
  cvReleaseImage(&debug1);
  cvReleaseImage(&debug2);
#endif

  cvEndFindContours(&scanner);
  cvReleaseMemStorage(&store);
}

static inline bool compare(Location2D *e1, Location2D *e2, float maxXDiff, float maxYDiff, float maxRatioDiff) {
  PROGRESS("Comparing ("<<e1->m_x<<","<<e1->m_y<<") ("<<e1->m_height<<","<<e1->m_width<<") with ("<<e2->m_x<<","<<e2->m_y<<") ("<<e2->m_height<<","<<e2->m_width<<")");

  return ( (fabs(e1->m_x - e2->m_x) < maxXDiff) &&
	   (fabs(e1->m_y - e2->m_y) < maxYDiff) &&
	   (fabs(e1->m_height/e1->m_width -
		 e2->m_height/e2->m_width)) < maxRatioDiff);
}

static inline bool calcerror(CvBox2D *ellipse, CvPoint2D32f *fpoints, int count, float maxFitError) {
  /* Work out the error for this fit.  This just tries every
     point on the contour with the ellipse function from the
     fitter and accumulates the difference 
     
     Ellipse eqn:
     
     x^2     y^2
     ---  +  ---  =  1
     a^2     b^2
     
     Translate to correct position:
     
     (x-p)^2      (y-q)^2
     --------  +  --------   =  1
       a^2           b^2
   
     Rotate to the correct orientation:
     
     ((x-p)cos(t)+(y-q)sin(t))^2      ((y-q)cos(t)-(x-p)sin(t))^2
     ---------------------------   +  ---------------------------  =  1
                a^2                           b^2
		
     p = box.center.x;
     q = box.center.y;
     a = box.size.width/2;
     b = box.size.height/2;
     t = box.angle (convert degrees to radians)
  */
  float asq = (float)ellipse->size.width*(float)ellipse->size.width / 4;
  float bsq = (float)ellipse->size.height*(float)ellipse->size.height / 4;
  float radians = (float)ellipse->angle / 180 * CV_PI;
  float sina = sin(radians);
  float cosa = cos(radians);
  float total = 0.0;
  for (int pt=0;pt<count;pt++) {
    float x = fpoints[pt].x-ellipse->center.x;
    float y = fpoints[pt].y-ellipse->center.y;
    float dist = 
      sqrt( (x*cosa+y*sina)*(x*cosa+y*sina)/asq +
	    (y*cosa-x*sina)*(y*cosa-x*sina)/bsq ) - 1;
    total+= dist*dist;
  }
  PROGRESS("Total error was "<< (total/count) << " and threshold is "<<maxFitError);
  return (total < maxFitError * count);
}
