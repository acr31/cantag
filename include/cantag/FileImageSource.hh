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
 *
 */
#ifndef FILE_IMAGE_SOURCE
#define FILE_IMAGE_SOURCE

#include <cantag/Config.hh>
#include <cantag/Image.hh>
#include <cantag/ImageSource.hh>

namespace Cantag {
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
