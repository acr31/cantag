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

#include <Cantag.hh>

struct Result {
  float angle_error;
  float distance_error;
  int bit_error;
  bool valid;

  Result() : valid(false) {}
  Result(float a, float d, int b) : angle_error(a), distance_error(d),bit_error(b),valid(true) {}
};

template<class TagType>
class RunTest {
private:
  TagType tag;
  Cantag::Camera camera;
  Cantag::GLImageSource fs;
  Cantag::CyclicBitSet<TagType::PayloadSize> stored_payload;
  int m_size;
  float m_fov;
  float m_tan_fov;
public:
  RunTest(int size, float fov, Cantag::DecodeEntity<TagType::PayloadSize>& d);
  
  Result Execute(float theta, float phi, float x, float y, float z);
  void ExecuteBatch(std::ostream& output, int pixel_min, int pixel_step, int theta_min, int theta_step, int jitter_step, const char* prefix = NULL);
};

template<class TagType> RunTest<TagType>::RunTest(int size, float fov, Cantag::DecodeEntity<TagType::PayloadSize>& d) :
  tag(),camera(),fs(size,size,fov),m_size(size),m_fov(fov),m_tan_fov(tan(fov/180.f*M_PI/2.f)) {
  
  if (!Cantag::Encode<TagType>()(d)) {
    throw "Failed to encode value";
  }

  // create the image that will hold the tag design
  Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> i(512,512);
  if (!Cantag::DrawTag(tag)(d,i)) {
    throw "Failed to draw encoded value";
  }
  
  if (!Cantag::Decode<TagType>()(d)) {
    throw "Failed to decode the encoded value";
  }

  stored_payload = (*d.GetPayloads().begin())->payload;

  fs.Init(i);
  fs.SetCameraParameters(camera);
}

template<class TagType> void RunTest<TagType>::ExecuteBatch(std::ostream& output, int pixel_min, int pixel_step, int theta_min, int theta_step, int jitter_step, const char* prefix) {
  
  //  Execute(125,0,0.0121775,0.00304439,13.0435);
  //return;

  for(int pixels=100;pixels>=pixel_min;pixels-=pixel_step) {
    float distance = (float)m_size/2.f/(float)pixels;
    // we want to jitter by half a pixel
    float jitter_range = m_tan_fov*distance/(float)m_size;
    float start = -jitter_range;
    float end = jitter_range;
    float step = jitter_range*2/(float)jitter_step;
    //    float tag_size = size/2/distance;
    float tag_size = pixels;
    float phi =0;
    for(int theta=180;theta>=theta_min;theta-=theta_step) {
      for(int u=0;u<jitter_step;++u) {
	float x0 = start + step*u;
	for(int v=0;v<jitter_step;++v) {
	  float y0 = start + step*v;
	  Result result = Execute(theta,phi,x0,y0,distance);
	  if (prefix) output << prefix << " ";
	  output << theta << " " << phi << " " << x0 << " " << y0 << " " << distance << " " << pixels << " ";
	  if (result.valid) {
	    output << result.distance_error << " " << result.angle_error << " " << result.bit_error << "\n";
	  }
	  else {
	    output << "FAIL FAIL FAIL\n";
	  }
	}
      }
    }
  }
}



template<class TagType> Result RunTest<TagType>::Execute(float theta, float phi, float x, float y, float z) {
  //  std::cout << "Execute " << theta << " " << phi << " " << x << " " << y << " " << z << std::endl;
  Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>* i = fs.Next(theta,phi,x,y,z);
  tag(*i,camera);
  const std::vector<typename TagType::Result>& loclist = tag.GetLocatedObjects();
  for(typename std::vector<typename TagType::Result>::const_iterator i = loclist.begin();i!=loclist.end();++i) {
    const typename TagType::Result& loc = *i;
    const Cantag::TransformEntity* te = loc.first;
    const Cantag::DecodeEntity<TagType::PayloadSize>* de = loc.second;

    // find out the rotation of the tag
    float v1[] = {0,0,0};
    float v2[] = {1,0,0};   
    te->GetPreferredTransform()->Apply3D(v1,1);
    te->GetPreferredTransform()->Apply3D(v2,1);
    float vec[] = {v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
      
    float angle = acos(vec[0]/sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]));

    Cantag::CyclicBitSet<TagType::PayloadSize> copy(de->GetPayloads()[0]->payload);
    copy.RotateRight(tag.GetPayloadRotation(angle));

    int errors = 0;
    for(int i=0;i<TagType::PayloadSize;++i) {
      if (copy[i] != stored_payload[i]) errors++;
    }

    //    if (errors > 0)  exit(-1);
    float nx = sin(theta/180*M_PI)*sin(phi/180*M_PI);
    float ny = sin(theta/180*M_PI)*cos(phi/180*M_PI);
    float nz = cos(theta/180*M_PI);

    float normal[3];
    te->GetPreferredTransform()->GetNormalVector(camera,normal);
    float dotprod = normal[0]*nx + normal[1]*ny + normal[2]*nz;
    float errorangle = acos(dotprod)/M_PI*180;
    
    float location[3];
    te->GetPreferredTransform()->GetLocation(location,1);
    
    float distance = sqrt( (location[0] - x)*(location[0] - x) + 
			   (location[1] - y)*(location[1] - y) + 
			   (location[2] - z)*(location[2] - z) );
    
    return Result(errorangle,distance,errors);
  }
  return Result();
}
