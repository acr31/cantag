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

#include <cantag/polysolve.hh>
#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <iostream>

using boost::unit_test_framework::test_suite;
using namespace Cantag;
 
boost::mt19937 rng;
boost::uniform_real<> numbersource(0,10);
boost::variate_generator<boost::mt19937, boost::uniform_real<> > numbers(rng, numbersource);

double minmatrix[9];
double minevec[9];
double mineval[9];

int minsigfig=100;
int maxsigfig=0;
int testcounter = 0;
int totalsig = 0;

int compare(double a, double b) {
  while(fabs(a) > 10) { a/=10; b/=10; }
  while(fabs(a) < 1) { a*=10; b*=10; }

  int numsigfig=0;
  while(trunc(a) == trunc(b) && numsigfig<30) {
    numsigfig++;
    a*=10;
    b*=10;
  }
  return numsigfig;
}


/**
 * Check the eigenvectors and values
 * 1) eigenvector modulus should be 1
 * 2) maxix * evec[i] = eval[i] * evec[i]
 */
bool check(double matrix[9], double evec[9], double eval[9]) {
  for(int i=0;i<3;++i) {

    double mod=0;
    for(int j=0;j<3;++j) {
      mod += evec[i+j*3] * evec[i+j*3];
    }
    
    if (fabs(mod - 1) > 1e-10) { return false; }

    double product[3] = {0};
    for(int k=0;k<3;++k) {
      for(int j=0;j<3;++j) {
	product[k] += matrix[k*3+j] * evec[j*3+i];
      }
    }
    
    double product2[3] = {0};
    for(int k=0;k<3;++k) {
      product2[k] = evec[k*3+i] * eval[i*4];
    }
    
    for(int k=0;k<3;++k) {
      int sigfig = compare(product2[k],product[k]);
      if (sigfig > maxsigfig) { maxsigfig = sigfig; }
      if (sigfig < minsigfig) { 
	minsigfig = sigfig; 
	for(int i=0;i<9;++i) {
	  minmatrix[i] = matrix[i];
	  minevec[i] = evec[i];
	  mineval[i] = eval[i];
	}
      }
      totalsig+=sigfig;
      testcounter++;      
    }
  }
  return true;
}

void print(const char* label, double matrix[9]) {
  std::cout.precision(20);
  std::cout << label << " = [ ";
  for (int i=0;i<3;++i) {
    for (int j=0;j<3;++j) std::cout << matrix[i*3+j] << " ";
    std::cout << std::endl;
  }
  std::cout << "];" << std::endl;  
}

void test_check(double matrix[9], double evec[9], double eval[9]) {
  if (!check(matrix,evec,eval)) {
    std::cout.precision(20);
    std::cout << "Failed test!" << std::endl;
    print("test",matrix);
    print("evec",evec);
    print("eval",eval);
    throw "Check for vector consistancy failed!";
  }
}

void test_random_symmetric() {
  minsigfig=100;
  maxsigfig=0;
  testcounter = 0;
  totalsig = 0;

  for(int count=0;count<10000000;++count) {
    double test1[9];
    for(int i=0;i<9;++i) {
      test1[i] = numbers();
    }
    test1[3] = test1[1];
    test1[6] = test1[2];
    test1[7] = test1[5];
    
    double evec[9];
    double eval[9];
    
    eigensolve(test1[0],test1[1],test1[2],
	       /*     */test1[4],test1[5],
	       /*              */test1[8],
	       evec,eval);
    
    test_check(test1,evec,eval);
  }

  std::cout << "Min Sig Fig " << minsigfig << std::endl;
  std::cout << "Max Sig Fig " << maxsigfig << std::endl;
  std::cout << "Mean Sig Fig " << (totalsig/testcounter) << std::endl;
  print("minmatrix",minmatrix);
  print("minevec",minevec);
  print("mineval",mineval);
}

void test_random() {
  minsigfig=100;
  maxsigfig=0;
  testcounter = 0;
  totalsig = 0;

  int noroot = 0;
  for(int count=0;count<10000000;++count) {
    double test1[9];
    for(int i=0;i<9;++i) {
      test1[i] = numbers();
    }

    double evec[9];
    double eval[9];
    
    // if the matrix doesn't have enough eigenvectors or values then
    // this returns false
    if (eigensolve(test1[0],test1[1],test1[2],
		   test1[3],test1[4],test1[5],
		   test1[6],test1[7],test1[8],
		   evec,eval)) {
      test_check(test1,evec,eval);
    }
    else {
      noroot++;
    }
  }

  std::cout << "No Root " << noroot << std::endl;
  std::cout << "Min Sig Fig " << minsigfig << std::endl;
  std::cout << "Max Sig Fig " << maxsigfig << std::endl;
  std::cout << "Mean Sig Fig " << (totalsig/testcounter) << std::endl;
  print("minmatrix",minmatrix);
  print("minevec",minevec);
  print("mineval",mineval);
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "EigenVector routine test" );

  test->add( BOOST_TEST_CASE( &test_random_symmetric ) );
  test->add( BOOST_TEST_CASE( &test_random ) );

  return test;
}

