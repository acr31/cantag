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

#ifndef BIGINT_GUARD
#define BIGINT_GUARD

#include <cantag/Config.hh>

#ifndef HAVE_GMPXX_H
#error "This version has been configured without GNU MP Bignum C++ library"
#endif

#ifndef HAVELIB_GMP
#error "This file requires libgmp.so"
#endif

#ifndef HAVELIB_GMPXX
#error "This file required libgmpxx.so"
#endif

#include <cantag/CyclicBitSet.hh>

#include <gmpxx.h>
#include <gmp.h>

#include <bitset>

namespace Cantag {

  /**
   * Arbitrary precision big integer using a given backing store.
   * Currently the destructor must be called to force the thing to flush
   * back to the backing store.
   *
   * \todo Change the implementation so it actually uses the backing store
   */
  class CANTAG_EXPORT BigInt  {
  private:

    const mpz_class m_bit1;
    mpz_class m_gmpint;

  public:
    BigInt(unsigned int i);
    template<int SIZE> BigInt(const CyclicBitSet<SIZE>& t);
    template<int SIZE> BigInt(CyclicBitSet<SIZE>& t);
  public:
    BigInt(const BigInt& bi);

    bool operator<(const BigInt& x) const;
    bool operator==(const BigInt& x) const;

    BigInt& operator+=(const BigInt& x);
    BigInt& operator-=(const BigInt& x);
    BigInt& operator*=(const BigInt& x);
    BigInt& operator/=(const BigInt& x);
    BigInt& operator%=(const BigInt& x);
    BigInt& operator|=(const BigInt& x);
    BigInt& operator&=(const BigInt& x);
    BigInt& operator^=(const BigInt& x);

    BigInt& operator++();
    BigInt& operator--();

    BigInt operator%(const BigInt& x) const;


    /**
     * Raise this BigInt to the given power.
     */
    BigInt& Pwr(unsigned int exponent);

    operator unsigned int() const;

    template<int SIZE> void ToCyclicBitSetDestructive(CyclicBitSet<SIZE>& t);
  
    friend std::ostream& operator<<(std::ostream& s, const BigInt& z);
  };

  inline std::ostream& operator<<(std::ostream& s, const BigInt& z) {
    s << z.m_gmpint;
    return s;
  }

  template<int SIZE> BigInt::BigInt(const CyclicBitSet<SIZE>& t) : m_bit1(1) , m_gmpint(0) {
    for(int i=SIZE-1;i>=0;i--) {
      m_gmpint <<= 1;
      if (t[i]) m_gmpint |= m_bit1;
    }
  }

  template<int SIZE> BigInt::BigInt(CyclicBitSet<SIZE>& t) : m_bit1(1), m_gmpint(0) {
    for(int i=SIZE-1;i>=0;i--) {
      m_gmpint <<= 1;
      if (t[i]) m_gmpint |= m_bit1;
    }
  }

  template<int SIZE> void BigInt::ToCyclicBitSetDestructive(CyclicBitSet<SIZE>& t)  {
    for(int i=0;i<SIZE;i++) {
      t.Set(i,((m_gmpint & m_bit1) == m_bit1 ? true : false));
      m_gmpint >>= 1;
    }
  }
  
}



#endif
