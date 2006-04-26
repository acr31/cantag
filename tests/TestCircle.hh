/*
  Copyright (C) 2005 Andrew C. Rice

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
 $Header$
 */

#ifndef TEST_CIRCLE_GUARD
#define TEST_CIRCLE_GUARD

#include <sstream>

#include <Cantag.hh>
#include "Functions.hh"


template<int RINGS, int SECTORS, class FitAlgorithm, class TransformAlgorithm, class SpecClass>
class TestCircle : public SpecClass, public Cantag::TripOriginalCoder<RINGS*SECTORS,RINGS,2> {
public:  
  enum { PayloadSize = Cantag::TagCircle<RINGS,SECTORS>::PayloadSize };
  typedef std::pair<const Cantag::SignalStrengthEntity*,std::pair<const Cantag::TransformEntity*,const Cantag::DecodeEntity<PayloadSize>*> > PipelineResult;

  typedef Cantag::TagCircle<RINGS,SECTORS> SpecType;
  typedef Cantag::TripOriginalCoder<RINGS*SECTORS,RINGS,2> CoderType;
  typedef Cantag::ComposedEntity<TL6(Cantag::ContourEntity,
				     Cantag::SignalStrengthEntity,
				     Cantag::ConvexHullEntity,
				     Cantag::ShapeEntity<Cantag::Ellipse>,
				     Cantag::TransformEntity,
				     Cantag::DecodeEntity<PayloadSize>
				     )> TagEntity;
private:
  Cantag::Tree<TagEntity> tree;
  std::vector<PipelineResult> m_located;

  struct TransformSelect : public Cantag::Function<TL0,TL1(Cantag::TransformEntity)> {
    const Cantag::Transform& m_ideal_transform;
    const Cantag::Camera& m_camera;
    TransformSelect(const Cantag::Transform& ideal_transform, const Cantag::Camera& camera) : m_ideal_transform(ideal_transform), m_camera(camera) {}
    bool operator()(Cantag::TransformEntity& te) const {
      Cantag::Transform* min = NULL;
      float minf = 1e10;

      float ref_normal[3];
      m_ideal_transform.GetNormalVector(m_camera,ref_normal);

      for(std::list<Cantag::Transform*>::iterator i = te.GetTransforms().begin(); i!=te.GetTransforms().end();++i) {
	Cantag::Transform* t = *i;
	float normal[3];
	t->GetNormalVector(m_camera,normal);

	double dotprod = normal[0]*ref_normal[0] + normal[1]*ref_normal[1] + normal[2]*ref_normal[2];
	if (dotprod > 1.f) dotprod = 1.f;

	double errorangle = acos(dotprod);
	if (errorangle < minf) {
	  min = t;
	  minf = errorangle;
	}
      }

      for(std::list<Cantag::Transform*>::iterator i = te.GetTransforms().begin(); i!=te.GetTransforms().end();++i) {
	Cantag::Transform* t = *i;
	if (t != min) {
	    t->AccrueConfidence(-1.f);
	}
	else {
	    float conf = t->GetConfidence();
	    if (conf < 1e-5) t->SetConfidence(1.f);
	}
      }
      
      return true;
    }
  };

  struct TransformRotate : public Cantag::Function<TL0,TL1(Cantag::TransformEntity)> {
    const Cantag::Camera& m_camera;
    TransformRotate(const Cantag::Camera& camera) : m_camera(camera) {}
    bool operator()(Cantag::TransformEntity& te) const {
	// find out the rotation of the tag
      float v1[] = {0,0,0};
      float v2[] = {1,0,0};   
      Cantag::Transform* t = te.GetPreferredTransform();

      if (t == NULL) {
	  std::cerr << "No preferred transform" << std::endl;
	  return false;
      }

      t->Apply3D(v1,1);
      t->Apply3D(v2,1);
      double vec[] = {v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
      double atanv = atan(fabs(vec[1]/vec[0]));
      double angle;
      if (vec[0] > 0.f) {
	if (vec[1] >= 0.f) angle = atanv;
	else angle = 2.f*M_PI - atanv;
      }
      else if (vec[0] == 0.f) { // unlikely
	if (vec[1] >= 0.f) angle = M_PI / 2.f;
	else angle = 3.f*M_PI/2.f;
      }
      else { // vec[0] < 0.f
	if (vec[1] >= 0.f) angle = M_PI - atanv;
	else angle = M_PI + atanv;
      }
      t->Rotate(cos(angle),-sin(angle));
      return true;
    };
  };


