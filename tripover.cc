#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <vector>

#include "TripOriginalTag.hh"

int 
main(int argc,char* argv[]) 
{

  IplImage *image = cvLoadImage(argv[1]);
  
  std::vector<Tag*> result;
  TripOriginalTag::Process(image,&result);

  for(std::vector<Tag*>::const_iterator i = result.begin();i!=result.end();i++) {
    std::cout << **i << std::endl;
  }

  cvReleaseImage(&image);
  
}
