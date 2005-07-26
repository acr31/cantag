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

#include <cantag/V4LImageSource.hh>

#include <boost/test/unit_test.hpp>
#include <ctime>
#include <iostream>

using boost::unit_test_framework::test_suite;
using namespace Cantag;

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
