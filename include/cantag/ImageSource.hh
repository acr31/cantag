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
#ifndef IMAGE_SOURCE_GUARD
#define IMAGE_SOURCE_GUARD

#include <cantag/Config.hh>
#include <cantag/Image.hh>

namespace Cantag {
  /**
   * Encapsulate a source of input images
   */
  template<Pix::Sze::Bpp B, Pix::Fmt::Layout L> class ImageSource {
  public:
    typedef Image<B,L> ImageType;
    static const Pix::Sze::Bpp PixSze = B;
    static const Pix::Fmt::Layout PixFmt = L;

    /**
     * Get the next frame.  This method may or may not invalidate the
     * current pointer to the buffer.
     */
    virtual Image<B,L>* Next() = 0;
    
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
  };
  
}
#endif//IMAGE_SOURCE_GUARD
