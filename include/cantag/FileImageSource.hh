/**
 * $Header$
 *
 */
#ifndef FILE_IMAGE_SOURCE
#define FILE_IMAGE_SOURCE

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/ImageSource.hh>

namespace Total {
  /**
   * An image source that provides an image loaded from disk
   */
  template<Colour::Type IMTYPE> class FileImageSource : public ImageSource<IMTYPE> {
  private:
    Image<IMTYPE>* m_original;
    Image<IMTYPE>* m_buffer;

  public:
    /**
     * Create the image source, displaying the file given (jpg or bmp)
     */ 
    FileImageSource(char* filename);
    virtual ~FileImageSource();
    Image<IMTYPE>* Next();
    int GetWidth() const;
    int GetHeight() const;
  };

    template<Colour::Type IMTYPE> FileImageSource<IMTYPE>::FileImageSource(char* filename) : m_original(new Image<IMTYPE>(filename)), m_buffer(new Image<IMTYPE>(filename)) {}    
    
    template<Colour::Type IMTYPE> int FileImageSource<IMTYPE>::GetWidth() const {
	return m_original->GetWidth();
    }

    template<Colour::Type IMTYPE> int FileImageSource<IMTYPE>::GetHeight() const {
	return m_original->GetHeight();
    }

    template<Colour::Type IMTYPE> FileImageSource<IMTYPE>::~FileImageSource() { 
	if (m_buffer != NULL) {
	    delete m_buffer;
	}
    }
    
    template<Colour::Type IMTYPE> Image<IMTYPE>* FileImageSource<IMTYPE>::Next() {
	if (m_buffer != NULL) {
	    delete m_buffer;
	}
	m_buffer = new Image<IMTYPE>(*m_original);
	m_buffer->SetValid(true);
	return m_buffer;
    }

}
#endif//FILE_IMAGE_SOURCE
