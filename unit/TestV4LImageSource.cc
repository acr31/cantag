/**
 * $Header$
 */

#include <boost/test/unit_test.hpp>
#include <tripover/V4LImageSource.hh>
#include <ctime>
#include <iostream>

using boost::unit_test_framework::test_suite;

void test_rate() {
  V4LImageSource s("/dev/video0",0);
  time_t cur_time = time(NULL);
  int count = 100;
  for(int i=0;i<count;i++) {
    Image* buffer = s.Next();
  }
  time_t elapsed = time(NULL)-cur_time;
  std::cout << "Took " << elapsed << " seconds to grab " << count << " frames" << std::endl;
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "V4LImageSource test" );

  test->add( BOOST_TEST_CASE( &test_rate ) );

  return test;
}
