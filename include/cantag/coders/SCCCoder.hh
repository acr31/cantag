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
 *
 * Copyright Chris Cain <cbc20@cam.ac.uk>
 */

#ifndef SCCCODER_GUARD
#define SCCCODER_GUARD

#include <cantag/Config.hh>
#include <cantag/coders/Coder.hh>

#ifdef TEXT_DEBUG
# define SCCCODER_DEBUG
# undef BERLEKAMP_DEBUG
#else
# undef SCCCODER_DEBUG
# undef BERLEKAMP_DEBUG
#endif

namespace Cantag {

  /**
   * A structured cyclic coding class.
   *
   * \todo Vile hackery abounds.  This currently remembers the code that
   * you issued and checks to see if the decoded code matches rather
   * than actually doing the decode.  If it does it returns the original
   * payload - if not it returns garbage.
   *
   * START is a number between 0 and the number of SECTORS 
   */
  template<int RINGS, int SECTORS, int ERRORS, int START>
  class SCCCoder : public virtual Coder<RINGS*SECTORS> {
  private:
    static const int q=1<<RINGS; // the symbol field size
    int degf;                    // the degree of f
    int degh;                    // the degree of h
    int qe;                      // the number of distinct polynomials mod h (there are two lookup tables size qe and 2*qe)
    int prim;                    // the bounds of alpha (stands for log of a primitive nth root of unity)
    int order;                   // qe-1 (the order of the multiplicative group mod h)
    int inverse;                 // solution of the eqn inverse*log(X) mod h = prim
    int symbolpoly;              // a constant used for the base field mulitplication
    int logx;                    // the discrete log of x
    int root;                    // the primitive root mod h - root has order "order"

    int f[SECTORS];              // the generator polynomial f
    int h[SECTORS];              // the auxillary polynomial h

    int* dlog;                   // discrete log lookup table
    int* power;                  // power[0] = 1, power[1] = root, power[2]=root**2

    int m_size;                  // the number of symbols in m

    int f1[SECTORS*2];           // Array used in extended euclid
    int g1[SECTORS*2];           // Array used in extended euclid
    int s1[SECTORS];             // Array used in extended euclid
    int t1[SECTORS];             // Array used in extended euclid
    int s2[SECTORS];             // Array used in extended euclid
    int t2[SECTORS];             // Array used in extended euclid
    int q1[SECTORS];             // Array used in extended euclid

    int *s;                      // will end up pointing to either s1 or s2 after extendedeuclid
    int *t;                      // will end up pointing to either t1 or t2 after extendedeuclid
  
  public:
    typedef SCCCoder<RINGS,SECTORS,ERRORS,START> CoderType;

    SCCCoder();

    ~SCCCoder();

    virtual bool IsErrorCorrecting() const;
    virtual int GetSymbolSize() const;
    virtual int GetHammingDistanceBits() const;
    virtual int GetHammingDistanceSymbols() const;

    /**
     * Encode the payload.  The data encoded consists of a message of
     * m_size symbols (m_size*RINGS bits) and a number alpha which must
     * be less than prim.  The data is interpreted with the low bits
     * providing m and the remainder as alpha.  If alpha is too large
     * this method will return false;
     */  
    virtual bool EncodePayload(CyclicBitSet<RINGS*SECTORS>& data) const;
    virtual int DecodePayload(CyclicBitSet<RINGS*SECTORS>& data) const;

  private:
    void print(const char* s, const int *f, int count) const;
    void print(const char *s,const int *f) const;

    /**
     * Multiplies two elements of the symbol field
     */
    int mul_symbol(int i, int j);

    /**
     * Multiples an element of the symbol field by an element of the
     * locator field
     */
    int mulconst(int i, int j);

    /**
     * Computes all powers of root mod h and stores them.  Returns
     * non-zero if root was not primitive.
     */
    int init_logtable(int *power, int *dlog,int root,int poly,int degpoly);

  
    /**
     * Populates various arrays and determines f and h.
     */
    void initialise();

