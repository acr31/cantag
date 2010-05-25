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
#include <cantag/imagesources/ImageSource.hh>

namespace Cantag {
  /**
   * An image source that provides an image loaded from disk
   */
  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> class FileImageSource : public ImageSource<B,L> {
  private:
    Image<B,L>* m_original;
    Image<B,L>* m_buffer;

  public:
    /**
     * Create the image source, displaying the file given (jpg or bmp)
     */ 
    FileImageSource(const char* filename);
    virtual ~FileImageSource();
    Image<B,L>* Next();
    int GetWidth() const;
    int GetHeight() const;
  };

  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> FileImageSource<B,L>::FileImageSource(const char* filename) : m_original(new Image<B,L>(filename)), m_buffer(new Image<B,L>(filename)) {}    
  
  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> int FileImageSource<B,L>::GetWidth() const {
    return m_original->GetWidth();
  }
  
  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> int FileImageSource<B,L>::GetHeight() const {
    return m_original->GetHeight();
  }
  
  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> FileImageSource<B,L>::~FileImageSource() { 
    if (m_buffer != NULL) {
      delete m_buffer;
    }
  }
  
  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> Image<B,L>* FileImageSource<B,L>::Next() {
    if (m_buffer != NULL) {
      delete m_buffer;
    }
    m_buffer = new Image<B,L>(*m_original);
    m_buffer->SetValid(true);
    return m_buffer;
  }
  
}
#endif//FILE_IMAGE_SOURCE
