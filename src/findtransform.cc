/**
 * $Header$
 */

#include <findtransform.hh>
#include <eigenvv.hh>
#include <gaussianelimination.hh>

#define SQUARE_TRANSFORM_DEBUG
#undef APPLY_TRANSFORM_DEBUG

void GetTransform(const QuadTangle& quad, float transform[16]) {
  // see the header file for a full explanation of what's going on here
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Calculating transform for :"
	   "("<<quad.GetX0()<<","<<quad.GetY0()<<"),"<<
	   "("<<quad.GetX1()<<","<<quad.GetY1()<<"),"<<
	   "("<<quad.GetX2()<<","<<quad.GetY2()<<"),"<<
	   "("<<quad.GetX3()<<","<<quad.GetY3()<<"),");	   
#endif

  // we particularly want coeffs to be an array of pointers to arrays
  // containing the columns of the matrix - then we can swap columns
  // conveniently by swapping pointers
  double coeffs0[] = {1,1,0,0,0,0,0,0};
  double coeffs1[] = {1,0,0,1,0,0,0,0};
  double coeffs2[] = {1,1,1,1,0,0,0,0};
  double coeffs3[] = {0,0,0,0,1,1,0,0};
  double coeffs4[] = {0,0,0,0,1,0,0,1};
  double coeffs5[] = {0,0,0,0,1,1,1,1};
  double coeffs6[] = {-quad.GetX1(),-quad.GetX0(),0,0,-quad.GetY1(),-quad.GetY0(),0,0};
  double coeffs7[] = {-quad.GetX1(),0,0,-quad.GetX2(),-quad.GetY1(),0,0,-quad.GetY2()};
  double* coeffs[] = {coeffs0,
		      coeffs1,
		      coeffs2,
		      coeffs3,
		      coeffs4,
		      coeffs5,
		      coeffs6,
		      coeffs7};
		     
  double xvals[] = { quad.GetX1(),
		     quad.GetX0(),
		     quad.GetX3(),
		     quad.GetX2(),
		     quad.GetY1(),
		     quad.GetY0(),
		     quad.GetY3(),
		     quad.GetY2() };
  double result[8];

  solve_simultaneous(xvals,coeffs,result,8);

#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Computed a0 "<<result[0]);
  PROGRESS("         a1 "<<result[1]);
  PROGRESS("         a2 "<<result[2]);
  PROGRESS("         a3 "<<result[3]);
  PROGRESS("         a4 "<<result[4]);
  PROGRESS("         a5 "<<result[5]);
  PROGRESS("         a6 "<<result[6]);
  PROGRESS("         a7 "<<result[7]);
#endif

  double scalefactor = sqrt(result[1]*result[1]+result[4]*result[4]+result[7]*result[7]);
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Scale factor is "<<scalefactor);
#endif

  //  for(int i=0;i<8;i++) {
  //    result[i] *= scalefactor;
  //  }
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Scaled   a0 "<<result[0]);
  PROGRESS("         a1 "<<result[1]);
  PROGRESS("         a2 "<<result[2]);
  PROGRESS("         a3 "<<result[3]);
  PROGRESS("         a4 "<<result[4]);
  PROGRESS("         a5 "<<result[5]);
  PROGRESS("         a6 "<<result[6]);
  PROGRESS("         a7 "<<result[7]);
#endif
 
  //  transform[0] = result[0];  transform[1] = result[1];  transform[2] = 0;  transform[3] = result[2];
  //  transform[4] = result[3];  transform[5] = result[4];  transform[6] = 0;  transform[7] = result[5];
  //  transform[8] = result[6];  transform[9] = result[7];  transform[10]= 0;  transform[11]= 1/scalefactor;
  //  transform[12]= 0;          transform[13]= 0;          transform[14]= 0;  transform[15]= 1;

  transform[0] = result[0];  transform[1] = result[1];  transform[2] = 0;  transform[3] = result[2];
  transform[4] = result[3];  transform[5] = result[4];  transform[6] = 0;  transform[7] = result[5];
  transform[8] = result[6];  transform[9] = result[7];  transform[10]= 0;  transform[11]= 1;
  transform[12]= 0;          transform[13]= 0;          transform[14]= 0;  transform[15]= 1;

#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";");
  PROGRESS("             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";");
  PROGRESS("             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";");
  PROGRESS("             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];");
#endif

}



void ApplyTransform(const float transform[16], float x, float y, float* projX, float* projY) {
  *projX = transform[0]*x + transform[1]*y + transform[2] + transform[3];
  *projY = transform[4]*x + transform[5]*y + transform[6] + transform[7];
  float projZ = transform[8]*x + transform[9]*y + transform[10] + transform[11];
  float projH = transform[12]*x + transform[13]*y + transform[14] + transform[15];

#ifdef APPLY_TRANSFORM_DEBUG
  PROGRESS("Transformed ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<","<<projZ<<","<<projH<<")");
#endif
  
  *projX /= projH;
  *projY /= projH;
  projZ /= projH;

  *projX /= projZ;
  *projY /= projZ;

#ifdef APPLY_TRANSFORM_DEBUG
  PROGRESS("Projected ("<<x<<","<<y<<","<<"0) on to ("<<*projX<<","<<*projY<<")");
#endif

}

void ApplyTransform(const float transform[16], float* points, int numpoints) {
  for(int i=0;i<numpoints*2;i+=2) {
    ApplyTransform(transform,points[i],points[i+1],points+i,points+i+1);
  }
}


void GetNormalVector(const float transform[16], float normal[3]) {

  // project (0,0,0) and (0,0,1).  Take the difference between them and normalize it

  float proj0x = transform[3];
  float proj0y = transform[7];
  float proj0z = transform[11];
  float proj0h = transform[15];

  float proj1x = transform[2] + transform[3];
  float proj1y = transform[6] + transform[7];
  float proj1z = transform[10] + transform[11];
  float proj1h = transform[14] + transform[15];

  normal[0] = proj1x/proj1h - proj0x/proj0h;
  normal[1] = proj1y/proj1h - proj0y/proj0h;
  normal[2] = proj1z/proj1h - proj0z/proj0h;
  
  float modulus = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
  
  normal[0]/=modulus;
  normal[1]/=modulus;
  normal[2]/=modulus;

#ifdef APPLY_TRANSFORM_DEBUG
  PROGRESS("Found normal vector ("<<normal[0]<<","<<normal[1]<<","<<normal[2]<<")");
#endif

}
