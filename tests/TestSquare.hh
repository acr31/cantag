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

#ifndef TEST_SQUARE_GUARD
#define TEST_SQUARE_GUARD

#include <Cantag.hh>
#include "Functions.hh"

template<int EDGE_CELLS, class FitAlgorithm, class TransformAlgorithm>
class TestSquare : public Cantag::TagSquare<EDGE_CELLS>, public Cantag::RawCoder<EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2),(EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2))/4> {

 public:  
  enum { PayloadSize = Cantag::TagSquare<EDGE_CELLS>::PayloadSize };
  typedef Cantag::TagSquare<EDGE_CELLS> SpecType;
  typedef Container<PayloadSize> PipelineResult;
  
  typedef Cantag::RawCoder<EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2),(EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2))/4> CoderType;
  typedef Cantag::ComposedEntity<TL7(Cantag::ContourEntity,
				     Cantag::SignalStrengthEntity,
				     Cantag::ConvexHullEntity,
				     Cantag::ShapeEntity<Cantag::QuadTangle>,
				     Cantag::TransformEntity,
				     Cantag::DecodeEntity<PayloadSize>,
				     Cantag::MaxSampleStrengthEntity
				     )> TagEntity;
 private:
  Cantag::Tree<TagEntity> tree;
  std::vector<PipelineResult> m_located;

  struct TransformRotate : public Cantag::Function<TL1(Cantag::TransformEntity),TL1(Cantag::DecodeEntity<PayloadSize>)> {
    const Cantag::Camera& m_camera;
    TransformRotate(const Cantag::Camera& camera) : m_camera(camera) {}
    bool operator()(const Cantag::TransformEntity& te, const Cantag::DecodeEntity<PayloadSize>& de) const {
      // find out the rotation of the tag
      float v1[] = {0,0,0};
      float v2[] = {0,1,0};   
      te.GetPreferredTransform()->Apply3D(v1,1);
      te.GetPreferredTransform()->Apply3D(v2,1);
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
    
      if (angle < M_PI/4.f) {
	// no rotation of payload
      }
      else if (angle < 3.f*M_PI/4.f) {
	// rotate by 1 quadrant
	(*de.GetPayloads().begin())->payload.RotateRight(PayloadSize/4);	
      }
      else if (angle < 5.f*M_PI/4.f) {
	// rotate by 2 quadrant
	(*de.GetPayloads().begin())->payload.RotateRight(PayloadSize/2);	
      }
      else if (angle < 7.f*M_PI/4.f) {
	// rotate by 3 quadrant
	(*de.GetPayloads().begin())->payload.RotateRight(3*PayloadSize/4);	
      }

      return true;
    };
  };


  bool Process(Cantag::Tree<TagEntity>& process, Cantag::MonochromeImage& m, const Cantag::Transform& ideal_transform, const Cantag::Camera& camera, const char* debug_name = NULL) {
    char name_buffer[255];
    int debug_counter = 0;

    if (debug_name) {
      std::cout << "Contours" << std::endl;
      ApplyTree(tree,Cantag::PrintEntityContour(std::cout));
      std::cout << std::endl;
      snprintf(name_buffer,255,debug_name,debug_counter++);
      Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
      Apply(m,Cantag::DrawEntityMonochrome(output));
      output.ConvertScale(0.25,190);
      ApplyTree(tree,Cantag::DrawEntityContour(output));
      output.Save(name_buffer);
    }
    ApplyTree(tree,Cantag::SignalStrengthContour());
    if (debug_name) {
      std::cout << "Signal Strength" << std::endl;
    }

    ApplyTree(tree,Cantag::ConvexHull(*this));
    if (debug_name) {
      std::cout << "Convex Hulls" <<std::endl;
      ApplyTree(tree,Cantag::PrintEntityConvexHull(std::cout));
      std::cout << std::endl;      
    }
    ApplyTree(tree,Cantag::DistortionCorrectionNone(camera));
    ApplyTree(tree,FitAlgorithm());
    if (debug_name) {
      std::cout << "Shapes" << std::endl;
      ApplyTree(tree,Cantag::PrintEntityShapeSquare(std::cout));
      std::cout << std::endl;
      snprintf(name_buffer,255,debug_name,debug_counter++);
      Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
      Apply(m,Cantag::DrawEntityMonochrome(output));
      output.ConvertScale(0.25,190);
      ApplyTree(tree,Cantag::DrawEntityShape<Cantag::QuadTangle>(output,camera));
      output.Save(name_buffer);
    }
    if (Regression()) {
      ApplyTree(tree,Cantag::FitQuadTangleRegression()); 
      if (debug_name) {
	snprintf(name_buffer,255,debug_name,debug_counter++);
	Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
	Apply(m,Cantag::DrawEntityMonochrome(output));
	output.ConvertScale(0.25,190);
	ApplyTree(tree,Cantag::DrawEntityShape<Cantag::QuadTangle>(output,camera));
	output.Save(name_buffer);
      }
    }
    ApplyTree(tree,TransformAlgorithm());
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
    }
    ApplyTree(tree,Cantag::Bind(Cantag::SampleTagSquare(*this,camera),m));
    ApplyTree(tree,TransformRotate(camera));
    ApplyTree(tree,Cantag::Decode<CoderType>());
    ApplyTree(tree,AddLocatedObject<PayloadSize>(m_located));
    return m_located.size() != 0;

  }

