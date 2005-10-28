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

#include <Cantag.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

typedef TestCircle TagType;

using namespace Cantag;

int main(int argc,char* argv[]) {

  try {
    //FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(argv[1]);
    //V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs("/dev/video0",1);
    //    IEEE1394ImageSource fs("/dev/video1394/0",0);
    IEEE1394ImageSource fs("/dev/video1394",0,0, FRAMERATE_60,125,530 );
    Camera camera;
    // set the intrinsic parameters of the camera
    camera.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    
    XDisplay<Pix::Sze::Byte1,Pix::Fmt::Grey8> o3(fs.GetWidth(),fs.GetHeight());
    
    TagType tag;
    
    int cnt=0;
    time_t cur_time = time(NULL);
    int count = 0;
    while(cnt<1) {
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = fs.Next();
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());
      Apply(*i,DrawEntityImage(output));
      output.ConvertScale(0.25,190);

      MonochromeImage m(i->GetWidth(),i->GetHeight());
      //Apply(*i,m,ThresholdAdaptive(atoi(argv[1]),atoi(argv[2])));
      Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(atoi(argv[1])));

      Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<TagType::PayloadSize>) > > tree;
      Apply(m,tree,ContourFollowerTree(tag));
      ApplyTree(tree,ConvexHull(tag));
      ApplyTree(tree,DrawEntityContour(output));
      ApplyTree(tree,DistortionCorrection(camera));
      ApplyTree(tree,FitEllipseLS()); 
      ApplyTree(tree,TransformEllipseFull(tag.GetBullseyeOuterEdge()));
      ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
      ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
      ApplyTree(tree,Decode<TagType>());
      ApplyTree(tree,TransformRotateToPayload(tag));      
      ApplyTree(tree,DrawEntityTransform(output,camera));
      o3.Output(output);
      ++count;
      if (count == 100) {
	time_t elapsed = time(NULL)-cur_time;
	float fps = 100/elapsed;
	std::cout << fps << " FPS" <<  std::endl;      
	count = 0;
	cur_time = time(NULL);
      }
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

