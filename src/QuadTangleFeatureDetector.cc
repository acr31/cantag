/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/02/01 14:26:24  acr31
 * moved rectangle2d to quadtangle2d and refactored implmentations
 *
 * Revision 1.5  2004/01/31 16:48:10  acr31
 * moved some arguments to #defines
 *
 * Revision 1.4  2004/01/30 16:54:28  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.3  2004/01/26 12:04:31  acr31
 * noted that the routines search for white rather than black objects
 *
 * Revision 1.2  2004/01/26 08:57:53  acr31
 * removed check for minumum area of contour
 *
 * Revision 1.1  2004/01/25 14:53:36  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.2  2004/01/23 22:38:26  acr31
 * renamed debug images.  Now throws away outer most contour - edge of image.
 * And filters on block size.
 *
 * Revision 1.1  2004/01/23 15:42:29  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#include <QuadTangleFeatureDetector.hh>

#include <opencv/cv.h>
#include <cmath>
#include <vector>

#undef FILENAME
#define FILENAME "QuadTangleFeatureDetector.cc"

#define MAXXDIFF 10
#define MAXYDIFF 10

QuadTangleFeatureDetector::QuadTangleFeatureDetector() {};

/** 
 * Find rectangular white objects- for black tags you _must_ invert the image.
 */
void QuadTangleFeatureDetector::FindFeatures(Image *image) {
  std::vector<QuadTangle2DChain*> results;
  IplImage *copy = cvCloneImage(image);

#ifdef IMAGE_DEBUG
  IplImage *debug0 = cvCloneImage(image);
  cvConvertScale(debug0,debug0,0.5,128);
  IplImage *debug1 = cvCloneImage(debug0);
  IplImage *debug2 = cvCloneImage(debug0);
#endif

  CvMemStorage* store = cvCreateMemStorage(0);
  PROGRESS("Initializing contour scanner");
  CvContourScanner scanner = cvStartFindContours(copy,store,sizeof(CvContour),CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE );
  CvSeq* c;
  CvPoint points[4];


  while ((c = cvFindNextContour(scanner)) != NULL) {
    int count = c->total;
#ifdef IMAGE_DEBUG
    cvDrawContours(debug0,c,0,0,0,3,8);
#endif
    CvSeq *result = cvApproxPoly( c, sizeof(CvContour), store,
				  CV_POLY_APPROX_DP, 
				  cvContourPerimeter(c)*0.02, 0 );
    // Check for 4 vertices
    // Check for a convex contour
    if( result->total == 4 &&
	cvCheckContourConvexity(result) &&
	fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 1000) {
#ifdef IMAGE_DEBUG
      cvDrawContours(debug1,result,0,0,0,3);
#endif	   

      cvCvtSeqToArray(result,points,cvSlice(0,4));
      QuadTangle2D *newbox = new QuadTangle2D(points[0].x,points[0].y,
					    points[1].x,points[1].y,
					    points[2].x,points[2].y,
					    points[3].x,points[3].y);
					   
      for(std::vector<QuadTangle2DChain*>::const_iterator i = results.begin();i!= results.end();i++) {
	if (compare(newbox,(*i)->current)) {
	  PROGRESS("Found concentric partner");
	  QuadTangle2DChain *toadd = *i;
	  while(toadd->nextchain != NULL) {
	    toadd = toadd->nextchain;
	  }
	  toadd->nextchain = new QuadTangle2DChain(newbox);
	  newbox = NULL;
	  break;
	}
      }
      
      if (newbox != NULL) {
	PROGRESS("No concentric partner found");
	results.push_back(new QuadTangle2DChain(newbox));
      }
    }
  }
#ifdef IMAGE_DEBUG
  for(std::vector<QuadTangle2DChain*>::const_iterator i = results.begin();i!=results.end();i++) {
    QuadTangle2DChain *tocheck = *i;
    if (tocheck->nextchain != NULL) {
      do {
	cvLine(debug2,
	       cvPoint((int)tocheck->current->m_x0,(int)tocheck->current->m_y0),
	       cvPoint((int)tocheck->current->m_x1,(int)tocheck->current->m_y1),
	       0,
	       3);
	cvLine(debug2,
	       cvPoint((int)tocheck->current->m_x1,(int)tocheck->current->m_y1),
	       cvPoint((int)tocheck->current->m_x2,(int)tocheck->current->m_y2),
	       0,
	       3);
	cvLine(debug2,
	       cvPoint((int)tocheck->current->m_x2,(int)tocheck->current->m_y2),
	       cvPoint((int)tocheck->current->m_x3,(int)tocheck->current->m_y3),
	       0,
	       3);
	cvLine(debug2,
	       cvPoint((int)tocheck->current->m_x3,(int)tocheck->current->m_y3),
	       cvPoint((int)tocheck->current->m_x0,(int)tocheck->current->m_y0),
	       0,
	       3);	       
	tocheck = tocheck->nextchain;
      } while (tocheck !=NULL);
    }
  }
  cvSaveImage("debug-findrectangles0.jpg",debug0);
  cvSaveImage("debug-findrectangles1.jpg",debug1);
  cvSaveImage("debug-findrectangles2.jpg",debug2);
  cvReleaseImage(&debug0);
  cvReleaseImage(&debug1);
  cvReleaseImage(&debug2);
#endif
  
  cvEndFindContours(&scanner);
  cvReleaseMemStorage(&store);
  cvReleaseImage(&copy);

  for(std::vector<QuadTangle2DChain*>::const_iterator step = results.begin();step!=results.end();step++) {
    QuadTangle2DChain *rect = *step;
    do {
      try {
	std::cout << "Tag:" << Decode(image, rect->current ) << std::endl;
      }
      catch (...) {
	std::cout << "Caught"<<std::endl;
      }
      rect = rect->nextchain;
    } while (rect != NULL);
  }
}

bool QuadTangleFeatureDetector::compare(QuadTangle2D *r1, QuadTangle2D *r2)
{
  /*
    Check if the centre points of each rectangle are close

     A         B
      +-------+
      |\     /|
      | +---+ |
      | |\ /| |
      | | X | |
      | |/ \| |
      | +---+ |
      |/     \|
      +-------+
     D         C
  
  */

  return ((fabs(r1->m_xc - r2->m_xc) < MAXXDIFF) &&
	  (fabs(r1->m_yc - r2->m_yc) < MAXYDIFF));
}

  
