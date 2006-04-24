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
#include "Functions.hh"

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
  
  Result Execute(double theta, double phi, double x, double y, double z,const char* debug_name= NULL);
  void ExecuteBatch(std::ostream& output, const char* prefix = NULL);
  void ExecuteSingle(std::ostream& output, float x0, float y0, float z0,float theta,float phi, const char* prefix = NULL);
};

template<class TagType> RunTest<TagType>::RunTest(int size, float fov, Cantag::DecodeEntity<TagType::PayloadSize>& d) :
  tag(),camera(),fs(size,size,fov),m_size(size),m_fov(fov),m_tan_fov(tan(fov/180.f*M_PI/2.f)) {
  
  if (!Cantag::Encode<TagType>()(d)) {
    throw "Failed to encode value";
  }

  // create the image that will hold the tag design
  Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> i(1024,1024);
  if (!Cantag::DrawTag(tag)(d,i)) {
    throw "Failed to draw encoded value";
  }

  if (!Cantag::Decode<TagType>()(d)) {
    throw "Failed to decode the encoded value";
  }

  stored_payload = (*d.GetPayloads().begin())->payload;

  fs.SetSuperSample(1);
  fs.Init(i);
  fs.SetCameraParameters(camera);
}

template<class TagType> void RunTest<TagType>::ExecuteBatch(std::ostream& output, const char* prefix) {
  const int numsteps = 5;
  const int pixel_min = 5;
  const int pixel_step = 10;
  const int angle_step = 10;
  for(int pixels=300;pixels>=pixel_min;pixels-=pixel_step) {
    double distance = (double)m_size/2.f/(double)pixels;
    
    // now work out the range of x and y to try for this distance
    double range = m_tan_fov * distance - 1.0;
    double step = 2*range / (double)numsteps;
//    for(int xc=-numsteps;xc<=numsteps;++xc) {
      int xc=0; {
      float x0 = xc * step;
      //for(int yc=-numsteps;yc<=numsteps;++yc) {
	int yc=0; {
	float y0 = yc * step;
//	for(int theta = -90; theta <= 90; theta += angle_step) {
	  int theta = 0; {
	  for(int phi = -90;phi<=90;phi += angle_step) {
	    Result result = Execute(theta,phi,x0,y0,distance);
	    if (prefix) output << prefix << " ";
	    output << theta << " " << phi << " " << x0 << " " << y0 << " " << distance << " " << pixels << " " << xc << " " << yc << " ";
	    if (result.valid) {
	      output << result.distance_error << " " << result.angle_error << " " << result.bit_error << " " << result.min_distance << " " << result.max_distance << " " << result.signal_strength << " " << result.min_width << " ";
	    }
	    else if (result.not_visible) {
	      output << "NONE NONE NONE " << result.min_distance << " NONE NONE NONE ";
	    }	    
	    else {
	      output << "FAIL FAIL FAIL " << result.min_distance << " FAIL FAIL FAIL ";
	    }	    

	    if (result.error_valid) {
		output << result.correct_transform_error;
		for(std::vector<float>::const_iterator i = result.incorrect_transform_errors.begin();
		    i!= result.incorrect_transform_errors.end();
		    ++i) {
		    output << " " << *i;
		}
		output << "\n";
	    }
	    else {
		output << "\n";
	    }
	  }
	}
      }
    }
  }
}

template<class TagType> void RunTest<TagType>::ExecuteSingle(std::ostream& output,float x0, float y0, float z0, float theta, float phi, const char* prefix) {
  int pixels = (int)((double)m_size/2.f/z0);

  Result result = Execute(theta,phi,x0,y0,z0,"debug-%0.2d.pnm");
	  
  if (prefix) output << prefix << " ";
  output << theta << " " << phi << " " << x0 << " " << y0 << " " << z0 << " " << pixels << " ";
  if (result.valid) {
    output << result.distance_error << " " << result.angle_error << " " << result.bit_error << " " << result.min_distance << " " << result.max_distance << " " << result.signal_strength << " " << result.min_width << " ";
  }
  else if (result.not_visible) {
    output << "NONE NONE NONE " << result.min_distance << " NONE NONE NONE ";
  }
  else {
    output << "FAIL FAIL FAIL " << result.min_distance << " FAIL FAIL FAIL ";
  }
  if (result.error_valid) {
      output << result.correct_transform_error;
      for(std::vector<float>::const_iterator i = result.incorrect_transform_errors.begin();
	  i!= result.incorrect_transform_errors.end();
	  ++i) {
	  output << " " << *i;
      }
      output << "\n";
  }
  else {
      output << "\n";
  }
}




