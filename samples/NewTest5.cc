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


struct TestSquare : public Cantag::TagSquare<6>,Cantag::CRCSymbolChunkCoder<36,6,9> {
  TestSquare() : Cantag::TagSquare<6>() {}
};
/*
struct TestSquare : public Cantag::TagSquare<5>,Cantag::CRCSymbolChunkCoder<24,4,6> {
  TestSquare() : Cantag::TagSquare<5>() {}
};
*/
using namespace Cantag;

enum option_types_t {
  DISPLAYMODE = 0,
  THRESHOLD,
  DISTORTION,
  SHAPEFIT,
  REGRESSION,
  TRANSFORM
};


enum option_t {
  DISPLAYMODE_NORMAL,
  DISPLAYMODE_THRESHOLD,
  DISPLAYMODE_CONTOUR,
  DISPLAYMODE_UNDISTORTEDCONTOUR,
  DISPLAYMODE_SHAPE,
  DISPLAYMODE_TRANSFORM,
  THRESHOLD_GLOBAL,
  THRESHOLD_ADAPTIVE,
  DISTORTION_NONE,
  DISTORTION_SIMPLE,
  DISTORTION_ITERATIVE,
  SHAPEFIT_CORNER,
  SHAPEFIT_CONVEXHULL,
  SHAPEFIT_POLYGON,
  REGRESSION_ON,
  REGRESSION_OFF,
  TRANSFORM_PROJECTIVE,
  TRANSFORM_CYBERCODE,
  TRANSFORM_SPACESEARCH
};

struct Setting {
  option_t current_option;
  int last_set;

  Setting(option_t default_option) : current_option(default_option), last_set(0) {};

  inline int GetR(int current) const {
    if (current - last_set> 1) return 0; else return 255;
  }

  inline int GetG(int current) const {
    return 0;
  }

  inline int GetB(int current) const {
    return 0;
  }
  
};

struct InterpretTags : public Function<TL0,TL1(DecodeEntity<TestSquare::PayloadSize>)> {
  std::map<CyclicBitSet<TestSquare::PayloadSize>,std::pair<option_t,Setting*> >& m_map;
  int m_current;
  InterpretTags(std::map<CyclicBitSet<TestSquare::PayloadSize>,std::pair<option_t,Setting*> >& map, int current) : m_map(map), m_current(current) {};
  bool operator()(DecodeEntity<TestSquare::PayloadSize>& de) const {
    CyclicBitSet<TestSquare::PayloadSize>& payload = (*(de.GetPayloads().begin()))->payload;
    if (m_map.find(payload) != m_map.end()) {
      std::pair<option_t,Setting*>& pair = m_map[payload];
      pair.second->current_option = pair.first;
      pair.second->last_set = m_current;
    }
    return true;
  }
};

struct DrawAll : public Function<TL1(TransformEntity),TL1(DecodeEntity<TestSquare::PayloadSize>)> {
  GLOutputMechanism& m_m;
  const std::map<CyclicBitSet<TestSquare::PayloadSize>,std::pair<option_t,Setting*> >& m_map;
  mutable bool m_found;
  DrawAll(GLOutputMechanism& m, const std::map<CyclicBitSet<TestSquare::PayloadSize>,std::pair<option_t,Setting*> >& map) : m_m(m),m_map(map), m_found(false) {};
  bool operator()(const TransformEntity& te,DecodeEntity<TestSquare::PayloadSize>& de) const {
    const CyclicBitSet<TestSquare::PayloadSize>& code = (*(de.GetPayloads().begin()))->payload;
    if (m_map.find((*(de.GetPayloads().begin()))->payload) != m_map.end()) {
      Transform t(*te.GetPreferredTransform());
      m_m.Draw(t,8,true);
    }
    else {
      unsigned long cnt = code.to_ulong();
      m_m.Draw(*te.GetPreferredTransform(),cnt % 8,true);
    }
    return true;
  }
};

#define ADD(x,y,z) tag_map[CyclicBitSet<TestSquare::PayloadSize>( x )] = std::pair<option_t,Setting*>( y , z )

