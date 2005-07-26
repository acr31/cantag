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

#include <cantag/ImageOutputMechanism.hh>

namespace Cantag {


  ImageOutputMechanism::ContourAlgorithm::ContourAlgorithm(Image& output_image): m_image(output_image) {};

  void ImageOutputMechanism::ContourAlgorithm::operator()(const ContourEntity& contour) const {
    if (contour.m_contourFitted) {
      for(std::vector<float>::const_iterator i = contour.points.begin();
	  i!=contour.points.end();
	  ++i) {
	const float x = *i;
	++i;
	const float y = *i;
	m_image.DrawPixel(x,y,COLOUR_BLACK);
      }
    }
  }
  
  ImageOutputMechanism::~ImageOutputMechanism() {
    if (m_saved_originalimage) delete m_saved_originalimage;
    if (m_saved_thresholdimage) delete m_saved_thresholdimage;
  }

  void ImageOutputMechanism::FromImageSource(const Image& image) {
    image.Save("debug-fromimagesource.pnm");
    
    if (m_saved_originalimage) delete m_saved_originalimage;
    m_saved_originalimage = new Image(image);
  }

  void ImageOutputMechanism::FromThreshold(const Image& image) {
    image.Save("debug-fromthreshold.pnm");
    
    if (m_saved_thresholdimage) delete m_saved_thresholdimage;
    m_saved_thresholdimage = new Image(image);
  }

  void ImageOutputMechanism::FromContourTree(const ContourTree& contours) {
    Image i(*m_saved_thresholdimage);
    i.Mask(1);
    i.ConvertScale(128,0);
    i.ConvertScale(-1,255);
    FromContourTree(i,contours.GetRootContour());
    i.Save("debug-fromcontourtree.pnm");
  }
  
  void ImageOutputMechanism::FromContourTree(Image& dest, const ContourTree::Contour* contour) {
    if (!contour->weeded) {
      for(std::vector<float>::const_iterator i = contour->points.begin();
	  i!=contour->points.end();
	  ++i) {
	const float x = *i;
	++i;
	const float y = *i;
	dest.DrawPixel(x,y,COLOUR_BLACK);
      }
    }
    for(std::vector<ContourTree::Contour*>::const_iterator i = contour->children.begin();
	i != contour->children.end();
	++i) {
      FromContourTree(dest,*i);
    }
  }
  
};
