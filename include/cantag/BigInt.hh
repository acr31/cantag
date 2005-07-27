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

  template<int SIZE> class BigInt;

  template<int T> std::ostream& operator<<(std::ostream& s, const BigInt<T>& z) {
    s << z.m_gmpint;
    return s;
  }
  /**
   * Arbitrary precision big integer using a given backing store.
   * Currently the destructor must be called to force the thing to flush
   * back to the backing store.
   *
   * \todo Change the implementation so it actually uses the backing store
   */
  template<int SIZE>
  class BigInt  {
  private:

    CyclicBitSet<SIZE> *m_backing_store;
    mpz_class m_bit1;
    mpz_class m_gmpint;

  public:
    BigInt(unsigned int i);
    BigInt(const CyclicBitSet<SIZE>& t);
    BigInt(CyclicBitSet<SIZE>& t);
    BigInt(const BigInt& bi);
    ~BigInt();

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
  
    friend std::ostream& operator<< <>(std::ostream& s, const BigInt<SIZE>& z);
  };

  template<int SIZE> BigInt<SIZE>::BigInt(unsigned int i) :  m_backing_store(NULL), m_bit1(1), m_gmpint(i) {};

  template<int SIZE> BigInt<SIZE>::BigInt(const CyclicBitSet<SIZE>& t) : m_backing_store(NULL), m_bit1(1) , m_gmpint(0) {
    for(int i=SIZE-1;i>=0;i--) {
      m_gmpint <<= 1;
      if (t[i]) {
	m_gmpint |= m_bit1;
      }
    }
  }

  template<int SIZE> BigInt<SIZE>::BigInt(CyclicBitSet<SIZE>& t) : m_backing_store(&t), m_bit1(1), m_gmpint(0) {
    for(int i=SIZE-1;i>=0;i--) {
      m_gmpint <<= 1;
      if (t[i]) m_gmpint |= m_bit1;
    }
  }

  template<int SIZE> BigInt<SIZE>:: BigInt(const BigInt& bi) :m_backing_store(NULL) , m_bit1(1), m_gmpint(bi.m_gmpint) {}

  template<int SIZE> BigInt<SIZE>::~BigInt() {
    if (m_backing_store) {
      for(int i=0;i<SIZE;i++) {
	(*m_backing_store)[i] = ((m_gmpint & m_bit1) == m_bit1 ? true : false);
	m_gmpint >>= 1;
      }
    }
  }

  template<int SIZE> bool BigInt<SIZE>::operator<(const BigInt& x) const {
    return this->m_gmpint < x.m_gmpint;
  }

  template<int SIZE> bool BigInt<SIZE>::operator==(const BigInt& x) const {
    return this->m_gmpint == x.m_gmpint;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator+=(const BigInt<SIZE>& x) {
    m_gmpint+= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator-=(const BigInt<SIZE>& x) {
    m_gmpint-= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator*=(const BigInt<SIZE>& x) {
    m_gmpint *= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator/=(const BigInt<SIZE>& x) {
    m_gmpint /= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator%=(const BigInt<SIZE>& x) {
    m_gmpint %= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE> BigInt<SIZE>::operator%(const BigInt<SIZE>& x) const {
    BigInt result(*this);
    result%=x;
    return result;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator|=(const BigInt<SIZE>& x) {
    m_gmpint |= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator&=(const BigInt<SIZE>& x) {
    m_gmpint &= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator^=(const BigInt<SIZE>& x) {
    m_gmpint ^= x.m_gmpint;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator++() {
    m_gmpint++;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::operator--() {
    m_gmpint--;
    return *this;
  }

  template<int SIZE> BigInt<SIZE>& BigInt<SIZE>::Pwr(unsigned int exponent) {
    mpz_pow_ui(m_gmpint.get_mpz_t(),m_gmpint.get_mpz_t(),exponent);
    return *this;
  }

  template<int SIZE> BigInt<SIZE>::operator unsigned int() const {
    return m_gmpint.get_ui();
  }
}



#endif
