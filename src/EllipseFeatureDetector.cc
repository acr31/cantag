/**
 * $Header$
 *
 * $Log$
 * Revision 1.6  2004/02/16 16:02:27  acr31
 * *** empty log message ***
 *
 * Revision 1.5  2004/02/16 08:02:03  acr31
 * *** empty log message ***
 *
 * Revision 1.4  2004/02/13 21:47:39  acr31
 * work on ellipse fitting
 *
 * Revision 1.3  2004/02/08 20:30:17  acr31
 * changes to interfaces to add the ImageFilter class
 *
 * Revision 1.2  2004/02/06 21:11:44  acr31
 * adding ellipse fitting
 *
 * Revision 1.1  2004/02/01 14:26:24  acr31
 * moved rectangle2d to quadtangle2d and refactored implmentations
 *
 * Revision 1.6  2004/01/31 16:48:10  acr31
 * moved some arguments to #defines
 *
 * Revision 1.5  2004/01/28 17:19:43  acr31
 * providing my own implementation of draw ellipse
 *
 * Revision 1.4  2004/01/28 16:34:38  acr31
 * some debugging code for drawing lines onto ellipses
 *
 * Revision 1.3  2004/01/26 12:04:31  acr31
 * noted that the routines search for white rather than black objects
 *
 * Revision 1.2  2004/01/26 08:57:11  acr31
 * added compensation for ellipse contour being on the outside of the
 * ellipse
 *
 * Revision 1.1  2004/01/25 14:53:35  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.8  2004/01/23 22:37:46  acr31
 * added missing ReleaseImage call
 *
 * Revision 1.7  2004/01/23 11:57:08  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
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
#include <EllipseFeatureDetector.hh>

#include <opencv/cv.h>
#include <cmath>
#include <vector>
#include <fitellipse.hh>

#undef FILENAME
#define FILENAME "EllipseFeatureDetector.cc"

#define MAXLENGTH 1000
#define MAXXDIFF 10
#define MAXYDIFF 10
#define MAXRATIODIFF 0.1
#define MAXFITERROR 1

EllipseFeatureDetector::EllipseFeatureDetector() {};

void EllipseFeatureDetector::FindFeatures(Image *image,Camera* camera) { 
  std::vector<Ellipse2DChain*> results;

  image = LoadImage(image);

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

  CvPoint points[MAXLENGTH];
  CvPoint2D32f fpoints[MAXLENGTH];
  float flpoints[MAXLENGTH*2];
  CvBox2D current;
  CvSeq* c;
  while ((c = cvFindNextContour(scanner)) != NULL) {
    int count = c->total;
#ifdef IMAGE_DEBUG
    cvDrawContours(debug0,c,0,0,0,3,8);
#endif
    if ((CV_IS_SEQ_CURVE(c)) && (count > 6)) {
      PROGRESS("Found contour with "<<count<<" (>6) vertices");
      if (count > MAXLENGTH) { count = MAXLENGTH; }
      /* Copy the points into floating point versions for the
	 ellipse fitter */
      cvCvtSeqToArray( c, points ,cvSlice(0,count));
      int pointer = 0;
      for( int pt = 0; pt < count; pt++ ) {
	fpoints[pt].x = (float)points[pt].x;
	fpoints[pt].y = (float)points[pt].y;
	flpoints[pointer++] = (float)points[pt].x;
	flpoints[pointer++] = (float)points[pt].y;	
      }

      camera->ImageToNPCF(flpoints,count);
      Ellipse2D* e2d = fitellipse(flpoints,count);

      if (e2d) {
#ifdef IMAGE_DEBUG
	DrawEllipse(debug1,
		    e2d,
		    0,3);
#endif
	
	/* We accept this ellipse as a good fit */
	/* If our ellipse shares aspect ratio and center points
	   with the parent ellipse then we accept them as
	   concentric */

	Ellipse2D *newbox = e2d;

	for(std::vector<Ellipse2DChain*>::const_iterator i = results.begin();i!= results.end();i++) {
	  if (compare(newbox,(*i)->current)) {
	    PROGRESS("Found concentric partner");
	    Ellipse2DChain *toadd = *i;
	    while(toadd->nextchain != NULL) {
	      toadd = toadd->nextchain;
	    }
	    toadd->nextchain = new Ellipse2DChain(newbox);
	    newbox = NULL;
	    break;
	  }
	}

	if (newbox != NULL) {
	  PROGRESS("No concentric partner found");
	  results.push_back(new Ellipse2DChain(newbox));
	}
      }
    }
  }
#ifdef IMAGE_DEBUG
  for(std::vector<Ellipse2DChain*>::const_iterator i = results.begin();i!=results.end();i++) {
    Ellipse2DChain *tocheck = *i;
    if (tocheck->nextchain != NULL) {
      do {
	DrawEllipse(debug2, 
		    tocheck->current,
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
  cvReleaseImage(&copy);

  for(std::vector<Ellipse2DChain*>::const_iterator step = results.begin();step!=results.end();step++) {
    if ((*step)->nextchain != NULL) {
      std::cout << Decode(image, camera,(*step)->current) << std::endl;
    }
  }
}

bool EllipseFeatureDetector::compare(Ellipse2D *e1, Ellipse2D *e2) {
  PROGRESS("Comparing ("<<e1->m_x<<","<<e1->m_y<<") ("<<e1->m_height<<","<<e1->m_width<<") with ("<<e2->m_x<<","<<e2->m_y<<") ("<<e2->m_height<<","<<e2->m_width<<")");
  PROGRESS("      XDIFF = " << (fabs(e1->m_x - e2->m_x)) << " (thresh " << MAXXDIFF<<")");
  PROGRESS("      YDIFF = " << (fabs(e1->m_y - e2->m_y)) << " (thresh " << MAXYDIFF<<")");
  PROGRESS("      RATIO = " << (fabs(e1->m_height/e1->m_width - e2->m_height/e2->m_width)) << " (thresh " << MAXRATIODIFF<<")");
  return ( (fabs(e1->m_x - e2->m_x) < MAXXDIFF) &&
	   (fabs(e1->m_y - e2->m_y) < MAXYDIFF) &&
	   (fabs(e1->m_height/e1->m_width -
		 e2->m_height/e2->m_width)) < MAXRATIODIFF);
}
