/**
 * $Header$
 */

#include <total/Camera.hh>

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>

using boost::unit_test_framework::test_suite;
using namespace total;

float intrinsics[] = { 651.374035092475992, 646.891646093565100,385.839970984750892, 258.272641514105032, 0,
		       1420.751194666989250, 1419.169243508142245, 344.678301126735334, 322.299040811109990, 0 };

float radials[] = { -0.402258674499810,  0.197158385544757, 0,
		    -0.156543248596387, 1.056520889730414, 0};

float tangentials[] = {  0.197158385544757, 0.000758667447393,
			 0.004022154925579, -0.008055858791186};
			 
boost::mt19937 rng;
boost::uniform_int<> numbersource(0,500);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > numbers(rng, numbersource);

#define EPSILON 10.01

void test_distort() {
  for(int i=0;i<2;i++) {
    float points[500];
    float points2[500];
    for(int j=0;j<500;j++) {
      points[j] = numbers();
      points2[j] = points[j];
    }
    Camera c;
    c.SetIntrinsic(intrinsics[5*i],intrinsics[5*i+1],intrinsics[5*i+2],intrinsics[5*i+3],intrinsics[5*i+4]);
    c.SetRadial(radials[3*i],radials[3*i+1],radials[3*i+2]);
    c.SetTangential(tangentials[2*i],tangentials[2*i+1]);
    c.ImageToNPCF(points,250);
    c.NPCFToImage(points,250);
    for(int j=0;j<500;j++) {
      std::cout << points[j] << " " << points2[j] << std::endl;
      if (fabs(points[j]-points2[j]) > EPSILON) {
	throw "Distort/Undistort error exceeded bounds";
      }
    }
  }
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "Camera test" );

  test->add( BOOST_TEST_CASE( &test_distort ) );

  return test;
}

