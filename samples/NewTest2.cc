/**
 * $Header$
 */

#include <iostream>

#include <Total.hh>

using namespace Total;

int main(int argc,char* argv[]) {
    try {
      FileImageSource fs(argv[1]);
    //    V4LImageSource fs("/dev/video0",1);
//    IEEE1394ImageSource fs("/dev/video1394",0);
    Camera camera;
    // set the intrinsic parameters of the camera
    camera.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    
    XOutputMechanism<Ellipse,34> o3(fs.GetWidth(),fs.GetHeight(),camera);
    
    TagCircle<2,17> tag(0.8,1.0,0.2,0.6);
    //TagSquare<5> tag;
    int cnt=0;
    time_t cur_time = time(NULL);
    int count = 0;
    int interval = 100;
    while(cnt<1) {
	Image* i = fs.Next();
      Apply(*i,o3.m_ImageAlgorithm);
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      //      Apply(*i,m,ThresholdAdaptive(atoi(argv[1]),atoi(argv[2])));
      Apply(*i,m,ThresholdGlobal(atoi(argv[2])));
      Apply(m,o3.m_ThresholdAlgorithm);

      Tree<ContourEntity> contour_tree;
      Apply(m,contour_tree,ContourFollowerTree(tag));
      Tree<ConvexHullEntity> convex_tree;
      ApplyTree(contour_tree,convex_tree,ConvexHull(tag)); 
      ApplyTree(contour_tree,o3.m_ContourAlgorithm);
      ApplyTree(contour_tree,DistortionCorrection(camera));
      Tree<ShapeEntity<Ellipse> > shape_tree;
      ApplyTree(contour_tree,shape_tree,FitEllipseLS()); 
      ApplyTree(shape_tree,o3.m_ShapeAlgorithm);
      Tree<TransformEntity> transform_tree;
      ApplyTree(shape_tree,transform_tree,TransformEllipseFull());
      Tree<DecodeEntity<34> > decode_tree;
      ApplyTree(transform_tree,decode_tree,Bind(SampleTagCircle<2,17>(tag,camera),m));
      ApplyTree(decode_tree,Decode<TripOriginalCoder<34,2,2> >());
//      Apply(transform_tree,decode_tree,decode_tree,o3.m_TransformAlgorithm);
      o3.Flush();
      ++count;
      if (count == interval) {
	time_t elapsed = time(NULL)-cur_time;
	if (elapsed != 0) {
	    float fps = 100/elapsed;
	    std::cout << fps << " FPS" <<  std::endl;      
	}
	else {
	    interval *=2;
	}
	count = 0;
	cur_time = time(NULL);
      }
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}
