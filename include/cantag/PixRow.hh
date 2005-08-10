
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

/**
 * A container class which looks like an STL vector, but is initialised with
 * a standard C++ array of pixels (array length cannot be modified).
 *
 * This file defines both a PixRow object to represent a row of pixel data
 * and an _PixelIterator class to allow the library user to access the data
 *
 * The _PixelIterator class is partially specialised on the pixel format
 * Therefore we define a _BaseIterator parent class to contain all the common
 * code which can be shared between the specialised implementations.
 *
 * Note the template parameter "Ret" in the _BaseIterator class is used to 
 * make sure the return values from the shared functions are of the correct 
 * type.
 */

#include<iostream>

namespace Cantag {

  template<class T, class Ret> class _BaseIterator {};
  
  /*********************************************************************/

  template<class T, class Ret> 
  class _BaseIterator <T*,Ret> {
  protected:
    T* m_current;
    _BaseIterator(T* ptr): m_current(ptr) {}
  public:
    T& operator*() const {
      return *m_current;
    }
    bool operator!=(_BaseIterator<T*,Ret> bi) const {
      return (bi.m_current!=m_current);
    }
    bool operator==(_BaseIterator<T*,Ret> bi) const {
      return (bi.m_current==m_current);
    }
    Ret& operator++() {
      ++m_current;
      //we know that everything of type _BaseIterator is also a _PixelIterator
      //since _BaseIterator has a protected constructor and _PixelIterator
      //is its only child.
      return static_cast<Ret&>(*this);
    }
    Ret& operator--() {
      --m_current;
      return static_cast<Ret&>(*this);
    }
    Ret& operator+(const unsigned int n) {
      m_current += n;
      return static_cast<Ret&>(*this);
    }
    int operator-(const Ret& r) {
      return m_current-r.m_current;
    }
    Ret& operator-(const unsigned int n) {
      m_current -= n;
      return static_cast<Ret&>(*this);
    }
    Ret& operator=(const Ret& val) {
      m_current = val.m_current;
      return static_cast<Ret&>(*this);
    } 
  };

  /*********************************************************************/

  template<class T, Pix::Fmt::Layout layout> class _PixelIterator
    : public _BaseIterator<T,_PixelIterator<T*,layout> > {};

  template<class T> 
  class _PixelIterator<T*,Pix::Fmt::Runtime>
    : public _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Runtime> > {
  private:
    unsigned int match[3];
    typedef _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Runtime> > self;
  public:
    _PixelIterator(T* ptr, unsigned int* m)
      : _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Runtime> >(ptr) {
      for(int i=0;i<3;i++)
	match[i]=m[i];
    }
    inline unsigned char r() const {return self::m_current->v[match[2]];}
    inline unsigned char g() const {return self::m_current->v[match[1]];}
    inline unsigned char b() const {return self::m_current->v[match[0]];}
    
    inline void r(unsigned char v) {self::m_current->v[match[2]]=v;}
    inline void g(unsigned char v) {self::m_current->v[match[1]]=v;}
    inline void b(unsigned char v) {self::m_current->v[match[0]]=v;}
  };


  template<class T> 
  class _PixelIterator<T*,Pix::Fmt::RGB24>
    : public _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::RGB24> > {
  private:
    unsigned int* match;
    typedef _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::RGB24> > self;
  public:
    _PixelIterator(T* ptr, unsigned int* m)
      : _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::RGB24> >(ptr), match(m) {}
   
    inline unsigned char r() const {return self::m_current->v1;}
    inline unsigned char g() const {return self::m_current->v2;}
    inline unsigned char b() const {return self::m_current->v3;}
    
    inline void r(unsigned char v) {self::m_current->v1=v;}
    inline void g(unsigned char v) {self::m_current->v2=v;}
    inline void b(unsigned char v) {self::m_current->v3=v;}
  };


  template<class T> 
  class _PixelIterator<T*,Pix::Fmt::BGR24>
    : public _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::BGR24> > {
  private:
    unsigned int* match;
    typedef _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::BGR24> > self;
  public:
    _PixelIterator(T* ptr, unsigned int* m)
      : _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::BGR24> >(ptr), match(m) {}
   
    inline unsigned char r() const {return self::m_current->v3;}
    inline unsigned char g() const {return self::m_current->v2;}
    inline unsigned char b() const {return self::m_current->v1;}
    
    inline void r(unsigned char v) {self::m_current->v3=v;}
    inline void g(unsigned char v) {self::m_current->v2=v;}
    inline void b(unsigned char v) {self::m_current->v1=v;}
  };


  template<class T> 
  class _PixelIterator<T*,Pix::Fmt::Grey8>
    : public _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Grey8> > {
  private:
    unsigned int* match;
    typedef _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Grey8> > self;
  public:
    _PixelIterator(T* ptr, unsigned int* m)
      : _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Grey8> >(ptr), match(m) {}
   
    inline unsigned char v() const {return self::m_current->v1;}
    
    inline void v(unsigned char val) {self::m_current->v1=val;}
  };


  template<class T> 
  class _PixelIterator<T*,Pix::Fmt::Mono8>
    : public _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Mono8> > {
  private:
    unsigned int* match;
    typedef _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Mono8> > self;
  public:
    _PixelIterator(T* ptr, unsigned int* m)
      : _BaseIterator<T*,_PixelIterator<T*,Pix::Fmt::Mono8> >(ptr), match(m) {}
   
    inline unsigned char v() const {return self::m_current->v1;}
    
    inline void v(unsigned char val) {self::m_current->v1=val;}
  };


  /*********************************************************************/


  template<Pix::Fmt::Layout layout> class PixRow {
  public:
    typedef Pixel<layout> value_type;

    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    typedef _PixelIterator<pointer,layout> iterator;
    typedef _PixelIterator<const_pointer,layout> const_iterator;

    typedef size_t size_type;

  private:
    pointer data;
    unsigned int length;
    unsigned int* match;

  public:
    PixRow(pointer d, unsigned int l) : data(d), length(l), match(0) {}
    PixRow(pointer d, unsigned int l, unsigned int m[3]) 
      : data(d), length(l), match(m) {}

    iterator begin() { return iterator(data,match); }
    const_iterator begin() const { return const_iterator(data,match); }
    iterator end() { return iterator(data+length,match); }
    const_iterator end() const { return const_iterator(data+length,match); }

    size_type size() const { return size_type(end() - begin()); }
    reference front() { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference front() const { return *begin(); }
    const_reference back() const { return *(end() - 1); }
  };
}
