#include <cv.h>
#include <highgui.h>
#include <cmath>
#include <iostream>

#include "TripOriginalTag.hh"
#include "Tag.hh"

#define SECTORS 16
#define RINGS 2
#define SYNC_SECTORS 24
#define CHECKSUM 2

void clearImage(IplImage *image) 
{
  for(int i=0;i<image->height;i++) {
    for(int j=0;j<image->width*3;j++) {
      ((uchar*)(image->imageData + image->widthStep*i))[j] = (uchar)255;
    }
  }
}

void renderTag(IplImage* image, unsigned long value, int width, int x, int y, int hangle, int vangle, int rotation) 
{
  CvPoint2D32f p;
  p.x=x;
  p.y=y;

  CvSize2D32f s;
  s.width=width;
  s.height=width;


  CvBox2D box;
  box.center=p;
  box.size=s;
  box.angle=hangle;
  
  TripOriginalTag<RINGS,SECTORS,SYNC_SECTORS,CHECKSUM> t(box,value);
  t.Synthesize(image,CV_RGB(255,255,255),0);    
}

int
main(int argc, char* argv[]) {
  CvSize sizes;
  sizes.width=600;
  sizes.height=600;

  IplImage *image = cvCreateImage(sizes, IPL_DEPTH_8U,3);

  for(int code=0; code<(int)pow(3,13);code++) {
    for(int size=30;size>0;size--) 
      {
	clearImage(image);
	renderTag(image,code,size,300,300,0,0,0);
	std::vector<Tag*> result;
	TripOriginalTag<RINGS,SECTORS,SYNC_SECTORS,CHECKSUM>::Process(image,&result);
	int breaking = 0;
	if (result.size() == 0 || 
	    result[0]->GetCode() != code) {

	  std::cout << code << " " << size << std::endl;
	  breaking =1;
	}
	
	for(std::vector<Tag*>::const_iterator i = result.begin(); i!=result.end();i++) {
	  delete(*i);
	}
	if (breaking) { break;}
      }
  }
}

