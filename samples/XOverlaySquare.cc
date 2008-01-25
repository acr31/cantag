/*
  Copyright (C) 2006 Andrew C. Rice

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
// samples.  This is then typedef'd to be of type SquareTagType which is
// used below
#include "TagDef.hh"

using namespace Cantag;

int main(int argc,char* argv[]) {
  if (argc < 4) {
    std::cerr << argv[0] << " display_width display_height image_source [image_device]" << std::endl;
    std::cerr << "image_source: " << std::endl;
    GetImageSourceHelp(std::cerr);    
  }

  const int display_width  = atoi(argv[1]);
  const int display_height = atoi(argv[2]);
  const int image_source = atoi(argv[3]);
  char* image_device = argc == 5 ? argv[4] : NULL;

  try {
    
    SquareTagType tag;
    ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>* source = GetImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>((image_source_t)image_source,image_device);
    Camera camera;
    camera.SetIntrinsic(source->GetWidth(),source->GetWidth(),source->GetWidth()/2,source->GetHeight()/2,0);
    XDisplay<Pix::Sze::Byte1,Pix::Fmt::Grey8> g(display_width,display_height);

    while(1) {
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = source->Next();
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());
      Apply(*i,DrawEntityImage(output));
      output.ConvertScale(0.25,190);
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
      Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<QuadTangle>,TransformEntity,DecodeEntity<SquareTagType::PayloadSize>) > > tree;
      Apply(m,ContourFollowerClearImageBorder());
      Apply(m,tree,ContourFollowerTree(tag));
      ApplyTree(tree,DrawEntityContour(output));
      ApplyTree(tree,ConvexHull(tag));
      ApplyTree(tree,DistortionCorrectionSimple(camera));
      ApplyTree(tree,FitQuadTangleConvexHull());
      ApplyTree(tree,FitQuadTangleRegression());
      ApplyTree(tree,TransformQuadTangleProjective());
      ApplyTree(tree,Bind(SampleTagSquare(tag,camera),m));
      ApplyTree(tree,Decode<SquareTagType>());
      ApplyTree(tree,TransformRotateToPayload(tag));
      g.Output(output);     
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

