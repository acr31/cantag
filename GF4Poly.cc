/**
 * Representation of a Galois Field(4) Polynomial. - i.e. polynomials over Z_2 modulo x^2+x+1
 * 
 * Code almost entirely written by Ian Caulfield
 * Comments entirely by Andy Rice <acr31@cam.ac.uk> ;-)
 *
 * $Header$
 *
 * $Log$
 * Revision 1.6  2004/01/24 17:53:22  acr31
 * Extended TripOriginalCoder to deal with base 2 encodings.  MatrixTag
 * implementation now works.
 *
 1.5  2004/01/23 11:59:25  acr31
 * removed my attempt at the Order function because it causes triptest to spin forever
 *
 * Revision 1.4  2004/01/23 11:57:07  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
 * Revision 1.3  2004/01/23 11:49:54  acr31
 * Finished integrating the GF4 coder - I've ripped out the error correcting stuff for now and its just looking for a match but it doesn't seem to work.  I need to spend some time to understand the division and mod operations in the GF4Poly to progress
 *
 * Revision 1.2  2004/01/23 09:08:40  acr31
 * More work integrating the GF4 stuff with tripover
 *
 * Revision 1.1  2004/01/22 12:02:10  acr31
 * added parts of Ian Caulkins GF4 coding.  Fixed a bug in the weight function (should be +=2 not ++) Need to finish off GF4Coder so it actually does something
 *
 *
 */
#include "GF4Poly.hh"


/**
 * This representation has two bits for each of the elements of the field
 *
 * 00 = 0
 * 01 = 1
 * 10 = x
 * 11 = 1+x
 */


/**
 * Addition corresponds to xor
 *
 * Note: 1+1 = x+x = x+1 + x+1 = 0 i.e. addition == subtraction
 * Note: there is never any carry so xor == addition
 *
 *
 * We have x(x+1)=1 and so
 *
 * 0+0 = 0
 * 0+1 = 1
 * 0+x = x
 * 0+1+x = 1+x
 *
 * 1+1 = 0
 * 1+x = 1+x
 * 1+1+x = 1+1+x+x-x = (1+x)(1+1)-x = -x = x
 *
 * x+x = x(1+1) = 0
 * x+1+x = x+1+x+1-1 = (1+x)(1+1)-1 = -1 = 1
 *
 * 1+x+1+x = (1+x)(1+1) = 0
 *
 *
 *       | 0     1    x   x+1
 *       | 00   01   10   11
 * ------+---------------------
 * 0   00| 00   01   10   11
 * 1   01| 01   00   11   10  
 * x   10| 10   11   00   01
 * x+1 11| 11   10   01   00
 *
 */
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

/* add a constant to the polynomial */
GF4Poly GF4Poly::operator +(int i) const
{

  GF4Poly res;

  res.val = val ^ (i&3);

  return res;
}

GF4Poly& GF4Poly::operator +=(int i)
{
  val ^= (i&3);

  return *this;
}


/**
 * Multiplication is tricky
 *
 * 0*0 = 0
 * 0*1 = 0
 * 0*x = 0
 * 0+(x+1) = 0
 *
 * 1*1 = 1
 * 1*x = x
 * 1*(x+1) = x+1
 *
 * x*x = x*x+x-x = x(x+1)- x = 1-x = 1+x
 * x(x+1) = 1
 *
 * (x+1)(x+1) = x*x + x + x + 1 = 1+x + x + 1+x = x
 *
 *       | 0     1    x   x+1
 *       | 00   01   10   11
 * ------+---------------------
 * 0   00| 00   00   00   00
 * 1   01| 00   01   10   11  
 * x   10| 00   10   11   01
 * x+1 11| 00   11   01   10
 *
 *
 * y*0 = 0
 * y*1 =           y
 * y*x =       x
 * y*(x+1) = y*x + y
 *
 * So - we work out how to add y (using xor) and we work out how to
 * multiply by y and then we do the former, the later, or both
 * depending on whether there it is *1, or *x, or *(x+1) respectively
 *
 * It just so happens that to multiply by x you do the following:
 *
 * 1) shift the operand left by one place (throwing away the MSB) - now
 * the former LSB is in the place of the MSB and the LSB is now 0
 * 2) take another copy of the operand and copy the MSB onto the LSB
 * 3) xor (add) these two together
 *
 * We have all of our coefficients in the same bit field so we for
 * step 1 we need to mask out the MSB of each operand and then shift
 * left - the old MSB (now 0) becomes the LSB of the next operand.
 * This can be done by shifting and then masking the pattern with
 * 0xa.... (16 a's infact) which is 1010 (16 more times) in binary.
 *
 * For step 2 we mask with the same pattern and then OR the result
 * with itself shifted right one place.
 *
 * The routine below goes through each term of x multiplying it by y
 * and then adding it to the total
 */
