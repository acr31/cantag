/**
 * $Header$
 */


#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <tripover/SceneGraph.hh>
#include <iostream>
#include <ctime>

using boost::unit_test_framework::test_suite;
 
boost::mt19937 rng;
boost::uniform_int<> numbersource(-1,1);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > numbers(rng, numbersource);

#define XLENGTH 100
#define YLENGTH 500
#define LAPS 4
#define START_X 100
#define START_Y 50
#define IMAGE_WIDTH 600
#define IMAGE_HEIGHT 600

void test_contour() {
  numbers();
  int* contour_points = new int[(XLENGTH+YLENGTH)*LAPS*2];
  float* points = new float[(XLENGTH+YLENGTH)*LAPS*2];
  for(int count=0;count<1000;count++) {
    // generate a contour 
    contour_points[0] = START_X;
    contour_points[1] = START_Y;
    Image image(IMAGE_WIDTH,IMAGE_HEIGHT);
    image.DrawPixel(START_X,START_Y,0);
    int pointer =2;    
    for(int j=0;j<LAPS;j++) {
      int step = (j%2==0 ? 1 : -1);
      for(int i=0;i<YLENGTH;i++) {
	int newx = contour_points[pointer-2]+numbers();
	int newy = contour_points[pointer-1]+step;
	contour_points[pointer++] = newx;
	contour_points[pointer++] = newy;
	image.DrawPixel(newx,newy,0);
      }

      for(int i=0;i<XLENGTH;i++) {
	int newx = contour_points[pointer-2]+1;
	int newy = contour_points[pointer-1]+0;//numbers();
	contour_points[pointer++] = newx;
	contour_points[pointer++] = newy;
	image.DrawPixel(newx,newy,0);
      }      
    }
    image.GlobalThreshold(128); // this will invert our image for the contour follower
    //    image.Save("test.bmp");
    

    ImageSegmentor s;
    
    s.FollowContour(image,START_X,START_Y,points,(XLENGTH+YLENGTH)*LAPS);

    //    Image t(IMAGE_WIDTH,IMAGE_HEIGHT);
    //    t.DrawPolygon(points,(XLENGTH+YLENGTH)*LAPS*2,0,1);
    //t.Save("done.bmp");
    
    int data_pointer =0;

    for(int i=0;i<(XLENGTH+YLENGTH)*LAPS*2;i++) {
      if (points[data_pointer] ==  contour_points[i]) {
	data_pointer++;
      }
    }

    if ((XLENGTH+YLENGTH)*LAPS*2 - data_pointer > 200) {
      std::cout << data_pointer <<std::endl;
      throw "Too many points missed on contour";
    }
  }
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "Segmentor test" );

  test->add( BOOST_TEST_CASE( &test_contour ) );


  return test;
}
