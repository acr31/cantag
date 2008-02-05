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
		
	const int NUM_ARGS = 7;
	if (argc < NUM_ARGS)
	{
		std::cerr << "Usage: " << argv[0] << " in_file out_file_stem bar1_min bar1_max bar2_min bar2_max" << std::endl;
		exit(-1);
	}
	
	std::string in_filename = argv[1];
	std::string out_filename_stem = argv[2];
	std::string out_filename_suffix = ".pgm";
	const int bar1_min = atoi(argv[3]);
    const int bar1_max = atoi(argv[4]);
    const int bar2_min = atoi(argv[5]);
    const int bar2_max = atoi(argv[6]);

#if 0
    // START MATRIX DEBUG
    float vals[] = { 1, 4, 7, 3, 0, 5, -1, 9, 11 };
    Matrix<float, 3, 3> m((float*) vals);
    Matrix<float, 3, 3> inv = m.Inverse();
    std::cout << "This is m(3,1): " << m.GetEntry(3, 1) << std::endl;
    std::cout << "This is m:" << std::endl;
    m.Print(std::cout);
    std::cout << "This is inv:" << std::endl;
    inv.Print(std::cout);
	float minor = m.Minor(2, 3);
    std::cout << "This is minor(2,3): " << minor << std::endl;
    std::cout << "This is m * inv:" << std::endl;
    (m * inv).Print(std::cout);
    abort();
    // END MATRIX DEBUG
#endif
    
#if 0
    // START MATRIX DEBUG
//    float vals[] = { 0.866019000000000, 0.499021000000000, 5.94435000000000e-06, 1.29628000000000,
//    	-0.500011000000000, 0.864334000000000, 8.67987000000000e-06, -0.746154000000000,
//		-8.09479000000000e-07, -1.04892000000000e-05, 0.998046000000000, 7.81041000000000,
//		0, 0, 0, 1 };
	float vals[] = { 2, 4, 1, 8, 12, 8, 2, 9, 3, 3, 2, 84, 0, 2, 50, 1 };
    Matrix<float, 4, 4> m((float*) vals);
    Matrix<float, 4, 4> inv = m.Inverse();
    std::cout << "This is m:" << std::endl;
    m.Print(std::cout);
    std::cout << "This is inv:" << std::endl;
    inv.Print(std::cout);
    std::cout << "This is m * inv:" << std::endl;
    (m * inv).Print(std::cout);
    abort();
    // END MATRIX DEBUG
#endif

#if 0
    // START Z-PROJECTION INVERSE DEBUG
//    FPoint p1(0, 0, 2), p2(0, 1, 2), p3(1, 1, 2); // the plane with z = 2 everywhere
//    FPoint testpt(3, 4, 2); // a point within the above plane - we expect it to be projected to (1.5, 2, 1)
	FPoint p1(1, 3, 6), p2(10, 15, 21), p3(28, 36, 45);
    FPoint testpt(1, 3, 6);
    FriendlyZProjectionTransform zproj(Plane(p1, p2, p3));
    FriendlyTransform* zprojinv = zproj.Clone();
    zprojinv->Invert();
    std::cout << "zproj:" << std::endl;
    zproj.Print(std::cout);
    std::cout << "*zprojinv:" << std::endl;
    zprojinv->Print(std::cout);
    testpt = zproj.Apply(testpt);
    std::cout << "Forward transforming testpt gives us " << testpt << std::endl;
    testpt = zprojinv->Apply(testpt);
    std::cout << "Inverse transforming testpt gives us " << testpt << std::endl;
    delete zprojinv;
    // END Z-PROJECTION INVERSE DEBUG
