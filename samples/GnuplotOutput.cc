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

#include <Total.hh>
#include <iostream>

#include "TagDef.hh"

using namespace Total;

/**
 * Simply pipe the output of this program to 
 * GNUPLOT to see positions plotted in real time
 * Eg.
 * GnuplotOutput | gnuplot
 *
 */


int
main(int argc, char* argv[]) {
  try {

    // create the camera model
    Camera c;

    // set the intrinsic parameters of the camera
    //    c.SetIntrinsic(1284.33,1064.55,450.534, 321.569,0 );
    c.SetIntrinsic(967,970,375.40,299.31,0);
    c.SetRadial(-0.15809,-0.19769,0.0);

    // create a tag
    TagType t;

    // create an image source which will load the tag data from /dev/video0
    V4LImageSource v("/dev/video0",1);
    
    // create an output mechanism which will display the decoding
    // process on screen
    //     XOutputStagesMechanism *o = new XOutputStagesMechanism(v.GetWidth(),v.GetHeight(),c);
    XOutputStagesMechanism *o = NULL;
    

    // Output some suitable GNUPLOT preamble
    std::cout << "set term x11"<< std::endl;
    std::cout  << "set xlabel \"x\""<< std::endl;
    std::cout  << "set ylabel \"y\""<< std::endl;
    std::cout  << "set zlabel \"z\""<< std::endl;

    // These ranges can be anything you like, but it gives
    // a better result if you set them to suitable values here
    std::cout  << "set xrange[-10:10]"<< std::endl;
    std::cout  << "set yrange[-10:10]"<< std::endl;
    std::cout  << "set zrange[0:30]"<< std::endl;
    std::cout  << "set grid ztics"<< std::endl;
    std::cout  << "set grid ytics"<< std::endl;
    std::cout  << "set grid xtics"<< std::endl;
    std::cout  << "set title \"Total GnuplotOutput\"" << std::endl;

    while(true) { 
      // Run through the decoding stages
      Image* buffer = v.Next();
      if (o!=NULL) o->FromImageSource(*buffer);
      buffer->AdaptiveThreshold(8,15);
      if (o!=NULL) o->FromThreshold(*buffer);
      ContourTree contours(*buffer);
      if (o!=NULL) o->FromContourTree(contours);
      contours.ImageToNPCF(c);
      ShapeTree<TagType::TagShapeType> shaped(*contours.GetRootContour());
      WorldState<TagType::TagPayloadSize> world;
      t.WalkGraph(shaped.GetRootNode(),&world,c,*buffer);
      t.DecodeTags(&world,c,*buffer);
      if (o!=NULL) o->FromTag(world);
      if (o!=NULL) o->Flush();

      // Now loop over the located objects
      std::vector<LocatedObject<TagType::TagPayloadSize>*> lo = world.GetNodes();
      if (lo.size()>0) {
	int started=0;
	for (unsigned int i=0; i<lo.size(); i++) {
	  if (lo[i]->tag_codes.size()>0) {
	    // Here's a tag that decoded well
	    if (!started) {
	      std::cout << "splot \"-\" ps 5" << std::endl;
	      started=1;
	    }
	    std::cout << lo[i]->location[0] << " " << lo[i]->location[1] << " " << lo[i]->location[2] << std::endl;
	  }
	}
	if (started) std::cout << "e" << std::endl;
      }
    }
  }
  catch (const char* message) {
    std::cout << "Caught exception (aborting): " << message << std::endl;
  }
}
