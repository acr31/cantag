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

struct TestSquare : public Cantag::TagSquare<8>,Cantag::CRCSymbolChunkCoder<64,4,16> {

  TestSquare() : Cantag::TagSquare<8>() {}
};

#define DRAW_IMAGE

using namespace Cantag;

int main(int argc,char* argv[]) {

  try {
    FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(argv[1]);
    //V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs("/dev/video0",1);
    //    IEEE1394ImageSource fs("/dev/video1394/0",0);
    //IEEE1394ImageSource fs("/dev/video1394",0,0, FRAMERATE_60,125,530 );
    Camera camera;
    // set the intrinsic parameters of the camera
    //camera.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    int width = 2848;
    float scale= 2614.f/(float)width;
    camera.SetIntrinsic( 2679.012135378859057 / scale,
			 2672.275946930749342 / scale,
			 1275.543094452055357 / scale,
			 919.780288430955466 / scale,
			 0.f);

    camera.SetRadial(-0.147572438077408,0.112655792817613,0.f);
    
   
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = fs.Next();
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());
    
    MonochromeImage m(i->GetWidth(),i->GetHeight());
    //Apply(*i,m,ThresholdAdaptive<Pix::Sze::Byte1,Pix::Fmt::Grey8>(atoi(argv[2]),atoi(argv[3])));
    Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(atoi(argv[2])));
#ifdef DRAW_IMAGE 
    Apply(m,DrawEntityMonochrome(output));
    output.Save("debug-01-monochrome.pnm");
#endif
    TestSquare tag;
    tag.SetContourRestrictions(5,2,2);

#ifdef DRAW_IMAGE
    output.ConvertScale(0.25,190);   
#endif
    
    Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<QuadTangle>,TransformEntity,DecodeEntity<64>) > > tree;
    Apply(m,tree,ContourFollowerTree(tag));
    ApplyTree(tree,ConvexHull(tag));
#ifdef DRAW_IMAGE
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output2(output);
    ApplyTree(tree,DrawEntityContour(output2));
    output2.Save("debug-02-contour.pnm");
#endif
    ApplyTree(tree,DistortionCorrectionIterative(camera,true));
    ApplyTree(tree,FitQuadTanglePolygon()); 
    //    ApplyTree(tree,FitQuadTangleRegression());
#ifdef DRAW_IMAGE
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output3(output);
    ApplyTree(tree,DrawEntityShape<QuadTangle>(output3,camera));
    output3.Save("debug-03-shape.pnm");
#endif
    ApplyTree(tree,TransformQuadTangleSpaceSearch());
    ApplyTree(tree,Bind(SampleTagSquare(tag,camera),m));
#ifdef DRAW_IMAGE
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output4(output);
    ApplyTree(tree,DrawEntitySample(output4,camera,tag));
    output4.Save("debug-04-sample.pnm");
#endif
    ApplyTree(tree,Decode<TestSquare>());
    ApplyTree(tree,TransformRotateToPayload(tag));
#ifdef DRAW_IMAGE
    ApplyTree(tree,DrawEntityTransform(output,camera));
    output.Save("debug-05-transform.pnm");
#endif
    
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