public:

  std::string Prefix() const {
    std::ostringstream result;
    result << typeid(*this).name() << " " <<
      PayloadSize << " 1 0 0 0 0";
    return result.str();
  }

  TestSquare() : Cantag::TagSquare<EDGE_CELLS>(), m_located() {}

  virtual bool Regression() { return false; }

  Cantag::Tree<TagEntity>& GetTree() { return tree; }

  bool operator()(const Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>& i,const Cantag::ContourEntity& ideal_contour, const Cantag::Transform& ideal_transform, const Cantag::Camera& camera, const char* debug_name = NULL) {
    m_located.erase(m_located.begin(),m_located.end());
    tree.DeleteAll();
    Cantag::MonochromeImage m(i.GetWidth(),i.GetHeight());
    Apply(i,m,Cantag::ThresholdGlobal<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>(128));
    Apply(m,Cantag::ContourFollowerClearImageBorder());
    Apply(m,tree,Cantag::ContourFollowerTree(*this));
    ApplyTree(tree,FindContour(ideal_contour));
    return Process(tree,m,ideal_transform,camera,debug_name);
  }  

  bool ProcessFromContourEntity(const Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>& i,const Cantag::Transform& ideal_transform,const Cantag::Camera& camera, const char* debug_name = NULL) {
    m_located.erase(m_located.begin(),m_located.end());
    Cantag::MonochromeImage m(i.GetWidth(),i.GetHeight());
    Apply(i,m,Cantag::ThresholdGlobal<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>(128));
    Apply(m,Cantag::ContourFollowerClearImageBorder());
    return Process(tree,m,ideal_transform,camera,debug_name);
  }

  const std::vector<PipelineResult>& GetLocatedObjects() const {
    return m_located;
  }

		  
};

template<int EDGE_CELSS, class FitAlgorithm, class TransformAlgorithm> 
class TestSquareRegression : public TestSquare<EDGE_CELSS,FitAlgorithm,TransformAlgorithm> {
public:
  bool Regression() { return true; }
};



class SquareCornerProj36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleProjective> {};
class SquareCornerReduced36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleReduced> {};
class SquareCornerSpaceSearch36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleSpaceSearch> {};
class SquareCornerCyberCode36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleCyberCode> {};
class SquareRegressCornerProj36 : public TestSquareRegression<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleProjective> {};
class SquareRegressCornerReduced36 : public TestSquareRegression<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleReduced> {};
class SquareRegressCornerSpaceSearch36 : public TestSquareRegression<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleSpaceSearch> {};
class SquareRegressCornerCyberCode36 : public TestSquareRegression<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleCyberCode> {};

class SquareConvexHullProj36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareConvexHullReduced36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleReduced> {};
class SquareConvexHullSpaceSearch36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleSpaceSearch> {};
class SquareConvexHullCyberCode36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleCyberCode> {};
class SquareRegressConvexHullProj36 : public TestSquareRegression<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullReduced36 : public TestSquareRegression<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleReduced> {};
class SquareRegressConvexHullSpaceSearch36 : public TestSquareRegression<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleSpaceSearch> {};
class SquareRegressConvexHullCyberCode36 : public TestSquareRegression<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleCyberCode> {};

class SquarePolygonProj36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonReduced36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleReduced> {};
class SquarePolygonSpaceSearch36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleSpaceSearch> {};
class SquarePolygonCyberCode36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleCyberCode> {};
class SquareRegressPolygonProj36 : public TestSquareRegression<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquareRegressPolygonReduced36 : public TestSquareRegression<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleReduced> {};
class SquareRegressPolygonSpaceSearch36 : public TestSquareRegression<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleSpaceSearch> {};
class SquareRegressPolygonCyberCode36 : public TestSquareRegression<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleCyberCode> {};

class SquarePolygonProj25 : public TestSquare<5,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj49 : public TestSquare<7,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj64 : public TestSquare<8,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj81 : public TestSquare<9,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj100 : public TestSquare<10,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj121 : public TestSquare<11,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj144 : public TestSquare<12,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj169 : public TestSquare<13,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj196 : public TestSquare<14,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};

class SquareRegressConvexHullProj25 : public TestSquareRegression<5,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj49 : public TestSquareRegression<7,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj64 : public TestSquareRegression<8,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj81 : public TestSquareRegression<9,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj100 : public TestSquareRegression<10,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj121 : public TestSquareRegression<11,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj141 : public TestSquareRegression<12,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj169 : public TestSquareRegression<13,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareRegressConvexHullProj196 : public TestSquareRegression<14,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};

#endif//TEST_SQUARE_GUARD
