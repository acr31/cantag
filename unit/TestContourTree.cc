/**
 * $Header$
 */

#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <tripover/ContourTree.hh>
#include <tripover/Image.hh>

#define IMAGE_WIDTH 1000
#define IMAGE_HEIGHT 600

using boost::unit_test_framework::test_suite;
 
boost::mt19937 rng;
boost::uniform_int<> widthsource(0,IMAGE_WIDTH);
boost::uniform_int<> heightsource(0,IMAGE_HEIGHT);
boost::uniform_int<> childrensource(0,10);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > widthnumbers(rng, widthsource);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > heightnumbers(rng, heightsource);
boost::variate_generator<boost::mt19937, boost::uniform_int<> > children(rng, childrensource);


void check_square(std::vector<float>& points,int xpos,int ypos, int xedge,int yedge, bool hole) {
  int* expected = new int[4*xedge+4*yedge];
  int ptr = 0;
  if (hole) {
    expected[ptr++] = 0;
    expected[ptr++] = 1;
    for(int i=1;i<xedge;++i) {
      expected[ptr++] = i;
      expected[ptr++] = 0;
    }
    for(int i=1;i<yedge;++i) {
      expected[ptr++] = xedge;
      expected[ptr++] = i;
    }
    for(int i=1;i<xedge;++i) {
      expected[ptr++] = xedge-i;
      expected[ptr++] = yedge;
    }
    for(int i=1;i<yedge-2;++i) {
      expected[ptr++] = 0;
      expected[ptr++] = yedge-i;
    }

  }
  else {
    for(int i=0;i<yedge;++i) {
      expected[ptr++] = 0;
      expected[ptr++] = i;
    }
    for(int i=0;i<xedge;++i) {
      expected[ptr++] = i;
      expected[ptr++] = yedge;
    }
    for(int i=0;i<yedge;++i) {
      expected[ptr++] = xedge;
      expected[ptr++] = yedge-i;
    }
    for(int i=0;i<xedge;++i) {
      expected[ptr++] = xedge-i;
      expected[ptr++] = 0;
    }
  }
  ptr = 0;
  int isx = 1;
  for(std::vector<float>::const_iterator j = points.begin();j!=points.end();++j) {
    if (expected[ptr++]+(isx?xpos:ypos) != *j) {	
      throw "Failed to follow expected contour";
    }
    isx^=1;
  }
}
 


void multi_square(int size) {
  int edge = size-2;

  Image image(IMAGE_WIDTH,IMAGE_HEIGHT);
  for(int i=2;i<IMAGE_WIDTH-size;i+=size) {
    for(int j=2;j<IMAGE_HEIGHT-size;j+=size) {
      image.DrawFilledQuadTangle(i,j,
				 i+edge,j,
				 i+edge,j+edge,
				 i,j+edge,
				 COLOUR_BLACK);
      image.DrawFilledQuadTangle(i+1,j+1,
             			 i+edge/2-1,j+1,
				 i+edge/2-1,j+edge-1,
				 i+1,j+edge-1,
				 COLOUR_WHITE);				 
      image.DrawFilledQuadTangle(i+edge/2+5,j+5,
      				 i+edge-1,j+5,
				 i+edge-1,j+edge-1,
				 i+edge/2+5,j+edge-1,
				 COLOUR_WHITE);

      image.DrawFilledQuadTangle(i+5,j+5,
				 i+8,j+5,
				 i+8,j+8,
				 i+5,j+8,
				 COLOUR_BLACK);
    }
  }
  image.Save("temp.bmp");
  image.GlobalThreshold(128);
  std::vector<ContourTree::ContourConstraint> v;
  ContourTree c(image,v);
  
 

  ContourTree::Contour* root = c.GetRootContour();
  int xpos = 2;
  int ypos = 2;
  for(std::vector<ContourTree::Contour*>::const_iterator i = root->children.begin(); i!=root->children.end();++i) {    
    ContourTree::Contour* current = *i;
    check_square(current->points,xpos,ypos,edge,edge,false);
    if (current->children.size() != 2) {
      throw "Wrong number of child contours";
    }
    check_square(current->children[0]->points,xpos,ypos,edge/2,edge,true);
    check_square(current->children[1]->points,xpos+edge/2+4,ypos+4,edge/2-4,edge-4,true);
    if (current->children[0]->children.size() != 1) {
      throw "Wrong number of child contours";
    }
    check_square(current->children[0]->children[0]->points,xpos+5,ypos+5,3,3,false);
    xpos+=size;
    if (xpos >= IMAGE_WIDTH -size) {
      ypos+=size;
      xpos=2;
    }
  }

}

void test_multi_square() {
  multi_square(10);
}

void test_circle() {
  
  Image i(IMAGE_WIDTH,IMAGE_HEIGHT);
  //x  draw_circle(i);


}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
  test_suite* test= BOOST_TEST_SUITE( "ContourTree test" );

  test->add( BOOST_TEST_CASE( &test_multi_square ) );

  return test;
}

