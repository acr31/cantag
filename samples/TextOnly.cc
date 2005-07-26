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

#include <Total.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

#include <iostream>

using namespace Total;

int
main(int argc, char* argv[]) {

  if (argc != 2) {
    std::cerr << "Usage " << argv[0] << " imageFile" << std::endl;
    exit(-1);
  }
  try {
    // create the camera model
    Camera c;

    // set the intrinsic parameters of the camera
    c.SetIntrinsic(600,600,300,300,0);

    // create a tag
    TagType t;  

    // create an image source which will load the tag image from disk
    FileImageSource fs(argv[1]);

    // create an output mechanism which will print out our results
    TextOutputMechanism o(std::cout);
    //ImageOutputMechanism o(c);

    // STAGE1: fetch the next image from the image source (in this case there will be only one)
    Image* buffer = fs.Next();

    // tell the output mechanism to output information about the image source stage of the process
    o.FromImageSource(*buffer);

    // STAGE2: adaptive threshold the image
    buffer->AdaptiveThreshold(8,15);

    // tell the output mechanism to output information about the threshold stage
    o.FromThreshold(*buffer);

    // STAGE3: create a tree of contours from the image
    ContourTree contours(*buffer);

    // tell the output mechanism to output information about the contour following stage
    o.FromContourTree(contours);

    // STAGE4: remove the camera distortion from the contours by transforming the contour tree
    contours.ImageToNPCF(c);

    // tell the output mechanism to output information about the remove distortion stage
    o.FromRemoveIntrinsic(contours);

    // STAGE5: create a tree of matched shapes from the contour tree
    ShapeTree<TagType::TagShapeType> shaped(*contours.GetRootContour());

    // tell the output mechanism to output information about the shape tree stage
    o.FromShapeTree(shaped);

    // create the worldstate object which will store possible tags
    WorldState<TagType::TagPayloadSize> world;

    // STAGE6: walk the tree of matched shapes finding 3d
    // transformations suitable for reading the tag data
    t.WalkGraph(shaped.GetRootNode(),&world,c,*buffer);

    // STAGE7: attempt to read valid codes from the possible tags in the
    // world state
    t.DecodeTags(&world,c,*buffer);

    // tell the output mechanism to output information about the tags found
    o.FromTag(world);
  }
  catch (const char* message) {
    std::cerr << "Caught exception (aborting): " << message << std::endl;
  }
}
