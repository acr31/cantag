/**
 * $Header$
 *
 * Copyright Chris Cain <cbc20@cam.ac.uk>
 */

#ifndef SCCCODER_GUARD
#define SCCCODER_GUARD

#include <Config.hh>
#include <Coder.hh>
#include <cstdio>


namespace cbc20 {

#define RINGS 5
#define SECTORS 31
#define ERRORS 5  // this gives 11 hamming distance
  //#define ERRORS 1 // this gives 3 hamming distance
#define START 6
#define SYMBOL_POLY 37
#define LOCATOR_POLY 32

  CyclicBitSet<RINGS*SECTORS> payload_copy;
  CyclicBitSet<RINGS*SECTORS> tag_data_copy;

int q=1<<RINGS,degree,qe,prim;
int* dlog;
int* power;
int R[SECTORS];
int S[SECTORS];
int T[SECTORS];
int omega[SECTORS];
int sigma[SECTORS];
int sigma1[SECTORS];
int m[SECTORS];
int f[SECTORS];
int h[SECTORS];
int f1[SECTORS];
int g1[SECTORS];
int s1[SECTORS];
int t1[SECTORS];
int s2[SECTORS];
int t2[SECTORS];
int q1[SECTORS];
int temp[SECTORS];
int *s;
int *t;


void print(int *f)
{
  int k;
  //  for(k=0; k<SECTORS; k++) printf("%d ",f[k]); printf("\n");
}

//Multiplies two elements of the symbol field
int mul_symbol(int i, int j)
{
  int res = 0;
  while(i)
    {
      if(i&1) res^=j;
      i>>=1;
      j<<=1;
      if(j&q) j^=SYMBOL_POLY;
    }
  return res;
}

//Multiples an element of the symbol 
//field by an element of the locator field
int mulconst(int i, int j)
{
  int res = 0;
  int tempq,temps,d;
  while(i)
    {
      if(i&1) res^=j;
      i>>=1;
      j<<=1;
      tempq = q;
      temps = SYMBOL_POLY;
      for(d=0; d<degree; d+=RINGS)
	{
	  if(j&tempq) j^=temps;
	  tempq<<=RINGS;
	  temps<<=RINGS;
	}
    }
  return res;
}

void init_fields()
{
  int i=1,j,c;
  degree=0;
  do
    {
      i *= q;
      i %= SECTORS;
      degree+=RINGS;
    }
  while(i!=1);

  qe = 1<<degree;
  dlog = (int *) malloc(qe*sizeof(int));
  power = (int *) malloc(2*qe*sizeof(int));

  for(i=0; i<qe; i++) dlog[i] = qe;

  //Attempt to find primitive element for locator field
  //We only search linear polys X + c

  j = 1;
  c = 2;
  for(i=0;i<qe-1;i++)
    {
      power[i] = j;
      power[i+qe-1] = j;
      if (dlog[j]==qe) dlog[j] = i; else { printf("Nope\n");exit(0);}
      j = mulconst(c,j);
      j ^= mulconst(j>>degree,LOCATOR_POLY);
      if (j&~(qe-1)) {printf("Fucked\n"); exit(0);}
    }
  //  printf("\nWahey\n");
}  

//Multiply i by j
inline int mul(int i, int j)
{
  if(i&&j)
    {
      /*      int log = dlog[i]+dlog[j];
      if(log>=qe-1) return power[log-qe+1];
      else return power[log];*/
      return power[dlog[i]+dlog[j]];
    }
  else
    return 0;
}

//Multiply j by alpha^i
inline int shift(int i,int j)
{
  if(j)
    {
      /*      int log = i+dlog[j];
      if(log>=qe-1) return power[log-qe+1];
      else return power[log];*/
      return power[i+dlog[j]];
    }
  else
    return 0;
}


// Add f times g to res.
void mulpoly(int *res, int *f, int degf, int* g, int degg)
{
  int i,j;
  for(i=degf; i>=0; i--) 
    for(j=degg;j>=0; j--)
      res[i+j] ^= mul(f[i],g[j]);
}
  
//Returns c mod f assumes f has exact degree degf
void mod(int *c, int degc, int* f, int degf)
{
  int topcoeffc,topcoefff=dlog[f[degf]],i;
  while(degc>=degf)
    {
      if(c[degc]) 
	{
	  topcoeffc = dlog[c[degc]]-topcoefff;
	  if(topcoeffc<0) topcoeffc+=qe-1; 
	}
      else
	{
	  degc--;
	  continue;
	}
      for(i=degf; i>=0; i--)
	{
	  if(f[i]) c[degc-degf+i] ^= power[topcoeffc+dlog[f[i]]];
	}
      degc--;
    }
}


int divmod(int *c, int degc, int* f, int degf,int* q)
{
  int topcoeffc,topcoefff,i;
  for(i=0;i<SECTORS;i++) q[i]=0;
  if(f[degf]==0) {printf("Whoops...\n"); exit(0);}
  topcoefff=dlog[f[degf]];
  while(degc>=degf)
    {
      if(c[degc]) 
	{
	  topcoeffc = dlog[c[degc]]-topcoefff;
	  if(topcoeffc<0) topcoeffc+=qe-1;
	  q[degc-degf] = power[topcoeffc];
	}
      else
	{
	  q[degc-degf]=0;
	  degc--;
	  continue;
	}
      for(i=degf; i>=0; i--)
	{
	  if(f[i]) c[degc-degf+i] ^= power[topcoeffc+dlog[f[i]]];
	}
      degc--;
    }
  while(c[degc]==0) degc--;
  if(degc<0) return 0 ;
  else return degc;
}

//This function destroys f and g. Calculates s and t such that fs+gt = 1
//The current value of f always equals s1 f + t1 g
//The current value of g always equals s2 f + t2 g
void extendedeuclid(int *f,int degf, int *g, int degg)
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
	  //	  for(k=0;k<SECTORS; k++) printf("%d ",s1[k]); printf("\n");
	  //	  for(k=0;k<SECTORS; k++) printf("%d ",t1[k]); printf("\n");
	  //	  printf("\n");
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
	  //	  for(k=0;k<SECTORS; k++) printf("%d ",s2[k]); printf("\n");
	  //      for(k=0;k<SECTORS; k++) printf("%d ",t2[k]); printf("\n");
	  // printf("\n");
	}
      else
	{
	  s = s1;
	  t = t1;
	  c = f1[0];
	  break;
	}
    }	  
  if (c) c=dlog[c]; else {printf("Hmmmph..."); exit(0);}
  for(i=0;i<SECTORS;i++)
    {
      if (s[i]) s[i] = power[qe-1+dlog[s[i]]-c];
      if (t[i]) t[i] = power[qe-1+dlog[t[i]]-c];
    }
}

