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

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <iostream>

#include <cantag/Ellipse.hh>
#include <cantag/Camera.hh>
#include <cantag/algorithms/FitEllipseSimple.hh>
#include <cantag/algorithms/FitEllipseLS.hh>

using boost::unit_test_framework::test_suite;
using namespace Cantag;

void test_ellipse(float x0, float y0, float width, float height, float angle) {
  const Ellipse e(x0,y0,width,height,angle);
  std::vector<float> points;
  e.Draw(points);

  float maxdev = -1e10;
  for(std::vector<float>::const_iterator i = points.begin(); i!= points.end();++i) {
    double x = *(i++);
    double y = *i;
    float output = x*x*e.GetA() + x*y*e.GetB() + y*y*e.GetC() + x*e.GetD() + y*e.GetE() + e.GetF();
    if (fabs(output) > maxdev) {
      maxdev = fabs(output);
    }
    if (fabs(output) > 1e-2) {
      throw "Ellipse algebraic equation outside tolerance";
    }
  }
}

void test_ellipse2(float x0, float y0, float width, float height, float angle) {
  const Ellipse e(x0,y0,width,height,angle);
  std::vector<float> points;
  e.Draw(points);
  
  const Ellipse e2(e.GetA(),e.GetB(),e.GetC(),e.GetD(),e.GetE(),e.GetF());
  std::vector<float> points2;
  e.Draw(points2);

  for(std::vector<float>::const_iterator i = points.begin(); i!= points.end();++i) {
    float origx = *(i++);
    float origy = *i;
    float mindist = 1e10;
    for(std::vector<float>::const_iterator j = points2.begin(); j != points2.end(); ++j) {
      float foundx = *(j++);
      float foundy = *j;
      float distance = sqrt( (origx-foundx)*(origx-foundx) + (origy-foundy)*(origy-foundy) );
      if (distance < mindist) mindist = distance;
    }
    if (mindist > 1e-10) throw "Contours are too far apart";
  }  
}

bool match_sign(int a, int b) {
  if (a < 0 && b < 0) return true;
  if (a == 0 && b == 0) return true;
  if (a > 0 && b > 0) return true;
  return false;
}

void test_ellipse3(float x0, float y0, float width, float height, float angle) {
  const Ellipse el(x0,y0,width,height,angle);
  std::vector<float> points;
  el.Draw(points);
  ContourEntity e;
  std::vector<float>::const_iterator i = points.begin();
  int currentx = Round(*(i++));
  int currenty = Round(*(i++));
  e.SetStart(currentx,currenty);
  while(i!=points.end()) {
    int nextx = Round(*(i++));
    int nexty = Round(*(i++));

    int offset_x[] = {-1,-1,0,1,1,1,0,-1};
    int offset_y[] = {0,1,1,1,0,-1,-1,-1};
    
    int diffx = nextx - currentx;
    int diffy = nexty - currenty;
    while(abs(diffx)>0||abs(diffy)>0) {
      int fc = 0;
      while( !match_sign(offset_x[fc],diffx) || !match_sign(offset_y[fc],diffy) ) { ++fc; assert(fc<8); }
      e.AddPoint(fc);
      diffx -= offset_x[fc];
      diffy -= offset_y[fc];
    }
    currentx = nextx;
    currenty = nexty;
  }

  ShapeEntity<Ellipse> fitted;
  bool result = FitEllipseLS()(e,fitted);
  std::vector<float>& points2 = e.GetPoints();  

  if (!result) {
    for(std::vector<float>::const_iterator i = points2.begin();i!=points2.end();++i) {
	std::cout << "con " << *(i++) << " " << *i << std::endl;
      }
      for(std::vector<float>::const_iterator i = points.begin();i!=points.end();++i) {
	std::cout << "ell " << *(i++) << " " << *i << std::endl;
      }
      throw "Failed to fit ellipse";
  }

  
  for(std::vector<float>::const_iterator i = points.begin(); i!= points.end();++i) {
    float origx = *(i++);
    float origy = *i;
    float mindist = 1e10;
    for(std::vector<float>::const_iterator j = points2.begin(); j != points2.end(); ++j) {
      float foundx = *(j++);
      float foundy = *j;
      float distance = sqrt( (origx-foundx)*(origx-foundx) + (origy-foundy)*(origy-foundy) );
      if (distance < mindist) mindist = distance;
    }
    if (mindist > 1) {
      std::vector<float> points3;
      fitted.GetShape()->Draw(points3);
      
      for(std::vector<float>::const_iterator i = points2.begin();i!=points2.end();++i) {
	std::cout << "con " << *(i++) << " " << *i << std::endl;
      }
      for(std::vector<float>::const_iterator i = points.begin();i!=points.end();++i) {
	std::cout << "ell " << *(i++) << " " << *i << std::endl;
      }
      for(std::vector<float>::const_iterator i = points3.begin();i!=points3.end();++i) {
	std::cout << "fit " << *(i++) << " " << *i << std::endl;
      }
      std::cerr << mindist << std::endl;
      throw "Contours are too far apart";
    }
  }  
}

void test_ellipse_draw_points() {
  //  test_ellipse3(-500,0,10,5,0);
  //  throw "done";
  int range = 500;
  int step = 100;
  float angle_step = 0.1;
  for(int x=-range;x<range;x+=step) {
    for(int y=-range;y<range;y+=step) {
      for(float angle=0.f;angle<M_PI/2;angle+=angle_step) {
	for(int width=10;width<1000;width+=100) {
	  for(int height=10;height<=width;height+=100) {
	    std::cout << x << " " << y << " " << width << " " << height << " " << angle << std::endl;
	    test_ellipse(x,y,width,height,angle);
	    test_ellipse2(x,y,width,height,angle);
	    test_ellipse3(x,y,width,height,angle);
	  }
	}
      }
    }
  }
};




test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "Ellipse Test" );

  test->add( BOOST_TEST_CASE( &test_ellipse_draw_points ) );

  return test;
}

