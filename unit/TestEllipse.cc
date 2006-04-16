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

#include <cantag/Ellipse.hh>

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <iostream>

using boost::unit_test_framework::test_suite;
using namespace Cantag;

boost::mt19937 rng;
boost::uniform_int<> numbersource(0,500);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > numbers(rng, numbersource);

void test_compose() {
  float maxx0 = 0;
  float maxy0 = 0;
  float maxwidth = 0;
  float maxheight = 0;
  float maxtheta = 0;
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
    float theta = (float)numbers()/500.f*FLT_PI;

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

    if (theta < 0.f) theta += 2*FLT_PI;
    while(theta >= FLT_PI) theta -= FLT_PI;
    float angle = e2.GetAngle();
    if (angle < 0.f) angle += 2*FLT_PI;
    while(angle >= FLT_PI) angle -= FLT_PI;

    float thetadiff = (fabs(width - height) <= FLT_EPSILON) ? 0.f : fabs(angle - theta);

    if (xdiff > maxx0) maxx0 = xdiff;
    if (ydiff > maxy0) maxy0 = ydiff;
    if (wdiff > maxwidth) maxwidth = wdiff;
    if (hdiff > maxheight) maxheight = hdiff;

    if (thetadiff > 0.1) {
      std::cout << x << " " << y << " " << width << " " << height << " " << theta << " (" << (theta/FLT_PI*180.f) << ")" << std::endl;
      std::cout << xdiff << " " << ydiff << " " << wdiff << " " << hdiff << " " << (thetadiff/FLT_PI*180) << std::endl;
      throw "angle error";
    }

    if (thetadiff > maxtheta) maxtheta = thetadiff;
  }

  std::cout << "Max X0 Error " << maxx0 << std::endl;
  std::cout << "Max Y0 Error " << maxy0 << std::endl;
  std::cout << "Max Width Error " << maxwidth << std::endl;
  std::cout << "Max Height Error " << maxheight << std::endl;
  std::cout << "Theta Error " << maxtheta << std::endl;
  
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "Ellipse Test" );

  test->add( BOOST_TEST_CASE( &test_compose ) );

  return test;
}

