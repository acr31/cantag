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
#include <cmath>
#include <Cantag.hh>
#include "TagDef.hh"

template<class Tag>
void RunTest(Tag& tag,const Cantag::Camera& camera, int size, double tan_fov, const char* prefix) {

  const int numsteps = 2;
    const int pixel_min = 5;
    const int pixel_step = 10;
    const int normal_numsteps = 50;
    for(int pixels=50;pixels>=pixel_min;pixels-=pixel_step) {
      double distance = (double)size/2.f/(double)pixels;
    
      // now work out the range of x and y to try for this distance
      double range = tan_fov * distance - 1.0;
      double step = range / (double)numsteps;
      double normal_step = 1.f/normal_numsteps;
      for(int xc=-numsteps;xc<=numsteps;++xc) {
	float x0 = xc * step;
	for(int yc=-numsteps;yc<=numsteps;++yc) {
	  float y0 = yc * step;
	  for(int nxc = -normal_numsteps; nxc <= normal_numsteps; ++nxc) {
	    float nx = nxc * normal_step;
	    for(int nxy = -normal_numsteps; nxy <= normal_numsteps; ++nxy) {
	      float ny = nxy * normal_step;
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

	      if (angle >= M_PI/2) {
		std::cout <<  prefix << " " << x0 << " " << y0 << " " << distance << " " << pixels << " " << xc << " " << yc << " " << " SKIP " << normal[0] << " " << normal[1] << " " << normal[2] << " " << nx << " " << ny << " " << nz << std::endl;
		continue;
	      }

	      Cantag::Minima m;
	      Cantag::SimulateMinDistance(m,tag,camera)(te);
	      std::cout << prefix << " " << x0 << " " << y0 << " " << distance << " " << pixels << " " << xc << " " << yc << " " << m.GetMinima() << " " << normal[0] << " " << normal[1] << " " << normal[2] << " " << nx << " " << ny << " " << nz << std::endl;
	    }
	  }
	}
      }
    }
}


template<class TagList>
struct Executor {
  static void Execute(const Cantag::Camera& camera,int size,double tan_fov) {
    typename TagList::Head t;
    RunTest<typename TagList::Head>(t,camera,size,tan_fov, typeid(typename TagList::Head).name());
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

template<int EDGE>
struct SimpleSquare : public TestSquare<EDGE,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleProjective> {};

template<int RINGS,int SECTORS>
struct SimpleCircle : public CircleInner<RINGS,SECTORS,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};

using namespace Cantag;
typedef TypeList<SimpleSquare<3>,
	TypeList<SimpleSquare<4>,
	TypeList<SimpleSquare<5>,
	TypeList<SimpleSquare<6>,
	TypeList<SimpleSquare<7>,
	TypeList<SimpleSquare<8>,
	TypeList<SimpleSquare<9>,
	TypeList<SimpleSquare<10>,
	TypeList<SimpleSquare<11>,
	TypeList<SimpleSquare<12>,
	TypeList<SimpleSquare<13>,
	TypeList<SimpleSquare<14>,
	TypeList<SimpleSquare<15>,
	TypeList<SimpleCircle<2,4>,
	TypeList<SimpleCircle<2,8>,
	TypeList<SimpleCircle<2,12>,
	TypeList<SimpleCircle<2,18>,
	TypeList<SimpleCircle<2,24>,
	TypeList<SimpleCircle<2,32>,
	TypeList<SimpleCircle<2,40>,
	TypeList<SimpleCircle<2,50>,
	TypeList<SimpleCircle<2,60>,
	TypeList<SimpleCircle<2,72>,
	TypeList<SimpleCircle<2,84>,
	TypeList<SimpleCircle<2,98>,
	TypeList<SimpleCircle<2,112>,
	TypeList<SimpleCircle<3,3>,
	TypeList<SimpleCircle<3,5>,
	TypeList<SimpleCircle<3,8>,
	TypeList<SimpleCircle<3,12>,
	TypeList<SimpleCircle<3,16>,
	TypeList<SimpleCircle<3,21>,
	TypeList<SimpleCircle<3,27>,
	TypeList<SimpleCircle<3,33>,
	TypeList<SimpleCircle<3,40>,
	TypeList<SimpleCircle<3,48>,
	TypeList<SimpleCircle<3,56>,
	TypeList<SimpleCircle<3,65>,
	TypeList<SimpleCircle<3,75>,
	TypeList<SimpleCircle<4, 4 >,
	TypeList<SimpleCircle<4, 6 >,
	TypeList<SimpleCircle<4, 9 >,
	TypeList<SimpleCircle<4, 12 >,
	TypeList<SimpleCircle<4, 16 >,
	TypeList<SimpleCircle<4, 20 >,
	TypeList<SimpleCircle<4, 25 >,
	TypeList<SimpleCircle<4, 30 >,
	TypeList<SimpleCircle<4, 36 >,
	TypeList<SimpleCircle<4, 42 >,
	TypeList<SimpleCircle<4, 49 >,
	TypeList<SimpleCircle<4, 56 >,
		 TypeListEOL> > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > >  MinDistanceTags;
	  

int main(int argc,char** argv) {

    const float fov = 70.f;
    const int size = 600;

    float invd = tan(fov/2/180*M_PI)*2;
    Cantag::Camera cam;
    cam.SetIntrinsic(size/invd,size/invd,size/2,size/2,0);

    Executor<TL2(CircleInnerLSFull36,SquarePolygonProj36)>::Execute(cam,size,tan(fov/2.f/180.f*M_PI));
    //Executor<Cantag::Append<BasicCircleTags,BasicSquareTags>::value>::Execute(cam,size,tan(fov/2.f/180.f*M_PI));
    //    Executor<MinDistanceTags>::Execute(cam,size,tan(fov/2.f/180.f*M_PI));
    /*
    float step=0.1;
    for(float i=0.4;i<1.0; i+=step) {
      typedef FreeCircle<2,32> Tag;
      char buff[255];
      sprintf(buff,"FreeCircle%f",i);
      Tag t(0.2,0.4,i,1.0);
      RunTest<Tag>(t,cam,size,tan(fov/2.f/180.f*M_PI),buff);
    }
    */
}

