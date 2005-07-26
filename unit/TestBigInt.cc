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

#include <cantag/BigInt.hh>

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>

using boost::unit_test_framework::test_suite;
using namespace Cantag;
 
boost::mt19937 rng;
boost::uniform_int<> numbersource(0,1<<15);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > numbers(rng, numbersource);

void test_lessthan() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    BigInt<32> bi(i);
    BigInt<32> bj(j);

    if ((i<j) != (bi<bj)) {
      throw "Failed less than test!";
    }    
  }
};

void test_equals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = i;
    BigInt<32> bi(i);
    BigInt<32> bj(j);

    if (!(bi==bj)) {
      throw "Failed equals test!";
    }    
  }
};

void test_plusequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi+=bj;

    if ( (unsigned int)bi != i+j) {
      throw "Failed plueequals test!";
    }    
  }
};

void test_minusequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    if (i<j) {
      int k = i;
      i = j;
      j = k;
    }
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi-=bj;
    if ( (unsigned int)bi != i-j) {
      throw "Failed minusequals test!";
    }    
  }
};

void test_timesequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi*=bj;

    if ( (unsigned int)bi != i*j) {
      throw "Failed timesequals test!";
    }    
  }
};

void test_divideequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi/=bj;

    if ( (unsigned int)bi != i/j) {
      throw "Failed divideequals test!";
    }    
  }
};

void test_modequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    if (i<j) {
      int k = i;
      i=j;
      j=k;
    }
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi%=bj;
    if ( (unsigned int)bi != i%j) {
      throw "Failed modequals test!";
    }    
  }
};

void test_orequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi|=bj;
    if ( (unsigned int)bi != (i|j)) {
      throw "Failed orequals test!";
    }    
  }
};

void test_andequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi&=bj;
    if ( (unsigned int)bi != (i&j)) {
      throw "Failed andequals test!";
    }    
  }
};

void test_xorequals() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    int j = numbers();
    BigInt<32> bi(i);
    BigInt<32> bj(j);
    bi^=bj;
    if ( (unsigned int)bi != i^j) {
      throw "Failed xorequals test!";
    }    
 }
};

void test_plusplus() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    BigInt<32> bi(i);
    bi+=1;
    if ( (unsigned int)bi != i+1) {
      throw "Failed plusplus test!";
    }    
  }  
};

void test_minusminus() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    BigInt<32> bi(i);
    bi-=1;
    if ( (unsigned int)bi != i-1) {
      throw "Failed minusminus test!";
    }    
  }  
};

void test_Pwr() {
  for(int i=0;i<100;i++) {
    int i = numbers();
    BigInt<32> bi(i);
    bi.Pwr(2);
    if ( (unsigned int)bi != i*i) {
      throw "Failed pwr test!";
    }    
  }  
}


test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "BigInt test" );

  test->add( BOOST_TEST_CASE( &test_lessthan ) );
  test->add( BOOST_TEST_CASE( &test_equals ) );
  test->add( BOOST_TEST_CASE( &test_plusequals ) );
  test->add( BOOST_TEST_CASE( &test_minusequals ) );
  test->add( BOOST_TEST_CASE( &test_timesequals ) );
  test->add( BOOST_TEST_CASE( &test_divideequals ) );
  test->add( BOOST_TEST_CASE( &test_modequals ) );
  test->add( BOOST_TEST_CASE( &test_orequals ) );
  test->add( BOOST_TEST_CASE( &test_andequals ) );
  test->add( BOOST_TEST_CASE( &test_plusplus ) );
  test->add( BOOST_TEST_CASE( &test_minusminus ) );
  test->add( BOOST_TEST_CASE( &test_Pwr ) );

  return test;
}

