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
    Image<Colour::Grey> i(512,512);
    if (!DrawTag(tag)(d,i)) {
      std::cerr << "Failed to draw tag. Aborting" << std::endl;
      exit(-1);
    }

    GLImageSource fs(600,600,80,i);
    Camera camera;
    fs.SetCameraParameters(camera);
    
    XOutputMechanism<Ellipse,34> o3(fs.GetWidth(),fs.GetHeight(),camera);
    
    
    int cnt=0;
    time_t cur_time = time(NULL);
    int count = 0;
    int angle = 90;
    float distance = 2;
    float direction = 0.1;
    while(cnt<1) {      
      Image<Colour::Grey>* i = fs.Next(angle++,0,0,0,2);
      if (angle == 270) angle = 90;
      distance += direction;
      if (distance >= 5) direction = -0.1;
      if (distance <= 2) direction = 0.1 ;
      //      i->Save("tmp1.ppm");
      //std::cout << "** ImageAlgorithm" << std::endl;
      Apply(*i,o3.m_ImageAlgorithm);
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      //Apply(*i,m,ThresholdAdaptive(atoi(argv[1]),atoi(argv[2])));
      //      std::cout << "** ThresholdGlobal" << std::endl;
      Apply(*i,m,ThresholdGlobal(128));
      //      m.Save("tmp2.ppm");

      //      std::cout << "** ThresholdAlgorithm" << std::endl;
      Apply(m,o3.m_ThresholdAlgorithm);

      Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<TagType::PayloadSize>)> > tree;
      //      std::cout << "** ContourFollowerTree" << std::endl;
      Apply(m,tree,ContourFollowerTree(tag));
      //      ApplyTree(tree,ConvexHull(tag));
      //      std::cout << "** ContourAlgorithm" << std::endl;
      ApplyTree(tree,o3.m_ContourAlgorithm);
      //m.Save("tmp3.ppm");
      //      std::cout << "** DistortionCorrection" << std::endl;
      ApplyTree(tree,DistortionCorrection(camera));
      //      std::cout << "** FitEllipse" << std::endl;
      ApplyTree(tree,FitEllipseLS()); // maximum fit error and error technique
      //      std::cout << "** RemoveNonConcentric" << std::endl;
      // weed out shapes that don't match
      //      std::cout << "** ShapeAlgorithm" << std::endl;
      ApplyTree(tree,o3.m_ShapeAlgorithm);
      //      std::cout << "** TransformEllipse" << std::endl;
      ApplyTree(tree,TransformEllipseFull());
      ApplyTree(tree,TransformSelectEllipse(tag,camera));
      ApplyTree(tree,RemoveNonConcentricEllipse(tag));
      //m.Save("tmp4.ppm");
      // select transform
      ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
      ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
      ApplyTree(tree,Decode<TagType>());
      ApplyTree(tree,TransformRotateToPayload(tag));
      o3.Flush();
      ApplyTree(tree,o3.m_TransformAlgorithm);
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

