/*
  Copyright (C) 2004 Andrew C. Rice

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  Email: acr31@cam.ac.uk
*/

/**
 * $Header$
 */

#include <cantag/Socket.hh>
#include <cantag/Image.hh>

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <iostream>
#define PORT 56789

using boost::unit_test_framework::test_suite;
using namespace Cantag;
 
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

