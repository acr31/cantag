/**
 * $Header$
 */

#ifndef QUADTANGLE_TRANSFORM_GUARD
#define QUADTANGLE_TRANSFORM_GUARD

#include <total/Config.hh>
#include <total/QuadTangle.hh>

#if defined(HAVE_GSL_MULTIMIN_H) and defined(HAVELIB_GSLCBLAS) and defined(HAVELIB_GSL)
#include <gsl/gsl_multimin.h>
#endif


namespace Total {
  class QuadTangleTransform {
  public:
    virtual bool TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const = 0;
  };


  /*
   * This function takes the four points of a
   * quadrilateralin array p such that p[0]=x0, p[1]=y0,
   * etc. It returns as good starting guess for the normal
   * vector of the plane in variable n
   */
  bool EstimatePoseQuadrant(float *p, float *n);



  class ProjectiveQuadTangleTransform  : public virtual QuadTangleTransform {
  public:
    /*
     * Taken from:
     *
     * @InProceedings{ip:apchi:rekimoto98,
     *  author        = "Jun Rekimoto",
     *  title         = "Matrix: A Realtime Object Identification and Registration Method for Augmented Reality",
     *  booktitle     = "Proceedings of Asia Pacific Computer Human Interaction",
     *  year          = "1998",
     * }
     *
     *
     * If the co-ordinates relative to the matrix are (u,v).  Then the
     * co-ordinates in the camera frame of reference are (x,y,z)
     *
     * x = c0 * u + c1 * v + c2
     * y = c3 * u + c4 * v + c5
     * z = c6 * u + c7 * v + c8
     *
     * There is nothing clever about the above - they are just an
     * arbitrary linear combination of u and v
     *
     * Our screen co-ordinates X and Y are a perspective projection of (x,y,z)
     *
     * X = x/z
     * Y = y/z
     *
     * Since both of the equations are over z we can divide through by c8 to give
     *
     *  X = (a0*u + a1*v + a2)/(a6*u+a7*v+1)
     *  Y = (a3*u + a4*v + a5)/(a6*u+a7*v+1)
     *
     * Where a0 = c0/c8, a1 = c1/c8,  etc...
     *
     * Given four points on the matrix and their points in the final
     * image we can set up a set of simultaneous linear equations and
     * solve for a0 to a7
     *
     * ( X0 )   (  u0  v0  1   0   0  0  -X0*u0  -X0*v0 ) ( a0 )
     * ( X1 )   (  u1  v1  1   0   0  0  -X1*u1  -X1*v1 ) ( a1 )
     * ( X2 )   (  u2  v2  1   0   0  0  -X2*u2  -X2*v2 ) ( a2 )
     * ( X3 )   (  u3  v3  1   0   0  0  -X3*u3  -X3*v3 ) ( a3 )
     * ( Y0 ) = (   0   0  0  u0  v0  1  -Y0*u0  -Y0*v0 ) ( a4 )
     * ( Y1 )   (   0   0  0  u1  v1  1  -Y1*u1  -Y1*v1 ) ( a5 )
     * ( Y2 )   (   0   0  0  u2  v2  1  -Y2*u2  -Y2*v2 ) ( a6 )
     * ( Y3 )   (   0   0  0  u3  v3  1  -Y3*u3  -Y3*y3 ) ( a7 )
     *
     * In the above (X_n,Y_n) are pairs of screen co-ordinates that
     * correspond to (u_n,v_n) pairs of co-ordinates on the tag
     *
     * We have found four points - the four corners which we have sorted
     * so that we have
     *
     * (x3,y3) +----------+ (x0,y0)
     *         |          |
     *         |          |
     *         |          |
     *         |          |
     * (x2,y2) +----------+ (x1,y1)
     *
     * Our screen co-ordinate origin is top left so:
     *
     * x0,y0 -> 1,0 (tag co-ordinates)
     * x1,y1 -> 1,1
     * x2,y2 -> 0,1
     * x3,y3 -> 0,0
     *
     * So we can now substitute x_n,y_n (known tag co-ordinates from
     * above) and X_n,Y_n (known screen co-ordinates from quadtangle into
     * the above matrix.  Do this in such a way that the matrix will be
     * easy to invert.
     *
     * x0,y0 -> u1,v1
     * x1,y1 -> u0,v0
     * x2,y2 -> u3,v3
     * x3,y3 -> u2,v2
     *
     * This gives (remembering to swap X and Y's too)
     *
     * ( X1 )   (  x1  y1  1   0   0  0  -X1*x1  -X1*y1 ) ( a0 )
     * ( X0 )   (  x0  y0  1   0   0  0  -X0*x0  -X0*y0 ) ( a1 )
     * ( X3 )   (  x3  y3  1   0   0  0  -X3*x3  -X3*y3 ) ( a2 )
     * ( X2 )   (  x2  y2  1   0   0  0  -X2*x2  -X2*y2 ) ( a3 )
     * ( Y1 ) = (   0   0  0  x1  y1  1  -Y1*x1  -Y1*y1 ) ( a4 )
     * ( Y0 )   (   0   0  0  x0  y0  1  -Y0*x0  -Y0*y0 ) ( a5 )
     * ( Y3 )   (   0   0  0  x3  y3  1  -Y3*x3  -Y3*y3 ) ( a6 )
     * ( Y2 )   (   0   0  0  x2  y2  1  -Y2*x2  -Y2*y2 ) ( a7 )
     *
     * which simplifies (because we know x_n,y_n) to:
     *
     * ( X1 )   (  1  1  1   0   0  0  -X1  -X1 ) ( a0 )
     * ( X0 )   (  1  0  1   0   0  0  -X0    0 ) ( a1 )
     * ( X3 )   (  0  0  1   0   0  0    0    0 ) ( a2 )
     * ( X2 )   (  0  1  1   0   0  0    0  -X2 ) ( a3 )
     * ( Y1 ) = (  0  0  0   1   1  1  -Y1  -Y1 ) ( a4 )
     * ( Y0 )   (  0  0  0   1   0  1  -Y0    0 ) ( a5 )
     * ( Y3 )   (  0  0  0   0   0  1    0    0 ) ( a6 )
     * ( Y2 )   (  0  0  0   0   1  1    0  -Y2 ) ( a7 )
     *
     *
     * To build a homogenous transform matrix from these parameters we need c0..c8
     *  
     * We already know a0..a7 which correspond to c1/c8...c8/c8
     * We know r = the ratio width/height = 1
     * 
     * x = c0 * u + c1 * v + c2
     * y = c3 * u + c4 * v + c5
     * z = c6 * u + c7 * v + c8 
     *
     * So to work out c8 we set u=v=0 this gives:
     *
     * x0 = c2
     * y0 = c5
     * z0 = c8
     *
     * Then set u=0,v=1 this gives:
     *
     * x1 = c1 + c2
     * y1 = c4 + c5
     * z1 = c7 + c8
     *
     * The distance between these two positions is the height of the tag:
     *
     * (x0-x1)^2 + (y0-y1)^2 + (z0-z1)^2 = 1
     *
     *  c1^2     +  c4^2     +  c7^2     = 1
     *
     *  c1^2        c4^2        c7^2          1
     *  ----     +  ----     +  ----     =  ----
     *  c8^2        c8^2        c8^2        c8^2
     *
     *  a1^2     +  a4^2     +  a7^2     =  1/c8^2
     *
     * So:
     *
     * c0 = a0 / sqrt(a1^2+a4^2+a7^2)
     * c1 = a1 / sqrt(a1^2+a4^2+a7^2)
     * etc...
     *
     * and
     * c8 = 1/sqrt(a1^2     +  a4^2     +  a7^2)
     *
     * This then gives the transform matrix as
     *
     * (  x  )    ( c0  c1  0 c2 ) ( u )
     * (  y  ) =  ( c3  c4  0 c5 ) ( v )
     * (  z  )    ( c6  c7  0 c8 ) ( w )
     * (  w  )    (  0   0  0  1 ) ( 1 )
     *
     * The final matrix is created using the cross product
     * of the first two columns to fill in the 3rd
     */
    bool TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const;
  };




