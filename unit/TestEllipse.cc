/**
 * $Header$
 */

#include <total/Ellipse.hh>

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <iostream>

using boost::unit_test_framework::test_suite;
using namespace Total;

boost::mt19937 rng;
boost::uniform_int<> numbersource(0,500);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > numbers(rng, numbersource);

void test_compose() {
  float maxx0 = 0;
  float maxy0 = 0;
  float maxwidth = 0;
  float maxheight = 0;
  for(int i=0;i<1e5;i++) {
    float x = numbers();
    float y = numbers();
    float width = numbers();
    float height = numbers();
    if (width < height) {
      float t = width;
      width = height;
      height = width;
    }
    float theta = (float)numbers()/500*PI;

    Ellipse e(x,y,width,height,theta);
    Ellipse e2(e.GetA(),e.GetB(),e.GetC(),e.GetD(),e.GetE(),e.GetF());
    //    std::cout << "x " << x << " " << e2.GetX0() << std::endl;
    //    std::cout << "y " << y << " " << e2.GetY0() << std::endl;
    //    std::cout << "w " <<width << " " << e2.GetWidth() << std::endl;
    //    std::cout << "h " << height << " " << e2.GetHeight() << std::endl;
    //    std::cout << "t " << theta << " " << e2.GetAngle() << std::endl;
    //    std::cout << "gen " << e.GetA()<< " " <<e.GetB()<< " " <<e.GetC()<< " " <<e.GetD()<< " " <<e.GetE()<< " " <<e.GetF() << std::endl;
    float xdiff = fabs(e2.GetX0() - x);
    float ydiff = fabs(e2.GetY0() - y);
    float wdiff = fabs(e2.GetWidth() - width);
    float hdiff = fabs(e2.GetHeight() - height);

    if (xdiff > maxx0) maxx0 = xdiff;
    if (ydiff > maxy0) maxy0 = ydiff;
    if (wdiff > maxwidth) maxwidth = wdiff;
    if (hdiff > maxheight) maxheight = hdiff;
  }

  std::cout << "Max X0 Error " << maxx0 << std::endl;
  std::cout << "Max Y0 Error " << maxy0 << std::endl;
  std::cout << "Max Width Error " << maxwidth << std::endl;
  std::cout << "Max Height Error " << maxheight << std::endl;
  
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "Ellipse Test" );

  test->add( BOOST_TEST_CASE( &test_compose ) );

  return test;
}

