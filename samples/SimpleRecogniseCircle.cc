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
// samples.  This is then typedef'd to be of type CircleTagType which is
// used below
#include "TagDef.hh"

using namespace Cantag;

int main(int argc,char* argv[]) {

  try {
    CircleTagType tag;
    FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(argv[1]);
    Camera camera;
    camera.SetIntrinsic(fs.GetWidth(),fs.GetWidth(),fs.GetWidth()/2,fs.GetHeight()/2,0);

    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = fs.Next();
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());
    Apply(*i,DrawEntityImage(output));
    output.ConvertScale(0.25,190);
    MonochromeImage m(i->GetWidth(),i->GetHeight());
    Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
    Tree<ComposedEntity<TL4(ContourEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<CircleTagType::PayloadSize>) > > tree;
    Apply(m,tree,ContourFollowerTree(tag));
    ApplyTree(tree,DrawEntityContour(output));
    ApplyTree(tree,DistortionCorrectionSimple(camera));
    ApplyTree(tree,FitEllipseLS());
    ApplyTree(tree,TransformEllipseFull(tag.GetBullseyeOuterEdge()));
    ApplyTree(tree,TransformSelectEllipse(tag,camera));
    ApplyTree(tree,RemoveNonConcentricEllipse(tag));
    ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
    ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
    ApplyTree(tree,Decode<CircleTagType>());
    ApplyTree(tree,TransformRotateToPayload(tag));
    ApplyTree(tree,DrawEntityTransform(output,camera));
    output.Save("output.pnm");
    ApplyTree(tree,PrintEntity3DPosition<CircleTagType::PayloadSize>(std::cout,camera));
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