  // \todo: optimise the matrix before inversion
  class ReducedProjectiveQuadTangleTransform  : public virtual QuadTangleTransform {
  public:
    /*
     * This algorithm is as the same as
     * ProjectiveQuadTangleTransform, but makes
     * better use of the information available
     * to reduce the parameter set.
     *
     * Given the quadtangle, calculate the point that
     * the square centre maps to (the intersection of
     * the diagonals). This is a projection Xc=x/z, Yc=y/z
     * for a tag at (x,y,z). Thus, given Xc and Yc, the tag
     * location is completely determined by z!
     *
     * Following the same analysis above, we find:
     * (X-Xc) = a0*u + a1*v - a5*u*X - a6*v*X
     * (Y-Yc) = a3*u + a4*v - a5*u*Y - a6*v*Y
     *
     * And so we solve for a0, a1, a3, a4, a5, a6
     * and calculate:
     *
     * c8 = 1.0/ sqrt( a0*a0 + a3*a3 +a5*a5 )
     *
     * which then gives us all the params we had before,
     * but has only required inverting a 6x6 matrix rather
     * than an 8x8
     */
    bool TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const;
  };




#if defined(HAVE_GSL_MULTIMIN_H) and defined(HAVELIB_GSLCBLAS) and defined(HAVELIB_GSL)
  /**
   * Perform transform based on assertion that tag is exactly
   * square in real life.  Solves for four parameters: z, alpha,
   * beta, gamma, where the three angles are standarad euler angles
   */
  class SpaceSearchQuadTangleTransform : public virtual QuadTangleTransform {
  public:
    /*
     * Static function so it can be passed as a function pointer
     * to GNU Scientific Library
     */
    static double SpaceSearchQuadFunc(const gsl_vector *v, void *params);