void addcheck(int *in, int* out, int *f, int degf)
{
  int i;
  for(i=0; i<degf; i++) out[i] = 0;
  for(i=degf; i<SECTORS; i++) out[i] = in[i-degf];
  mod(out,SECTORS-1,f,degf);
  for(i=degf; i<SECTORS; i++) out[i] = in[i-degf];
}

void encode(unsigned int alpha, int*m,int*f, int degf,int *h, int degh,int *R)
{
  unsigned int i,r=degf+degh,y=alpha;
  for(i=0;i<SECTORS;i++) R[i]=0;
  if(alpha>prim) {printf("Too big\n"); exit(0);}
  for(i=0;i<r;i++)
    {
      f1[i] = 0;
      g1[i] = 0;
    }
  for(i=r;i<SECTORS;i++)
    {
      f1[i] = m[i-r];
      g1[i] = m[i-r];
    }

  mod(f1,SECTORS-1,h,degh);
  mod(g1,SECTORS-1,f,degf);

  y = power[prim*alpha];
  for(i=0; i<degh; i++)
    {
      f1[i] ^= y&(q-1);
      y>>=RINGS;
    }

  mulpoly(g1,g1,degf-1,t,degf-1);
  mulpoly(f1,f1,degh-1,s,degh-1);

  mod(f1,2*(degh-1),h,degh);
  mod(g1,2*(degf-1),f,degf);

  mulpoly(R,g1,degf-1,h,degh);
  mulpoly(R,f1,degh-1,f,degf);

  for(i=r;i<SECTORS;i++)
    {
      R[i]=m[i-r];
    }
}

int decode(int *R)
{
  int modh=0,step=5,//dlog[q],
    index=0,i;
  for(i=0; i<SECTORS; i++)
    {
      if(R[i]) modh ^= power[dlog[R[i]]+index];
      index+=step;
      if(index>=qe-1) index-=qe-1;
    }
  if(modh) modh = dlog[modh];
  else {printf("modh = 0\n"); return -1; }// exit(0);}

  //  printf("rot: %d",(25*modh)%31);
  return (25*modh)%31;
  /*  int x = (8*modh)%15;
  int rot = (3*modh)%17;

  for(i=16+rot;i>=12+rot;i--)
    x = (x<<2)|R[i%SECTORS];
    printf("Rot: %d Answer: %d\n",rot,x);*/


}

void getsyndrome(int* S, int *R)
{
  int beta,jump,i,j;
  for(i=0; i<2*ERRORS; i++) S[i] = 0;
  for(i=0; i<SECTORS; i++)
    {
      if (R[i]) 
	beta = (dlog[R[i]]+i*START*prim)%(qe-1);
      else 
	continue;
      
      jump = (i*prim) % (qe-1);

      for(j=0; j<2*ERRORS; j++)
	{
	  S[j] ^= power[beta];
	  beta += jump;
	  if(beta>=(qe-1)) beta -= (qe-1);
	}
    }
}

