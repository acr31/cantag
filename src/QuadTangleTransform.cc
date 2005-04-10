/**
 * $Header$
 */

#include <total/QuadTangleTransform.hh>
#include <total/gaussianelimination.hh>

#include <iostream>

#undef SQUARE_TRANSFORM_DEBUG

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


  // BEGIN NonLinearQuadTangleTransform

  NonLinearQuadTangleTransform::NLQuadData::NLQuadData(const QuadTangle& q) : FitData(1), mQuad(&q) {
    FitData::Datum datum;
    datum.push_back(q.GetX0()); AddDatum(datum,0.0,0.001); datum.clear();
    datum.push_back(q.GetY0()); AddDatum(datum,0.0,0.001); datum.clear();
    datum.push_back(q.GetX1()); AddDatum(datum,0.0,0.001); datum.clear();
    datum.push_back(q.GetY1()); AddDatum(datum,0.0,0.001); datum.clear();
    datum.push_back(q.GetX2()); AddDatum(datum,0.0,0.001); datum.clear();
    datum.push_back(q.GetY2()); AddDatum(datum,0.0,0.001); datum.clear();
    datum.push_back(q.GetX3()); AddDatum(datum,0.0,0.001); datum.clear(); 
    datum.push_back(q.GetY3()); AddDatum(datum,0.0,0.001); datum.clear();
//     std::cout << "P " << q.GetX0() << " " << q.GetY0() << std::endl;
//     std::cout << "P " << q.GetX1() << " " << q.GetY1() << std::endl;
//     std::cout << "P " << q.GetX2() << " " << q.GetY2() << std::endl;
//     std::cout << "P " << q.GetX3() << " " << q.GetY3() << std::endl;
  }


  void NonLinearQuadTangleTransform::NLQuadFunction::InitialiseParameters(FitData *fd) {
    NLQuadData *data = dynamic_cast<NLQuadData *>(fd);

    // Zero the derivatives
    for (int i=0; i<6; i++) {
      mDeriv[i]=1.0;
    }

    // Find the centre point
    float cx = (data->GetData(0,0) + data->GetData(0,2) + data->GetData(0,4) + data->GetData(0,6))/4.0;
    float cy = (data->GetData(0,1) + data->GetData(0,3) + data->GetData(0,5) + data->GetData(0,7))/4.0;

    mXseq[0] = 1; 
    mYseq[0] = 1;
    
    if ( (data->GetData(0,0)-cx) < 0.0) mXseq[0]=-1;
    if ( (data->GetData(0,1)-cy) < 0.0) mYseq[0]=-1;

    int s=0;
    if ( mXseq[0]==-1 && mYseq[0]==-1) s=1;
    if ( mXseq[0]==1 && mYseq[0]==-1)  s=2;
    if ( mXseq[0]==1 && mYseq[0]==1)   s=3;

    if (s==0) {
      mXseq[1]=-1; mYseq[1]=-1;
      mXseq[2]=1; mYseq[2]=-1;
      mXseq[3]=1; mYseq[3]=1;
    }
    else if (s==1) {
      mXseq[1]=1; mYseq[1]=-1;
      mXseq[2]=1; mYseq[2]=1;
      mXseq[3]=-1; mYseq[3]=1;
    }
    else if (s==2) {
      mXseq[1]=1; mYseq[1]=1;
      mXseq[2]=-1; mYseq[2]=1;
      mXseq[3]=-1; mYseq[3]=-1; 
    }
    else if (s==3) {
      mXseq[1]=-1; mYseq[1]=1;
      mXseq[2]=-1; mYseq[2]=-1;
      mXseq[3]=1; mYseq[3]=-1;
    }

   

    // Start by doing the linear approach
    ProjectiveQuadTangleTransform pqtt;
    float t[16];
    pqtt.TransformQuadTangle(*(data->GetQuadTangle()),t);

    // Where does the z' point?
    float zx = t[2];
    float zy = t[6];
    float zz = t[10];
    float mag = sqrt(zx*zx+zy*zy+zz*zz);
    zx/=mag; zy/=mag; zz/=mag;

    float m = sqrt(t[0]*t[0]+t[4]*t[4]+t[8]*t[8]);
    t[0]/=m; t[4]/=m; t[8]/=m;
    m = sqrt(t[1]*t[1]+t[5]*t[5]+t[9]*t[9]);
    t[1]/=m; t[5]/=m; t[9]/=m;
    m = sqrt(t[2]*t[2]+t[6]*t[6]+t[10]*t[10]);
    t[2]/=m; t[6]/=m; t[10]/=m;

    mParams[3] = atan(-t[8]/t[9]);
    mParams[4] = atan( t[8] / (sin(mParams[3])*t[10]));
    mParams[5] = atan(t[2]/t[6]);

    // Add in location
    mParams[0] = t[3];
    mParams[1] = t[7];
    mParams[2] = t[11];

    for (int i=3; i<6; i++) if (mParams[i]!=mParams[i]) mParams[i]=0.0;
  }

  REAL  NonLinearQuadTangleTransform::NLQuadFunction::Evaluate(const int i, REAL *p, FitData *fd) {
     NLQuadData *d = dynamic_cast<NLQuadData *>(fd);

    // Wrap angles
    while (p[3]>M_PI) p[3]-=2*M_PI;
    while (p[3]<=-M_PI) p[3]+=2*M_PI;
    while (p[4]>M_PI) p[4]-=2*M_PI;
    while (p[4]<=-M_PI) p[4]+=2*M_PI;
    while (p[5]>M_PI) p[5]-=2*M_PI;
    while (p[5]<=-M_PI) p[5]+=2*M_PI;

    // cosine, sine of euler angles
    REAL ca = cos(p[3]);
    REAL sa = sin(p[3]);
    REAL cb = cos(p[4]);
    REAL sb = sin(p[4]);
    REAL cg = cos(p[5]);
    REAL sg = sin(p[5]);

    REAL x = p[0];
    REAL y = p[1];
    REAL z = p[2];

    // Fit a dataset point to a specific corner
    REAL u,v,w;
    if (i==0 || i==1) {
      u=(REAL)mXseq[0];
      v=(REAL)mYseq[0];
      w=0;
    }
    else if (i==2 || i==3) {
      u=(REAL)mXseq[1];
      v=(REAL)mYseq[1];
      w=0;
    }
    else if (i==4 || i==5) {
      u=(REAL)mXseq[2];
      v=(REAL)mYseq[2];
      w=0;
    }
    else if (i==6 || i==7) {
      u=(REAL)mXseq[3];
      v=(REAL)mYseq[3];
      w=0;
    }

    // Calculate the true position of the corner for the
    // current parameters
    REAL xx = (ca*cg - cb*sa*sg)*u + (sa*cg + cb*ca*sg)*v + sg*sb*w + x;
    REAL yy = (-sg*ca - cb*sa*cg)*u + (-sg*sa + cb*ca*cg)*v + (cg*sb)*w + y;
    REAL zz = sb*sa*u - sb*ca*v + cb*w + z;

    // Form partial derivatives
    REAL dxxdx = 1.0;
    REAL dxxdy = 0.0;
    REAL dxxdz = 0.0;
    REAL dxxda = (-sa*cg - cb*ca*sg)*u + (ca*cg - cb*sa*sg)*v;
    REAL dxxdb = (sb*sa*sg)*u - (sb*ca*sg)*v + sg*cb*w;
    REAL dxxdg = (-ca*sg - cb*sa*cg)*u + (-sa*sg + cb*ca*cg)*v + cg*sb*w;

    REAL dyydx = 0.0;
    REAL dyydy = 1.0;
    REAL dyydz = 0.0;
    REAL dyyda = (sg*sa - cb*ca*cg)*u + (-sg*ca - cb*sa*cg)*v;
    REAL dyydb = (sb*sa*cg)*u + (-sb*ca*cg)*v + (cg*cb)*w;
    REAL dyydg = (-cg*ca + cb*sa*sg)*u + (-cg*sa - cb*ca*sg)*v + (-sg*sb)*w;

    REAL dzzdx = 0.0;
    REAL dzzdy = 0.0;
    REAL dzzdz = 1.0;
    REAL dzzda = sb*ca*u + sb*sa*v;
    REAL dzzdb = cb*sa*u - cb*ca*v - sb*w;
    REAL dzzdg = 0.0;


    // Data is striped x0,y0,x1,y1,...
    if (i%2==0) {
      // Calculate projection and derivatives for X
      mDeriv[0] = dxxdx/zz - xx*dzzdx/(zz*zz);
      mDeriv[1] = dxxdy/zz - xx*dzzdy/(zz*zz);
      mDeriv[2] = dxxdz/zz - xx*dzzdz/(zz*zz);
      mDeriv[3] = dxxda/zz - xx*dzzda/(zz*zz);
      mDeriv[4] = dxxdb/zz - xx*dzzdb/(zz*zz);
      mDeriv[5] = dxxdg/zz - xx*dzzdg/(zz*zz);
      //      std::cout << xx/zz - d->GetData(0,i) << std::endl;
      return xx/zz - d->GetData(0,i);
    }
    else {
      // Calculate projection and derivatives for Y
      mDeriv[0] = dyydx/zz - yy*dzzdx/(zz*zz);
      mDeriv[1] = dyydy/zz - yy*dzzdy/(zz*zz);
      mDeriv[2] = dyydz/zz - yy*dzzdz/(zz*zz);
      mDeriv[3] = dyyda/zz - yy*dzzda/(zz*zz);
      mDeriv[4] = dyydb/zz - yy*dzzdb/(zz*zz);
      mDeriv[5] = dyydg/zz - yy*dzzdg/(zz*zz);
      //   std::cout << "R " << i << " " << yy/zz <<" " << d->GetData(0,i) << " " << yy/zz - d->GetData(0,i) << std::endl;
      return yy/zz - d->GetData(0,i);
    }
  }


  bool NonLinearQuadTangleTransform::TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const {
    
    NLQuadData nlcd(quadtangle);
    NLQuadFunction nlcf;
    nlcf.InitialiseParameters(&nlcd);

    NonLinearModel nlm(&nlcf, &nlcd);
    try {
      // Fit with up to 1000 iterations
      // Stop if the % change in chi squared
      // is less that 0.1%
      nlm.Fit(100,0.0001);
    }
    catch (SingularMatrix &e) {
      // If we've done a few iterations and suddenly got a singular matrix
      // this result may be acceptable

      if (nlm.GetNumIter()<2 || nlm.GetStdErr()> 6.0) {
	std::cerr << "NLM failed: " << e.what() << std::endl;
	return false;
      }
    }
    catch (NLMAPException &e) {
      std::cerr << "NLM failed: " << e.what() << std::endl;
      return false;
    }


    REAL *p= nlcf.GetParams();
    
    
    //   std::cout << "Result " << p[0] << " " << p[1] << " " << p[2] << " " << p[3]/M_PI*180 << " " << p[4]/M_PI*180 << " " << p[5]/M_PI*180 << std::endl;
    
    REAL ca = cos(p[3]);
    REAL sa = sin(p[3]);
    REAL cb = cos(p[4]);
    REAL sb = sin(p[4]);
    REAL cg = cos(p[5]);
    REAL sg = sin(p[5]);
    
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
    transform[3] = p[0];
    transform[7] = p[1];
    transform[11] = p[2];
    
    // Add in the 4th row (homogeneous coords)
    transform[12] = 0.0;
    transform[13] = 0.0;
    transform[14] = 0.0;
    transform[15] = 1.0;

    REAL u,v,w=0.0;
    REAL x,y;
    
  //   u=1.0; v=1.0;
//     nlcf.GetPointProj(u,v,w,&x,&y);
//     std::cout << "R " << x << " " << y << std::endl;
//     u=-1.0; v=1.0;
//     nlcf.GetPointProj(u,v,w,&x,&y);
//     std::cout << "R " << x << " " << y << std::endl;
//     u=-1.0; v=-1.0;
//     nlcf.GetPointProj(u,v,w,&x,&y);
//     std::cout << "R " << x << " " << y << std::endl;
//     u=1.0; v=-1.0;
//     nlcf.GetPointProj(u,v,w,&x,&y);
//     std::cout << "R " << x << " " << y << std::endl;
    return true;
  }


  void NonLinearQuadTangleTransform::NLQuadFunction::GetPoint3D(REAL u, REAL v, REAL w, REAL *xo, REAL *yo, REAL *zo) {
    REAL ca = cos(mParams[3]);
    REAL sa = sin(mParams[3]);
    REAL cb = cos(mParams[4]);
    REAL sb = sin(mParams[4]);
    REAL cg = cos(mParams[5]);
    REAL sg = sin(mParams[5]);
    
    REAL x = mParams[0];
    REAL y = mParams[1];
    REAL z = mParams[2];

    REAL xx = (ca*cg - cb*sa*sg)*u + (sa*cg + cb*ca*sg)*v + sg*sb*w + x;
    REAL yy = (-sg*ca - cb*sa*cg)*u + (-sg*sa + cb*ca*cg)*v + (cg*sb)*w + y;
    REAL zz = sb*sa*u - sb*ca*v + cb*w + z;

    *xo = xx;
    *yo = yy;
    *zo = zz;
  }
  
  void NonLinearQuadTangleTransform::NLQuadFunction::GetPointProj(REAL u, REAL v, REAL w, REAL *xo, REAL *yo) {
    REAL ca = cos(mParams[3]);
    REAL sa = sin(mParams[3]);
    REAL cb = cos(mParams[4]);
    REAL sb = sin(mParams[4]);
    REAL cg = cos(mParams[5]);
    REAL sg = sin(mParams[5]);

    REAL x = mParams[0];
    REAL y = mParams[1];
    REAL z = mParams[2];

    REAL xx = (ca*cg - cb*sa*sg)*u + (sa*cg + cb*ca*sg)*v + sg*sb*w + x;
    REAL yy = (-sg*ca - cb*sa*cg)*u + (-sg*sa + cb*ca*cg)*v + (cg*sb)*w + y;
    REAL zz = sb*sa*u - sb*ca*v + cb*w + z;

    *xo=xx/zz;
    *yo=yy/zz;
  }


}