    /*
     * Calculate the transform by least squares minimising the 
     * euclidean distance for each point AND the angle between the
     * sides.
     *
     * This transform assumes the tag is square (the correct thing to do)
     * and moves it in space to find a best fit to the input corner points.
     * 
     * Position is fully defined by the depth distance, z, whilst pose is
     * completely defined by the Euler angles alpha, beta, gamma --
     * see http://mathworld.wolfram.com/EulerAngles.html
     * 
     * Therefore this is a minimisation wrt (z,alpha,beta,gamma).
     *
     * This version uses the downhill simplex minimisation
     * found in the GNU Scientific Library because that does
     * not require derivatives, and more importantly, can have 
     * the search space easily restricted
     */
    bool TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const;
  };



  class PlaneRotationQuadTangleTransform : public virtual QuadTangleTransform {
  public:
    /*
     * Static function so it can be passed as a function pointer
     * to GNU Scientific Library
     */
    static double PlaneRotationQuadFunc(const gsl_vector *v, void *params);

    /*
     * Helper function
     */
    static bool ComputeCameraPointsFromAngles(const gsl_vector *v, void *p, float *pts, float *n);

    /*
     * An implementation of the CyberCode algorithm
     * The details are scant in the paper and the maths
     * that is quoted is not actually correct/optimal
     *
     * The idea is to separate out determination of the
     * normal from determination of distance since
     * this is simply a scaling factor
     *
     * Thus we select a normal vector n and place a plane
     * with this normal at the point (Xc, Yc, 1) which is 
     * where the quad diagonals intersect. The normal is
     * represented by two spherical polar angles, theta and
     * phi
     *
     * We then look at the points P where the rays passing
     * through the corners {X,Y} intersect the plane. We compute
     * the dot products of the sides defined by P and add on
     * the dot product of the diagonals. If P is a square all
     * these angles are 90 degrees and the sum evaluates as zero.
     * Hence we minimise the sum to estimate the normal.
     *
     * To calculate the distance, we simply measure the length
     * of one of the sides defined by P and find the factor that
     * makes it 2.0
     */
    bool TransformQuadTangle(const QuadTangle& quadtangle, float transform[16]) const;
  };

#endif

}
#endif//QUADTANGLE_TRANSFORM_GUARD
