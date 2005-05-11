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
    GLOutputMechanism o(argc, argv,v.GetWidth(),v.GetHeight());

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
    }
  }
  catch (const char* message) {
    std::cout << "Caught exception (aborting): " << message << std::endl;
  }
}
  
