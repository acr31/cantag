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
 */

#include <total/algorithms/TransformQuadTangleProjective.hh>
#include <total/gaussianelimination.hh>

#undef PROJECTIVE_TRANSFORM_DEBUG
namespace Total {

  bool TransformQuadTangleProjective::operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& dest) const {
    
    const QuadTangle& quadtangle = *(shape.m_shapeDetails);

    // see the header file for a full explanation of what's going on here
#ifdef PROJECTIVE_TRANSFORM_DEBUG
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

#ifdef PROJECTIVE_TRANSFORM_DEBUG
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
#ifdef PROJECTIVE_TRANSFORM_DEBUG
    PROGRESS("Scale factor is "<<scalefactor);
#endif

    for(int i=0;i<8;i++) {
      result[i] *= c8;
    }

#ifdef PROJECTIVE_TRANSFORM_DEBUG
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
  

    Transform* t = new Transform(1.f);
    dest.GetTransforms().push_back(t);

    Transform& transform = *t;

    
    transform[0] = result[0]; transform[1] = result[1]; transform[2] = final[0];  transform[3] = (result[0]+result[1])+result[2]*2;
    transform[4] = result[3]; transform[5] = result[4]; transform[6] = final[1];  transform[7] = (result[3]+result[4])+result[5]*2;
    transform[8] = result[6]; transform[9] = result[7]; transform[10] = final[2]; transform[11] = (result[6]+result[7])+c8*2;
    transform[12] = 0;          transform[13] = 0;          transform[14] = 0;  transform[15] = 1;

#ifdef PROJECTIVE_TRANSFORM_DEBUG
    PROGRESS("Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";");
    PROGRESS("             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";");
    PROGRESS("             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";");
    PROGRESS("             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];");
#endif
    return true;

  }

}
