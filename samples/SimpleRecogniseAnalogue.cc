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
using namespace Cantag;
#include "TagDef.hh"

const int NUM_BARS = 2;

int main(int argc,char* argv[]) {
		
	const int NUM_ARGS = 6;
	if (argc < NUM_ARGS)
	{
		std::cerr << "Usage: " << argv[0] << " filename bar1_min bar1_max bar2_min bar2_max" << std::endl;
		exit(-1);
	}
	
	char* filename = argv[1];
	const int bar1_min = atoi(argv[2]);
    const int bar1_max = atoi(argv[3]);
    const int bar2_min = atoi(argv[4]);
    const int bar2_max = atoi(argv[5]);

	try {
		
		// create tag types (digital square and analogue)
		SquareTagType digital_tag;
		boost::array<std::pair<int, int>, 2> bounds = { std::make_pair(bar1_min, bar1_max), std::make_pair(bar2_min, bar2_max) };
		TagAnalogue<NUM_BARS> analogue_tag = TagAnalogue<NUM_BARS>::CreateAnalogueTagForDebugging(bounds);
		
		FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(filename);
		Camera camera;
		camera.SetIntrinsic(fs.GetWidth(),fs.GetWidth(),fs.GetWidth()/2,fs.GetHeight()/2,0);
		
		/*
		 * Doing the distortion correction maps from camera co-ordinates (I think) into a co-ordinate frame from -1 to 1.
		 * These constants will undo that transformation for the purposes of writing an image out to disk.
		 */
//		const float minx = -camera.GetPrincipleX() / camera.GetXScale();
//		const float maxx = (fs.GetWidth()-camera.GetPrincipleX()) / camera.GetXScale();
//		const float miny = -camera.GetPrincipleY() / camera.GetYScale();
//		const float maxy = (fs.GetHeight()-camera.GetPrincipleY()) / camera.GetYScale();

		Tree<ComposedEntity<TL7(ContourEntity,ConvexHullEntity,ShapeEntity<QuadTangle>,TransformEntity,DecodeEntity<SquareTagType::PayloadSize>,SampleAnalogueEntity<NUM_BARS>,DecodeAnalogueEntity<NUM_BARS>) > > tree;
		GreyImage* i = fs.Next();
		GreyImage output(i->GetWidth(),i->GetHeight());
		
		Apply(*i,DrawEntityImage(output));
		output.ConvertScale(0.25,190);
		MonochromeImage m(i->GetWidth(),i->GetHeight());
		Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
		
		Apply(m,ContourFollowerClearImageBorder());
		Apply(m,tree,ContourFollowerTree(analogue_tag));
		ApplyTree(tree, DrawEntityContour(output));
		ApplyTree(tree, DistortionCorrectionSimple(camera));
		ApplyTree(tree, ConvexHull(analogue_tag));
		ApplyTree(tree, FitQuadTangleConvexHull());
		ApplyTree(tree, FitQuadTangleRegression());
		ApplyTree(tree, TransformQuadTangleProjective());
		ApplyTree(tree, Bind(SampleTagSquare(digital_tag, camera), m));
		ApplyTree(tree, Decode<SquareTagType>());
		ApplyTree(tree, PrintEntityDecode<SquareTagType::PayloadSize>(std::cout));
		std::cout << "Printing transforms before TransformRotateToPayload" << std::endl;
		ApplyTree(tree, PrintEntityTransform(std::cout));
		ApplyTree(tree, TransformRotateToPayload(digital_tag));
		std::cout << "Printing transforms after TransformRotateToPayload" << std::endl;
		ApplyTree(tree, PrintEntityTransform(std::cout));
		ApplyTree(tree, Bind(SampleTagAnalogue<NUM_BARS>::DefaultFunctor(output, analogue_tag, camera), m));
		ApplyTree(tree, DecodeAnalogue<NUM_BARS>(analogue_tag));
		
		// do some printing for debug purposes
		// (N.B. shouldn't really do it down here, as I believe it means we only print the entities that are valid at the end of the pipeline, rather than at each intermediate stage)
		std::cout << "Printing transforms..." << std::endl;
		ApplyTree(tree,PrintEntityTransform(std::cout));
		std::cout << "Printing analogue decode..." << std::endl;
		ApplyTree(tree,PrintEntityDecodeAnalogue<NUM_BARS>(std::cout));
//		std::cout << "Printing decode..." << std::endl;
//		ApplyTree(tree,PrintEntityDecode<SquareTagType::PayloadSize>(std::cout));
//		std::cout << "Printing 3D positions..." << std::endl;
//		ApplyTree(tree,PrintEntity3DPosition<SquareTagType::PayloadSize>(std::cout,camera));

		ApplyTree(tree, DrawEntitySample(output,camera,digital_tag));
		ApplyTree(tree, DrawEntitySampleAnalogue<NUM_BARS>(output, camera));
		ApplyTree(tree, DrawEntityTransform(output, camera));
		output.Save("output.pnm");
	}
	catch (const char* exception) {
		std::cerr << "Caught exception: " << exception<< std::endl;
	}
}

