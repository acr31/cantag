#include <iostream>
#include <vector>

#include "GrayScaleFileImageSource.hh"
#include "adaptivethreshold.hh"
#include "findellipses.hh"
#include "TripOuterTag.hh"
#include "TripOriginalCoder.hh"
#include "Drawing.hh"

int
main (int argc, char* argv[]) {

  GrayScaleFileImageSource i("output.jpg");
  Image* buf = i.GetBuffer();
  //  int count =0;
  //  while(count++<100) {
  i.Next();
  AdaptiveThreshold(buf);
  std::vector<Location2DChain*> ellipses;
  FindEllipses(buf,10,1000,4,4,0.01,0.001,&ellipses);

  TripOuterTag<TripOriginalCoder<2,16,2>,2,16,24,2> t;

  for(std::vector<Location2DChain*>::const_iterator i = ellipses.begin();i!=ellipses.end();i++) {
    if ((*i)->next != NULL) {
      std::cout << t.Decode(buf, (*i)->current ) << std::endl;
    }
    //  }
  }
}
