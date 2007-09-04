/*
  Copyright (C) 2007 Tom Craig

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

  Email: jatc2@cam.ac.uk
*/

/**
 * $Header$
 */

#include <iostream>

#include <Cantag.hh>

#include "TagDef.hh"

using namespace Cantag;

// Problem with this: must apply after having added your AnalogueEntities to the tree
class NumberOfValidEntities : public Function<TL0, TL1(AnalogueEntity)> {
public:
    bool operator()(AnalogueEntity& entity) const {
        std::cout << "Valid" << std::endl;
        return true;
    }
};

int main(int argc,char* argv[]) {

  try {
    SquareTagType tag;
    FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(argv[1]);
    Camera camera;
    camera.SetIntrinsic(fs.GetWidth(),fs.GetWidth(),fs.GetWidth()/2,fs.GetHeight()/2,0);
    
    /*
     * Doing the distortion correction maps from camera co-ordinates (I think) into a co-ordinate frame from -1 to 1.
     * These constants will undo that transformation for the purposes of writing an image out to disk.
     */
    const float minx = -camera.GetPrincipleX() / camera.GetXScale();
    const float maxx = (fs.GetWidth()-camera.GetPrincipleX()) / camera.GetXScale();
    const float miny = -camera.GetPrincipleY() / camera.GetYScale();
    const float maxy = (fs.GetHeight()-camera.GetPrincipleY()) / camera.GetYScale();

    Tree<ComposedEntity<TL6(ContourEntity,ConvexHullEntity,ShapeEntity<QuadTangle>,TransformEntity,DecodeEntity<SquareTagType::PayloadSize>,AnalogueEntity) > > tree;
    GreyImage* i = fs.Next();
    GreyImage output(i->GetWidth(),i->GetHeight());
    
    Apply(*i,DrawEntityImage(output));
    output.ConvertScale(0.25,190);
    MonochromeImage m(i->GetWidth(),i->GetHeight());
    Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
    
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
//    ApplyTree(tree,NumberOfValidEntities());
    ApplyTree(tree,TransformRotateToPayload(tag));
    ApplyTree(tree,DrawEntitySample(output,camera,tag));
    ApplyTree(tree,Bind(DecodeAnalogue(output,camera),m));
//    ApplyTree(tree,DrawEntitySampleAnalogue(output,camera));
    ApplyTree(tree,DrawEntityTransform(output,camera));
    output.Save("output.pnm");
    
    // do some printing for debug purposes
    std::cout << "Printing convex hulls..." << std::endl;
    ApplyTree(tree,PrintEntityConvexHull(std::cout));
    std::cout << "Printing shapes..." << std::endl;
    ApplyTree(tree,PrintEntityShapeSquare(std::cout));
    std::cout << "Printing transforms..." << std::endl;
    ApplyTree(tree,PrintEntityTransform(std::cout));
    std::cout << "Printing analogue decode..." << std::endl;
    ApplyTree(tree,PrintEntityDecodeAnalogue(std::cout));
    std::cout << "Printing decode..." << std::endl;
    ApplyTree(tree,PrintEntityDecode<SquareTagType::PayloadSize>(std::cout));
    std::cout << "Printing 3D positions..." << std::endl;
    ApplyTree(tree,PrintEntity3DPosition<SquareTagType::PayloadSize>(std::cout,camera));
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

