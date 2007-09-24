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

/**
 * $Header$
 */

#include <iostream>

#include <Cantag.hh>

// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type CircleTagType which is
// used below
#include "TagDef.hh"

using namespace Cantag;

struct DrawOverlay : public Function<TL0,TL1(TransformEntity)> {
  GLOutputMechanism<GlutRenderWindow>& m_m;

  DrawOverlay(GLOutputMechanism<GlutRenderWindow>& m) : m_m(m) {};
  bool operator()(TransformEntity& te) const {
    m_m.Draw(*te.GetPreferredTransform(),8,true);
    return true;
  }
};


int main(int argc,char* argv[]) {
  const int image_source   = argc == 4 ? atoi(argv[1]) : -1;
  const int display_width  = argc == 4 ? atoi(argv[2]) : -1;
  const int display_height = argc == 4 ? atoi(argv[3]) : -1;
  ImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>* source = NULL;

  try {

    CircleTagType tag;

    switch(image_source) {
    case 0:
#ifdef HAVE_LINUX_VIDEODEV_H
      source = new V4LImageSource<Pix::Sze::Byte1,Pix::Fmt::Grey8>("/dev/video0",0);
      break;
#else
      throw "Image source selection V4LImageSource is not available";
#endif
    case 1:
#ifdef HAVE_DC1394_CONTROL_H_V1
      source = new IEEE1394ImageSource("/dev/video1394/0",0,MODE_640x480_MONO, FRAMERATE_30,500,32 );      
      break;
#else
      throw "Image source selection IEEE1394ImageSource_V1 is not available";
#endif
    case 2:
#ifdef HAVE_DC1394_CONTROL_H_V2
      source = new IEEE1394ImageSource_V2(DC1394_VIDEO_MODE_640x480_MONO8, 640, 480, DC1394_FRAMERATE_30, DC1394_ISO_SPEED_400,0,10);
      break;
#else
      throw "Image source selection IEEE1394ImageSource_V2 is not available";
#endif
    case 3:
#ifdef HAVE_UEYE_H
      source = new UEyeImageSource();
      break;
#else
      throw "Image source selection UEyeImageSource is not available";
#endif
    default:
      throw "Unrecognised image source. Valid options are 0 - V4LImageSource; 1 - IEEE1394ImageSource_V1; 2 - IEEE1394ImageSource_V2; 3 - UEyeImageSource";
    }

    Camera camera;
    camera.SetIntrinsic(source->GetWidth(),source->GetWidth(),source->GetWidth()/2,source->GetHeight()/2,0);
    GLOutputMechanism<GlutRenderWindow> g(display_width,display_height,source->GetWidth(),source->GetHeight(),camera);

    time_t current_time = time(NULL);
    int fcount = 0;
    char fps_buf[255];
    sprintf(fps_buf,"FPS: ?");

    while(1) {
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* i = source->Next();
      Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> output(i->GetWidth(),i->GetHeight());
      Apply(*i,DrawEntityImage(output));
      output.ConvertScale(0.25,190);
      MonochromeImage m(i->GetWidth(),i->GetHeight());
      Apply(*i,m,ThresholdGlobal<Pix::Sze::Byte1,Pix::Fmt::Grey8>(128));
      Tree<ComposedEntity<TL4(ContourEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<CircleTagType::PayloadSize>) > > tree;
      Apply(m,ContourFollowerClearImageBorder());
      Apply(m,tree,ContourFollowerTree(tag));
      ApplyTree(tree,DrawEntityContour(output));
      ApplyTree(tree,DistortionCorrectionSimple(camera));
      ApplyTree(tree,FitEllipseLS());
      ApplyTree(tree,TransformEllipseFull(tag.GetBullseyeOuterEdge()));
      ApplyTree(tree,TransformSelectEllipse(tag,camera));
      ApplyTree(tree,RemoveNonConcentricEllipse(tag));
      ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
      ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
      ApplyTree(tree,Decode<CircleTagType>());
      ApplyTree(tree,TransformRotateToPayload(tag));
      g.Draw(output,true);

      ++fcount;
      time_t new_time = time(NULL);
      if (new_time - current_time > 0) {
	float fps = (float)fcount / (new_time - current_time);
	sprintf(fps_buf,"FPS: %.0f",fps);
	current_time = new_time;
	fcount = 0;
      }
      g.DrawText(0.9,0.95,fps_buf);

      ApplyTree(tree,DrawOverlay(g));
      
      g.Flush();

      for(std::vector<Key::Code>::const_iterator i = g.GetKeypresses().begin();i!=g.GetKeypresses().end();++i) {
	switch(*i) {
	case Key::ESC:
	  throw "Escape pressed. Exiting";
	default:
	  break;
	}
      }
    }
  }
  catch (const char* exception) {
    std::cerr << "Caught exception: " << exception<< std::endl;
    if (source != NULL) delete source;
  }
}

