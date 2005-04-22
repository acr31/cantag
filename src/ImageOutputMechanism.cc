/**
 * $Header$
 */

#include <ImageOutputMechanism.hh>

namespace Total {

  ImageOutputMechanism::~ImageOutputMechanism() {
    if (m_saved_originalimage) delete m_saved_originalimage;
    if (m_saved_thresholdimage) delete m_saved_thresholdimage;
  }

  void ImageOutputMechanism::FromImageSource(const Image& image) {
    image.Save("debug-fromimagesource.pnm");
    
    if (m_saved_originalimage) delete m_saved_originalimage;
    m_saved_originalimage = new Image(image);
  };

  void ImageOutputMechanism::FromThreshold(const Image& image) {
    image.Save("debug-fromthreshold.pnm");
    
    if (m_saved_thresholdimage) delete m_saved_thresholdimage;
    m_saved_thresholdimage = new Image(image);
  };

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
