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

#include <Cantag.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

#include <iostream>

using namespace Cantag;

int
main(int argc, char* argv[]) {
  try {

    // create the camera model
    Camera c;

    // set the intrinsic parameters of the camera
    c.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    
    // create a tag
    TagType t;

    // create an image source which will load the tag data from /dev/video0
    V4LImageSource v("/dev/video0",1);
    
    // create an output mechanism which will display the decoding
    // process on screen
    XOutputStagesMechanism o(v.GetWidth(),v.GetHeight(),c);
    //GLOutputMechanism o(argc,argv,v.GetWidth(),v.GetHeight());
    
    // start a timer to measure FPS
    time_t cur_time = time(NULL);
    int count = 0;

    while(true) {

      // STAGE1: fetch the next image from the image source.
      Image* buffer = v.Next();

      // tell the output mechanism to output information about the
      // image source stage of the process
      o.FromImageSource(*buffer);

      // STAGE2: adaptive threshold the image
      buffer->AdaptiveThreshold(8,15);

      // tell the output mechanism to output information about the
      // threshold stage
      o.FromThreshold(*buffer);

      // STAGE3: create a tree of contours from the image
      ContourTree contours(*buffer);

      // tell the output mechanism to output information about the
      // contour following stage
      o.FromContourTree(contours);

      // STAGE4: remove the camera distortion from the contours by
      // transforming the contour tree
      contours.ImageToNPCF(c);

      // tell the output mechanism to output information about the
      // remove distortion stage
      o.FromRemoveIntrinsic(contours);

      // STAGE5: create a tree of matched shapes from the contour tree
      ShapeTree<TagType::TagShapeType> shaped(*contours.GetRootContour());

      // tell the output mechanism to output information about the
      // shape tree stage
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

      // tell the output mechanism that we have finished this frame
      o.Flush();

      count ++;
      if (count == 100) {
	time_t elapsed = time(NULL)-cur_time;
	float fps = 100/elapsed;
	std::cout << fps << " FPS" <<  std::endl;      
	count = 0;
	cur_time = time(NULL);
      }
    }
  }
  catch (const char* message) {
    std::cout << "Caught exception (aborting): " << message << std::endl;
  }
}
  
