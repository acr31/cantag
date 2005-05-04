/**
 * $Header$
 */

#include <total/QuadTangleTransform.hh>
#include <total/gaussianelimination.hh>
#include <total/SpeedMath.hh>

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

    double scalefactor;

    scalefactor=sqrt(result[0]*result[0]+result[3]*result[3]+result[6]*result[6]);
    // Alternative scale factor:
    //scalefactor=sqrt(result[1]*result[1]+result[4]*result[4]+result[7]*result[7]);
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

  //   std::cout << "Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";"<< std::endl;
//     std::cout << "             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";"<< std::endl;
//     std::cout << "             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";"<< std::endl;
//     std::cout << "             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];"<< std::endl;

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






  
  // SpaceSearchQuadTangleTransform
#if defined(HAVE_GSL_MULTIMIN_H) and defined(HAVELIB_GSLCBLAS) and defined(HAVELIB_GSL)
  bool SpaceSearchQuadTangleTransform::TransformQuadTangle(const QuadTangle& q, float transform[16]) const {
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
  
    errfunc.f = &(Total::SpaceSearchQuadTangleTransform::SpaceSearchQuadFunc);
    errfunc.n = nparam;
    errfunc.params = &p;

    //  Start by getting an estimate to help with ambiguities
    // I'm too lazy to compute the same starting conditions as I 
    // did with the Cyber algorithm so I'll just use that
    // for the moment: it should make no difference other
    // than to slow things down!
 
    float n[3]={0.0};

    EstimatePoseQuadrant(p,n);

    // Store where the z' points
    float ezx = n[0]; //t[2];
    float ezy = n[1]; //t[6];
    float ezz = n[2]; //t[10];
    float mag = sqrt(ezx*ezx+ezy*ezy+ezz*ezz);
    ezx/=mag; ezy/=mag; ezz/=mag;

    float zest = 10.0;
    float alpha=0.0;
    float beta = 0.0;
    float gamma  = 0.0;

    x = gsl_vector_alloc (nparam);
    gsl_vector_set (x, 0, zest);
    gsl_vector_set (x, 1, alpha);
    gsl_vector_set (x, 2, beta);
    gsl_vector_set (x, 3, gamma);

    // Characteristic steps differ 
    // for distances and angles
    step = gsl_vector_alloc (nparam);
    gsl_vector_set (step, 0, zest/2.0);
    gsl_vector_set (step, 1, 1.5);
    gsl_vector_set (step, 2, 1.5);
    gsl_vector_set (step, 3, 1.5);

  
    T = gsl_multimin_fminimizer_nmsimplex;
    s = gsl_multimin_fminimizer_alloc (T, nparam);
  
    gsl_multimin_fminimizer_set (s, &errfunc, x, step); 
  
    do {
      iter++;
      status = gsl_multimin_fminimizer_iterate (s);
      if (status)
	break;      
      status = gsl_multimin_test_size(s->size,1e-4);

    }  while (status == GSL_CONTINUE && iter < 500);

    if (iter <500) {
      // Converged
      float z = gsl_vector_get(s->x, 0);
      float alpha = gsl_vector_get(s->x, 1);
      float beta  = gsl_vector_get(s->x, 2);
      float gamma = gsl_vector_get(s->x, 3);

      // Wrap the angles around
      while (alpha >  M_PI) alpha -=  2*M_PI;
      while (alpha < -M_PI) alpha +=  2*M_PI;
      while (beta  >  M_PI) beta  -=  2*M_PI;
      while (beta  < -M_PI) beta  +=  2*M_PI;
      while (gamma >  M_PI) gamma -=  2*M_PI;
      while (gamma < -M_PI) gamma +=  2*M_PI;
      

      // Now figure out the transform
      float ca = DCOS(8,alpha); 
      float sa = DSIN(8,alpha);
      float cb = DCOS(8,beta); 
      float sb = DSIN(8,beta);
      float cg = DCOS(8,gamma);
      float sg = DSIN(8,gamma);
      

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

    
      gsl_multimin_fminimizer_free (s);
      gsl_vector_free (x);
      return true;
    }

    gsl_multimin_fminimizer_free (s);
    gsl_vector_free (x);
    return false;
  }



  double SpaceSearchQuadTangleTransform::SpaceSearchQuadFunc(const gsl_vector *v, void *params)
  {
    float *p = (float *) params;
    float X0, Y0, X1, Y1, X2, Y2, X3, Y3, XC, YC;

    float z = gsl_vector_get(v, 0);
    float alpha = gsl_vector_get(v, 1);
    float beta  = gsl_vector_get(v, 2);
    float gamma = gsl_vector_get(v, 3);
    
    // Wrap the angles around
    while (alpha >  M_PI) alpha -=  2*M_PI;
    while (alpha < -M_PI) alpha +=  2*M_PI;
    while (beta  >  M_PI) beta  -=  2*M_PI;
    while (beta  < -M_PI) beta  +=  2*M_PI;
    while (gamma >  M_PI) gamma -=  2*M_PI;
    while (gamma < -M_PI) gamma +=  2*M_PI;
    
    
    // Now figure out the transform
    float ca = DCOS(8,alpha); 
    float sa = DSIN(8,alpha);
    float cb = DCOS(8,beta); 
    float sb = DSIN(8,beta);
    float cg = DCOS(8,gamma);
    float sg = DSIN(8,gamma);
          
    float x = p[8] * z;
    float y = p[9] * z;


    // Projections of what we currently have
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

    // Centre point of current
    float lambda = ( (Y0-Y1)*(X3-X1) - (Y3-Y1)*(X0-X1) ) /
      ( (X2-X0)*(Y3-Y1) - (Y2-Y0)*(X3-X1) );
  
    XC = X0+lambda*(X2-X0);
    YC = Y0+lambda*(Y2-Y0);

  
 //    // Vectors of projected sides
//     float s1x = X1-X0; float s1y = Y1-Y0;
//     float s2x = X2-X1; float s2y = Y2-Y1;
//     float s3x = X3-X2; float s3y = Y3-Y2;
//     float s4x = X0-X3; float s4y = Y0-Y3;

//     // Actual vectors of sides
//     float p1x = p[2]-p[0]; float p1y = p[3]-p[1];
//     float p2x = p[4]-p[2]; float p2y = p[5]-p[3];
//     float p3x = p[6]-p[4]; float p3y = p[7]-p[5];
//     float p4x = p[0]-p[6]; float p4y = p[1]-p[7];
    
//     // Angle betweeen actual and current
//     float ca1 = (p1x*s1x + p1y*s1y) / (sqrt(p1x*p1x+p1y*p1y)*sqrt(s1x*s1x+s1y*s1y));
//     float ca2 = (p2x*s2x + p2y*s2y) / (sqrt(p2x*p2x+p2y*p2y)*sqrt(s2x*s2x+s2y*s2y));
//     float ca3 = (p3x*s3x + p3y*s3y) / (sqrt(p3x*p3x+p3y*p3y)*sqrt(s3x*s3x+s3y*s3y));
//     float ca4 = (p4x*s4x + p4y*s4y) / (sqrt(p4x*p4x+p4y*p4y)*sqrt(s4x*s4x+s4y*s4y));

//     // Scaling/weighting factors for angles and distances
//     float xmin = p[0];
//     float xmax = p[0];
//     float ymin = p[1];
//     float ymax = p[1];
//     for (int i=2; i<8; i+=2) {
//       if (p[i] < xmin) xmin=p[i];
//       if (p[i] > xmax) xmax=p[i];
//       if (p[i+1] < ymin) ymin=p[i+1];
//       if (p[i+1] > ymax) ymax=p[i+1];
//     }
//     float scale = ((ymax-ymin)>(xmax-xmin)) ? (ymax-ymin):(xmax-xmin);

//     float f= (scale/5)/(cos(20.0/180.0*M_PI)-1);
//     f*=f;

    // Sum of squares
    return (X0-p[0])*(X0-p[0]) + (Y0-p[1])*(Y0-p[1]) +
      (X1-p[2])*(X1-p[2]) + (Y1-p[3])*(Y1-p[3]) +
      (X2-p[4])*(X2-p[4]) + (Y2-p[5])*(Y2-p[5]) +
      (X3-p[6])*(X3-p[6]) + (Y3-p[7])*(Y3-p[7])  +
      (XC-p[8])*(XC-p[8]) + (YC-p[8])*(YC-p[9]);
  }




  bool EstimatePoseQuadrant(float *p, float *n) {
    // Now find where a vertical line through the centre intersects the shape
    int side1=0;
    float mu = (p[8]-p[2*side1])/(p[2*(side1+1)]-p[2*side1]);
    if (mu!=mu || mu <=0.0 || mu > 1.0) {
      // It doesn't intersect this side: must intersect the next
      side1++;
      mu = (p[8]-p[2*side1])/(p[2*(side1+1)]-p[2*side1]);
    }
    int side2=(side1+2)%4;
    float mu2 = (p[8]-p[2*side2])/(p[2*(side2+1)]-p[2*side2]);

    // y co-ordinates: one will be -ve, one +ve
    float d1 = p[2*side1+1] + mu*(p[2*(side1+1)+1]-p[2*side1+1]) - p[9];
    float d2 = p[2*side2+1] + mu2*(p[2*(side2+1)+1]-p[2*side2+1]) - p[9];
    float yratio = (d1>0) ? -d1/d2:-d2/d1;

    // Now find where a horizontal line through the centre intersects the shape
    int sideh = side1+1;
    mu = (p[9]-p[2*sideh+1])/(p[2*(sideh+1)+1]-p[2*sideh+1]);
    d1 = p[2*sideh] + mu*(p[2*(sideh+1)]-p[2*sideh]) - p[8];
    
    sideh = (sideh+2)%4;
    mu = (p[9]-p[2*sideh+1])/(p[2*((sideh+1)%4)+1]-p[2*sideh+1]);
    d2 = p[2*sideh] + mu*(p[2*((sideh+1)%4)]-p[2*sideh]) - p[8];

    float xratio = (d1>0) ? -d1/d2:-d2/d1;
 
 //    if (xratio>0.99 && xratio<1.01 && yratio>0.99 && yratio<1.01) {
//       // It's a rectangle
//       float size1 = (p[0]-p[2])*(p[0]-p[2]) + (p[1]-p[3])*(p[1]-p[3]);
//       float size2 = (p[2]-p[4])*(p[2]-p[4]) + (p[3]-p[5])*(p[3]-p[5]);
//       // if (size1/size2 !=1.0) return false;
//     }


    if (xratio > 1.0)  n[0]=1.0;
    else if (xratio == 1.0) n[0]=0.0;
    else n[1]=-1.0;

    if (yratio > 1.0) n[1]=1.0;
    else if (yratio == 1.0) n[1]=0.0;
    else n[1]=-1.0;

    n[2]=1.0;

    float m = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    for (int i=0; i<3;i++) n[i]/=m;
    return true;
  }




  bool PlaneRotationQuadTangleTransform::TransformQuadTangle(const QuadTangle& q, float transform[16]) const {

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
  

    // Calculate the cross of the diagonals
    float lambda = ( (p[1]-p[3])*(p[6]-p[2]) - (p[7]-p[3])*(p[0]-p[2]) ) /
      ( (p[4]-p[0])*(p[7]-p[3]) - (p[5]-p[1])*(p[6]-p[2]) );
    p[8] = p[0]+lambda*(p[4]-p[0]);
    p[9] = p[1]+lambda*(p[5]-p[1]);


    float n[3]={0.0};

    EstimatePoseQuadrant(p,n);


    float phistart=0.0;
    if (n[0]<=0.0 && n[1]>=0.0) {
      phistart = 7.0/4.0*M_PI;
    }
    else if (n[0]<0.0 && n[1]<0.0) {
      phistart = M_PI/4.0;
    }
    else if (n[0] > 0.0 && n[1] > 0.0) {
      phistart = 5.0/4.0*M_PI;
    }
    else {
      phistart = 3.0/4.0*M_PI;
    }

    gsl_vector *x;
    gsl_vector *step;
    gsl_multimin_function errfunc;

    int nparam = 2;
  
    errfunc.f = &(Total::PlaneRotationQuadTangleTransform::PlaneRotationQuadFunc);
    errfunc.n = nparam;
    errfunc.params = &p;
   
    x = gsl_vector_alloc (nparam);
    gsl_vector_set (x, 0, M_PI/4.0*3.0);
    gsl_vector_set (x, 1, phistart);

    // Characteristic steps 
    step = gsl_vector_alloc (nparam);
    gsl_vector_set (step, 0, M_PI/4.0);
    gsl_vector_set (step, 1, M_PI/2.0);
    //   gsl_vector_set (step, 0, M_PI/4.0);
    //  gsl_vector_set (step, 1, M_PI);
  
    T = gsl_multimin_fminimizer_nmsimplex;
    s = gsl_multimin_fminimizer_alloc (T, nparam);
  
    gsl_multimin_fminimizer_set (s, &errfunc, x, step); 
    float pts[12] = {0.0, 0.0};
 
    do {
      iter++;
      status = gsl_multimin_fminimizer_iterate (s);
      if (status) break;

      // There is a danger that theta ~ 180 and phi
      // is being moved randomly without effect
      if (gsl_vector_get(s->x, 0)/M_PI > 0.98 && 
	  PlaneRotationQuadFunc(s->x,p) < 1e-3) break;;

      status = gsl_multimin_test_size(s->size,1e-5);
    }  while (status == GSL_CONTINUE && iter < 500);
    

    if (iter <500) {
      
      // Converged
      ComputeCameraPointsFromAngles(s->x,p,pts,n);
      // Choose a side as the x axis of the tag
      float xaxis[3] = { pts[6]-pts[3],
			 pts[7]-pts[4],
			 pts[8]-pts[5] };
      float yaxis[3] = { pts[0]-pts[3],
			 pts[1]-pts[4],
			 pts[2]-pts[5] };
      float m=0.0;
      
     
      m=sqrt (xaxis[0]*xaxis[0]+
	      xaxis[1]*xaxis[1]+
	      xaxis[2]*xaxis[2]);

      // Alternative Scale factor
      // 	m = sqrt (yaxis[0]*yaxis[0]+
      // 		yaxis[1]*yaxis[1]+
      // 		yaxis[2]*yaxis[2]);
    
      for (int i=0; i<3; i++) xaxis[i]/=m;
      for (int i=0; i<3; i++) yaxis[i]/=m;
	
      // We want m to be 2.0
      transform[0]  = xaxis[0];
      transform[4]  = xaxis[1];
      transform[8]  = xaxis[2];
    
      transform[1]  = yaxis[0];
      transform[5]  = yaxis[1];
      transform[9]  = yaxis[2];
    
      transform[2]  = n[0];
      transform[6]  = n[1];
      transform[10] = n[2];
    
      // Add in the 4th col (offsets)
      transform[3]  = p[8]*2.0/m;
      transform[7]  = p[9]*2.0/m;
      transform[11] = 2.0/m;
    
      // Add in the 4th row (homogeneous coords)
      transform[12] = 0.0;
      transform[13] = 0.0;
      transform[14] = 0.0;
      transform[15] = 1.0;

      gsl_multimin_fminimizer_free (s);
      gsl_vector_free (x);
      return true;
    }
  
    gsl_multimin_fminimizer_free (s);
    gsl_vector_free (x);
    return false;

  };



  bool  PlaneRotationQuadTangleTransform::ComputeCameraPointsFromAngles(const gsl_vector *v, void *vp, float *pts, float *n) {
    float *p = (float *)vp;
    // v contains the theta/phi for spherical polars
    float theta = gsl_vector_get(v, 0);
    float phi   = gsl_vector_get(v, 1);
    
    // theta=0, phi=0 points along 1,0,0
    float nx = -DSIN(8,theta)*DCOS(8,phi);
    float ny = -DSIN(8,theta)*DSIN(8,phi);
    float nz = -DCOS(8,theta);

    // std::cout << theta/M_PI*180.0 << " " << phi/M_PI*180.0 << " " << nx << " " << ny << " " << nz << std::endl;
    
    float Xc = p[8];
    float Yc = p[9];
    
    for (int i=0; i<4; i++) {
      float lambda = (Xc*nx + Yc*ny + nz) / 
	(p[i*2]*nx + p[i*2+1]*ny +nz);
      pts[3*i]   = lambda*p[2*i];
      pts[3*i+1] = lambda*p[2*i+1];
      pts[3*i+2] = lambda;
      // if (lambda <= 1e-05) return false;
    }

    n[0] = nx;
    n[1] = ny;
    n[2] = nz;
    return true;
  } 



  double PlaneRotationQuadTangleTransform::PlaneRotationQuadFunc(const gsl_vector *v, void *vp) {
    float *p = (float *)vp;
    float pts[12]={0.0};
    float n[3]={0.0};
    if (!ComputeCameraPointsFromAngles(v,p,pts,n)) return 1000.0;

    // Compute the vectors for the four sides
    float s[18]={0.0};

    for (int i=0; i<12; i+=3) {
      // vector from this to next
      s[i] = pts[i] - pts[(i+3)%12];
      s[i+1] = pts[i+1] - pts[(i+4)%12];
      s[i+2] = pts[i+2] - pts[(i+5)%12];
    }


    // Add diagonals
    s[12]  = pts[0]-pts[6];
    s[13]  = pts[1]-pts[7];
    s[14]  = pts[2]-pts[8];
    s[15]  = pts[3]-pts[9];
    s[16]  = pts[4]-pts[10];
    s[17]  = pts[5]-pts[11];
    
    float sumsq=0.0;
    for (int i=0; i<12; i+=3) {
      float dotprod = s[i]*s[(i+3)%12] + 
	s[i+1]*s[(i+4)%12] +
	s[i+2]*s[(i+5)%12];
      float m1 = sqrt(s[i]*s[i]+s[i+1]*s[i+1] + s[i+2]*s[i+2]);
      float m2 = sqrt(s[(i+3)%12]*s[(i+3)%12] + s[(i+4)%12]*s[(i+4)%12] + s[(i+5)%12]*s[(i+5)%12]);
      sumsq += dotprod*dotprod/(m1*m1*m2*m2);
    }

    float diagdp = (s[12]*s[15] +
      s[13]*s[16] +
      s[14]*s[17])/ (sqrt(s[12]*s[12]+s[13]*s[13]+s[14]*s[14])*sqrt(s[15]*s[15]+s[16]*s[16]+s[17]*s[17]));
    sumsq += diagdp*diagdp;
    return sumsq;
  }

#endif



};
