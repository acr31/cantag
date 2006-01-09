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
    TagType tag;
    //    tag.SetContourRestrictions(40,100,100);

    DecodeEntity<TagType::PayloadSize> d;
    DecodeEntity<TagType::PayloadSize>::Data* data = d.Add();
    data->payload.SetCode(argv[1]);
    if (!Encode<TagType>()(d)) {
      std::cerr << "Failed to encode this value. Aborting" << std::endl;
      exit(-1);
    }
    // create the image that will hold the tag design
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> i(512,512);
    if (!DrawTag(tag)(d,i)) {
      std::cerr << "Failed to draw tag. Aborting" << std::endl;
      exit(-1);
    }

    GLImageSource fs(600,600,80,i);
    Camera camera;
    fs.SetCameraParameters(camera);
    
    XDisplay<Pix::Sze::Byte1,Pix::Fmt::Grey8> o3(fs.GetWidth(),fs.GetHeight());
        
    int cnt=0;
    time_t cur_time = time(NULL);
    int count = 0;
    int angle = atoi(argv[2]);
    int direction = 5;
    while(cnt<1) {      
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = fs.Next(angle,0,0,0,2);
      if (angle >= 270 || angle <= 90) direction = -direction;
      //      angle += direction;

      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());
      Apply(*i,DrawEntityImage(output));
      output.ConvertScale(0.25,190);
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      //Apply(*i,m,ThresholdAdaptive(atoi(argv[1]),atoi(argv[2])));
      Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));

      Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<TagType::PayloadSize>) > > tree;
      Apply(m,tree,ContourFollowerTree(tag));
      //      ApplyTree(tree,DrawEntityContour(output));
      ApplyTree(tree,DistortionCorrection(camera));
      ApplyTree(tree,FitEllipseLS());
      ApplyTree(tree,TransformEllipseFull(tag.GetBullseyeOuterEdge()));
      ApplyTree(tree,TransformSelectEllipse(tag,camera));
      ApplyTree(tree,RemoveNonConcentricEllipse(tag));
      ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
      ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
      ApplyTree(tree,Decode<TagType>());
      ApplyTree(tree,TransformRotateToPayload(tag));
      //      ApplyTree(tree,DrawEntityTransform(output,camera));
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