GF4Poly GF4Poly::operator *(const GF4Poly& rhs) const
{
  GF4Poly res;

  unsigned long long int x = rhs.val;
  unsigned long long int y, m;

  int i = 0;

  while (x) {
    y = 0;

    if (x & 2) { // do we have an x term
      y = val << 1;
      y &= 0xaaaaaaaaaaaaaaaa;
      m = val & 0xaaaaaaaaaaaaaaaa;
      m |= m >> 1;
      y ^= m;
    }

    if (x & 1)  // do we have a +1 term
      y ^= val;

    res.val ^= y << (2*i);  // add this to the running total

    i++;
    x >>= 2;  // move on to the next term in x
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

  unsigned int shift = 0;
  unsigned int order = 0;

  order = rhs.Order();
 
  /**
   * multiplicative inverse of 1 is 1, of x is x+1 and of x+1 is x
   */
  mul = rhs * GF4Poly(

		      (rhs.val >> (order * 2 )) == 1 ? 
		      1 : 
		      (rhs.val >> (order * 2 )) ^ 1);
  /**
   * Now mul is a multiple of rhs with highest co-efficient == 1
   * because we have multiplied through by the constant term that is
   * the inverse of the highest term
   */

  res.val = val;
 
  shift = res.Order();
  /**
   * shift = highest power of X with a non-zero co-efficient
   */

  while (shift >= order) {
    /**
     * The order of (mul << (shift - order)) is shift - because mul
     * has same order as rhs and rhs has order "order"
     *
     * GF4Poly(res.val >> (shift*2)) is a new poly containing only the
     * leading co-efficient of res
     *
     * The product of these two is a polynomial with same degree as
     * res and same leading co-efficient.  So when we add them to res
     * the leading co-efficient of res is cancelled out - remember
     * that addition = subraction ( because 1+1=0 )
     */
    res += (mul << (shift - order)) * GF4Poly(res.val >> (shift * 2));
    /*
     * res has now been changed by the correct multiple of rhs that
     * knocks off the top co-efficient (of res)
     */

    /*
     * Now search down for the next non-zero co-efficient of res.
     */
    while (!((res.val >> (shift * 2)) || shift == 0))
      shift--;

    /*
     * Keep doing this until the order of res (shift) is strictly less
     * than the order of rhs (order).  Once this is true we know there
     * are no more factors to remove
     */
  }

  /**
   * So now res is a polynomial of degree less than the order of rhs
   * and was arrived at by removing multiples of rhs.  Therefore it
   * must be the modulus
   */
  return res;
}

GF4Poly& GF4Poly::operator %=(const GF4Poly& rhs)
{
  return *this = *this % rhs;
}

GF4Poly GF4Poly::operator /(const GF4Poly& rhs) const
{
  /**
   * This method is basically the same as % but you have to keep track
   * of the multiples that you remove and then return them at the end
   * all added up
   */
  GF4Poly res, mul, rem;

  unsigned int shift = 0;
  unsigned int order = 0;

  order = rhs.Order();

  /**
   * build mul in the same way as for the % operator - mul is of the
   * same order as rhs but with 1 as the leading co-efficient
   */
  mul = GF4Poly((rhs.val >> (order * 2 )) == 1 ? 1 : (rhs.val >> (order * 2 )) ^ 1);

  rem.val = val;
 
  shift = rem.Order();

  while (shift >= order) {
    /**
     * We now remove multiples of rhs from the remainder (rem) until
     * the remainder's order becomes less than the order of rhs. 
     *
     * Keep adding those multiples that we remove to res so that we
     * can return it as the result
     */
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

/**
 * Rotate the polynomial (of given length) by a given rotation
 */ 
GF4Poly& GF4Poly::Rotate(unsigned int rotation, unsigned int length)
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

/**
 * Compute the order of this polynomial - i.e. the highest power that
 * has non-zero co-efficient.  
 */
unsigned int GF4Poly::Order() const
{
 
  int res = 0;
 
  while (val >> ((res + 1) * 2))
    res ++;

  return res; 
}

/**
 * Compute the number of non zero co-efficients
 */
unsigned int GF4Poly::Weight() const
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

// GF4Poly::operator string() const
// {
//   unsigned long long int x;

//   string res;

//   x = val;

//   while (x) {
//     res = (char) ((x & 3) + '0') + res;

//     x >>= 2;
//   }

//   return res;
// }

unsigned int GF4Poly::GetTerm(int order) const
{
  return (val >> (order*2)) & 3;
}

unsigned long long GF4Poly::GetValue() const
{
  return val;
}

GF4Poly GF4Poly::pwr(unsigned int exp, const GF4Poly& modulus) const {
  if (exp == 0) {
    return GF4Poly(1);
  }
  else {
    GF4Poly r =  (*this * *this) % modulus;
    if (exp % 2 == 0) {
      return r.pwr( exp/2,modulus) % modulus;;
    }
    else {
      return (*this * r.pwr( exp/2,modulus)) % modulus;
    }
  }
}
