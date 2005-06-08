/**
 * $Header$
 */

#include <Total.hh>
#include <total/Entity.hh>
#include <total/ComposeEntity.hh>
#include <total/EntityTree.hh>
#include <total/ContourFollower.hh>
#include <total/algorithms/FitEllipseLS.hh>
#include <total/algorithms/FitEllipseSimple.hh>
#include <total/algorithms/FitQuadTangleCorner.hh>
#include <total/algorithms/FitQuadTangleConvexHull.hh>
#include <total/algorithms/FitQuadTanglePolygon.hh>
#include <total/algorithms/DistortionCorrection.hh>
#include <total/algorithms/TransformEllipseLinear.hh>
#include <total/algorithms/TransformEllipseFull.hh>
#include <total/algorithms/TransformQuadTangleProjective.hh>
#include <total/algorithms/TransformQuadTangleReduced.hh>
#include <total/algorithms/TransformQuadTangleSpaceSearch.hh>
#include <total/algorithms/SampleTagCircle.hh>
#include <total/algorithms/SampleTagSquare.hh>
#include <total/algorithms/ContourFollowerTree.hh>
#include <total/algorithms/Decode.hh>
#include <total/algorithms/ThresholdAdaptive.hh>
#include <total/TagCircle.hh>
#include <total/Apply.hh>

#include <iostream>

#include <total/Bind.hh>

using namespace Total;

int main(int argc,char* argv[]) {
  try {
    //    FileImageSource fs(argv[1]);
    //    V4LImageSource fs("/dev/video0",1);
    IEEE1394ImageSource fs("/dev/video1394",0);
    Camera camera;
    // set the intrinsic parameters of the camera
    camera.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    
    XOutputMechanism<Ellipse,34> o3(fs.GetWidth(),fs.GetHeight(),camera);
    
    TagCircle<2,17> tag(0.8,1.0,0.2,0.6);
    //TagSquare<5> tag;
    int cnt=0;
    while(cnt<1) {
      Image* i = fs.Next();
      Apply(*i,o3.m_ImageAlgorithm);
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      Apply(*i,m,ThresholdAdaptive(8,10));
      Apply(m,o3.m_ThresholdAlgorithm);

      Tree<ComposedEntity<TL4(ContourEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<34>)> > tree;
      Apply(m,tree,ContourFollowerTree());
      Apply(tree,o3.m_ContourAlgorithm);
      Apply(tree,DistortionCorrection(camera));
      Apply(tree,FitEllipseLS());
      Apply(tree,o3.m_ShapeAlgorithm);
      Apply(tree,TransformEllipseFull());
      Apply(tree,Bind(SampleTagCircle<2,17>(tag,camera),m));
      Apply(tree,Decode<TripOriginalCoder<34,2,2> >());
      Apply(tree,o3.m_TransformAlgorithm);
      o3.Flush();
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

