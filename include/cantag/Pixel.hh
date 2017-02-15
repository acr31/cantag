/*
  Copyright (C) 2005 Alastair R. Beresford

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

  Email: arb33@cam.ac.uk
*/

#ifndef PIXEL_GUARD
#define PIXEL_GUARD

namespace Cantag {
  
  namespace Pix {
    namespace Sze {
      enum Bpp {Byte3=0,Byte1=1,Byte4=2};
    }
    namespace Fmt {
      enum Layout {RGB24=0,BGR24=1,Grey8=2,Mono8=3,Runtime=4,YUV420=5,BGRA32=6};
    }
  }
  
  template<Pix::Fmt::Layout layout> class Pixel {};

  template<class T,Pix::Fmt::Layout l> class _PixelIterator;
  
   template<> class Pixel<Pix::Fmt::Runtime> {
     friend class _PixelIterator<Pixel*,Pix::Fmt::Runtime>;
     friend class _PixelIterator<const Pixel*,Pix::Fmt::Runtime>;
     unsigned char v[3]; 
   public:
     Pixel(unsigned char v1, unsigned char v2, unsigned char v3) {v[0]=v1;v[1]=v2;v[2]=v3;}
     inline unsigned char intensity() const {return (v[0]+v[1]+v[2])/3; }
  }; 
  
  template<> class Pixel<Pix::Fmt::RGB24> {
    friend class _PixelIterator<Pixel*,Pix::Fmt::RGB24>;
    friend class _PixelIterator<const Pixel*,Pix::Fmt::RGB24>;
    unsigned char v1;
    unsigned char v2;
    unsigned char v3;
  public:
    Pixel(unsigned char r, unsigned char g, unsigned char b) : v1(r), v2(g), v3(b) {}
    inline unsigned char intensity() const {return (v1+v2+v3)/3; }
  };

  template<> class Pixel<Pix::Fmt::BGR24> {
    friend class _PixelIterator<Pixel*,Pix::Fmt::BGR24>;
    friend class _PixelIterator<const Pixel*,Pix::Fmt::BGR24>;
    unsigned char v1;
    unsigned char v2;
    unsigned char v3;
  public:
    Pixel(unsigned char b, unsigned char g, unsigned char r) : v1(b), v2(g), v3(r) {}
    inline unsigned char intensity() const {return (v1+v2+v3)/3; }
  };

  template<> class Pixel<Pix::Fmt::BGRA32> {
      friend class _PixelIterator<Pixel*,Pix::Fmt::BGRA32>;
      friend class _PixelIterator<const Pixel*,Pix::Fmt::BGRA32>;
      unsigned char v1;
      unsigned char v2;
      unsigned char v3;
      unsigned char v4;
  public:
      Pixel(unsigned char b, unsigned char g, unsigned char r, unsigned char a) : v1(b), v2(g), v3(r), v4(r) {}
      inline unsigned char intensity() const {return (v1+v2+v3)/3; }
  };

  template<> class Pixel<Pix::Fmt::Grey8> {
    friend class _PixelIterator<Pixel*,Pix::Fmt::Grey8>;
    friend class _PixelIterator<const Pixel*,Pix::Fmt::Grey8>;
    unsigned char v1;
  public:
    Pixel(unsigned char v) : v1(v) {}
    inline bool operator<(const Pixel<Pix::Fmt::Grey8>& rhs) const { return intensity() < rhs.intensity(); }
    inline unsigned char intensity() const {return v1; }
  };

#define COLOUR_BLACK Pixel<Pix::Fmt::Grey8>(0)
#define COLOUR_WHITE Pixel<Pix::Fmt::Grey8>(255)

  template<> class Pixel<Pix::Fmt::YUV420> {
    friend class _PixelIterator<Pixel*,Pix::Fmt::YUV420>;
    friend class _PixelIterator<const Pixel*,Pix::Fmt::YUV420>;
    unsigned char v1;
    unsigned char v2;
    unsigned char v3;
  public:
    Pixel(unsigned char y, unsigned char u, unsigned char v) : v1(y), v2(u), v3(v) {}
    inline unsigned char intensity() const {return v1; }
  };

  template<> class Pixel<Pix::Fmt::Mono8> {
    friend class _PixelIterator<Pixel*,Pix::Fmt::Mono8>;
    friend class _PixelIterator<const Pixel*,Pix::Fmt::Mono8>;
    unsigned char v1;
  public:
    Pixel(unsigned char v) : v1(v) {}
  };
}

#endif//PIXEL_GUARD
