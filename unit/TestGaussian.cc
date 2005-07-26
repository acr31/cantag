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

#include <total/gaussianelimination.hh>

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <cmath>
#include <iostream>
#define MAX_ERROR 0.001
#define SIZE 10

using boost::unit_test_framework::test_suite;
using namespace Total;
 
boost::mt19937 rng;
boost::uniform_real<> numbersource(-1024,1024);
boost::variate_generator<boost::mt19937, boost::uniform_real<> > numbers(rng, numbersource);

void test_solve_simultaneous() {

  for(int count=0;count<100;count++) {
    // this finds the solution to Ax = y  for a given A and y
    // we generate A and x randomly and then multiply out to find y  
    double* A[SIZE];
    double x[SIZE];
    for(int i=0;i<SIZE;i++) {
      A[i] = new double[SIZE];
      x[i] = numbers();
      for(int j=0;j<SIZE;j++) {
	A[i][j] = numbers();
      }
    }
    
    double y[SIZE];
    for(int i=0;i<SIZE;i++) {
      y[i] = 0;
      for(int j=0;j<SIZE;j++) {
	y[i] += A[i][j]*x[j];
      }
    }
    
    double calc_x[SIZE];
    
    solve_simultaneous(y,A,calc_x,SIZE);
        
    for(int i=0;i<SIZE;i++) {
      if (fabs(calc_x[i] - x[i]) > MAX_ERROR) {
	throw "Difference between calc_x and x exceeded threshold.";
      }
    }

    for(int i=0;i<SIZE;i++) {
      delete[] A[i];
    }
  }
}


void test_invert() {

  for(int count=0;count<1;count++) {
    double* m1[SIZE];
    double* m2[SIZE];
    for(int i=0;i<SIZE;i++) {
      m1[i] = new double[SIZE];
      m2[i] = new double[SIZE];
      for(int j=0;j<SIZE;j++) {
	m1[i][j] = numbers();
	m2[i][j] = m1[i][j];
      }
    }
    
    double* ans[SIZE];
    for(int i=0;i<SIZE;i++) {
      ans[i] = new double[SIZE];
    }
  
    invert_matrix(m1,ans,SIZE);
    
    double product[SIZE][SIZE];
    for(int i=0;i<SIZE;i++) {
      for(int j=0;j<SIZE;j++) {
	product[i][j] = 0;
	for(int k=0;k<SIZE;k++) {
	  product[i][j] += ans[i][k] * m2[k][j];
	}
      }
    }
    
    for(int i=0;i<SIZE;i++) {
      for(int j=0;j<SIZE;j++) {
	if (i==j) {
	  if (fabs(product[i][j] - 1) > MAX_ERROR) {
	    throw "Difference between leading diagonal element and 1 exceeded threshold.";
	  }       
	}
	else {
	  if (fabs(product[i][j]) > MAX_ERROR) {
	    throw "Difference between non-leading diagonal element and 0 exceeded threshold.";	  
	  }
	}
      }
    }  

    for(int i=0;i<SIZE;i++) {
      delete[] m1[i];
      delete[] m2[i];
    }
  }
}

void test_predivide() {
  for(int count =0;count<100;count++) {
    // generate two matrices
    // multiply one by the other
    // predivide the product
    // check that the result equals the second original matrix
    
    double* m1[SIZE];
    double* m2[SIZE];
    for(int i=0;i<SIZE;i++) {
      m1[i] = new double[SIZE];
      m2[i] = new double[SIZE];
      for(int j=0;j<SIZE;j++) {
	m1[i][j] = numbers();
	m2[i][j] = numbers();
      }
    }
    
    double* product[SIZE];
    for(int i=0;i<SIZE;i++) {
      product[i] = new double[SIZE];
      for(int j=0;j<SIZE;j++) {
	product[i][j] = 0;
	for(int k=0;k<SIZE;k++) {
	  product[i][j] += m1[i][k] * m2[k][j];
	}
      }
    }
    
    predivide(m1,product,SIZE,SIZE);
    
    for(int i=0;i<SIZE;i++) {
      for(int j=0;j<SIZE;j++) {
	if (fabs(product[i][j] - m2[i][j]) > MAX_ERROR) {
	  throw "Difference between answer and original matrix exceeds bounds.";
	}       
      }
    } 

    for(int i=0;i<SIZE;i++) {
      delete[] m1[i];
      delete[] m2[i];
      delete[] product[i];
    }
  }
  
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "Gaussian test" );

  test->add( BOOST_TEST_CASE( &test_solve_simultaneous ) );
  test->add( BOOST_TEST_CASE( &test_invert ) );
  test->add( BOOST_TEST_CASE( &test_predivide ) );

  return test;
}