void berlekamp(int *S, int *sigma, int* omega,int botch)
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
	      tau[i] = shift(qe-1-logdelta,sigma[i]);
	      gamma[i] = shift(qe-1-logdelta,omega[i]);
	      if(tau[i-1]) sigma[i] ^= shift(logdelta,tau[i-1]);
	      if(gamma[i-1]) omega[i] ^= shift(logdelta,gamma[i-1]);	  
	    }
	  tau[0] = shift(qe-1-logdelta,sigma[0]);
	  gamma[0] = shift(qe-1-logdelta,omega[0]);
	}
      if(botch)
	{
	  //	  printf("Delta: %d Logdelta: %d d: %d k: %d\n",delta,dlog[delta],d,k+1);
	  //	  printf("Gamma: ");
	  //	  for(i=0;i<2*ERRORS+1; i++)
	  //	    printf("%d ",gamma[i]);
	  //	  printf("\nTau:   ");
	  //	  for(i=0;i<2*ERRORS+1; i++)
	  //	    printf("%d ",tau[i]);
	  //	  printf("\nSigma: ");
	  //	  for(i=0;i<2*ERRORS+1; i++)
	  //	    printf("%d ",sigma[i]);
	  //	  printf("\nOmega: ");
	  //	  for(i=0;i<2*ERRORS+1; i++)
	  //	    printf("%d ",omega[i]);
	  //	  printf("\n(1+S)s ");
	  for(j=-1;j<k;j++)
	    {
	      delta = sigma[j+1];
	      for(i=0;i<=j;i++) delta ^= mul(sigma[i],S[j-i]);
	      //	      printf("%d ",delta);
	    }	  
	  //	  printf("\n(1+S)t ");
	  for(j=-1;j<k+1;j++)
	    {
	      delta = tau[j+1];
	      for(i=0;i<=j;i++) delta ^= mul(tau[i],S[j-i]);
	      //	      printf("%d ",delta);
	    }	  
	  //	  printf("\n\n");
	}
    }
}

int chiensearch(int *R, int *sigma,int *omega)
{
  int beta,jump,i,j,deg;
  for(i=0; i<SECTORS; i++) T[i] = 0;
  for(i=0;i<2*ERRORS+1;i++) if (sigma[i]||omega[i]) deg=i;
  if(deg>ERRORS) return -1;
  for(i=0; i<=deg; i++)
    {
      if (sigma[i]) 
	beta = dlog[sigma[i]];
      else 
	continue;
      
      jump = (i*prim) % (qe-1);

      for(j=0; j<SECTORS; j++)
	{
	  T[j] ^= power[beta];
	  beta += jump;
	  if(beta>=(qe-1)) beta -= (qe-1);
	}
    }

  j=0;
  for(i=0; i<SECTORS; i++)
    if(T[i]==0) 
      {
	j++;
	//	printf("Location: %d ",SECTORS-i);
	
	int w=0,s=0,k,error=-1;
	for(k=0; k<=deg; k++)
	  {
	    if(k&1) s ^= shift((k*i*prim)%(qe-1),sigma[k]);
	    w ^= shift((k*i*prim)%(qe-1),omega[k]);
	  }	

	if(s&&w)
	  {
	    error = (qe-1+i*prim*(START-1)+dlog[w]-dlog[s])%(qe-1);
	    error = power[error];
	    R[(SECTORS-i)%SECTORS]^=error;
	  }
	else return 1;

	//	printf("Error: %d\n",error);
	if (error&(~(q-1))) return 2;
	

      }
  //  printf("--------------\n");
  if (j!=deg) return 1;
  print(R);  
  //  printf("%d errors corrected\n",j);
  return 0;
}  

