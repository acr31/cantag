/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/23 15:42:29  acr31
 * Initial commit of rectangle detection code
 *
 *
 */
#include "findrectangles.hh"

#include <cv.h>
#include <cmath>

void FindRectangles(Image *image, int maxXDiff, int maxYDiff, std::vector<Rectangle2DChain*> *results) { 

#ifdef IMAGE_DEBUG
  IplImage *debug0 = cvCloneImage(image);
  cvConvertScale(debug0,debug0,0.5,128);
  IplImage *debug1 = cvCloneImage(debug0);
  IplImage *debug2 = cvCloneImage(debug0);
#endif

  CvMemStorage* store = cvCreateMemStorage(0);
  PROGRESS("Initializing contour scanner");
  CvContourScanner scanner = cvStartFindContours(image,store,sizeof(CvContour),CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE );
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
	cvCheckContourConvexity(result) ) {
#ifdef IMAGE_DEBUG
      cvDrawContours(debug1,result,0,0,0,3);
#endif	   

      cvCvtSeqToArray(result,points,cvSlice(0,4));
      Rectangle2D *newbox = new Rectangle2D(points[0].x,points[0].y,
					    points[1].x,points[1].y,
					    points[2].x,points[2].y,
					    points[3].x,points[3].y);
					   
      for(std::vector<Rectangle2DChain*>::const_iterator i = results->begin();i!= results->end();i++) {
	if (compare(newbox,(*i)->current,maxXDiff,maxYDiff)) {
	  PROGRESS("Found concentric partner");
	  Rectangle2DChain *toadd = *i;
	  while(toadd->nextchain != NULL) {
	    toadd = toadd->nextchain;
	  }
	  toadd->nextchain = new Rectangle2DChain(newbox);
	  newbox = NULL;
	  break;
	}
      }
      
      if (newbox != NULL) {
	PROGRESS("No concentric partner found");
	results->push_back(new Rectangle2DChain(newbox));
      }
    }
  }
#ifdef IMAGE_DEBUG
  for(std::vector<Rectangle2DChain*>::const_iterator i = results->begin();i!=results->end();i++) {
    Rectangle2DChain *tocheck = *i;
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

static bool compare(Rectangle2D *r1, Rectangle2D *r2, float xdist, float ydist) 
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

  return ((fabs(r1->m_xc - r2->m_xc) < xdist) &&
	  (fabs(r1->m_yc - r2->m_yc) < ydist));
}

  