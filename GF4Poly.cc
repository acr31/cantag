/**
 * Representation of a Galois Field(4) Polynomial. 
 * 
 * Code almost entirely written by Ian Caulfield
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/22 12:02:10  acr31
 * added parts of Ian Caulkins GF4 coding.  Fixed a bug in the weight function (should be +=2 not ++) Need to finish off GF4Coder so it actually does something
 *
 *
 */
#include "GF4Poly.hh"

GF4Poly GF4Poly::operator +(const GF4Poly& rhs) const
{
  GF4Poly res;

  res.val = val ^ rhs.val;

  return res;
}

GF4Poly& GF4Poly::operator +=(const GF4Poly& rhs)
{
  val ^= rhs.val;

  return *this;
}

GF4Poly GF4Poly::operator *(const GF4Poly& rhs) const
{
  GF4Poly res;

  unsigned long long int x = rhs.val;
  unsigned long long int y, m;

  int i = 0;

  while (x) {
    y = 0;

    if (x & 2) {
      y = val << 1;
      y &= 0xaaaaaaaaaaaaaaaa;
      m = val & 0xaaaaaaaaaaaaaaaa;
      m |= m >> 1;
      y ^= m;
    }

    if (x & 1)
      y ^= val;

    res.val ^= y << (2*i);

    i++;
    x >>= 2;
  }

  return res;
}

GF4Poly& GF4Poly::operator *=(const GF4Poly& rhs)
{
  *this = *this * rhs;

  return *this;
}

GF4Poly GF4Poly::operator %(const GF4Poly& rhs) const
{
  GF4Poly res, mul;

  int shift = 0;
  int order = 0;

  order = rhs.order();
 
  mul = rhs * GF4Poly((rhs.val >> (order * 2 )) == 1 ? 1 : (rhs.val >> (order * 2 )) ^ 1);

  res.val = val;
 
  shift = res.order();

  while (shift >= order) {
    res += (mul << (shift - order)) * GF4Poly(res.val >> (shift * 2));
      
    while (!((res.val >> (shift * 2)) || shift == 0))
      shift--;
  }

  return res;
}

GF4Poly& GF4Poly::operator %=(const GF4Poly& rhs)
{
  return *this = *this % rhs;
}

GF4Poly GF4Poly::operator /(const GF4Poly& rhs) const
{
  GF4Poly res, mul, rem;

  int shift = 0;
  int order = 0;

  order = rhs.order();
 
  mul = GF4Poly((rhs.val >> (order * 2 )) == 1 ? 1 : (rhs.val >> (order * 2 )) ^ 1);

  rem.val = val;
 
  shift = rem.order();

  while (shift >= order) {
    res += (mul << (shift - order)) * GF4Poly(rem.val >> (shift * 2));
    rem += rhs * (mul << (shift - order)) * GF4Poly(rem.val >> (shift * 2));

    while (!((rem.val >> (shift * 2)) || shift == 0))
      shift--;
  }

  return res;
}

GF4Poly& GF4Poly::operator /=(const GF4Poly& rhs)
{
  return *this = *this / rhs;
}

GF4Poly GF4Poly::operator <<(const unsigned int shift) const
{
  GF4Poly res;

  res.val = val << (shift * 2);

  return res;
}

GF4Poly& GF4Poly::operator <<=(const unsigned int shift)
{
  val <<= (shift * 2);

  return *this;
}

GF4Poly GF4Poly::operator >>(const unsigned int shift) const
{
  GF4Poly res;

  res.val = val >> (shift * 2);

  return res;
}

GF4Poly& GF4Poly::operator >>=(const unsigned int shift)
{
  val >>= (shift * 2);

  return *this;
}

GF4Poly& GF4Poly::rotate(unsigned int rotation, unsigned int length)
{
  unsigned long long int t;
  unsigned long long int m = 1;

  m <<= length * 2;
  m -= 1;

  t = val << (rotation * 2);

  t |= val >> ((length - rotation) * 2);

  t &= m;

  val = t;

  return *this;
}

unsigned int GF4Poly::order() const
{
  int res = 0;
 
  while (val >> ((res + 1) * 2))
    res ++;

  return res;
}

unsigned int GF4Poly::weight() const
{
  int res = 0;

  for (int i = 0; i < 32; i+=2)
    if ((val >> i) & 3)
      res++;

  return res;
}

GF4Poly::operator unsigned long long int() const
{
  return val;
}

GF4Poly::operator string() const
{
  unsigned long long int x;

  string res;

  x = val;

  while (x) {
    res = (char) ((x & 3) + '0') + res;

    x >>= 2;
  }

  return res;
}