  bool Process(Cantag::Tree<TagEntity>& tree,Cantag::MonochromeImage& m, const Cantag::Transform& ideal_transform, const Cantag::Camera& camera, const char* debug_name = NULL) {
    char name_buffer[255];
    int debug_counter = 0;
    if (debug_name) {
      snprintf(name_buffer,255,debug_name,debug_counter++);
      Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
      Apply(m,Cantag::DrawEntityMonochrome(output));
      output.Save(name_buffer);
    }

    if (debug_name) {
      std::cout << "Contours" << std::endl;
      ApplyTree(tree,Cantag::PrintEntityContour(std::cout));
      std::cout << std::endl;      
      snprintf(name_buffer,255,debug_name,debug_counter++);
      Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
      ApplyTree(tree,Cantag::DrawEntityContour(output));
      output.Save(name_buffer);
    }

    ApplyTree(tree,Cantag::SignalStrengthContour());
    if (debug_name) {
      std::cout << "SignalStrengthContour" << std::endl;
    }


    ApplyTree(tree,Cantag::DistortionCorrectionNone(camera));
    ApplyTree(tree,FitAlgorithm()); 
    if (debug_name) {
      std::cout << "Shapes" << std::endl;
      ApplyTree(tree,Cantag::PrintEntityShapeCircle(std::cout));
      std::cout << std::endl;
      snprintf(name_buffer,255,debug_name,debug_counter++);
      Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
      Apply(m,Cantag::DrawEntityMonochrome(output));
      output.ConvertScale(0.25,190);
      ApplyTree(tree,Cantag::DrawEntityShape<Cantag::Ellipse>(output,camera));
      output.Save(name_buffer);
    }
    ApplyTree(tree,TransformAlgorithm(this->GetBullseyeOuterEdge()));
    ApplyTree(tree,Cantag::TransformSelectEllipseErrorOfFit<Cantag::CheckEllipseStricker<Cantag::AggregateMean<float> > >(*this,camera));
    ApplyTree(tree,TransformSelect(ideal_transform,camera));
    //ApplyTree(tree,Cantag::RemoveNonConcentricEllipse(*this));
    //ApplyTree(tree,Cantag::Bind(Cantag::TransformEllipseRotate(*this,camera),m));
    ApplyTree(tree,TransformRotate(camera));

    ApplyTree(tree,Cantag::Bind(Cantag::SampleTagCircle(*this,camera),m));
    if (debug_name) {
      std::cout << "Transforms" << std::endl;
      ApplyTree(tree,Cantag::PrintEntityTransform(std::cout));
      std::cout << std::endl;      
      snprintf(name_buffer,255,debug_name,debug_counter++);
      Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
      Apply(m,Cantag::DrawEntityMonochrome(output));
      output.ConvertScale(0.25,190);
      ApplyTree(tree,Cantag::DrawEntityTransform(output,camera));
      ApplyTree(tree,Cantag::DrawEntitySample(output,camera,*this));
      output.Save(name_buffer);

      std::cout << "Sampled" << std::endl;
      ApplyTree(tree,Cantag::PrintEntityDecode<PayloadSize>(std::cout));
    }
    ApplyTree(tree,Cantag::Decode<CoderType>());
    ApplyTree(tree,AddLocatedObject<PayloadSize>(this->m_located));
    return m_located.size() != 0;

  }


public:

  std::string Prefix() const {
    std::ostringstream result;
    result << typeid(*this).name() << " " << 
      RINGS << " " << 
      SECTORS << " " <<
	Cantag::TagCircle<RINGS,SECTORS>::GetBullseyeInnerEdge() << " " <<
      Cantag::TagCircle<RINGS,SECTORS>::GetBullseyeOuterEdge() << " " <<
      Cantag::TagCircle<RINGS,SECTORS>::GetDataInnerEdge() << " " <<
      Cantag::TagCircle<RINGS,SECTORS>::GetDataOuterEdge(); 
    return result.str();
  }



  bool operator()(const Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>& i,const Cantag::ContourEntity& ideal_contour, const Cantag::Transform& ideal_transform, const Cantag::Camera& camera, const char* debug_name = NULL) {
    m_located.erase(m_located.begin(),m_located.end());
    tree.DeleteAll();
    Cantag::MonochromeImage m(i.GetWidth(),i.GetHeight());
    Apply(i,m,Cantag::ThresholdGlobal<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>(128));
    Apply(m,Cantag::ContourFollowerClearImageBorder());
    Apply(m,tree,Cantag::ContourFollowerTree(*this));
//    ApplyTree(tree,FindContour(ideal_contour));
    return Process(tree,m,ideal_transform,camera,debug_name);
  }  