#endif

	try {
		
		// create tag types (digital square and analogue)
		SquareTagType digital_tag;
		boost::array<std::pair<int, int>, 2> bounds = { std::make_pair(bar1_min, bar1_max), std::make_pair(bar2_min, bar2_max) };
		TagAnalogue<NUM_BARS> analogue_tag = TagAnalogue<NUM_BARS>::CreateAnalogueTagForDebugging(bounds);
		
		// START BAR-TO-TAG TRANSFORM DEBUG
		FriendlyTransform* ft = analogue_tag.GetBarSpecs()[0].GetTransform().Clone();
		FriendlyTransform* fti = ft->Clone();
		fti->Invert();
		FPoint p((analogue_tag.GetBarSpecs()[0].GetMinVal() + analogue_tag.GetBarSpecs()[0].GetMaxVal()) / 2,
		         (analogue_tag.GetBarSpecs()[0].GetMinY() + analogue_tag.GetBarSpecs()[0].GetMaxY()) / 2,
		         analogue_tag.GetBarSpecs()[0].GetZ());
		std::cout << "ft is:" << std::endl;
		ft->Print(std::cout);
		std::cout << "fti is:" << std::endl;
		fti->Print(std::cout);
		std::cout << "p is " << p << std::flush;
		p = ft->Apply(p);
		std::cout << ", transformed is " << p << std::flush;
		p = fti->Apply(p);
		std::cout << ", transformed then inverted is " << p << std::endl;
		delete ft;
		delete fti;
		// END BAR-TO-TAG TRANSFORM DEBUG
		
		FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs(in_filename.c_str());
		Camera camera;
		camera.SetIntrinsic(fs.GetWidth(), fs.GetWidth(), fs.GetWidth()/2, fs.GetHeight()/2, 0);
		
		/*
		 * Doing the distortion correction maps from camera co-ordinates (I think) into a co-ordinate frame from -1 to 1.
		 * These constants will undo that transformation for the purposes of writing an image out to disk.
		 */
//		const float minx = -camera.GetPrincipleX() / camera.GetXScale();
//		const float maxx = (fs.GetWidth()-camera.GetPrincipleX()) / camera.GetXScale();
//		const float miny = -camera.GetPrincipleY() / camera.GetYScale();
//		const float maxy = (fs.GetHeight()-camera.GetPrincipleY()) / camera.GetYScale();

		Tree<ComposedEntity<TL7(ContourEntity, ConvexHullEntity, ShapeEntity<QuadTangle>, TransformEntity, DecodeEntity<SquareTagType::PayloadSize>, SampleAnalogueEntity<NUM_BARS>, DecodeAnalogueEntity<NUM_BARS>) > > tree;
		GreyImage* i = fs.Next();
		GreyImage output_image(i->GetWidth(), i->GetHeight());
		
		Apply(*i, DrawEntityImage(output_image));
		output_image.ConvertScale(0.25, 190);
		MonochromeImage m(i->GetWidth(), i->GetHeight());
		Apply(*i, m, ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
		
		Apply(m, ContourFollowerClearImageBorder());
		Apply(m, tree, ContourFollowerTree(analogue_tag));
		ApplyTree(tree, DrawEntityContour(output_image));
		ApplyTree(tree, DistortionCorrectionSimple(camera));
		ApplyTree(tree, ConvexHull(analogue_tag));
		ApplyTree(tree, FitQuadTangleConvexHull());
		ApplyTree(tree, FitQuadTangleRegression());
		ApplyTree(tree, TransformQuadTangleProjective());
		ApplyTree(tree, Bind(SampleTagSquare(digital_tag, camera), m));
//		ApplyTree(tree, DrawEntitySample(output_image, camera, digital_tag));
		ApplyTree(tree, Decode<SquareTagType>());
		ApplyTree(tree, PrintEntityDecode<SquareTagType::PayloadSize>(std::cout));
//		std::cout << "Printing and drawing transforms before TransformRotateToPayload" << std::endl;
//		GreyImage output_image_before_transform_rotate(output_image);
//		ApplyTree(tree, DrawEntityTransform(output_image_before_transform_rotate, camera));
//		ApplyTree(tree, PrintEntityTransform(std::cout));
		ApplyTree(tree, TransformRotateToPayload(digital_tag));
//		std::cout << "Printing and drawing transforms after TransformRotateToPayload" << std::endl;
//		GreyImage output_image_after_transform_rotate(output_image);
//		ApplyTree(tree, DrawEntityTransform(output_image_after_transform_rotate, camera));
//		ApplyTree(tree, PrintEntityTransform(std::cout));
		ApplyTree(tree, Bind(SampleTagAnalogue<NUM_BARS>::DefaultFunctor(output_image, analogue_tag, camera), m));
		ApplyTree(tree, DecodeAnalogue<NUM_BARS>(analogue_tag));
		ApplyTree(tree, PrintEntityDecodeAnalogue<NUM_BARS>(std::cout));
		
		// do some printing for debug purposes
		// (N.B. shouldn't really do it down here, as I believe it means we only print the entities that are valid at the end of the pipeline, rather than at each intermediate stage)
//		std::cout << "Printing transforms..." << std::endl;
//		ApplyTree(tree, PrintEntityTransform(std::cout));
//		std::cout << "Printing decode..." << std::endl;
//		ApplyTree(tree,PrintEntityDecode<SquareTagType::PayloadSize>(std::cout));
//		std::cout << "Printing 3D positions..." << std::endl;
//		ApplyTree(tree,PrintEntity3DPosition<SquareTagType::PayloadSize>(std::cout,camera));
		
		ApplyTree(tree, DrawEntitySample(output_image, camera, digital_tag));
		ApplyTree(tree, DrawEntitySampleAnalogue(output_image, camera, analogue_tag));
		ApplyTree(tree, DrawEntityTransform(output_image, camera));
		output_image.Save((out_filename_stem + out_filename_suffix).c_str());
//		output_image_before_transform_rotate.Save((out_filename_stem + "_before" + out_filename_suffix).c_str());
//		output_image_after_transform_rotate.Save((out_filename_stem + "_after" + out_filename_suffix).c_str());
	}
	catch (const char* exception) {
		std::cerr << "Caught exception: " << exception<< std::endl;
	}
}

