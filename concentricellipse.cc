#include "concentricellipse.hh"
#include <cmath>
#include <iostream>
int
FindConcentricEllipses(CvSeq *contourTree, 		       
		       std::vector<CvBox2D> *result,
		       int maxDepth,
		       double maxXDiff,
		       double maxYDiff,
		       double maxRatioDiff,
		       double maxFitError)
{
  if (contourTree == NULL) return 0;
  /* Traverse the the tree of contours finding concentric ellipses */
  CvTreeNodeIterator i;
  cvInitTreeNodeIterator(&i,contourTree,maxDepth);
  
  CvBox2D boxes[maxDepth];
  
  /* TreeNodeIterator does a depth first search so we know that we
     will find any concentric ellipses from consecutive contours */
  while(cvNextTreeNode(&i)) {
    CvSeq* c = (CvSeq*)i.node;
    if (c != NULL) {
      int count = c->total;
      if (count > 6) {
	/* Copy the points into floating point versions for the
	   ellipse fitter */
	CvPoint points[count];
	CvPoint2D32f fpoints[count];
	cvCvtSeqToArray( c, points );
	for( int pt = 0; pt < count; pt++ ) {
	  fpoints[pt].x = (float)points[pt].x;
	  fpoints[pt].y = (float)points[pt].y;
	}
	
	cvFitEllipse( fpoints, count, boxes+i.level );
	
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
	double asq = (double)boxes[i.level].size.width*(double)boxes[i.level].size.width / 4;
	double bsq = (double)boxes[i.level].size.height*(double)boxes[i.level].size.height / 4;
	double radians = (double)boxes[i.level].angle / 180 * M_PI;
	double sina = sin(radians);
	double cosa = cos(radians);
	double total = 0.0;
	for ( int pt=0;pt<count;pt++) {
	  double x = fpoints[pt].x-boxes[i.level].center.x;
	  double y = fpoints[pt].y-boxes[i.level].center.y;
	  double dist = 
	    sqrt( (x*cosa+y*sina)*(x*cosa+y*sina)/asq +
		  (y*cosa-x*sina)*(y*cosa-x*sina)/bsq ) - 1;
	  total+= dist*dist;
	}
	
	if (total < maxFitError * count) {
	    /*
	      The contour followed is the outside edge of the circle so
	      our estimate is out by 2 pixels.
	    */
	    boxes[i.level].size.width -=2;
	    boxes[i.level].size.height -=2;
	    


	  //	  std::cout << "erro " << total/count << " " << count << " " << boxes[i.level].center.x << " " << boxes[i.level].center.y << " " << ((double)boxes[i.level].size.height/(double)boxes[i.level].size.width) << std::endl;
	  
	  /* We accept this ellipse as a good fit */
	  /* If our ellipse shares aspect ratio and center points
	     with the parent ellipse then we accept them as
	     concentric */
	  if ((i.level > 0) &&
	      (fabs(boxes[i.level].center.x - boxes[i.level-1].center.x) < maxXDiff)  &&
	      (fabs(boxes[i.level].center.y - boxes[i.level-1].center.y) < maxYDiff) &&
	      (fabs((double)boxes[i.level].size.height/(double)boxes[i.level].size.width - 
		    (double)boxes[i.level-1].size.height/(double)boxes[i.level-1].size.width) < maxRatioDiff)) {
	    
	    result->push_back(boxes[i.level-1]);
	  }
	}
      }
    }
  }
  return result->size();
}
