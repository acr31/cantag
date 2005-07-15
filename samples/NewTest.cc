/**
 * $Header$
 */

#include <iostream>

#include <Total.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"

using namespace Total;

int main(int argc,char* argv[]) {
  try {
    //FileImageSource fs(argv[1]);
    //    V4LImageSource fs("/dev/video0",1);
    IEEE1394ImageSource fs("/dev/video1394",0);
    Camera camera;
    // set the intrinsic parameters of the camera
    camera.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    
    XOutputMechanism<Ellipse,34> o3(fs.GetWidth(),fs.GetHeight(),camera);

    TagType tag;

    int cnt=0;
    time_t cur_time = time(NULL);
    int count = 0;
    while(cnt<1) {
      Image* i = fs.Next();
      Apply(*i,o3.m_ImageAlgorithm);
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      //Apply(*i,m,ThresholdAdaptive(atoi(argv[1]),atoi(argv[2])));
      Apply(*i,m,ThresholdGlobal(atoi(argv[1])));
      Apply(m,o3.m_ThresholdAlgorithm);

      Tree<ComposedEntity<TL5(ContourEntity,ConvexHullEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<TagType::PayloadSize>)> > tree;
      Apply(m,tree,ContourFollowerTree(tag));
      ApplyTree(tree,ConvexHull(tag));
      ApplyTree(tree,o3.m_ContourAlgorithm);
      ApplyTree(tree,DistortionCorrection(camera));
      ApplyTree(tree,FitEllipseLS()); // maximum fit error and error technique
      // weed out shapes that don't match
      ApplyTree(tree,o3.m_ShapeAlgorithm);
      ApplyTree(tree,TransformEllipseFull());
      // select transform
      ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
      ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
      ApplyTree(tree,Decode<TestTag>());
      ApplyTree(tree,TransformRotateToPayload(tag));
      ApplyTree(tree,o3.m_TransformAlgorithm);
      o3.Flush();
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