void encode_M_to_R()
{
  init_fields();
  int i,j,k,alpha=0,degf=0,degh; prim=(qe-1)/SECTORS;
  int tick[SECTORS];
  for(i=0; i<SECTORS; i++) {f[i]=0; h[i] = 0;tick[i]=0;}
  f[0]=1; h[0]=1;
  
  for(i=START; i<START+2*ERRORS; i++)
    {
      if(tick[i%SECTORS]==0)
	{
	  j = i%SECTORS;
	  do
	    {
	      degf++;
	      tick[j] = 1;
	      //	      printf("%d ",j);
	      for(k=SECTORS-1;k>0;k--)
		{
		  if (f[k]) f[k] = power[(dlog[f[k]]+j*prim)%(qe-1)];
		  f[k] ^= f[k-1];
		}
	      if (f[k]) f[k] = power[(dlog[f[k]]+j*prim)%(qe-1)];
	      j *= q;
	      j %= SECTORS;
	    }
	  while((j-i)%SECTORS);
	}
    }
  //  printf("\n");

  print(f);


  h[0] = 5; h[1] = 1;
  degh = 1;
  print(h);

  extendedeuclid(f,degf,h,degh);
  
  print(s); print(t);

  for(k=0;k<SECTORS; k++) temp[k]=0;

  mulpoly(temp,f,degf,s,degh-1);
  mulpoly(temp,h,degh,t,degf-1);

  print(temp);
  //  printf("\n");
  
  //Majorly lazy bastard

  t[0]^=1;
  s[0]^=1;

  int in[9];
  int x,rot;

  encode(alpha,m,f,degf,h,degh,R);
}

int decode_R() {
  /* these three things do the error correct */
  getsyndrome(S,R);
  
  berlekamp(S,sigma,omega,0);
  
  if (chiensearch(R,sigma,omega))  // returns non-zero if it fails
    {
      //      printf("More than %d errors\n",ERRORS);
      return -1;
      /*	  printf("Locations %d %d %d\n",p1,p2,p3);
		  
      printf("Actual: ");
      for(k=0;k<SECTORS; k++)
      printf("%d ",sigma1[k]);
      printf("\n");
      
      printf("Sigma: ");
      for(k=0;k<SECTORS; k++)
      printf("%d ",sigma[k]);
      printf("\n");
      
      printf("Omega: ");
      for(i=0;i<2*ERRORS+1; i++)
      printf("%d ",omega[i]);
      printf("\n");
      
      printf("(1+S)s ");
      for(k=-1;k<2*ERRORS;k++)
      {
      e = sigma[k+1];
      for(i=0;i<=k;i++) e ^= mul(sigma[i],S[k-i]);
      printf("%d ",e);
      }	  
      
      printf("\n\n");
      
      berlekamp(S,sigma,omega,1);*/
    }
  else return decode(R);  /* call decode to decode R  - this just gives the rotation so I need to rotate it back and check it */
}

}

/**
 * A structured cyclic coding class.
 *
 * \todo Vile hackery abounds.  This currently remembers the code that
 * you issued and checks to see if the decoded code matches rather
 * than actually doing the decode.  If it does it returns the original
 * payload - if not it returns garbage.
 */
template<int BIT_COUNT, int GRANULARITY>
class SCCCoder : public virtual Coder<BIT_COUNT> {
public:

  SCCCoder() {}

  virtual bool IsErrorCorrecting() const { return true; }
  virtual int GetSymbolSize() const { return GRANULARITY; }
  virtual int GetHammingDistanceBits() const { return 11; }
  virtual int GetHammingDistanceSymbols() const { return 11; }
  
  virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    cbc20::tag_data_copy = data; // copy    
    for(int i=0;i<BIT_COUNT/GRANULARITY;i++) {
      cbc20::m[i] = data.GetSymbol(i,GRANULARITY);
    }

    cbc20::encode_M_to_R();
    for(int i=0;i<BIT_COUNT/GRANULARITY;i++) {
      data.PutSymbol(cbc20::R[i],i,GRANULARITY);
    }
    cbc20::payload_copy = data;//copy
    return true;    
  }

  virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    // copy the payload into cbc20:R which holds the thing to be decoded
    // each set of GRANULARITY bits are combined to make a symbol
    for(int i=0;i<BIT_COUNT/GRANULARITY;i++) {
      cbc20::R[i] = data.GetSymbol(i,GRANULARITY);
    }

    // this returns the amount of rotation required to read the
    // corrected code or -1 if it fails
    int rotation = cbc20::decode_R();

    // copy the corrected code back to the payload
    for(int i=0;i<BIT_COUNT/GRANULARITY;i++) {
      data.PutSymbol(cbc20::R[i],i,GRANULARITY);
    }

    if (rotation >= 0) {
      // rotate the payload back by the given number of symbols
      data.RotateLeft(rotation*GRANULARITY);
      // if the payload is the same as the thing we sent then copy back the tag data
      if (data == cbc20::payload_copy) {
	for(int i=0;i<BIT_COUNT;i++) {
	  data[i] = cbc20::tag_data_copy[i];
	}
	return rotation;
      }
      else {
	// if the payload is different to what we send then copy back
	// !tag_data to make sure it wont match.
	for(int i=0;i<BIT_COUNT;i++) {
	  data[i] = !cbc20::tag_data_copy[i];
	}
	return rotation;
      }
    }
    else {
      // if the decode failed return -1
      return -1;
    }
  }  
};
#endif//SCCCODER_GUARD
