/**
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
 *  Simulate the minimum distance for square, and circle tags for various positions
 */
#include <sstream>
#include <Cantag.hh>
#include "TagDef.hh"

template<class Tag>
void RunTest(Tag& tag,const Cantag::Camera& camera, int size, double tan_fov, const char* prefix) {

  const int numsteps = 1;
  const int pixel_min = 10;
  const int pixel_step = 10;
  const int normal_numsteps = 10;
  for(int pixels=100;pixels>=pixel_min;pixels-=pixel_step) {
    //int pixels=100; {
    double distance = (double)size/(double)pixels*2.f;
    
      // now work out the range of x and y to try for this distance
      double range = tan_fov * distance - 1.0;
      double step = range / (double)numsteps;
      double normal_step = 1.f/normal_numsteps;
      //int xc=0; {
      for(int xc=-numsteps;xc<=numsteps;++xc) {
	float x0 = xc * step;
	//int yc=0; {
	for(int yc=-numsteps;yc<=numsteps;++yc) {
	  float y0 = yc * step;
	  //int nxc = 0; {
	  for(int nxc = -normal_numsteps; nxc <= normal_numsteps; ++nxc) {
	      // treat nxc as an angle and then nx = cos(nxc)
	      // NOT THIS float nx = nxc * normal_step;
	      float nxangle = nxc / normal_numsteps * FLT_PI / 2.f;
	      float nxdegrees = nxc / normal_numsteps * 90;
	      float nx = cos(nxangle);
	    //int nxy = 0; {
	    for(int nxy = -normal_numsteps; nxy <= normal_numsteps; ++nxy) {
	      float nyangle = nxy / normal_numsteps * FLT_PI / 2.f;
	      float nydegrees = nxy / normal_numsteps * 90;
	      //float ny = nxy * normal_step;
	      float ny = cos(nyangle);
	      float len = nx*nx+ny*ny;
	      if (len > 1.f) continue;
	      float nz = sqrt(1.f-nx*nx-ny*ny);
	      if (isnan(nz)) nz = 0.f;

	      Cantag::TransformEntity te;
	      te.GetTransforms().push_back(new Cantag::Transform(x0,y0,distance,nx,ny,nz,"normal vector"));
	      float normal[3];
	      te.GetPreferredTransform()->GetNormalVector(camera,normal);
	      float location[3];
	      te.GetPreferredTransform()->GetLocation(location,1.f);

	      float mod = sqrt(location[0]*location[0] + location[1]*location[1] + location[2]*location[2]);
	      
	      float angle = acos((normal[0]*location[0] + normal[1]*location[1] + normal[2]*location[2])/mod);
	      std::cout <<  prefix <<  " " << x0 << " " << y0 << " " << distance << " " << pixels << " " << xc << " " << yc << " " << nxdegrees << " " << nydegrees << " " << angle;
	      if (angle >= M_PI/2) {
		std::cout <<  " SKIP" << std::endl;
		continue;
	      }

	      std::vector<float> m;
	      Cantag::SimulateMinDistance(m,tag,camera)(te);
	      float min = 1e10;
	      for(std::vector<float>::const_iterator i = m.begin();i!=m.end();++i) {
		float v = *i;
		if (v < min) min = v;
	      }
	      std::cout << " " << min;
	      for(std::vector<float>::const_iterator i = m.begin();i!=m.end();++i) {
		std::cout << " " << *i;
	      }
	      std::cout << std::endl;
	    }
	  }
	}
      }
    }
}

template<class TagList>
struct Executor {
  static void Execute(const Cantag::Camera& camera,int size,double tan_fov) {
    try {
      typename TagList::Head t;
      RunTest<typename TagList::Head>(t,camera,size,tan_fov, t.Prefix().c_str());
    }
    catch (const char* message) {
      std::cerr << message << std::endl;
    }
    Executor<typename TagList::Tail>::Execute(camera,size,tan_fov);
  }
};

template<> struct Executor<Cantag::TypeListEOL> {
  static void Execute(const Cantag::Camera& camera,int size, double tan_fov) {};
};

template<int RINGS,int SECTORS>
struct FreeCircle : public Cantag::TagCircle<RINGS,SECTORS>, public Cantag::RawCoder<RINGS*SECTORS,RINGS> {
  FreeCircle(float i,float j,float k,float l) : Cantag::TagCircle<RINGS,SECTORS>(i,j,k,l) {}
};

