/**
 * Representation of a Galois Field(4) Polynomial. 
 * 
 * Code almost entirely written by Ian Caulfield
 *
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/01/24 17:53:22  acr31
 * Extended TripOriginalCoder to deal with base 2 encodings.  MatrixTag
 * implementation now works.
 *
 * Revision 1.3  2004/01/23 11:49:54  acr31
 * Finished integrating the GF4 coder - I've ripped out the error correcting stuff for now and its just looking for a match but it doesn't seem to work.  I need to spend some time to understand the division and mod operations in the GF4Poly to progress
 *
 * Revision 1.2  2004/01/23 09:08:41  acr31
 * More work integrating the GF4 stuff with tripover
 *
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

