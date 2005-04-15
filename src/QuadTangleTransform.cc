/**
 * $Header$
 */

#include <total/QuadTangleTransform.hh>
#include <total/gaussianelimination.hh>

#include <iostream>

#undef SQUARE_TRANSFORM_DEBUG
#undef NON_LINEAR_DEBUG

namespace Total {

  bool ProjectiveQuadTangleTransform::TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const {
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

    // [ 2 0 0 0 ]               [ 1 0 0 0.5 ]   [ 0.5 0     0   0 ]    [ 2 0 0 0 ]                [ 0.5 0   0   0.5 ]
    // [ 0 2 0 0 ] * transform * [ 0 1 0 0.5 ] * [ 0   0.5   0   0 ]  = [ 0 2 0 0 ] *  transform * [ 0   0.5 0   0.5 ]
    // [ 0 0 2 0 ]               [ 0 0 1 0   ]   [ 0   0     0.5 0 ]    [ 0 0 2 0 ]                [ 0   0   0.5 0   ]
    // [ 0 0 0 1 ]               [ 0 0 0 1   ]   [ 0   0     0   1 ]    [ 0 0 0 1 ]                [ 0   0   0   1   ]
  


    transform[0] = result[0]; transform[1] = result[1]; transform[2] = final[0];  transform[3] = (result[0]+result[1])+result[2]*2;
    transform[4] = result[3]; transform[5] = result[4]; transform[6] = final[1];  transform[7] = (result[3]+result[4])+result[5]*2;
    transform[8] = result[6]; transform[9] = result[7]; transform[10] = final[2]; transform[11] = (result[6]+result[7])+c8*2;
    transform[12] = 0;          transform[13] = 0;          transform[14] = 0;  transform[15] = 1;

#ifdef SQUARE_TRANSFORM_DEBUG
    PROGRESS("Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";");
    PROGRESS("             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";");
    PROGRESS("             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";");
    PROGRESS("             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];");
#endif
    return true;
  }




  // ReducedProjectiveQuadTangleTransform


  bool ReducedProjectiveQuadTangleTransform::TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const {
    // see the header file for a full explanation of what's going on here
#ifdef SQUARE_TRANSFORM_DEBUG
    PROGRESS("Calculating transform for :"
	     "("<<quadtangle.GetX0()<<","<<quadtangle.GetY0()<<"),"<<
	     "("<<quadtangle.GetX1()<<","<<quadtangle.GetY1()<<"),"<<
	     "("<<quadtangle.GetX2()<<","<<quadtangle.GetY2()<<"),"<<
	     "("<<quadtangle.GetX3()<<","<<quadtangle.GetY3()<<"),");	   
#endif


    // Set up a data params array
    float  p[10] = {quadtangle.GetX0(), quadtangle.GetY0(),
		    quadtangle.GetX1(), quadtangle.GetY1(),
		    quadtangle.GetX2(), quadtangle.GetY2(),
		    quadtangle.GetX3(), quadtangle.GetY3(),
		    0.0, 0.0};

    // Calculate the centre point
    float lambda = ( (p[1]-p[3])*(p[6]-p[2]) - (p[7]-p[3])*(p[0]-p[2]) ) /
      ( (p[4]-p[0])*(p[7]-p[3]) - (p[5]-p[1])*(p[6]-p[2]) );
  
    float Xc = p[0]+lambda*(p[4]-p[0]);
    float Yc = p[1]+lambda*(p[5]-p[1]);
   

    // we particularly want coeffs to be an array of pointers to arrays
    // containing the rows of the matrix - then we can swap rows
    // conveniently by swapping pointers

    double coeffs0[] = {-1, 1, 0, 0, quadtangle.GetX0(), -quadtangle.GetX0() };
    double coeffs1[] = { 1, 1, 0, 0,-quadtangle.GetX1(), -quadtangle.GetX1() };
    double coeffs2[] = { 1,-1, 0, 0,-quadtangle.GetX2(),  quadtangle.GetX2() };
    double coeffs3[] = { 0, 0,-1, 1, quadtangle.GetY0(), -quadtangle.GetY0() };
    double coeffs4[] = { 0, 0, 1, 1,-quadtangle.GetY1(), -quadtangle.GetY1() };
    double coeffs5[] = { 0, 0,-1,-1, quadtangle.GetY3(),  quadtangle.GetY3() };

    double* coeffs[] = {coeffs0,
			coeffs1,
			coeffs2,
			coeffs3,
			coeffs4,
			coeffs5};

   
    
    double xvals[] = { quadtangle.GetX0()-Xc,
		       quadtangle.GetX1()-Xc,
		       quadtangle.GetX2()-Xc,
		       quadtangle.GetY0()-Yc,
		       quadtangle.GetY1()-Yc,
		       quadtangle.GetY3()-Yc};



    double result[8];

    solve_simultaneous(xvals,coeffs,result,6);

    float c88 = 1.0 /sqrt(result[0]*result[0]+result[2]*result[2] + result[4]*result[4]);

    if (c88!=c88) return false;

    result[7] = result[5];
    result[6] = result[4];
    result[4] = result[3];
    result[3] = result[2];

    result[2] = Xc;
    result[5] = Yc;

    for (int i=0; i<8; i++) result[i]*=c88;


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
  

    transform[0] = result[1]; transform[1] = result[0]; transform[2] = final[0];  transform[3] = result[2];
    transform[4] = result[4]; transform[5] = result[3]; transform[6] = final[1];  transform[7] = result[5];
    transform[8] = result[7]; transform[9] = result[6]; transform[10] = final[2]; transform[11] = c88;
    transform[12] = 0;          transform[13] = 0;          transform[14] = 0;  transform[15] = 1;


#ifdef SQUARE_TRANSFORM_DEBUG
    PROGRESS("Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";");
    PROGRESS("             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";");
    PROGRESS("             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";");
    PROGRESS("             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];");
#endif
    return true;
  }






  
  // NLMSimplexQuadTangleTransform
#if defined(HAVE_GSL_MULTIMIN_H) and defined(HAVELIB_GSLCBLAS) and defined(HAVELIB_GSL)
  bool NLMSimplexQuadTangleTransform::TransformQuadTangle(const QuadTangle& q, float transform[16]) const {
    size_t iter = 0;
    int status;
  
    const gsl_multimin_fminimizer_type *T;
    gsl_multimin_fminimizer *s;
    

    // Set up a data params array
    float  p[10] = {q.GetX0(), q.GetY0(),
		    q.GetX1(), q.GetY1(),
		    q.GetX2(), q.GetY2(),
		    q.GetX3(), q.GetY3(),
		    0.0, 0.0};
  

    // Calculate the centre point
    float lambda = ( (p[1]-p[3])*(p[6]-p[2]) - (p[7]-p[3])*(p[0]-p[2]) ) /
      ( (p[4]-p[0])*(p[7]-p[3]) - (p[5]-p[1])*(p[6]-p[2]) );
  
    p[8] = p[0]+lambda*(p[4]-p[0]);
    p[9] = p[1]+lambda*(p[5]-p[1]);
  
    gsl_vector *x;
    gsl_vector *step;
    gsl_multimin_function errfunc;

    int nparam = 4;
  
    errfunc.f = &(Total::NLMSimplexQuadTangleTransform::NLMQuadFunc);
    errfunc.n = nparam;
    errfunc.params = &p;


  // Start by doing the linear approach
    ReducedProjectiveQuadTangleTransform pqtt;
    float t[16];
    pqtt.TransformQuadTangle(q,t);

    // Where does the z' point?
    float zx = t[2];
    float zy = t[6];
    float zz = -t[10];
    float mag = sqrt(zx*zx+zy*zy+zz*zz);
    zx/=mag; zy/=mag; zz/=mag;

    float m = sqrt(t[0]*t[0]+t[4]*t[4]+t[8]*t[8]);
    t[0]/=m; t[4]/=m; t[8]/=m;
    m = sqrt(t[1]*t[1]+t[5]*t[5]+t[9]*t[9]);
    t[1]/=m; t[5]/=m; t[9]/=m;
    m = sqrt(t[2]*t[2]+t[6]*t[6]+t[10]*t[10]);
    t[2]/=m; t[6]/=m; t[10]/=m;

    float alpha = atan(-t[8]/t[9]); // DATAN
    float beta =  -atan( t[8] / (sin(alpha)*t[10])); // DSINE
    float gamma = atan(t[2]/t[6]); // DATAN
    float zest = t[11];

    if (alpha!=alpha) alpha=0.0;
    if (beta!=beta) beta=0.0;
    if (gamma!=gamma) gamma=0.0;

   
    x = gsl_vector_alloc (nparam);
    gsl_vector_set (x, 0, zest);
    gsl_vector_set (x, 1, alpha);
    gsl_vector_set (x, 2, beta);
    gsl_vector_set (x, 3, gamma);

    // Characteristic steps differ 
    // for distances and angles
    step = gsl_vector_alloc (nparam);
    gsl_vector_set (step, 0, zest/2.0);
    gsl_vector_set (step, 1, 0.1);
    gsl_vector_set (step, 2, 0.1);
    gsl_vector_set (step, 3, 1.5);

  
    T = gsl_multimin_fminimizer_nmsimplex;
    s = gsl_multimin_fminimizer_alloc (T, nparam);
  
    gsl_multimin_fminimizer_set (s, &errfunc, x, step); 
  
    do
      {
	iter++;
	status = gsl_multimin_fminimizer_iterate (s);
      
	if (status)
	  break;
      
	status = gsl_multimin_test_size(s->size,1e-4); 
   
      }  while (status == GSL_CONTINUE && iter < 500);

    if (iter <500) {
      // Success
      float ca = cos(gsl_vector_get(s->x, 1)); // DCOS
      float sa = sin(gsl_vector_get(s->x, 1)); // DSINE
      float cb = cos(gsl_vector_get(s->x, 2)); // DCOS
      float sb = sin(gsl_vector_get(s->x, 2)); // DSINE
      float cg = cos(gsl_vector_get(s->x, 3)); // DCOS
      float sg = sin(gsl_vector_get(s->x, 3)); // DCOS
      float z = gsl_vector_get(s->x, 0);

      transform[0] = ca*cg - cb*sa*sg;
      transform[1] = sa*cg + cb*ca*sg;
      transform[2] = sg*sb;
    
      transform[4] = -sg*ca - cb*sa*cg;
      transform[5] = -sg*sa + cb*ca*cg;
      transform[6] = cg*sb;
    
      transform[8] = sb*sa;
      transform[9] = -sb*ca;
      transform[10] = cb;
    
      // Add in the 4th col (offsets)
      transform[3] = p[8]*z;
      transform[7] = p[9]*z;
      transform[11] = z;
    
      // Add in the 4th row (homogeneous coords)
      transform[12] = 0.0;
      transform[13] = 0.0;
      transform[14] = 0.0;
      transform[15] = 1.0;
 
      return true;
    }

    gsl_multimin_fminimizer_free (s);
    gsl_vector_free (x);
    return false;
  }



  double NLMSimplexQuadTangleTransform::NLMQuadFunc(const gsl_vector *v, void *params)
  {
    float *p = (float *) params;
    float X0, Y0, X1, Y1, X2, Y2, X3, Y3;
    float ca = cos(gsl_vector_get(v, 1)); // DCOS
    float sa = sin(gsl_vector_get(v, 1)); // DSINE
    float cb = cos(gsl_vector_get(v, 2)); // DCOS
    float sb = sin(gsl_vector_get(v, 2)); // DSINE
    float cg = cos(gsl_vector_get(v, 3)); // DCOS
    float sg = sin(gsl_vector_get(v, 3)); // DSINE
    
    float z = gsl_vector_get(v, 0);
    float x = p[8] * z;
    float y = p[9] * z;
    
    X0 = ((ca*cg - cb*sa*sg)*(-1.0) + (sa*cg + cb*ca*sg)*(1.0) + x) /
      (sb*sa*(-1.0) - sb*ca*(1.0) + z);
    Y0 = ((-sg*ca - cb*sa*cg)*(-1.0) + (-sg*sa + cb*ca*cg)*(1.0)+ y) /
      (sb*sa*(-1.0) - sb*ca*(1.0) + z);

    X1 = ((ca*cg - cb*sa*sg)*(-1.0) + (sa*cg + cb*ca*sg)*(-1.0) + x) /
      (sb*sa*(-1.0) - sb*ca*(-1.0) + z);
    Y1 = ((-sg*ca - cb*sa*cg)*(-1.0) + (-sg*sa + cb*ca*cg)*(-1.0) + y) /
      (sb*sa*(-1.0) - sb*ca*(-1.0) + z);

    X2 = ((ca*cg - cb*sa*sg)*(1.0) + (sa*cg + cb*ca*sg)*(-1.0) + x) /
      (sb*sa*(1.0) - sb*ca*(-1.0) + z);
    Y2 = ((-sg*ca - cb*sa*cg)*(1.0) + (-sg*sa + cb*ca*cg)*(-1.0) + y) /
      (sb*sa*(1.0) - sb*ca*(-1.0) + z);

    X3 = ((ca*cg - cb*sa*sg)*(1.0) + (sa*cg + cb*ca*sg)*(1.0) + x) /
      (sb*sa*(1.0) - sb*ca*(1.0) + z);
    Y3 = ((-sg*ca - cb*sa*cg)*(1.0) + (-sg*sa + cb*ca*cg)*(1.0) + y) /
      (sb*sa*(1.0) - sb*ca*(1.0) + z);
  
    float s1x = X1-X0; float s1y = Y1-Y0;
    float s2x = X2-X1; float s2y = Y2-Y1;
    float s3x = X3-X2; float s3y = Y3-Y2;
    float s4x = X0-X3; float s4y = Y0-Y3;

    float p1x = p[2]-p[0]; float p1y = p[3]-p[1];
    float p2x = p[4]-p[2]; float p2y = p[5]-p[3];
    float p3x = p[6]-p[4]; float p3y = p[7]-p[5];
    float p4x = p[0]-p[6]; float p4y = p[1]-p[7];
    
    float ca1 = (p1x*s1x + p1y*s1y) / (sqrt(p1x*p1x+p1y*p1y)*sqrt(s1x*s1x+s1y*s1y));
    float ca2 = (p2x*s2x + p2y*s2y) / (sqrt(p2x*p2x+p2y*p2y)*sqrt(s2x*s2x+s2y*s2y));
    float ca3 = (p3x*s3x + p3y*s3y) / (sqrt(p3x*p3x+p3y*p3y)*sqrt(s3x*s3x+s3y*s3y));
    float ca4 = (p4x*s4x + p4y*s4y) / (sqrt(p4x*p4x+p4y*p4y)*sqrt(s4x*s4x+s4y*s4y));

    float xmin = p[0];
    float xmax = p[0];
    float ymin = p[1];
    float ymax = p[1];
    for (int i=2; i<8; i+=2) {
      if (p[i] < xmin) xmin=p[i];
      if (p[i] > xmax) xmax=p[i];
      if (p[i+1] < ymin) ymin=p[i+1];
      if (p[i+1] > ymax) ymax=p[i+1];
    }
    float scale = ((ymax-ymin)>(xmax-xmin)) ? (ymax-ymin):(xmax-xmin);

    float f= (scale/5)/(cos(20.0/180.0*M_PI)-1);
    f*=f;

    // Sum of squares
    return (X0-p[0])*(X0-p[0]) + (Y0-p[1])*(Y0-p[1]) +
      (X1-p[2])*(X1-p[2]) + (Y1-p[3])*(Y1-p[3]) +
      (X2-p[4])*(X2-p[4]) + (Y2-p[5])*(Y2-p[5]) +
      (X3-p[6])*(X3-p[6]) + (Y3-p[7])*(Y3-p[7])  +
      f*(ca1-1.0)*(ca1-1.0) +
      f*(ca2-1.0)*(ca2-1.0) +
      f*(ca3-1.0)*(ca3-1.0) +
      f*(ca4-1.0)*(ca4-1.0);
  }
#endif



};
