/**
 * $Header$
 */

#ifndef BIGINT_GUARD
#define BIGINT_GUARD

#include <gmpxx.h>
#include <gmp.h>
#include <bitset>
#include <boost/operators.hpp>
#include <CyclicBitSet.hh>
/**
 * Arbitrary precision big integer using a given backing store.
 * Currently the destructor must be called to force the thing to flush
 * back to the backing store.
 *
 * \todo Change the implementation so it actually uses the backing store
 */
template<int SIZE>
class BigInt : boost::operators< BigInt<SIZE> > {
private:

  CyclicBitSet<SIZE> *m_backing_store;
  mpz_class m_bit1;

public:
  mpz_class m_gmpint;
  BigInt(unsigned int i) : m_gmpint(i), m_backing_store(NULL), m_bit1(1) {}

  BigInt(const CyclicBitSet<SIZE>& t) : m_gmpint(0), m_backing_store(NULL), m_bit1(1) {
    for(int i=SIZE-1;i>=0;i--) {
      m_gmpint <<= 1;
      if (t[i]) {
	m_gmpint |= m_bit1;
      }
    }
  }

  BigInt(CyclicBitSet<SIZE>& t) : m_gmpint(0), m_backing_store(&t), m_bit1(1) {
    for(int i=SIZE-1;i>=0;i--) {
      m_gmpint <<= 1;
      if (t[i]) m_gmpint |= m_bit1;
    }
  }

  ~BigInt() {
    if (m_backing_store) {
      for(int i=0;i<SIZE;i++) {
	(*m_backing_store)[i] = ((m_gmpint & m_bit1) == m_bit1 ? true : false);
	m_gmpint >>= 1;
      }
    }
  }

  BigInt(const BigInt& bi) : m_gmpint(bi.m_gmpint), m_backing_store(NULL), m_bit1(1) {}

  bool operator<(const BigInt& x) const {
    return this->m_gmpint < x.m_gmpint;
  }

  bool operator==(const BigInt& x) const {
    return this->m_gmpint == x.m_gmpint;
  }

  BigInt& operator+=(const BigInt& x) {
    m_gmpint+= x.m_gmpint;
    return *this;
  }

  BigInt& operator-=(const BigInt& x) {
    m_gmpint-= x.m_gmpint;
    return *this;
  }

  BigInt& operator*=(const BigInt& x) {
    m_gmpint *= x.m_gmpint;
    return *this;
  }

  BigInt& operator/=(const BigInt& x) {
    m_gmpint /= x.m_gmpint;
    return *this;
  }

  BigInt& operator%=(const BigInt& x) {
    m_gmpint %= x.m_gmpint;
    return *this;
  }

  BigInt& operator|=(const BigInt& x) {
    m_gmpint |= x.m_gmpint;
    return *this;
  }

  BigInt& operator&=(const BigInt& x) {
    m_gmpint &= x.m_gmpint;
    return *this;
  }

  BigInt& operator^=(const BigInt& x) {
    m_gmpint ^= x.m_gmpint;
    return *this;
  }

  BigInt& operator++() {
    m_gmpint++;
    return *this;
  }

  BigInt& operator--() {
    m_gmpint--;
    return *this;
  }

  /**
   * Raise this BitInt to the given power.
   */
  BigInt& Pwr(unsigned int exponent) {
    mpz_pow_ui(m_gmpint.get_mpz_t(),m_gmpint.get_mpz_t(),exponent);
    return *this;
  }

  operator unsigned int() const {
    return m_gmpint.get_ui();
  }

};

template<int T> std::ostream& operator<<(std::ostream& s, const BigInt<T>& z) {
  s << z.m_gmpint;
  return s;
}

#endif
