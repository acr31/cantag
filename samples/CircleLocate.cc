/*
  Copyright (C) 2006 Andrew C. Rice

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


#include <Cantag.hh>

struct TestCircle : public Cantag::TagCircleInner<2,20>,Cantag::TripOriginalCoder<40,2,6> {};

using namespace Cantag;


int main(int argc,char* argv[]) {

  try {

    //typedef IEEE1394ImageSource ImageSource ;
    //IEEE1394ImageSource fs("/dev/video1394",0,MODE_640x480_MONO, FRAMERATE_30,500,32 );
    //IEEE1394ImageSource fs("/dev/video1394",0,MODE_640x480_MONO, FRAMERATE_30,500,32 );
    //typedef V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> ImageSource ;
    //V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs("/dev/video0",0);
    //V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> fs("/dev/video0",0);
    //typedef DSVLImageSource ImageSource ;
    //DSVLImageSource fs(argv[5]);
    typedef UEyeImageSource ImageSource;
    UEyeImageSource fs;
    //    typedef FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8> ImageSource ;
    //    FileImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>fs("processing-original.bmp");
    TestCircle tag;
    tag.SetContourRestrictions(25,10,10);

    Camera camera;
    camera.SetIntrinsic(fs.GetWidth(),fs.GetWidth(),fs.GetWidth()/2,fs.GetHeight()/2,0);
    float radial1 = -0.246574651979379;
    float radial2 = 0.103141587733613;
    float radial3 = -0.000374087230627;
    camera.SetRadial(radial1,radial2,radial3);
        
    GLOutputMechanism<GlutRenderWindow> g(atoi(argv[1]),atoi(argv[2]),fs.GetWidth(),fs.GetHeight(),camera);
   
    time_t current_time = time(NULL);
    int fcount = 0;
    char fps_buf[255];
    sprintf(fps_buf,"FPS: ?");
    int counter = 0;
    int global_threshold = 128;

    char thresh_buf[255];
    sprintf(thresh_buf,"Threshold: %d",global_threshold);
    bool show_threshold = true;

    const float minx = -camera.GetPrincipleX() / camera.GetXScale();
    const float maxx = (fs.GetWidth()-camera.GetPrincipleX()) / camera.GetXScale();
    const float miny = -camera.GetPrincipleY() / camera.GetYScale();
    const float maxy = (fs.GetHeight()-camera.GetPrincipleY()) / camera.GetYScale();

    
    while(true) {
      ImageSource::ImageType* i = fs.Next();
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* output;
      output = new Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>(i->GetWidth(),i->GetHeight());
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      
      Apply(*i,m,ThresholdGlobal<ImageSource::PixSze,ImageSource::PixFmt>(global_threshold));
      
      if (show_threshold) {
	Apply(m,DrawEntityMonochrome(*output));
	output->ConvertScale(0.25,190);
      }
      
      Apply(m,ContourFollowerClearImageBorder());
      
      Tree<ComposedEntity<TL4(ContourEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<TestCircle::PayloadSize>) > > tree;
      Apply<ContourFollowerTree>(m,tree,ContourFollowerTree(tag));

      ApplyTree(tree,DistortionCorrectionIterative(camera,true));
      //      ApplyTree(tree,DrawEntityContour(*output,ROI(minx,maxx,miny,maxy)));
      ApplyTree(tree,FitEllipseLS()); 
      ApplyTree(tree,CheckEllipseGradient<AggregateMax<float> >(tag));
      ApplyTree(tree,RemoveNonConcentricEllipse(tag));
      ApplyTree(tree,TransformEllipseFull(tag.GetBullseyeOuterEdge()));
      ApplyTree(tree,TransformSelectEllipseErrorOfFit<CheckEllipseGradient<AggregateMax<float> > >(tag,camera));
      ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
      ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
      ApplyTree(tree,Decode<TestCircle>());
      ApplyTree(tree,TransformRotateToPayload(tag));
      ApplyTree(tree,DrawEntityTransform(*output,camera));
      ApplyTree(tree,DrawEntitySample(*output,camera,tag));
      ApplyTree(tree,PrintEntity3DPosition<TestCircle::PayloadSize>(std::cout,camera));
      g.Draw(*output,true);
      delete output;
	  
      time_t new_time = time(NULL);
      float y = -0.45;

      ++fcount;
      if (new_time - current_time > 0) {
	float fps = (float)fcount / (new_time - current_time);
	sprintf(fps_buf,"FPS: %.0f",fps);
	current_time = new_time;
	fcount = 0;
      }
	  
      g.DrawText(0.05,0.95,fps_buf);
      g.DrawText(0.05,0.92,thresh_buf);

      g.Flush();

      for(std::vector<Key::Code>::const_iterator i = g.GetKeypresses().begin();i!=g.GetKeypresses().end();++i) {
	switch(*i) {
	case Key::ESC:
	  exit(-1);
	case Key::V:
	  show_threshold = !show_threshold;
	  break;
	case Key::A:
	  global_threshold++;
	  sprintf(thresh_buf,"Threshold: %d",global_threshold);
	  break;
	case Key::Z:
	  global_threshold--;
	  sprintf(thresh_buf,"Threshold: %d",global_threshold);
	  break;
	}
      }
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
  }
}

