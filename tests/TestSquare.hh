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

template<int EDGE_CELLS, class FitAlgorithm, class TransformAlgorithm>
class TestSquare : public Cantag::TagSquare<EDGE_CELLS>, public Cantag::RawCoder<EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2),(EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2))/4> {

 public:  
  enum { PayloadSize = Cantag::TagSquare<EDGE_CELLS>::PayloadSize };
  typedef std::pair<const Cantag::TransformEntity*,const Cantag::DecodeEntity<PayloadSize>*> Result;
  typedef Cantag::TagSquare<EDGE_CELLS> SpecType;
  typedef Cantag::RawCoder<EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2),(EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2))/4> CoderType;
private:
  typedef Cantag::ComposedEntity<TL5(Cantag::ContourEntity,
				     Cantag::ConvexHullEntity,
				     Cantag::ShapeEntity<Cantag::QuadTangle>,
				     Cantag::TransformEntity,
				     Cantag::DecodeEntity<PayloadSize>
				     )> TagEntity;
  Cantag::Tree<TagEntity> tree;
  std::vector<Result> m_located;

  struct AddLocatedObject : public Cantag::Function<TL1(Cantag::TransformEntity),TL1(Cantag::DecodeEntity<PayloadSize>)> {
    TestSquare<EDGE_CELLS,FitAlgorithm,TransformAlgorithm>& m_parent;
    const Cantag::Camera& m_camera;

    AddLocatedObject(TestSquare<EDGE_CELLS,FitAlgorithm,TransformAlgorithm>& parent, const Cantag::Camera& camera) : m_parent(parent), m_camera(camera) {}

    bool operator()(const Cantag::TransformEntity& te, Cantag::DecodeEntity<PayloadSize>& de) const {
      m_parent.m_located.push_back(Result(&te,&de));
      return true;
    }
  };
  
public:
  TestSquare() : Cantag::TagSquare<EDGE_CELLS>(), m_located() {}

  bool operator()(const Cantag::Image<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>& i,const Cantag::Camera& camera) {
    m_located.erase(m_located.begin(),m_located.end());
    tree.DeleteAll();
    Cantag::MonochromeImage m(i.GetWidth(),i.GetHeight());
    Apply(i,m,Cantag::ThresholdGlobal<Cantag::Pix::Sze::Byte1,Cantag::Pix::Fmt::Grey8>(128));
    Apply(m,tree,Cantag::ContourFollowerTree(*this));
    ApplyTree(tree,Cantag::ConvexHull(*this));
    ApplyTree(tree,Cantag::DistortionCorrection(camera));
    ApplyTree(tree,FitAlgorithm()); 
    ApplyTree(tree,TransformAlgorithm());
    ApplyTree(tree,Cantag::Bind(Cantag::SampleTagSquare(*this,camera),m));
    ApplyTree(tree,Cantag::Decode<CoderType>());
    ApplyTree(tree,AddLocatedObject(*this,camera));
    return m_located.size() != 0;

  }

  const std::vector<Result>& GetLocatedObjects() const {
    return m_located;
  }
};


class SquareCornerProj36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleProjective> {};
class SquareCornerReduced36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleReduced> {};
class SquareCornerSpaceSearch36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleSpaceSearch> {};
class SquareCornerCyberCode36 : public TestSquare<6,Cantag::FitQuadTangleCorner,Cantag::TransformQuadTangleCyberCode> {};

class SquareConvexHullProj36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleProjective> {};
class SquareConvexHullReduced36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleReduced> {};
class SquareConvexHullSpaceSearch36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleSpaceSearch> {};
class SquareConvexHullCyberCode36 : public TestSquare<6,Cantag::FitQuadTangleConvexHull,Cantag::TransformQuadTangleCyberCode> {};

class SquarePolygonProj36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonReduced36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleReduced> {};
class SquarePolygonSpaceSearch36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleSpaceSearch> {};
class SquarePolygonCyberCode36 : public TestSquare<6,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleCyberCode> {};



class SquarePolygonProj25 : public TestSquare<5,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj49 : public TestSquare<7,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj64 : public TestSquare<8,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj81 : public TestSquare<9,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj100 : public TestSquare<10,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj121 : public TestSquare<11,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj141 : public TestSquare<12,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj169 : public TestSquare<13,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};
class SquarePolygonProj196 : public TestSquare<14,Cantag::FitQuadTanglePolygon,Cantag::TransformQuadTangleProjective> {};

#endif//TEST_SQUARE_GUARD
