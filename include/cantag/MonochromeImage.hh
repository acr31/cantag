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

#ifndef MONOCHROME_IMAGE_GUARD
#define MONOCHROME_IMAGE_GUARD

#include <cassert>
#include <cantag/entities/Entity.hh>
namespace Cantag {
  /**
   * \todo pack the data into ints
   */
  class MonochromeImage : public Entity {
  public:
    enum {
      LEFT_PIXEL = 0x1,
      CENTRE_PIXEL = 0x2,
      RIGHT_PIXEL = 0x4
    };
  private:
    unsigned int m_width;
    unsigned int m_height;
    bool* m_data;    

  public:
    MonochromeImage(unsigned int width, unsigned int height);
    ~MonochromeImage();
    void Save(const char* filename) const;

    inline bool GetPixel(unsigned int x, unsigned int y) const {
      assert(x<m_width);
      assert(y<m_height);
      return m_data[x+m_width*y];
    }

    inline bool GetPixel(int x, int y) const {
      return GetPixel((unsigned int)x,(unsigned int)y);
    }

    inline bool GetPixel(float x, float y) const {
      return GetPixel((unsigned int)x,(unsigned int)y);
    }
    
    inline void SetPixel(unsigned int x, unsigned int y, bool value) {
      m_data[x+m_width*y] = value;
    }

    inline int GetWidth() const { return m_width; }

    inline int GetHeight() const { return m_height; }

    inline int GetPixel3(unsigned int x,unsigned int y) const{
      return GetPixel(x-1,y) | (GetPixel(x,y) << 1) | (GetPixel(x+1,y)<< 2);
    }
  };
}
#endif//IMAGE_GUARD
