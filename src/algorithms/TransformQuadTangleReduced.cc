/**
 * $Header$
 */

#include <total/algorithms/TransformQuadTangleReduced.hh>
#include <total/gaussianelimination.hh>

#undef REDUCED_TRANSFORM_DEBUG
namespace Total {

  bool TransformQuadTangleReduced::operator()(const ShapeEntity<QuadTangle>& shape, TransformEntity& dest) const {
    
    const QuadTangle& quadtangle = *(shape.m_shapeDetails);

    // see the header file for a full explanation of what's going on here
#ifdef REDUCED_TRANSFORM_DEBUG
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


#ifdef REDUCED_TRANSFORM_DEBUG
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
  
    Transform* t = new Transform(1.f);
    dest.GetTransforms().push_back(t);

    Transform& transform = *t;

    transform[0] = result[1]; transform[1] = result[0]; transform[2] = final[0];  transform[3] = result[2];
    transform[4] = result[4]; transform[5] = result[3]; transform[6] = final[1];  transform[7] = result[5];
    transform[8] = result[7]; transform[9] = result[6]; transform[10] = final[2]; transform[11] = c88;
    transform[12] = 0;          transform[13] = 0;          transform[14] = 0;  transform[15] = 1;


#ifdef REDUCED_TRANSFORM_DEBUG
    PROGRESS("Final trans=[" << transform[0] << "," << transform[1] << "," << transform[2] << ","<<transform[3] <<";");
    PROGRESS("             " << transform[4] << "," << transform[5] << "," << transform[6] << ","<<transform[7] <<";");
    PROGRESS("             " << transform[8] << "," << transform[9] << "," << transform[10]<< ","<<transform[11]<<";");
    PROGRESS("             " << transform[12]<< "," << transform[13]<< "," << transform[14]<< ","<<transform[15]<<"];");
#endif
    return true;

  }

}
