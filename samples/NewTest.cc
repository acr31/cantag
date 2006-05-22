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

#include <iostream>
#include <string>
#include <Cantag.hh>

#include "Permute.hh"

struct TestCircle : public Cantag::TagCircle<2,17>,Cantag::TripOriginalCoder<34,2,2> {
  TestCircle(float binner,float bouter, float dinner,float douter) : Cantag::TagCircle<2,17>(binner,bouter,dinner,douter) {}
};

struct TestSquare : public Cantag::TagSquare<6>,Cantag::TripOriginalCoder<36,3,2> {
  TestSquare() : Cantag::TagSquare<6>() {}
};

#undef DRAW_IMAGE

using namespace Cantag;

struct RecogniseSquare : public Function<TL2(TransformEntity,DecodeEntity<36>),TL1(MaxSampleStrengthEntity)> {
  const Camera& m_camera;
  const char* m_prefix;
  RecogniseSquare(const Camera& cam, const char* prefix) : m_camera(cam),m_prefix(prefix) {};
  
  bool operator()(const TransformEntity& te, const DecodeEntity<36>& de, MaxSampleStrengthEntity& me) const {
    const CyclicBitSet<36>& code = de.GetPayloads()[0]->payload;
    
    const Transform* t = te.GetPreferredTransform();
    if (t==NULL) return false;
    float normal[3];
    t->GetNormalVector(m_camera,normal);
    float location[3];

    if (code == "000000000000001100110011000101010100") {
      t->GetLocation(m_camera,location,0.015);
    }
    else if (code == "000000000000001101000011000101010100") {
      t->GetLocation(m_camera,location,0.0225);
    }
    else if (code == "000000000000001101010011000101010100") {
      t->GetLocation(m_camera,location,0.03);
    }
    else { return false; }

    std::cout << m_prefix << " " << code << " ";
    std::cout << normal[0] << " " << normal[1] << " " << normal[2] << " ";
    std::cout << location[0] << " " << location[1] << " " << location[2] << " ";
    std::cout << me.GetSampleStrength() << std::endl;
    return true;
  }
};

struct RecogniseCircle : public Function<TL2(TransformEntity,DecodeEntity<34>),TL1(MaxSampleStrengthEntity)> {
  const Camera& m_camera;
  const char* m_prefix;
  RecogniseCircle(const Camera& cam, const char* prefix) : m_camera(cam),m_prefix(prefix) {};

  bool operator()(const TransformEntity& te, const DecodeEntity<34>& de, MaxSampleStrengthEntity& me) const {
    const CyclicBitSet<34>& code = de.GetPayloads()[0]->payload;

    const Transform* t = te.GetPreferredTransform();
    if (t==NULL) return false;
    float normal[3];
    t->GetNormalVector(m_camera,normal);
    float location[3];


    if ((code == "0000000000000000000011000101010100") ||
	(code == "0000000000000000000011000101010100") ||
	(code == "0000000000000000000011001001010100") ||
	(code == "0000000000000000000011001101010100") ||
	(code == "0000000000000000000011010001010100") ||
	(code == "0000000000000000000011011101010100") ||
	(code == "0000000000001100000011000101010100")) {
      t->GetLocation(m_camera,location,0.015);
    }
    else if ((code == "0000000000000000000011010101010100") ||
	     (code == "0000000000000000000011100001010100") ||
	     (code == "0000000000001100010011000101010100")) {
      t->GetLocation(m_camera,location,0.0225);
    }
    else if ((code == "0000000000000000000011011001010100") ||
	     (code == "0000000000000000000011100101010100") ||
	     (code == "0000000000001100100011000101010100")) {
      t->GetLocation(m_camera,location,0.03);
    }
    else { return false; }

    std::cout << m_prefix << " " << code << " ";
    std::cout << normal[0] << " " << normal[1] << " " << normal[2] << " ";
    std::cout << location[0] << " " << location[1] << " " << location[2] << " ";
    std::cout << me.GetSampleStrength() << std::endl;
    return true;
  }
};

