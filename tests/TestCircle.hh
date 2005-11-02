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
 $Header$
 */

#ifndef TEST_CIRCLE_GUARD
#define TEST_CIRCLE_GUARD

#include <Cantag.hh>

template<int RINGS, int SECTORS, class FitAlgorithm, class TransformAlgorithm>
class TestCircle : public Cantag::TagCircle<RINGS,SECTORS>, public Cantag::RawCoder<RINGS*SECTORS,RINGS> {
public:  
  enum { PayloadSize = Cantag::TagCircle<RINGS,SECTORS>::PayloadSize };
  typedef std::pair<const Cantag::TransformEntity*,const Cantag::DecodeEntity<PayloadSize>*> Result;
  typedef Cantag::TagCircle<RINGS,SECTORS> SpecType;
  typedef Cantag::RawCoder<RINGS*SECTORS,RINGS> CoderType;
private:
  typedef Cantag::ComposedEntity<TL5(Cantag::ContourEntity,
				     Cantag::ConvexHullEntity,
				     Cantag::ShapeEntity<Cantag::Ellipse>,
				     Cantag::TransformEntity,
				     Cantag::DecodeEntity<PayloadSize>
				     )> TagEntity;
  Cantag::Tree<TagEntity> tree;
  std::vector<Result> m_located;

  struct AddLocatedObject : public Cantag::Function<TL1(Cantag::TransformEntity),TL1(Cantag::DecodeEntity<PayloadSize>)> {
    TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm>& m_parent;
    const Cantag::Camera& m_camera;

    AddLocatedObject(TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm>& parent, const Cantag::Camera& camera) : m_parent(parent), m_camera(camera) {}

    bool operator()(const Cantag::TransformEntity& te, Cantag::DecodeEntity<PayloadSize>& de) const {
      m_parent.m_located.push_back(Result(&te,&de));
      return true;
    }
  };

public:
  TestCircle(float inner_bullseye,
	     float outer_bullseye,
	     float inner_data,
	     float outer_data) : Cantag::TagCircle<RINGS,SECTORS>(inner_bullseye,outer_bullseye,inner_data,outer_data), m_located() {}

  bool operator()(const Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>& i,const Cantag::Camera& camera) {
    m_located.erase(m_located.begin(),m_located.end());
    tree.DeleteAll();
    Cantag::MonochromeImage m(i.GetWidth(),i.GetHeight());
    Apply(i,m,Cantag::ThresholdGlobal<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>(128));
    Apply(m,tree,Cantag::ContourFollowerTree(*this));
    ApplyTree(tree,Cantag::DistortionCorrection(camera));
    ApplyTree(tree,FitAlgorithm()); 
    ApplyTree(tree,TransformAlgorithm(GetBullseyeOuterEdge()));
    ApplyTree(tree,Cantag::TransformSelectEllipse(*this,camera));
    ApplyTree(tree,Cantag::RemoveNonConcentricEllipse(*this));
    ApplyTree(tree,Cantag::Bind(Cantag::TransformEllipseRotate(*this,camera),m));
    ApplyTree(tree,Cantag::Bind(Cantag::SampleTagCircle(*this,camera),m));
    ApplyTree(tree,Cantag::Decode<CoderType>());
    ApplyTree(tree,Cantag::TransformRotateToPayload(*this));
    ApplyTree(tree,AddLocatedObject(*this,camera));
    return m_located.size() != 0;

  }

  const std::vector<Result>& GetLocatedObjects() const {
    return m_located;
  }
};

template<int RINGS,int SECTORS, class FitAlgorithm, class TransformAlgorithm>
struct CircleInner : public TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm> {
  CircleInner() : TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm>(0.2,0.4,0.6,1.0) {}
};

template<int RINGS,int SECTORS, class FitAlgorithm, class TransformAlgorithm>
struct CircleSplit : public TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm> {
  CircleSplit() : TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm>(0.2,1.0,0.4,0.8) {}
};

template<int RINGS,int SECTORS, class FitAlgorithm, class TransformAlgorithm>
struct CircleOuter : public TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm> {
  CircleOuter() : TestCircle<RINGS,SECTORS,FitAlgorithm,TransformAlgorithm>(0.8,1.0,0.2,0.6) {}
};

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