template<int CURRENT_EDGE>
struct SquareList {
  typedef Cantag::TypeList<TestSquare<CURRENT_EDGE,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleProjective>, typename SquareList<CURRENT_EDGE-1>::List> List;
};

template<>
struct SquareList<2> {
  typedef Cantag::TypeListEOL List;
};


template<int RINGS, int SECTORS = 100> struct CircleInnerList {
  typedef Cantag::TypeList<CircleInner<RINGS,SECTORS,Cantag::FitEllipseLS,Cantag::TransformEllipseFull>, typename CircleInnerList<RINGS,SECTORS-10>::List> List;
};

template<int RINGS> struct CircleInnerList<RINGS,10> {
  typedef typename CircleInnerList<RINGS-1,500/(RINGS-1) - (500/(RINGS-1) % 10)>::List List;
};

template<int SECTORS> struct CircleInnerList<1,SECTORS> {
  typedef Cantag::TypeListEOL List;
};


template<int RINGS, int SECTORS = 100> struct CircleFixedList {
  typedef Cantag::TypeList<CircleInnerFixed<RINGS,SECTORS,Cantag::FitEllipseLS,Cantag::TransformEllipseFull>, typename CircleFixedList<RINGS,SECTORS-10>::List> List;
};

template<int RINGS> struct CircleFixedList<RINGS,10> {
  typedef typename CircleFixedList<RINGS-1,500/(RINGS-1) - (500/(RINGS-1) % 10)>::List List;
};

template<int SECTORS> struct CircleFixedList<1,SECTORS> {
  typedef Cantag::TypeListEOL List;
};


template<int BIT_COUNT, int RINGS> struct Iterator {
    template<int COND, int SECTORS> struct Alternation {
	typedef typename Iterator<BIT_COUNT,RINGS-1>::List List;
    };
    template<int SECTORS> struct Alternation<0,SECTORS> {
	typedef Cantag::TypeList<CircleInner<RINGS,SECTORS,Cantag::FitEllipseLS, Cantag::TransformEllipseFull>, typename Iterator<BIT_COUNT,RINGS-1>::List> List;
    };
    typedef typename Alternation<BIT_COUNT % RINGS,BIT_COUNT/RINGS>::List List;
};

template<int BIT_COUNT> struct Iterator<BIT_COUNT,1> {
    typedef Cantag::TypeListEOL List;
};

template<int MAX_BITS> struct SearchList {
  typedef typename Cantag::Append<typename Iterator<MAX_BITS,5>::List, typename SearchList<MAX_BITS-1>::List>::value List;
};

template<> struct SearchList<10> {
  typedef Cantag::TypeListEOL List;
};

using namespace Cantag;

//typedef SquareList<15>::List MinDistanceTags;
//typedef CircleInnerList<5>::List MinDistanceTags;
//typedef CircleFixedList<5>::List MinDistanceTags;
//typedef Iterator<400,10>::List MinDistanceTags;
typedef TL2(CircleInnerLSFull36,SquarePolygonProj36) MinDistanceTags;

int main(int argc,char** argv) {
  try {
    const float fov = 70.f;
    const int size = 600;
    
    float invd = tan(fov/2/180*M_PI)*2;
    Cantag::Camera cam;
    cam.SetIntrinsic(size,size,size/2,size/2,0);

    //    Executor<TL2(CircleInnerLSFull36,SquarePolygonProj36)>::Execute(cam,size,tan(fov/2.f/180.f*M_PI));
    //Executor<Cantag::Append<BasicCircleTags,BasicSquareTags>::value>::Execute(cam,size,tan(fov/2.f/180.f*M_PI));
    Executor<MinDistanceTags>::Execute(cam,size,tan(fov/2.f/180.f*M_PI));
    /*float step =0.05;
    for(float i=0.f;i<1.f;i+=step) {
      for(float j=i+step;j<1.f;j+=step) {
	typedef FreeCircle<3,45> Tag;
	char buff[255];
	sprintf(buff,"Circle %f %f",i,j);
	Tag t(0.f,1.f,i,j);
	RunTest<Tag>(t,cam,size,tan(fov/2.f/180.f*M_PI),buff);	    
      }
      }*/
  }
  catch (const char* message) {
    std::cout << "Top Level: " << message << std::endl;
  }
}