template<class FitAlgorithm>
void process_square(MonochromeImage& m,const Camera& camera, const TagSquare<6>& tag, const char* prefix, bool regress) {
#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
  Apply(m,DrawEntityMonochrome(output));
  output.ConvertScale(0.25,190);   
#endif

  Tree<ComposedEntity<TL6(ContourEntity,ConvexHullEntity,ShapeEntity<QuadTangle>,TransformEntity,DecodeEntity<36>,MaxSampleStrengthEntity) > > tree;
  Apply(m,tree,ContourFollowerTree(tag));
  ApplyTree(tree,ConvexHull(tag));
#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output2(output);
  ApplyTree(tree,DrawEntityContour(output2));
  output2.Save("debug-02-contour.pnm");
#endif
  ApplyTree(tree,DistortionCorrectionIterative(camera,true));
  ApplyTree(tree,FitAlgorithm()); 
  if (regress) ApplyTree(tree,FitQuadTangleRegression());
#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output3(output);
  ApplyTree(tree,DrawEntityShape<QuadTangle>(output3,camera));
  output3.Save("debug-03-shape.pnm");
#endif
  ApplyTree(tree,TransformQuadTangleProjective());
  ApplyTree(tree,Bind(SampleTagSquare(tag,camera),m));
#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output4(output);
  ApplyTree(tree,DrawEntitySample(output4,camera,tag));
  output4.Save("debug-04-sample.pnm");
#endif
  ApplyTree(tree,Decode<TripOriginalCoder<36,3,2> >());
  ApplyTree(tree,TransformRotateToPayload(tag));
  ApplyTree(tree,Bind(EstimateMaxSampleStrength(tag,camera),m));
  ApplyTree(tree,RecogniseSquare(camera,prefix));
#ifdef DRAW_IMAGE
  ApplyTree(tree,DrawEntityTransform(output,camera));
  output.Save("debug-05-transform.pnm");
#endif
}

template<class FitAlgorithm,class TransformAlgorithm>
void process_circle(MonochromeImage& m,const Camera& camera, const TagCircle<2,17>& tag, const char* prefix) {

#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(m.GetWidth(),m.GetHeight());
  Apply(m,DrawEntityMonochrome(output));
  output.ConvertScale(0.25,190);   
#endif

  Tree<ComposedEntity<TL6(ContourEntity,ConvexHullEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<34>,MaxSampleStrengthEntity) > > tree;
  Apply(m,tree,ContourFollowerTree(tag));
  ApplyTree(tree,ConvexHull(tag));
#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output2(output);
  ApplyTree(tree,DrawEntityContour(output2));
  output2.Save("debug-02-contour.pnm");
#endif
  ApplyTree(tree,DistortionCorrectionIterative(camera,true));
  ApplyTree(tree,FitAlgorithm()); 
  ApplyTree(tree,RemoveNonConcentricEllipse(tag));
#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output3(output);
  ApplyTree(tree,DrawEntityShape<Ellipse>(output3,camera,ROI(minx,maxx,miny,maxy)));
  output3.Save("debug-03-shape.pnm");
#endif
  ApplyTree(tree,TransformAlgorithm(tag.GetBullseyeOuterEdge()));
  ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
  ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
#ifdef DRAW_IMAGE
  Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output4(output);
  ApplyTree(tree,DrawEntitySample(output4,camera,tag));
  output4.Save("debug-04-sample.pnm");
#endif
  ApplyTree(tree,Permute<34>());
  ApplyTree(tree,Decode<TripOriginalCoder<34,2,2> >());
  ApplyTree(tree,TransformRotateToPayload(tag));
  ApplyTree(tree,Bind(EstimateMaxSampleStrength(tag,camera),m));
  ApplyTree(tree,RecogniseCircle(camera,prefix));
#ifdef DRAW_IMAGE
  ApplyTree(tree,DrawEntityTransform(output,camera));
  output.Save("debug-05-transform.pnm");
#endif
}



float minx;
float maxx;
float miny;
float maxy;


int main(int argc,char* argv[]) {

  try {
    FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(argv[1]);
    //V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs("/dev/video0",1);
    //    IEEE1394ImageSource fs("/dev/video1394/0",0);
    //IEEE1394ImageSource fs("/dev/video1394",0,0, FRAMERATE_60,125,530 );
    Camera camera;
    // set the intrinsic parameters of the camera
    //camera.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    int width = 2614;
    float scale= 2614.f/(float)width;
    camera.SetIntrinsic( 2679.012135378859057 / scale,
			 2672.275946930749342 / scale,
			 1275.543094452055357 / scale,
			 919.780288430955466 / scale,
			 0.f);

    camera.SetRadial(-0.147572438077408,0.112655792817613,0.f);
    
   
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = fs.Next();
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());

    minx = -camera.GetPrincipleX() / camera.GetXScale();
    maxx = (fs.GetWidth()-camera.GetPrincipleX()) / camera.GetXScale();
    miny = -camera.GetPrincipleY() / camera.GetYScale();
    maxy = (fs.GetHeight()-camera.GetPrincipleY()) / camera.GetYScale();

    
    MonochromeImage m(i->GetWidth(),i->GetHeight());
    //Apply(*i,m,ThresholdAdaptive<Pix::Sze::Byte1,Pix::Fmt::Grey8>(atoi(argv[2]),atoi(argv[3])));
    Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(atoi(argv[2])));