    /**
     * Multiply i by j
     */
    inline int mul(int i, int j) const;

    /**
     * Multiply j by root^i
     */
    inline int shift(int i,int j) const;

    /**
     * Add f times g to res.
     */
    void mulpoly(int *res, const int *f, int degf, const int* g, int degg) const;
 
    /**
     * Reduces c mod f assumes f has exact degree degf
     */
    void mod(int *c, int degc, const int* f, int degf) const;

  
    /**
     * Divides c by f and returns the new degree of c
     */
    int divmod(int *c, int degc, int* f, int degf,int* q) const;

    /**
     * The Extended Euclid Algorithm
     *
     * Calculates s and t such that fs+gt = 1
     * The value of f1 always equals s1 f + t1 g
     * The value of g1 always equals s2 f + t2 g
     * Fails and returns non-zero if f and g have a common factor 
     */
    bool extendedeuclid(int *f,int degf, int *g, int degg);

    void encode(unsigned int alpha, int*m,int *R) const;
  
    int decode(int *R,int* alpha) const;

    /**
     * Works out the syndromes from R and stores them in S
     */
    void getsyndrome(int* S, int *R) const;

    /**
     * Implementation of Berlekamp-Massey method for correcting BCH codes
     *
     * Most hideous code ever. Attempts to find smallest polynomials
     * omega and sigma such that (1+S)*sigma == omega (mod
     * X^(2*ERRORS+1))  (cbc20)
     */
    void berlekamp(int *S, int *sigma, int* omega) const;

    /**
     * Finds the roots of sigma amongst the nth roots of unity.
     * These correspond to the error locations.
     * Then tries to find the error values from omega.
     * Will fail if more than ERROR errors have occured
     * Returns true if it is unsuccessful in correcting errors
     * Returns false if it succeeds
     */
    int chiensearch(int *R, int *sigma,int *omega) const;
  };

