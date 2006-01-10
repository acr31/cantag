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

#include <cantag/BigInt.hh>

namespace Cantag {

  BigInt::BigInt(unsigned int i) :  m_bit1(1), m_gmpint(i) {};

  BigInt::BigInt(const BigInt& bi) : m_bit1(1), m_gmpint(bi.m_gmpint) {}

  bool BigInt::operator<(const BigInt& x) const {
    return this->m_gmpint < x.m_gmpint;
  }

  bool BigInt::operator==(const BigInt& x) const {
    return this->m_gmpint == x.m_gmpint;
  }

  BigInt& BigInt::operator+=(const BigInt& x) {
    m_gmpint+= x.m_gmpint;
    return *this;
  }

  BigInt& BigInt::operator-=(const BigInt& x) {
    m_gmpint-= x.m_gmpint;
    return *this;
  }

  BigInt& BigInt::operator*=(const BigInt& x) {
    m_gmpint *= x.m_gmpint;
    return *this;
  }

  BigInt& BigInt::operator/=(const BigInt& x) {
    m_gmpint /= x.m_gmpint;
    return *this;
  }

  BigInt& BigInt::operator%=(const BigInt& x) {
    m_gmpint %= x.m_gmpint;
    return *this;
  }

  BigInt BigInt::operator%(const BigInt& x) const {
    BigInt result(*this);
    result%=x;
    return result;
  }

  BigInt& BigInt::operator|=(const BigInt& x) {
    m_gmpint |= x.m_gmpint;
    return *this;
  }

  BigInt& BigInt::operator&=(const BigInt& x) {
    m_gmpint &= x.m_gmpint;
    return *this;
  }

  BigInt& BigInt::operator^=(const BigInt& x) {
    m_gmpint ^= x.m_gmpint;
    return *this;
  }

  BigInt& BigInt::operator++() {
    m_gmpint++;
    return *this;
  }

  BigInt& BigInt::operator--() {
    m_gmpint--;
    return *this;
  }

  BigInt& BigInt::Pwr(unsigned int exponent) {
    mpz_pow_ui(m_gmpint.get_mpz_t(),m_gmpint.get_mpz_t(),exponent);
    return *this;
  }

  BigInt::operator unsigned int() const {
    return m_gmpint.get_ui();
  }

}
