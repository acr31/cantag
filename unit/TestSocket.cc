/**
 * $Header$
 */

#include <total/Socket.hh>
#include <total/Image.hh>

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <iostream>
#define PORT 56789

using boost::unit_test_framework::test_suite;
 
boost::mt19937 rng;
boost::uniform_int<> numbersource(0,255);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > numbers(rng, numbersource);

void test_Image() {
  // generate a random image
  Image i(640,480);
  
  // send it over the network to myself
  Socket receiver;
  receiver.Bind("127.0.0.1",PORT);
  receiver.Listen();
  Socket sender;
  sender.Connect("127.0.0.1",PORT);
  Socket* datasock = receiver.Accept();
  std::cout << "accepted" << std::endl;
  i.Save(sender);
  std::cout << "saved" << std::endl;
  Image i2(*datasock);
  delete datasock;
  // check for the same received
  
}


test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "Socket test" );

  test->add( BOOST_TEST_CASE( &test_Image ) );
  return test;
}