int main(int argc,char* argv[]) {

  try {
    std::map<CyclicBitSet<TestSquare::PayloadSize>,std::pair<option_t,Setting*> > tag_map;
    Setting settings[] = { Setting(DISPLAYMODE_NORMAL),
			   Setting(THRESHOLD_GLOBAL),
			   Setting(DISTORTION_NONE),
			   Setting(SHAPEFIT_CORNER),
			   Setting(REGRESSION_OFF),
			   Setting(TRANSFORM_PROJECTIVE) };
    
    ADD("000000000000000000000000000000000000",DISPLAYMODE_NORMAL,&settings[DISPLAYMODE]);
    ADD("000000000000000000000000000000000001",DISPLAYMODE_THRESHOLD,&settings[DISPLAYMODE]);
    ADD("000000000000000000000000000000000010",DISPLAYMODE_CONTOUR,&settings[DISPLAYMODE]);
    ADD("000000000000000000000000000000000011",DISPLAYMODE_UNDISTORTEDCONTOUR,&settings[DISPLAYMODE]);
    ADD("000000000000000000000000000000000100",DISPLAYMODE_SHAPE,&settings[DISPLAYMODE]);
    ADD("000000000000000000000000000000000101",DISPLAYMODE_TRANSFORM,&settings[DISPLAYMODE]);
    
    ADD("000000000000000000000000000000000110",THRESHOLD_GLOBAL,&settings[THRESHOLD]);
    ADD("000000000000000000000000000000000111",THRESHOLD_ADAPTIVE,&settings[THRESHOLD]);
    
    ADD("000000000000000000000000000000001000",DISTORTION_NONE,&settings[DISTORTION]);
    ADD("000000000000000000000000000000001001",DISTORTION_SIMPLE,&settings[DISTORTION]);
    ADD("000000000000000000000000000000001010",DISTORTION_ITERATIVE,&settings[DISTORTION]);
    
    ADD("000000000000000000000000000000001011",SHAPEFIT_CORNER,&settings[SHAPEFIT]);
    ADD("000000000000000000000000000000001100",SHAPEFIT_CONVEXHULL,&settings[SHAPEFIT]);
    ADD("000000000000000000000000000000001101",SHAPEFIT_POLYGON,&settings[SHAPEFIT]);
    
    ADD("000000000000000000000000000000001110",REGRESSION_ON,&settings[REGRESSION]);
    ADD("000000000000000000000000000000001111",REGRESSION_OFF,&settings[REGRESSION]);
    
    ADD("000000000000000000000000000000010000",TRANSFORM_PROJECTIVE,&settings[TRANSFORM]);
    ADD("000000000000000000000000000000010001",TRANSFORM_CYBERCODE,&settings[TRANSFORM]);
    ADD("000000000000000000000000000000010010",TRANSFORM_SPACESEARCH,&settings[TRANSFORM]);

    IEEE1394ImageSource fs("/dev/video1394",0,MODE_640x480_MONO, FRAMERATE_30,500,32 );
    //V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs("/dev/video0",0);
    TestSquare tag;
    tag.SetContourRestrictions(25,10,10);

    Camera camera;
    //camera.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    //    camera.SetIntrinsic(640,480,320,240,0);
    camera.SetIntrinsic(924,576,462,288,0);
    camera.SetRadial(-0.147572438077408,0.112655792817613,0.f);
    
    GLOutputMechanism g(atoi(argv[1]),atoi(argv[2]),fs.GetWidth(),fs.GetHeight());
    //GLOutputMechanism g(fs.GetWidth(),fs.GetHeight(),fs.GetWidth(),fs.GetHeight());
   
    Transform t;
    time_t current_time = time(NULL);
    int fcount = 0;
    char fps_buf[255];
    sprintf(fps_buf,"FPS: ?");

    while(true) {
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = fs.Next();

      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* output;
      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_THRESHOLD) {
	output = new Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>(i->GetWidth(),i->GetHeight());
      }
      else {
	output = new Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>(*i);
	output->ConvertScale(0.25,190); 
      }

      MonochromeImage m(i->GetWidth(),i->GetHeight());

      switch(settings[THRESHOLD].current_option) {
      case THRESHOLD_GLOBAL:
	Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
	break;
      case THRESHOLD_ADAPTIVE:
	Apply(*i,m,ThresholdAdaptive<Pix::Sze::Byte1,Pix::Fmt::Grey8>(atoi(argv[3]),atoi(argv[4])));
      }

      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_THRESHOLD) { 
	Apply(m,DrawEntityMonochrome(*output));
	output->ConvertScale(0.25,190);
      }

      Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<QuadTangle>,TransformEntity,DecodeEntity<TestSquare::PayloadSize>) > > tree;
      Apply(m,tree,ContourFollowerTree(tag));
      
      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_CONTOUR) { 
	ApplyTree(tree,DrawEntityContour(*output));
      }

      if (settings[SHAPEFIT].current_option == SHAPEFIT_CONVEXHULL) {
	ApplyTree(tree,ConvexHull(tag));
      }

      switch(settings[DISTORTION].current_option) {
      case DISTORTION_NONE:
      case DISTORTION_SIMPLE:
	ApplyTree(tree,DistortionCorrection(camera));
	break;
      case DISTORTION_ITERATIVE:
	ApplyTree(tree,DistortionCorrectionIterative(camera,true));
      }

      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_UNDISTORTEDCONTOUR) { 
	ApplyTree(tree,DrawEntityContour(*output,ROI(-0.5,0.5,-0.5,0.5)));
      }
      
      switch(settings[SHAPEFIT].current_option) {
      case SHAPEFIT_CORNER:
	ApplyTree(tree,FitQuadTangleCorner()); 
	break;
      case SHAPEFIT_CONVEXHULL:
	ApplyTree(tree,FitQuadTangleConvexHull()); 
	break;
      case SHAPEFIT_POLYGON:
	ApplyTree(tree,FitQuadTanglePolygon());        
      }

      if (settings[REGRESSION].current_option == REGRESSION_ON) {
	ApplyTree(tree,FitQuadTangleRegression());
      }

      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_SHAPE) { 
	ApplyTree(tree,DrawEntityShape<QuadTangle>(*output,camera));
      }
      
      switch(settings[TRANSFORM].current_option) {
      case TRANSFORM_PROJECTIVE:
	ApplyTree(tree,TransformQuadTangleProjective());
	break;
      case TRANSFORM_SPACESEARCH:
	ApplyTree(tree,TransformQuadTangleSpaceSearch());
	break;
      case TRANSFORM_CYBERCODE: 
	ApplyTree(tree,TransformQuadTangleCyberCode());
      }

      ApplyTree(tree,Bind(SampleTagSquare(tag,camera),m));
      ApplyTree(tree,Decode<TestSquare>());

      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_TRANSFORM) { 
	ApplyTree(tree,DrawEntityTransform(*output,camera));
      }

      ApplyTree(tree,TransformRotateToPayload(tag));
      g.Draw(*output,true);
      delete output;
      ApplyTree(tree,DrawAll(g,tag_map));

      time_t new_time = time(NULL);
      float y = -0.45;

      switch (settings[THRESHOLD].current_option) {
      case THRESHOLD_GLOBAL:
	g.DrawText(-0.47,y,"ThresholdGlobal",settings[THRESHOLD].GetR(new_time), settings[THRESHOLD].GetG(new_time), settings[THRESHOLD].GetB(new_time)); y += 0.05;
	break;
      case THRESHOLD_ADAPTIVE:
	g.DrawText(-0.47,y,"ThresholdAdaptive",settings[THRESHOLD].GetR(new_time), settings[THRESHOLD].GetG(new_time), settings[THRESHOLD].GetB(new_time)); y += 0.05;
      }

      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_THRESHOLD) {
	g.DrawText(-0.47,y,"DrawEntityMonochrome",settings[DISPLAYMODE].GetR(new_time), settings[DISPLAYMODE].GetG(new_time), settings[DISPLAYMODE].GetB(new_time)); y += 0.05;
      }

      g.DrawText(-0.47,y,"ContourFollowerTree"); y += 0.05;

      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_CONTOUR) {
	g.DrawText(-0.47,y,"DrawEntityContour",settings[DISPLAYMODE].GetR(new_time), settings[DISPLAYMODE].GetG(new_time), settings[DISPLAYMODE].GetB(new_time)); y += 0.05;
      }
      if (settings[SHAPEFIT].current_option == SHAPEFIT_CONVEXHULL) {
	g.DrawText(-0.47,y,"ConvexHull",settings[SHAPEFIT].GetR(new_time), settings[SHAPEFIT].GetG(new_time), settings[SHAPEFIT].GetB(new_time)); y += 0.05;
      }
      switch (settings[DISTORTION].current_option) {
      case DISTORTION_NONE:
      case DISTORTION_SIMPLE:
	g.DrawText(-0.47,y,"DistortionCorrection",settings[DISTORTION].GetR(new_time), settings[DISTORTION].GetG(new_time), settings[DISTORTION].GetB(new_time)); y += 0.05;
	break;
      case DISTORTION_ITERATIVE:
	g.DrawText(-0.47,y,"DistortionCorrectionIterative",settings[DISTORTION].GetR(new_time), settings[DISTORTION].GetG(new_time), settings[DISTORTION].GetB(new_time)); y += 0.05;
      }
      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_UNDISTORTEDCONTOUR) {
	g.DrawText(-0.47,y,"DrawEntityContour",settings[DISPLAYMODE].GetR(new_time), settings[DISPLAYMODE].GetG(new_time), settings[DISPLAYMODE].GetB(new_time)); y += 0.05;
      }
      switch (settings[SHAPEFIT].current_option) {
      case SHAPEFIT_CORNER:
	g.DrawText(-0.47,y,"FitQuadTangleCorner",settings[SHAPEFIT].GetR(new_time), settings[SHAPEFIT].GetG(new_time), settings[SHAPEFIT].GetB(new_time)); y += 0.05;
	break;
      case SHAPEFIT_POLYGON:
	g.DrawText(-0.47,y,"FitQuadTanglePolygon",settings[SHAPEFIT].GetR(new_time), settings[SHAPEFIT].GetG(new_time), settings[SHAPEFIT].GetB(new_time)); y += 0.05;
	break;
      case SHAPEFIT_CONVEXHULL:
	g.DrawText(-0.47,y,"FitQuadTangleConvexHull",settings[SHAPEFIT].GetR(new_time), settings[SHAPEFIT].GetG(new_time), settings[SHAPEFIT].GetB(new_time)); y += 0.05;
      }
      if (settings[REGRESSION].current_option == REGRESSION_ON) {
	g.DrawText(-0.47,y,"FitQuadTangleRegression",settings[REGRESSION].GetR(new_time), settings[REGRESSION].GetG(new_time), settings[REGRESSION].GetB(new_time)); y += 0.05;
      }
      if (settings[DISPLAYMODE].current_option == DISPLAYMODE_SHAPE) {
	g.DrawText(-0.47,y,"DrawEntityShape",settings[DISPLAYMODE].GetR(new_time), settings[DISPLAYMODE].GetG(new_time), settings[DISPLAYMODE].GetB(new_time)); y += 0.05;
      }
      switch (settings[TRANSFORM].current_option) { 
      case TRANSFORM_PROJECTIVE:
	g.DrawText(-0.47,y,"TransformQuadTangleProjective",settings[TRANSFORM].GetR(new_time), settings[TRANSFORM].GetG(new_time), settings[TRANSFORM].GetB(new_time)); y += 0.05;
	break;
      case TRANSFORM_SPACESEARCH:
	g.DrawText(-0.47,y,"TransformQuadTangleSpaceSearch",settings[TRANSFORM].GetR(new_time), settings[TRANSFORM].GetG(new_time), settings[TRANSFORM].GetB(new_time)); y += 0.05;
	break;
      case TRANSFORM_CYBERCODE:
	g.DrawText(-0.47,y,"TransformQuadTangleCyberCode",settings[TRANSFORM].GetR(new_time), settings[TRANSFORM].GetG(new_time), settings[TRANSFORM].GetB(new_time)); y += 0.05;
      }
      g.DrawText(-0.47,y,"SampleTagSquare"); y += 0.05;
      g.DrawText(-0.47,y,"Decode"); y += 0.05;
      if (settings[DISPLAYMODE].current_option == TRANSFORM) {
	g.DrawText(-0.47,y,"DrawEntityTransform",settings[DISPLAYMODE].GetR(new_time), settings[DISPLAYMODE].GetG(new_time), settings[DISPLAYMODE].GetB(new_time)); y += 0.05;
      }
      g.DrawText(-0.47,y,"TransformRotateToPayload"); y += 0.05;


      ApplyTree(tree,InterpretTags(tag_map,new_time));

      ++fcount;
      if (new_time - current_time > 0) {
	float fps = (float)fcount / (new_time - current_time);
	sprintf(fps_buf,"FPS: %.0f",fps);
	current_time = new_time;
	fcount = 0;
      }
      g.DrawText(0.39,0.47,fps_buf);

      g.Flush();
      //      exit(-1);
    }  
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}
