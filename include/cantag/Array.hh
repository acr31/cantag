/*
  Copyright (C) 2007 Tom Craig

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

  Email: jatc2@cam.ac.uk
*/

/**
 * $Header$
 */

#ifndef ARRAY_GUARD
#define ARRAY_GUARD

#include <iostream>
#include <string>

namespace Cantag {

  class CANTAG_EXPORT xOutOfRange : public std::exception
  {
  private:
    int m_index;
  public:
    xOutOfRange(int index) : m_index(index) {}
    const char* what() const throw() { std::cout << m_index << std::endl; return "Out of range"; }
  };

  template<class T, int n1> class CANTAG_EXPORT Array1
  {
  private:
    T m_vals[n1];
  protected:
    bool InRange(int i1) const { return (i1 >= 0 && i1 < n1); }
  public:
    Array1() {}
    Array1(const T* array);
    T& operator[](int i) { if (InRange(i)) return m_vals[i]; else throw xOutOfRange(i); }
    const T& operator[](int i) const { if (InRange(i)) return m_vals[i]; else throw xOutOfRange(i); }
    void Print(const std::string& separator1) const;
  };

  template<class T, int n1, int n2> class CANTAG_EXPORT Array2
  {
  private:
    Array1<T, n1> m_vals[n2];
    bool InRange(int i2) const { return (i2 >= 0 && i2 < n2); }
  public:
    Array2() {}
    Array2(const T* array);
    Array1<T, n1>& operator[](int i) { if (InRange(i)) return m_vals[i]; else throw xOutOfRange(i); }
    const Array1<T, n1>& operator[](int i) const { if (InRange(i)) return m_vals[i]; else throw xOutOfRange(i); }
    void Print(const std::string& separator1, const std::string& separator2) const;
  };

  template<class T, int n1> Array1<T, n1>::Array1(const T* array)
  {
    if (array != NULL)
      memcpy(m_vals, array, n1 * sizeof(T));
  }

  template<class T, int n1> void Array1<T, n1>::Print(const std::string& separator1) const
  {
    for (int i = 0; i < n1; i++)
      std::cout << m_vals[i] << separator1;
  }

  template<class T, int n1, int n2> Array2<T, n1, n2>::Array2(const T* array)
  {
    if (array != NULL)
      for (int i = 0; i < n2; i++)
	for (int j = 0; j < n1; j++)
	  m_vals[i][j] = array[i * n1 + j];
  }

  template<class T, int n1, int n2> void Array2<T, n1, n2>::Print(const std::string& separator1, const std::string& separator2) const
  {
    for (int i = 0; i < n2; i++)
    {
      m_vals[i].Print(separator1);
      std::cout << separator2;
    }
  }

}

#endif //ARRAY_GUARD
