/**
 * $Header$
 */

#include <tripover/QuadTangleTransform.hh>
#include <tripover/gaussianelimination.hh>

#undef SQUARE_TRANSFORM_DEBUG

void ProjectiveQuadTangleTransform::TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const {
  // see the header file for a full explanation of what's going on here
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Calculating transform for :"
	   "("<<quadtangle.GetX0()<<","<<quadtangle.GetY0()<<"),"<<
	   "("<<quadtangle.GetX1()<<","<<quadtangle.GetY1()<<"),"<<
	   "("<<quadtangle.GetX2()<<","<<quadtangle.GetY2()<<"),"<<
	   "("<<quadtangle.GetX3()<<","<<quadtangle.GetY3()<<"),");	   
#endif

  // we particularly want coeffs to be an array of pointers to arrays
  // containing the rows of the matrix - then we can swap rows
  // conveniently by swapping pointers
  double coeffs0[] = {1,1,1,0,0,0,-quadtangle.GetX1(),-quadtangle.GetX1() };
  double coeffs1[] = {1,0,1,0,0,0,-quadtangle.GetX0(),0             };
  double coeffs2[] = {0,0,1,0,0,0,0            ,0             };
  double coeffs3[] = {0,1,1,0,0,0,0            ,-quadtangle.GetX2() };
  double coeffs4[] = {0,0,0,1,1,1,-quadtangle.GetY1(),-quadtangle.GetY1() };
  double coeffs5[] = {0,0,0,1,0,1,-quadtangle.GetY0(),0             };
  double coeffs6[] = {0,0,0,0,0,1,0            ,0             };
  double coeffs7[] = {0,0,0,0,1,1,0            ,-quadtangle.GetY2() };
  /*
  double coeffs0[] = {1,1,0,0,0,0,0,0};
  double coeffs1[] = {1,0,0,1,0,0,0,0};
  double coeffs2[] = {1,1,1,1,0,0,0,0};
  double coeffs3[] = {0,0,0,0,1,1,0,0};
  double coeffs4[] = {0,0,0,0,1,0,0,1};
  double coeffs5[] = {0,0,0,0,1,1,1,1};
  double coeffs6[] = {-quadtangle.GetX1(),-quadtangle.GetX0(),0,0,-quadtangle.GetY1(),-quadtangle.GetY0(),0,0};
  double coeffs7[] = {-quadtangle.GetX1(),0,0,-quadtangle.GetX2(),-quadtangle.GetY1(),0,0,-quadtangle.GetY2()};
  */
  double* coeffs[] = {coeffs0,
		      coeffs1,
		      coeffs2,
		      coeffs3,
		      coeffs4,
		      coeffs5,
		      coeffs6,
		      coeffs7};
		     
  double xvals[] = { quadtangle.GetX1(),
		     quadtangle.GetX0(),
		     quadtangle.GetX3(),
		     quadtangle.GetX2(),
		     quadtangle.GetY1(),
		     quadtangle.GetY0(),
		     quadtangle.GetY3(),
		     quadtangle.GetY2() };
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
  double c8 = 1.0/scalefactor;
#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Scale factor is "<<scalefactor);
#endif

  for(int i=0;i<8;i++) {
    result[i] *= c8;
  }

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
 
  // the final vector for the transform is simply the cross product of the first two
  double final[] = { result[3]*result[7] - result[6]*result[4],
		     result[6]*result[1] - result[0]*result[7],
		     result[0]*result[4] - result[3]*result[1] };
  

  //  transform[0] = result[0];  transform[1] = result[1];  transform[2] = final[0];  transform[3] = result[2];
  //  transform[4] = result[3];  transform[5] = result[4];  transform[6] = final[1];  transform[7] = result[5];
  //  transform[8] = result[6];  transform[9] = result[7];  transform[10]= final[2];  transform[11]= scalefactor;
  //  transform[12]= 0;          transform[13]= 0;          transform[14]= 0;  transform[15]= 1;

  // however, our tag co-ordinate frame is actually a square centred
  // on the origin with sides of length 2.  Therefore we need to scale
  // and translate our co-ordinates onto the square expected by these
  // equations

  // this corresponds to post-multiplying the transformation here by a translation of (0.5,0.5,0) and then a scaling of factor (0,5,0.5,1)

  //             [ 1 0 0 0.5 ]   [ 0.5 0   0 0 ]                 [ 0.5 0   0 0.5 ]
  // transform * [ 0 1 0 0.5 ] * [ 0   0.5 0 0 ]  =  transform * [ 0   0.5 0 0.5 ]
  //             [ 0 0 1 0   ]   [ 0   0   1 0 ]                 [ 0   0   1 0   ]
  //             [ 0 0 0 1   ]   [ 0   0   0 1 ]                 [ 0   0   0 1   ]
  
  transform[0] = result[0]/2; transform[1] = result[1]/2; transform[2] = final[0];  transform[3] = (result[0]+result[1])/2+result[2];
  transform[4] = result[3]/2; transform[5] = result[4]/2; transform[6] = final[1];  transform[7] = (result[3]+result[4])/2+result[5];
  transform[8] = result[6]/2; transform[9] = result[7]/2; transform[10] = final[2]; transform[11] = (result[6]+result[7])/2+c8;
  transform[12] = 0;          transform[13] = 0;          transform[14] = 0;  transform[15] = 1;



#ifdef SQUARE_TRANSFORM_DEBUG
  PROGRESS("Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";");
  PROGRESS("             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";");
  PROGRESS("             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";");
  PROGRESS("             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];");
#endif

}
