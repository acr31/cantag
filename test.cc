#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <vector>

using namespace std;

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
  int num = cvFindContours(gray,store,&seq,sizeof(CvContour));

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
	  boxes.push_back(box);
	  /*	  cout << endl << "Box--------"<< endl;
	  cout << "Size " << box.size.width << " " << box.size.height << endl;
	  cout << "Center " << box.center.x << " " << box.center.y << endl;
	  cout << "Angle " << box.angle << endl;
	  cvEllipseBox(img,box,16711680,1);
	  */
	}
      }
      seq = seq->h_next;
    }

  int first=1;
  for(vector<CvBox2D>::const_iterator step = boxes.begin();step != boxes.end();step++) {
    for(vector<CvBox2D>::const_iterator search = boxes.begin();search != boxes.end();search++) {
      if ((search != step) && 
	  (fabs(step->center.x - search->center.x) + fabs(step->center.y - search->center.y) < 10) &&
	  (step->size.width > search->size.width) &&
	  (step->size.height > search->size.height))
	{
	  if (first == 1 || 1) {
	    int color = CV_RGB( rand(), rand(), rand() );
	    cvEllipseBox(img,*step,color,5);
	    cvEllipseBox(img,*search,color,5);
	    first = 0;
	  }

	}
    }
  }

  cvNamedWindow(argv[1],1);
  cvShowImage(argv[1],img);
  cvWaitKey(0);
  cvReleaseImage(&img);
  cvReleaseImage(&gray);
  return 0;
}
