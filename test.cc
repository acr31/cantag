#include <cv.h>
#include <highgui.h>
#include <iostream>

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
  cvCanny(gray,gray,10,250,3);  
  CvMemStorage *store = cvCreateMemStorage(0);
  CvSeq *seq = NULL;
  int num = cvFindContours(gray,store,&seq,sizeof(CvContour));

  cvDrawContours(img,seq,16776960,0,5,1,8);

  while(num-->0) 
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
	  cout << endl << "Box--------"<< endl;
	  cout << "Size " << box.size.width << " " << box.size.height << endl;
	  cout << "Center " << box.center.x << " " << box.center.y << endl;
	  cout << "Angle " << box.angle << endl;
	  cvEllipseBox(img,box,16711680,1);
	}
      }
      seq = seq->h_next;
    }

  cvNamedWindow(argv[1],1);
  cvShowImage(argv[1],img);
  cvWaitKey(0);
  cvReleaseImage(&img);
  cvReleaseImage(&gray);
  return 0;
}
