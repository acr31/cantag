#ifndef FILE_IMAGE_SOURCE
#define FILE_IMAGE_SOURCE

#include "Drawing.hh"
#include "ImageSource.hh"
#include "Config.hh"

class GrayScaleFileImageSource : ImageSource {
private:
  Image* m_original;
  Image* m_buffer;

public:
  GrayScaleFileImageSource(char *filename);
  virtual ~GrayScaleFileImageSource();
  virtual void Next();
  virtual Image* GetBuffer(); 
};

#endif//FILE_IMAGE_SOURCE
