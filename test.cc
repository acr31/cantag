#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

#define MAX_FIT_ERROR 0.01
#define MAX_X_OFFSET 4
#define MAX_Y_OFFSET 4
#define MAX_RATIO_DIFF 0.1

int 
main(int argc,char* argv[]) 
{
  IplImage *img = cvLoadImage(argv[1]);
  CvSize size;
  size.width=img->width;
  size.height=img->height;
  IplImage *gray = cvCreateImage(size,IPL_DEPTH_8U,1);
  cvCvtColor(img,gray,CV_RGB2GRAY);

  int window_size = gray->width/8;
  window_size+= 1-(window_size %2);
  cvAdaptiveThreshold(gray,gray,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,window_size,40);
  cvCanny(gray,gray,128,128,3);  


  CvMemStorage *store = cvCreateMemStorage(0);
  CvSeq *seq = NULL;
  int num = cvFindContours(gray,store,&seq,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
  //  cvNamedWindow(argv[1],1);
  //  cvShowImage(argv[1],gray);
  //  cvWaitKey(0);
  cvDrawContours(img,seq,16776960,0,5,1,8);

  vector<CvBox2D> boxes;

  for(int i=0;i<num;i++) 
    {
      int count = seq->total;
      if (count > 6) {
	CvPoint points[count];
	CvPoint2D32f fpoints[count];
	cvCvtSeqToArray( seq, points );
	for( int i = 0; i < count; i++ )
	  {
	    fpoints[i].x = (float)points[i].x;
	    fpoints[i].y = (float)points[i].y;
	  }
	CvBox2D box;
	cvFitEllipse( fpoints, count, &box );
	if ((box.size.width < img->width) &&
	    (box.size.height < img->height)) {

	  double asq = (double)box.size.width*(double)box.size.width / 4;
	  double bsq = (double)box.size.height*(double)box.size.height / 4;
	  double sina = sin((double)box.angle / 180 * M_PI);
	  double cosa = cos((double)box.angle / 180 * M_PI);
	  // work out the error
	  double total = 0.0;
	  for ( int i=0;i<count;i++) {
	    double x = fpoints[i].x-box.center.x;
	    double y = fpoints[i].y-box.center.y;
	    double dist = 
	      sqrt( (x*cosa+y*sina)*(x*cosa+y*sina)/asq +
		    (y*cosa-x*sina)*(y*cosa-x*sina)/bsq ) - 1;
	    total+= dist*dist;
	  }
	  if (total < MAX_FIT_ERROR * count) {
	    boxes.push_back(box);
	  }
	}
      }
      seq = seq->h_next;
    }

  for(vector<CvBox2D>::const_iterator step = boxes.begin();step != boxes.end();step++) {
    for(vector<CvBox2D>::const_iterator search = boxes.begin();search != boxes.end();search++) {
      if ((search != step) && 
	  (abs(step->center.x - search->center.x) < MAX_X_OFFSET)  &&
	  (abs(step->center.y - search->center.y) < MAX_Y_OFFSET) &&
	  (fabs( (double)step->size.height/(double)step->size.width - (double)search->size.height/(double)search->size.width) < MAX_RATIO_DIFF))
      {
	  int color = CV_RGB( rand(), rand(), rand() );
	  cvEllipseBox( img, *search,color,3);
      }
    }
  }

  cvSaveImage(argv[2],img);
  cvSaveImage(argv[3],gray);
  cvReleaseImage(&img);
  cvReleaseImage(&gray);
  return 0;
}