template<class TagType> Result RunTest<TagType>::Execute(double theta, double phi, double x, double y, double z, const char* debug_name) {

  // work out if the tag will be visible if the angle between the
  // normal vector of the tag and the vector from the camera to the
  // tag if greater than 90 degrees then we will be looking at the
  // back
  Cantag::TransformEntity ref_te;

  ref_te.GetTransforms().push_back(new Cantag::Transform(x,y,z,theta/180.f*M_PI,phi/180.f*M_PI,1.f));
  float ref_normal[3];
  ref_te.GetPreferredTransform()->GetNormalVector(camera,ref_normal);
  float location[3];
  ref_te.GetPreferredTransform()->GetLocation(location,1.f);
  float mod = sqrt(location[0]*location[0] + location[1]*location[1] + location[2]*location[2]);
  float angle = acos((ref_normal[0]*location[0] + ref_normal[1]*location[1] + ref_normal[2]*location[2])/mod);

  if (fabs(angle) >= M_PI/2) {
    Result r;
    r.not_visible = true;
    return r;
  }

  // or if the corners of the tag lie outside the image
  float corners[] = {-1,-1,
		     -1,1,
		     1,1,
		     1,-1};
  ref_te.GetPreferredTransform()->Apply(corners,4);
  camera.NPCFToImage(corners,4);
  for(int i=0;i<4;++i) {
    float x = corners[2*i];
    float y = corners[2*i+1];
    if (x < 0.f || x > m_size || y < 0.f || y > m_size) {
      Result r;
      r.not_visible = true;
      return r;
    }
  }

  // get the next image from the image source
  Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>* i = fs.Next(theta,phi,x,y,z);
  // simulate the contour and decode from there
  Cantag::ContourEntity* contour;
#undef SIMULATE_CONTOUR
#ifdef SIMULATE_CONTOUR
    Cantag::Tree<typename TagType::TagEntity>& tree = tag.GetTree();
    tree.DeleteAll();
    contour = tree.AddChild()->GetNode();
#else
    Cantag::ContourEntity ce;
    contour = &ce;
#endif
      
    contour->SetValid(true);
    Cantag::SimulateContour(tag,camera)(ref_te,*contour);

    // uncomment this to demonstrate that the simulated contour agrees with GL
    /*
    Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(*i);
    output.ConvertScale(0.25,190);
    Apply(ce,Cantag::DrawEntityContour(output));
    output.Save("test.pnm");
    */
#ifdef SIMULATE_CONTOUR
    tag.ProcessFromContourEntity(*i,ref_te,camera,debug_name);
#else
  // process the tag
  tag(*i,*contour,*(ref_te.GetPreferredTransform()),camera,debug_name);
#endif

  const typename TagType::PipelineResult* favorite = NULL;
  float minerror = 1e10;

  const std::vector<typename TagType::PipelineResult>& loclist = tag.GetLocatedObjects();
  for(typename std::vector<typename TagType::PipelineResult>::const_iterator i = loclist.begin();i!=loclist.end();++i) {
    const typename TagType::PipelineResult& loc = *i;
    const Cantag::TransformEntity* te = loc.second.first;

    float location[3];
    te->GetPreferredTransform()->GetLocation(location,1);
    double distance = sqrt( (location[0] - x)*(location[0] - x) + 
			    (location[1] - y)*(location[1] - y) + 
			    (location[2] - z)*(location[2] - z) );
    if (distance < minerror) {
      favorite = &(*i);
      minerror = distance;
    }
  }

  Result r;
  // simulate the minimum distance for this transform
  std::vector<float> min;
  Cantag::SimulateMinDistance(min,tag,camera)(ref_te);
  float minv = FLT_MAX;
  for(std::vector<float>::const_iterator i = min.begin();i!=min.end();++i) {
    float v = *i;
    if (v < minv) minv = v;
  }    
  r.min_distance = minv;

  if (favorite) {

    const typename TagType::PipelineResult& loc = *favorite;
    const Cantag::SignalStrengthEntity* ce = loc.first;
    const Cantag::TransformEntity* te = loc.second.first;

    const Cantag::DecodeEntity<TagType::PayloadSize>* de = loc.second.second;

    Cantag::CyclicBitSet<TagType::PayloadSize> copy(de->GetPayloads()[0]->payload);

    int errors = 0;
    for(int i=0;i<TagType::PayloadSize;++i) {
      if (copy[i] != stored_payload[i]) errors++;
    }

    float normal[3];
    te->GetPreferredTransform()->GetNormalVector(camera,normal);
    double dotprod = normal[0]*ref_normal[0] + normal[1]*ref_normal[1] + normal[2]*ref_normal[2];
    if (dotprod > 1.f) dotprod = 1.f;
    double errorangle = acos(dotprod)/M_PI*180.f;

    
    float location[3];
    te->GetPreferredTransform()->GetLocation(location,1);
    
    double distance = sqrt( (location[0] - x)*(location[0] - x) + 
			    (location[1] - y)*(location[1] - y) + 
			    (location[2] - z)*(location[2] - z) );

    // now work out the maximum sampling distance error take the derived
    // tag transform, work out the sample points and measure their
    // distance to the correct sample point for this position and pose.
    // Return the maximum distance.
    Cantag::Maxima m;
    Cantag::SimulateMaxSampleError(m,tag,camera)(*te,ref_te);

    r.Update(errorangle,distance,errors,m.GetMaxima());

    r.SetSignalStrength(ce->GetSignalStrength(),ce->GetMin());

    // now work out the error of fit values for the transforms
    for(std::list<Cantag::Transform*>::const_iterator i = te->GetTransforms().begin();
	i!=te->GetTransforms().end();
	++i) {
	const Cantag::Transform* t = *i;
	float error = Cantag::abs<float>(t->GetConfidence());
	if (t->GetConfidence() > 0.f) { // this one corresponds to the correct transform
	    r.SetCorrectTransformError(error);
	}
	else {
	    r.AddIncorrectTransformError(error);
	}
    }
  }
  
  return r;
 
}
