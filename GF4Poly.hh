/**
 * Representation of a Galois Field(4) Polynomial. 
 * 
 * Code almost entirely written by Ian Caulfield
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/22 12:02:11  acr31
 * added parts of Ian Caulkins GF4 coding.  Fixed a bug in the weight function (should be +=2 not ++) Need to finish off GF4Coder so it actually does something
 *
 *
 *
 * This representation is good for an order 31 polynomial with GF4
 * co-efficients (2 bits each)
 */
#include <string>

class GF4Poly
{
 public:
  GF4Poly() : val(0) {};
  GF4Poly(unsigned long long int a) : val(a) {};

  inline GF4Poly& operator =(const GF4Poly& rhs)
    { val = rhs.val; return *this; }

  GF4Poly operator +(const GF4Poly&) const;
  GF4Poly& operator +=(const GF4Poly&);
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
  operator string() const;

  GF4Poly& rotate(unsigned int rotation, unsigned int length);

  unsigned int order() const;
  unsigned int weight() const;

 private:
  unsigned long long int val;
};