#ifdef DRAW_IMAGE 
    Apply(m,DrawEntityMonochrome(output));
    output.Save("debug-01-monochrome.pnm");
#endif

    Apply(m,ContourFollowerClearImageBorder());
    TestSquare tag;
    tag.SetContourRestrictions(5,2,2);
    /*
    std::string prefix(argv[1]);
    prefix += ":Corner";
    process_square<FitQuadTangleCorner>(m,camera,tag,prefix.c_str(),false);

    std::string prefix2(argv[1]);
    prefix2 += ":Polygon";
    process_square<FitQuadTanglePolygon>(m,camera,tag,prefix2.c_str(),false);

    std::string prefix3(argv[1]);
    prefix3 += ":CH";
    process_square<FitQuadTangleConvexHull>(m,camera,tag,prefix3.c_str(),false);
    */
    std::string prefix4(argv[1]);
    prefix4 += ":Regress";
    process_square<FitQuadTangleConvexHull>(m,camera,tag,prefix4.c_str(),true);
    
    TestCircle inner(0.2,0.4,0.6,1);
    inner.SetContourRestrictions(5,2,2);
    std::string prefix5(argv[1]);
    prefix5 += ":InnerLS";
    process_circle<FitEllipseLS,TransformEllipseFull>(m,camera,inner,prefix5.c_str());
    /*
    std::string prefix6(argv[1]);
    prefix6 += ":InnerSimple";
    process_circle<FitEllipseSimple,TransformEllipseFull>(m,camera,inner,prefix6.c_str());
    */

    if (1==1) {
      TestCircle split(0.2,1.0,0.4,0.8);
      split.SetContourRestrictions(5,2,2);
      
      std::string prefix7(argv[1]);
      prefix7 += ":SplitLS";
      process_circle<FitEllipseLS,TransformEllipseFull>(m,camera,split,prefix7.c_str());
      /*
      std::string prefix8(argv[1]);
      prefix8 += ":SplitSimple";
      process_circle<FitEllipseSimple,TransformEllipseFull>(m,camera,split,prefix8.c_str());

      std::string prefix11(argv[1]);
      prefix11 += ":SplitSimpleLinear";
      process_circle<FitEllipseSimple,TransformEllipseLinear>(m,camera,split,prefix11.c_str());
      std::string prefix12(argv[1]);
      prefix12 += ":SplitLSLinear";
      process_circle<FitEllipseLS,TransformEllipseLinear>(m,camera,split,prefix12.c_str());
      */
    }

    if (1==1) {
      TestCircle inner(0.2,0.4,0.6,1);
      inner.SetContourRestrictions(5,2,2);

      std::string prefix7(argv[1]);
      prefix7 += ":InnerLS";
      process_circle<FitEllipseLS,TransformEllipseFull>(m,camera,inner,prefix7.c_str());
      /*
      std::string prefix8(argv[1]);
      prefix8 += ":InnerSimple";
      process_circle<FitEllipseSimple,TransformEllipseFull>(m,camera,inner,prefix8.c_str());
      std::string prefix11(argv[1]);
      prefix11 += ":InnerSimpleLinear";
      process_circle<FitEllipseSimple,TransformEllipseLinear>(m,camera,inner,prefix11.c_str());
      std::string prefix12(argv[1]);
      prefix12 += ":InnerLSLinear";
      process_circle<FitEllipseLS,TransformEllipseLinear>(m,camera,inner,prefix12.c_str());
      */
    }

    TestCircle outer(0.8,1.0,0.2,0.6);
    outer.SetContourRestrictions(5,2,2);
    std::string prefix9(argv[1]);
    prefix9 += ":OuterLS";
    process_circle<FitEllipseLS,TransformEllipseFull>(m,camera,outer,prefix9.c_str());
    /*
    std::string prefix10(argv[1]);
    prefix10 += ":OuterLS";
    process_circle<FitEllipseSimple,TransformEllipseFull>(m,camera,outer,prefix10.c_str());
    */
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

