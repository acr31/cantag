/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/29 17:34:13  acr31
 * another image source
 *
 */
#ifndef GRAYSCALE_IMAGE_SOURCE
#define GRAYSCALE_IMAGE_SOURCE

#include "Drawing.hh"
#include "ImageSource.hh"
#include "Config.hh"

class GrayScaleImageSource : public ImageSource {
private:
  Image* m_original;
  Image* m_buffer;

public:
  GrayScaleImageSource(Image *original);
  virtual ~GrayScaleImageSource();
  virtual void Next();
  virtual Image* GetBuffer(); 
};

#endif//GRAYSCALE_IMAGE_SOURCE
