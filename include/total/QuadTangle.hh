/*
 * $Header$ 
 */
#ifndef QUADTANGLE_GUARD
#define QUADTANGLE_GUARD

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/Camera.hh>

#include <vector>
#include <list>

namespace Total {

  /**
   * A class to fit points to a quadtangle and encapsulate the result
   *
   * \todo regression test give it sets of points we see if it fits them properly
   */
  class QuadTangle{
  protected:
    float m_x0;
    float m_y0;
    float m_x1;
    float m_y1;
    float m_x2;
    float m_y2;
    float m_x3;
    float m_y3;

    float m_xc;
    float m_yc;

    

  public:
    QuadTangle();
    //  QuadTangle(const std::vector<float>& points, bool prev_fitted=false);
    QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3);
    
    bool Compare(const QuadTangle& o) const;
    void Draw(Image& image, const Camera& camera) const;

    inline float GetX0() const { return m_x0; }
    inline float GetY0() const { return m_y0; }
    inline float GetX1() const { return m_x1; }
    inline float GetY1() const { return m_y1; }
    inline float GetX2() const { return m_x2; }
    inline float GetY2() const { return m_y2; }
    inline float GetX3() const { return m_x3; }
    inline float GetY3() const { return m_y3; }


    QuadTangle(Socket& socket);
    int Save(Socket& socket) const;

    /**
     * \todo implement the check method
     */ 
    bool Check(const std::vector<float>& points) const {return true;};

    /**
     * This function returns a good starting guess for the normal
     * vector of the plane in variable n
     */
    bool EstimatePoseQuadrant(float n[3]) const;
  private:
    void swap(float *a, float *b);
    float find_angle(float x, float y, float cx, float cy);

  protected:
    inline void compute_central_point();
    void sort_points();
    //   virtual bool Fit(const std::vector<float>& points) = 0;

  };




  class QuadTangleFitter : public QuadTangle {
  public:
    QuadTangleFitter() : QuadTangle(), m_fitted(false) {};

    inline bool IsFitted() const { return m_fitted; }

    void Draw(Image& image,const Camera& camera) const {
      if (m_fitted) QuadTangle::Draw(image,camera);
    }

    bool Fit(const std::vector<float>& points, bool prev_fit) { //, const Image &image) = 0;
      if (prev_fit) return true;
      if (FitPoints(points) && Refine(points)) {m_fitted=true; return true;}
      return false;
    }
    
  protected:
    bool m_fitted;
    virtual bool FitPoints(const std::vector<float>& points) = 0;
    virtual bool Refine(const std::vector<float>& points) { return true; };
  };




  class CornerQuadTangle : public virtual QuadTangleFitter {
  protected:
    bool FitPoints(const std::vector<float>& points);
  };

  class ConvexHullQuadTangle : public virtual QuadTangleFitter {
  protected:

    /**
     * Take the set of points and form a convex hull
     * to smooth out quantization 'kinks'. Filter out
     * vertices clearly not corners based on angles.
     * What remains should be clusters of points at
     * the true corners. Take one from each cluster
     * as an _estimate_ of that corner position.
     */
    bool FitPoints(const std::vector<float>& points);

  private:
    
    /**
     * Check where a point p lies relative to a line
     * specified by the points l0 and l1.
     * >0 means p is on the left
     * =0 means on the line
     * <0 means to the right
     */
    float isLeft( const std::vector<float> &V, int l0, int l1, int p);
    
    /**
     * Get the convex hull of the n points in V. This assumes
     * H has been allocated to size n.
     * The _indexes_ to the points are then returned in H, and
     * the number of vertices (<=n) is also returned.
     */
    int ConvexHull(const std::vector<float> &V, int n, int* H);
  };


  class RegressionQuadTangle : public virtual QuadTangleFitter  {
  protected:
    /**
     * Take the set of points and form a convex hull
     * to smooth out quantization 'kinks'. Filter out
     * vertices clearly not corners based on angles.
     * What remains should be clusters of points at
     * the true corners. Take one from each cluster
     * as an _estimate_ of that corner position.
     * Use the estimates to split the original poly
     * up into 4 groups, one per side. Regress the
     * groups linearly and find the intersection of
     * the lines
     */
    bool Refine(const std::vector<float>& points);

  };




  class PolygonQuadTangle : public virtual QuadTangleFitter {
  private:
    /**
     * Compute the cosine of the angle between the vectors pq and qr
     */
    float DPAngle(const std::pair<float,float>& p, const std::pair<float,float>& q, const std::pair<float,float>& r);

    /**
     * Join two lists of vertexes together.  List 1 is of the form
     * [....,y,z] and List 2 of the form [a,b,...].  If the curvature of
     * yz to za is less than the threshold (or in our case the cosine of
     * the curvature is greater than the threshold) then delete point 'z'.
     * If the curvature of za to ab is less than the threshold then
     * delete point 'a'.
     *
     * fulllist is the list of all the working vertexes
     * start is an iterator pointing to the start of the list 1 window (inclusive)
     * mid is an iterator pointing at the end of the list 1 window (exclusive)
     * mid is also an iterator pointing at the start of the list 2 window (inclusive) 
     * this must be one place in the list past that pointed to by reverse_mid
     * end is an iterator pointing at the end of the list 2 window (exclusive)
     */
    void DPJoin(std::list<std::pair<float,float> >& fulllist,
		std::list<std::pair<float,float> >::iterator start,
		std::list<std::pair<float,float> >::iterator mid,
		std::list<std::pair<float,float> >::iterator end);

    /**
     * Split the list by finding the point on the contour that is
     * maximally far from the edge running from the first point to the
     * last point.  
     *
     * Returns an iterator pointing to the first value of the second list in the split
     *
     * fulllist is the working list of vertexes
     * start points to the first element in the list that our current window covers (inclusive)
     * end points to the end of our window (exclusive)
     */
    std::list<std::pair<float,float> >::iterator DPSplit(std::list<std::pair<float,float> >& fulllist,
							 std::list<std::pair<float,float> >::iterator start,
							 std::list<std::pair<float,float> >::iterator end);
    
    /**
     * Split the list, recurse on the two halves, and then join them
     * back together.  If split does not split the list - i.e. all the
     * points between the first and last are close enough to the line
     * then delete all the intervening points
     *
     * fulllist is the list of all vertexes
     * start is an iterator pointing at the start of our window (inclusive)
     * end is an iterator pointing at the end of our window (exlusive)
     */
    void DPRecurse(std::list<std::pair<float,float> >& fulllist,
		   std::list<std::pair<float,float> >::iterator start,
		   std::list<std::pair<float,float> >::iterator end);
      
    
  protected:
    bool FitPoints(const std::vector<float>& points);
  };



  class RegressConvexHullQuadTangle : public ConvexHullQuadTangle,
				      public RegressionQuadTangle 
  {};

  class RegressPolygonQuadTangle : public PolygonQuadTangle,
				   public RegressionQuadTangle 
  {};


}


#else
namespace Total {
  class QuadTangle;
}
#endif//QUADTANGLE_GUARD
