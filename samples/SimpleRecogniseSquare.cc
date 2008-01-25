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

  try {
    SquareTagType tag;
    FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(argv[1]);
    Camera camera;
    camera.SetIntrinsic(fs.GetWidth(),fs.GetWidth(),fs.GetWidth()/2,fs.GetHeight()/2,0);

    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = fs.Next();
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());
    Apply(*i,DrawEntityImage(output));
    output.ConvertScale(0.25,190);
    MonochromeImage m(i->GetWidth(),i->GetHeight());
    Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
    Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<QuadTangle>,TransformEntity,DecodeEntity<SquareTagType::PayloadSize>) > > tree;
    Apply(m,ContourFollowerClearImageBorder());
    Apply(m,tree,ContourFollowerTree(tag));
    ApplyTree(tree,DrawEntityContour(output));
    ApplyTree(tree,DistortionCorrectionSimple(camera));
    ApplyTree(tree,ConvexHull(tag));
    ApplyTree(tree,FitQuadTangleConvexHull());
    ApplyTree(tree,FitQuadTangleRegression());
    ApplyTree(tree,TransformQuadTangleProjective());
    ApplyTree(tree,PrintEntityTransform(std::cout));
    ApplyTree(tree,Bind(SampleTagSquare(tag,camera),m));
    ApplyTree(tree,Decode<SquareTagType>());
    ApplyTree(tree,PrintEntityDecode<SquareTagType::PayloadSize>(std::cout));
    ApplyTree(tree,DrawEntitySample(output,camera,tag));
    ApplyTree(tree,TransformRotateToPayload(tag));
    ApplyTree(tree,DrawEntityTransform(output,camera));
    output.Save("output.pnm");
    ApplyTree(tree,PrintEntity3DPosition<SquareTagType::PayloadSize>(std::cout,camera));
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

