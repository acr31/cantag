/**
 * $Header$
 *
 */
#ifndef FILE_IMAGE_SOURCE
#define FILE_IMAGE_SOURCE

#include <Config.hh>
#include <Image.hh>
#include <ImageSource.hh>

class FileImageSource : public ImageSource {
private:
  Image* m_original;
  Image* m_buffer;

public:
  FileImageSource(char* filename);
  virtual ~FileImageSource();
  virtual void Next();
  virtual Image* GetBuffer(); 
};

#endif//FILE_IMAGE_SOURCE