  template<int RINGS,int SECTORS,int ERRORS, int START> SCCCoder<RINGS,SECTORS,ERRORS,START>::SCCCoder() {
    assert(SECTORS % 2 != 0);
    initialise();
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> SCCCoder<RINGS,SECTORS,ERRORS,START>::~SCCCoder() {
    delete[] dlog;
    delete[] power;  
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> bool SCCCoder<RINGS,SECTORS,ERRORS,START>::IsErrorCorrecting() const { return true; }
  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::GetSymbolSize() const { return RINGS; }
  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::GetHammingDistanceBits() const { return 2*ERRORS+1; }
  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::GetHammingDistanceSymbols() const { return 2*ERRORS+1; }
  template<int RINGS,int SECTORS,int ERRORS, int START> bool SCCCoder<RINGS,SECTORS,ERRORS,START>::EncodePayload(CyclicBitSet<RINGS*SECTORS>& data) const {
#ifdef SCCCODER_DEBUG
    PROGRESS("Value to encode " << data);
#endif
    int m[SECTORS];              // Stores part of message to be encoded (the other part is alpha)
    for(int i=0;i<m_size;i++) {
      m[i] = data.GetSymbol(i,RINGS);
    }
    
    int alpha = 0;
    for(int i=RINGS*SECTORS-1;i>=m_size;i--) {
      alpha<<=1;
      alpha |= data[i];
    }

    bool return_code;
    if (alpha>=prim) {
#ifdef SCCCODER_DEBUG
      PROGRESS("Message is too large");
#endif
      alpha %=prim;
      return_code =false;
    }
    else {
      return_code = true;
    }

    int R[SECTORS];              // Stores the codeword
    encode(alpha,m,R);

    // message is now in R

    for(int i=0;i<SECTORS;i++) {
      data.PutSymbol(R[i],i,RINGS);
    }
#ifdef SCCCODER_DEBUG
    PROGRESS("Encoded Value is "<< data);
#endif
    return return_code;
  }


  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::DecodePayload(CyclicBitSet<RINGS*SECTORS>& data) const {
#ifdef SCCCODER_DEBUG
    PROGRESS("Payload is " << data);
#endif
    // copy the payload into R which holds the thing to be decoded
    // each set of RING bits are combined to make a symbol
    int R[SECTORS];              // Stores the codeword
    for(int i=0;i<SECTORS;i++) {
      R[i] = data.GetSymbol(i,RINGS);
    }

    /* these three things do the error correct */
    /*
      int S[SECTORS];              // Will store the syndromes after calling getsyndrom
      getsyndrome(S,R);
      int omega[SECTORS];          // Will store error evaluator after berlekamp
      int sigma[SECTORS];          // Will store error locator after berlekamp 
      berlekamp(S,sigma,omega);
      if (chiensearch(R,sigma,omega))  // returns non-zero if it fails
      {
      #ifdef SCCCODER_DEBUG
      PROGRESS("Too many errors to correct code");
      return -1;
      #endif
      }
    */
    int alpha;

    // this returns the amount of rotation required to read the
    // corrected code or -1 if it fails
    int rotation = decode(R,&alpha);

    if (rotation < 0) {
#ifdef SCCCODER_DEBUG
      PROGRESS("Failed to decode corrected code");
#endif
      data.SetInvalid();
      return -1;
    }

    data.reset();
    
    for(int i=0;i<m_size;i++) {
      data.PutSymbol(R[(SECTORS-m_size+rotation+i) % SECTORS],i,RINGS);
    }
    
    int pointer = m_size;
    while(alpha) {
      data[pointer++] = (alpha & 0x1);
      alpha>>=1;
    }

#ifdef SCCCODER_DEBUG
    PROGRESS("Result is " << data);
#endif

    return rotation;
  }  

#ifdef SCCCODER_DEBUG
  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::print(const char* s, const int *f, int count) const {
    int k;
    PROGRESS(s);
    for(k=0; k<count; k++) {
      std::cout << f[k] << " ";
    } 
    std::cout << std::endl;    
  }
#endif

#ifdef SCCCODER_DEBUG
  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::print(const char *s,const int *f) const
  {
    print(s,f,SECTORS);
  }
#endif

  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::mul_symbol(int i, int j)
  {
    int res = 0;
    while(i)
      {
	if(i&1) res^=j;
	i>>=1;
	j<<=1;
	if(j&q) j^=symbolpoly;
      }
    return res;
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::mulconst(int i, int j)
  {
    int res = 0;
    int tempq,temps,d;
    while(i)
      {
	if(i&1) res^=j;
	i>>=1;
	j<<=1;
	tempq = q;
	temps = symbolpoly;
	for(d=0; d<degh; d++)
	  {
	    if(j&tempq) j^=temps;
	    tempq<<=RINGS;
	    temps<<=RINGS;
	  }
      }
    return res;
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::init_logtable(int *power, int *dlog,int root,int poly,int degpoly)
  {
    int i,j,temp,temp2;
    for(i=0; i<qe; i++) dlog[i] = qe;

    //Attempt to find primitive element for locator field

    j = 1;
    for(i=0;i<order;i++)
      {
	power[i] = j;
	power[i+order] = j;
	if (dlog[j]==qe) 
	  dlog[j] = i; 
	else 
	  {
	    //printf("%d not primitive mod %d\n",root,poly);
	    return 1;
	  }

	temp = j;
	temp2= root;
	while(temp2)
	  {
	    j = (j<<RINGS)^mulconst(temp2&(q-1),temp);
	    j ^= mulconst(j>>(degpoly*RINGS),poly);
	    temp2>>=RINGS;
	  }
#ifdef SCCCODER_DEBUG
	if (j&~order) {
	  PROGRESS("Multiplication error - j&~order != 0");
	}
#endif
	assert(!(j&~order));
      }
#ifdef SCCCODER_DEBUG
    PROGRESS(root<<" is a primitive root for " << poly << ". Log x = " << dlog[q]);
#endif
    return 0;
  }


  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::initialise()
  {
    if(RINGS==1) symbolpoly=2;
    if(RINGS==2) symbolpoly=7;
    if(RINGS==3) symbolpoly=11;
    if(RINGS==4) symbolpoly=19;
    if(RINGS==5) symbolpoly=37;
#ifdef SCCCODER_DEBUG
    if(RINGS>5) { 
      PROGRESS("Rings must be < 6");
    }
#endif
    assert(RINGS<=5);

    int i,j,k;
    int tick[SECTORS];
  
    for(i=0; i<SECTORS; i++) { h[i] = 0; tick[i]=0; }
    h[0]=1; degh=0; degf=0; 
  
    for(i=START; i<START+2*ERRORS; i++)
      {
	if(tick[i%SECTORS]==0)
	  {
	    j = i%SECTORS;
	    do
	      {
		tick[j] = 1;
		degf++;
		j = (j*q)%SECTORS;
	      }
	    while((j-i)%SECTORS);
	  }
      }
    
    // now work out qe - qe is smallest power of q which is congruent to 1 modulo SECTORS
    qe=1;
    do
      {
	qe *= q;
	degh++;
      }
    while((qe-1)%SECTORS);

#ifdef SCCCODER_DEBUG
    if(degh*RINGS>16) {
      PROGRESS("Locator field exceeds maximum size for multiplication.");
    }
#endif
    assert(degh*RINGS<=16);

    order = qe-1;
    prim = (qe-1)/SECTORS;  // we know that prim must be an integer
    dlog = new int[qe];
    power = new int[2*qe];
  
    int poly=qe;
    while(init_logtable(power,dlog,q,++poly,degh));
	
    //Determine h:
    j = 1;
    for(i=0;i<degh;i++)
      {
	for(k=SECTORS-1;k>0;k--)
	  h[k] = shift(j*prim,h[k]) ^ h[k-1];
      
	h[0] = shift(j*prim,h[0]);
      
	j = (j*q)%SECTORS;
      }
#ifdef SCCCODER_DEBUG
    print("h ",h);
#endif
	
    poly = 0; j=1;
    for(i=0;i<=degh;i++)
      {
	poly += j*h[i];
	j <<= RINGS;
      }
#ifdef SCCCODER_DEBUG
    PROGRESS("Poly (an internal representation of h): "<< poly);
#endif

    root=0;
    do
      {
	//Find a primitive root mod h
	while(init_logtable(power,dlog,++root,poly,degh));

	//Determine f:
	for(i=1; i<SECTORS; i++) f[i]=0;
	f[0]=1; 
      
	for(i=0; i<SECTORS; i++)
	  if(tick[i])
	    {
	      for(k=SECTORS-1;k>0;k--)
		f[k] = shift(i*prim,f[k]) ^ f[k-1];
	    
	      f[0] = shift(i*prim,f[0]);
	    }
      
#ifdef SCCCODER_DEBUG
	print("f ",f);
#endif

	//However. Need to try again if f and h have a common factor
      }
    while (extendedeuclid(f,degf,h,degh));
  
    //Print out variables altered by extendedeuclid
#ifdef SCCCODER_DEBUG
    print("s ",s); print("t ",t);
#endif

#ifdef SCCCODER_DEBUG
    //Check that s and t solve the desired equations.
    //i.e. f1 should end up being 1.
    for(k=0;k<SECTORS; k++) f1[k]=0;

    mulpoly(f1,f,degf,s,degh-1);
    mulpoly(f1,h,degh,t,degf-1);

    print("",f1);
#endif

    //Work out the effect of rotation by one place.
    inverse=0;
    if(degh==1) logx = dlog[q^poly]; else logx = dlog[q];
    while((inverse*logx-prim)%order) inverse++;

#ifdef SCCCODER_DEBUG
    PROGRESS("Inverse: " << inverse);
#endif 
  
    
    // distort s so that so multiplication in place becomes mulitplication - s
    t[0]^=1;
    s[0]^=1;

    m_size = SECTORS-degf-degh;

#ifdef SCCCODER_DEBUG
    PROGRESS("Maximum alpha: "<<(prim-1));
    PROGRESS("Symbols required for m: "<<(SECTORS-degf-degh));
    PROGRESS("Bits per symbol: " << RINGS);
#endif
  } 
  

  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::mul(int i, int j) const
  {
    if(i&&j) return power[dlog[i]+dlog[j]];
    else return 0;
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::shift(int i,int j) const
  {
    if(j) {
      return power[i+dlog[j]];
    }
    else {
      return 0;
    }
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::mulpoly(int *res, const int *f, int degf, const int* g, int degg) const
  {
    int i,j;
    for(i=degf; i>=0; i--) {
      for(j=degg;j>=0; j--) {
	res[i+j] ^= mul(f[i],g[j]);
      }
    }
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::mod(int *c, int degc, const int* f, int degf) const
  {
#ifdef SCCCODER_DEBUG
    if(f[degf]==0) {
      PROGRESS("f must have degree degf");
    }
#endif
    assert(f[degf] != 0);

    int topcoeffc,topcoefff=dlog[f[degf]],i;
    while(degc>=degf)
      {
	if(c[degc]) 
	  {
	    topcoeffc = dlog[c[degc]]-topcoefff;
	    if(topcoeffc<0) topcoeffc+=order; 
	    for(i=degf; i>=0; i--) {
	      if(f[i]) {
		c[degc-degf+i] ^= power[topcoeffc+dlog[f[i]]];
	      }
	    }
	  }
	degc--;
      }
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> int  SCCCoder<RINGS,SECTORS,ERRORS,START>::divmod(int *c, int degc, int* f, int degf,int* q) const
  {
    int topcoeffc,topcoefff,i;
    for(i=0;i<SECTORS;i++) q[i]=0;
#ifdef SCCCODER_DEBUG
    if(f[degf]==0) {
      PROGRESS("f must have degree degf");
    }
#endif
    assert(f[degf] != 0);
    topcoefff=dlog[f[degf]];
    while(degc>=degf)
      {
	if(c[degc]) 
	  {
	    topcoeffc = dlog[c[degc]]-topcoefff;
	    if(topcoeffc<0) topcoeffc+=order;
	    q[degc-degf] = power[topcoeffc];
	    for(i=degf; i>=0; i--)
	      if(f[i]) c[degc-degf+i] ^= power[topcoeffc+dlog[f[i]]];
	  }
	else q[degc-degf]=0;
	degc--;
      }
    while(c[degc]==0) degc--;
    if(degc<0) return 0 ;
    else return degc;
  }
  

  template<int RINGS,int SECTORS,int ERRORS, int START> bool SCCCoder<RINGS,SECTORS,ERRORS,START>::extendedeuclid(int *f,int degf, int *g, int degg)
  {
    int i,k,c;
    for(i=0; i<SECTORS; i++)
      {
	f1[i] = f[i];
	g1[i] = g[i];
	s1[i]=s2[i]=t1[i]=t2[i]=0;
      }
    s1[0] = 1;
    t2[0] = 1;
    while(1)
      {
	if(degg)
	  {
	    c = degf-degg;
	    degf = divmod(f1,degf,g1,degg,q1);
	    mulpoly(s1,q1,c,s2,SECTORS-1-c);
	    mulpoly(t1,q1,c,t2,SECTORS-1-c);
	  }
	else
	  {
	    s = s2;
	    t = t2;
	    c = g1[0];
	    break;
	  }
      
	if(degf)
	  {
	    c = degg-degf;
	    degg = divmod(g1,degg,f1,degf,q1);
	    mulpoly(s2,q1,c,s1,SECTORS-1-c);
	    mulpoly(t2,q1,c,t1,SECTORS-1-c);
	  }
	else
	  {
	    s = s1;
	    t = t1;
	    c = f1[0];
	    break;
	  }
      }	  

    if (c) {
      c=dlog[c];
    }
    else {
      return true;
    }

    for(i=0;i<SECTORS;i++)
      {
	s[i] = shift(order-c,s[i]);
	t[i] = shift(order-c,t[i]);
      }
    return false;
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::encode(unsigned int alpha, int*m,int *R) const
  {
    int tempf1[2*SECTORS-1];
    int tempg1[2*SECTORS-1];
    unsigned int i,r=degf+degh,y=alpha;
    for(i=0;i<SECTORS;i++) R[i]=0;
#ifdef SCCCODER_DEBUG
    if (alpha > prim) {
      PROGRESS("alpha is too large");
    }
#endif
    assert(alpha <= prim);

    //tempf1 becomes X^r * m + w^alpha mod h
    //tempg1 becomes X^r * m mod f

    for(i=0;i<r;i++)
      {
	tempf1[i] = 0;
	tempg1[i] = 0;
      }
    for(i=r;i<SECTORS;i++)
      {
	tempf1[i] = m[i-r];
	tempg1[i] = m[i-r];
      }

    y = power[alpha];
    for(i=0; i<degh; i++)
      {
	tempf1[i] ^= y&(q-1);
	y>>=RINGS;
      }

    mod(tempf1,SECTORS-1,h,degh);
    mod(tempg1,SECTORS-1,f,degf);

    //This is the chinese remainder theorem part.
    //Uses the s and t computed from extendedeuclid.

    mulpoly(tempg1,tempg1,degf-1,t,degf-1);
    mulpoly(tempf1,tempf1,degh-1,s,degh-1);

    mod(tempf1,2*(degh-1),h,degh);
    mod(tempg1,2*(degf-1),f,degf);

    mulpoly(R,tempg1,degf-1,h,degh);
    mulpoly(R,tempf1,degh-1,f,degf);

    //Now R is the required check poly.
    //Need to add the message...

    for(i=r;i<SECTORS;i++)
      R[i]=m[i-r];
  }
  

  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::decode(int *R,int* alpha) const
  {
    int modh=0,index=0,i;
    for(i=0; i<SECTORS; i++)
      {
	modh ^= shift(index,R[i]);
	index+=logx;
	if(index>=order) index-=order;
      }
    if(modh)
      {
	modh = dlog[modh];
#ifdef SCCCODER_DEBUG
	PROGRESS("alpha is : " << (modh%prim));
	PROGRESS("rotation : " << ((modh/prim)*inverse % SECTORS));
#endif
	*alpha = modh%prim;
	return (modh/prim)*inverse % SECTORS;
      }
    else 
      {
	// this may be a valid codeword for the cylic code we are
	// using but it is not a valid code for our structured scheme
#ifdef SCCCODER_DEBUG
	PROGRESS("modh = 0");
	mod(R,SECTORS-1,h,degh);
	print("R ",R);
#endif
	return -1;
      }
  }
  

  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::getsyndrome(int* S, int *R) const
  {
    int beta,jump=0,i,j;
    for(i=0; i<2*ERRORS; i++) S[i] = 0;
    for(i=0; i<SECTORS; i++)
      {
	if (R[i]) 
	  {
	    beta = (dlog[R[i]]+i*START*prim)%order;
	    for(j=0; j<2*ERRORS; j++)
	      {
		S[j] ^= power[beta];
		beta += jump;
		if(beta>=order) beta -= order;
	      }
	  }
	jump += prim;
      }
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> void SCCCoder<RINGS,SECTORS,ERRORS,START>::berlekamp(int *S, int *sigma, int* omega) const
  {
    int gamma[2*ERRORS+1];
    int tau[2*ERRORS+1];
    int d=0,b=0,k=0,delta,logdelta,i,j;
    for(i=0; i<2*ERRORS+1; i++)
      {
	omega[i]=tau[i]=gamma[i]=sigma[i]=0;
      }
    omega[0]=tau[0]=sigma[0]=1;
  
    for(k=0;k<2*ERRORS;k++)
      {
	delta = sigma[k+1];
	for(i=0;i<=k;i++) delta ^= mul(sigma[i],S[k-i]);

	if((delta==0)||(2*d>k+1)||(delta&&(2*d==k+1)&&(b==0)))
	  {
	    if(delta)
	      {
		logdelta=dlog[delta];
		for(i=k-d;i>=0;i--)
		  {
		    if(tau[i]) sigma[i+1] ^= shift(logdelta,tau[i]);
		    if(gamma[i]) omega[i+1] ^= shift(logdelta,gamma[i]);
		  }
	      }
	    for(i=k-d+1;i>0;i--)
	      {
		tau[i] = tau[i-1];
		gamma[i] = gamma[i-1];
	      }
	    tau[0] = 0;
	    gamma[0] = 0;
	  }
	else
	  {
	    d = k+1-d;
	    b = 1-b;
	    logdelta=dlog[delta];
	    for(i=d;i>0;i--)
	      {
		tau[i] = shift(order-logdelta,sigma[i]);
		gamma[i] = shift(order-logdelta,omega[i]);
		if(tau[i-1]) sigma[i] ^= shift(logdelta,tau[i-1]);
		if(gamma[i-1]) omega[i] ^= shift(logdelta,gamma[i-1]);	  
	      }
	    tau[0] = shift(order-logdelta,sigma[0]);
	    gamma[0] = shift(order-logdelta,omega[0]);
	  }

#ifdef BERLEKAMP_DEBUG
	PROGRESS("Delta: "<<delta<<" Log: " << dlog[delta] << " d: " << d << " k: " << (k+1));
	print("Gamma",gamma,2*ERRORS+1);
	print("Tau",tau,2*ERRORS+1);
	print("Sigma",sigma,2*ERRORS+1);
	print("Omega",omega,2*ERRORS+1);
	/*
	  printf("\n(1+S)s ");
	  for(j=-1;j<k;j++)
	  {
	  delta = sigma[j+1];
	  for(i=0;i<=j;i++) delta ^= mul(sigma[i],S[j-i]);
	  printf("%d ",delta);
	  }	  
	  printf("\n(1+S)t ");
	  for(j=-1;j<k+1;j++)
	  {
	  delta = tau[j+1];
	  for(i=0;i<=j;i++) delta ^= mul(tau[i],S[j-i]);
	  printf("%d ",delta);
	  }	  
	  printf("\n\n");
	  }
	*/
#endif
      }
  }

  template<int RINGS,int SECTORS,int ERRORS, int START> int SCCCoder<RINGS,SECTORS,ERRORS,START>::chiensearch(int *R, int *sigma,int *omega) const
  {
    int T[SECTORS];              // Temp array used in chiensearch
    int beta,jump=0,i,j,deg;
    for(i=0; i<SECTORS; i++) T[i] = 0;
    for(i=0;i<2*ERRORS+1;i++) if (sigma[i]||omega[i]) deg=i;
    if(deg>ERRORS) return -1;
    for(i=0; i<=deg; i++)
      {
	if (sigma[i]) 
	  {
	    beta = dlog[sigma[i]];
	    for(j=0; j<SECTORS; j++)
	      {
		T[j] ^= power[beta];
		beta += jump;
		if(beta>=order) beta -= order;
	      }
	  }
	jump += prim;
      }

    j=0;
    for(i=0; i<SECTORS; i++)
      if(T[i]==0) 
	{
	  j++;
	  int w=0,s=0,k,error=-1;
	  for(k=0; k<=deg; k++)
	    {
	      if(k&1) s ^= shift((k*i*prim)%order,sigma[k]);
	      w ^= shift((k*i*prim)%order,omega[k]);
	    }	

	  if(s&&w)
	    {
	      error = (order+i*prim*(START-1)+dlog[w]-dlog[s])%order;
	      error = power[error];
	      R[(SECTORS-i)%SECTORS]^=error;
	    }
	  else return 1;

	  if (error&(~(q-1))) return 2;
	}
    if (j!=deg) return 1;
    //  print("No errors:  ",R);  
    //  printf("%d errors corrected\n",j);
    return 0;
  }  
}

#endif//SCCCODER_GUARD
