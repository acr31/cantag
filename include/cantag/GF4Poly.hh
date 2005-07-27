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
 * Representation of a Galois Field(4) Polynomial. 
 * 
 * Code almost entirely written by Ian Caulfield
 *
 * $Header$
 *
 * This representation is good for an order 31 polynomial with GF4
 * co-efficients (2 bits each)
 *
 * \todo incorporate this representation in the general GF(2^s) stuff the cbc20 wrote
 */
#include <string>

namespace Cantag {

  class GF4Poly
  {
  public:
    GF4Poly() : val(0) {};
    GF4Poly(unsigned long long int a) : val(a) {};

    inline GF4Poly& operator =(const GF4Poly& rhs)
    { val = rhs.val; return *this; }

    GF4Poly operator +(const GF4Poly&) const;
    GF4Poly& operator +=(const GF4Poly&);
    GF4Poly operator +(int) const;
    GF4Poly& operator +=(int);
    GF4Poly operator *(const GF4Poly&) const;
    GF4Poly& operator *=(const GF4Poly&);
    GF4Poly operator %(const GF4Poly&) const;
    GF4Poly& operator %=(const GF4Poly&);
    GF4Poly operator /(const GF4Poly&) const;
    GF4Poly& operator /=(const GF4Poly&);
    GF4Poly operator <<(const unsigned int) const;
    GF4Poly& operator <<=(const unsigned int);
    GF4Poly operator >>(const unsigned int) const;
    GF4Poly& operator >>=(const unsigned int);

    GF4Poly pwr(unsigned int, const GF4Poly&) const;

    inline bool operator ==(const unsigned long long int rhs) const
    { return val == rhs; };
    inline bool operator ==(const GF4Poly& rhs) const
    { return val == rhs.val; };
    inline bool operator !=(const unsigned long long int rhs) const
    { return val != rhs; };
    inline bool operator !=(const GF4Poly& rhs) const
    { return val != rhs.val; };
    inline bool operator >(const GF4Poly& rhs) const
    { return val > rhs.val; };
    inline bool operator >=(const GF4Poly& rhs) const
    { return val >= rhs.val; };
    inline bool operator <(const GF4Poly& rhs) const
    { return val < rhs.val; };
    inline bool operator <=(const GF4Poly& rhs) const
    { return val <= rhs.val; };

    operator unsigned long long int() const;

    GF4Poly& Rotate(unsigned int rotation, unsigned int length);

    unsigned int Order() const;
    unsigned int Weight() const;
    unsigned int GetTerm(int order) const;
    unsigned long long int GetValue() const;

  private:
    unsigned long long int val;
  };
}