  bool ProcessFromContourEntity(const Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>& i,const Cantag::Transform& ideal_transform, const Cantag::Camera& camera, const char* debug_name = NULL) {
    m_located.erase(m_located.begin(),m_located.end());
    Cantag::MonochromeImage m(i.GetWidth(),i.GetHeight());
    Apply(i,m,Cantag::ThresholdGlobal<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>(128));
    Apply(m,Cantag::ContourFollowerClearImageBorder());    
    return Process(tree,m,ideal_transform, camera,debug_name);
  }

  
  Cantag::Tree<TagEntity>& GetTree() { return tree; }

  const std::vector<PipelineResult>& GetLocatedObjects() const {
    return m_located;
  }
};

template<int RINGS,int SECTORS> 
class TagCircleFixed : public Cantag::TagCircle<RINGS,SECTORS> {
public:
  //  TagCircleFixed() : Cantag::TagCircle<RINGS,SECTORS>(0.2,0.4,0.6,1.0) {}
  TagCircleFixed() : Cantag::TagCircle<RINGS,SECTORS>(0.272727,0.454545,0.5454545,1.0) {}
};

template<int RINGS,int SECTORS, class FitAlgorithm, class TransformAlgorithm>
struct CircleInnerFixed : public TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm,TagCircleFixed<RINGS,SECTORS> > {};

template<int RINGS,int SECTORS, class FitAlgorithm, class TransformAlgorithm>
struct CircleInner : public TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm,Cantag::TagCircleInner<RINGS,SECTORS> > {};

template<int RINGS,int SECTORS, class FitAlgorithm, class TransformAlgorithm>
struct CircleSplit : public TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm,Cantag::TagCircleSplit<RINGS,SECTORS> > {};

template<int RINGS,int SECTORS, class FitAlgorithm, class TransformAlgorithm>
struct CircleOuter : public TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm,Cantag::TagCircleOuter<RINGS,SECTORS> > {};

class CircleOuterLSFull36 : public CircleOuter<2,18,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleOuterLSLinear36 : public CircleOuter<2,18,Cantag::FitEllipseLS,Cantag::TransformEllipseLinear> {};
class CircleOuterSimpleFull36 : public CircleOuter<2,18,Cantag::FitEllipseSimple,Cantag::TransformEllipseFull> {};
class CircleOuterSimpleLinear36 : public CircleOuter<2,18,Cantag::FitEllipseSimple,Cantag::TransformEllipseLinear> {};
class CircleInnerLSFull36 : public CircleInner<2,18,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSLinear36 : public CircleInner<2,18,Cantag::FitEllipseLS,Cantag::TransformEllipseLinear> {};
class CircleInnerSimpleFull36 : public CircleInner<2,18,Cantag::FitEllipseSimple,Cantag::TransformEllipseFull> {};
class CircleInnerSimpleLinear36 : public CircleInner<2,18,Cantag::FitEllipseSimple,Cantag::TransformEllipseLinear> {};
class CircleSplitLSFull36 : public CircleSplit<2,18,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleSplitLSLinear36 : public CircleSplit<2,18,Cantag::FitEllipseLS,Cantag::TransformEllipseLinear> {};
class CircleSplitSimpleFull36 : public CircleSplit<2,18,Cantag::FitEllipseSimple,Cantag::TransformEllipseFull> {};
class CircleSplitSimpleLinear36 : public CircleSplit<2,18,Cantag::FitEllipseSimple,Cantag::TransformEllipseLinear> {};

class CircleInnerLSFull4 : public CircleInner<2,2,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull8 : public CircleInner<2,4,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull16 : public CircleInner<2,8,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull24 : public CircleInner<2,12,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull48 : public CircleInner<2,24,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull64 : public CircleInner<2,32,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull80 : public CircleInner<2,40,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull100 : public CircleInner<2,50,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull120 : public CircleInner<2,60,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull144 : public CircleInner<2,72,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull168 : public CircleInner<2,84,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};
class CircleInnerLSFull196 : public CircleInner<2,98,Cantag::FitEllipseLS,Cantag::TransformEllipseFull> {};

#endif//TEST_CIRCLE_GUARD
