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
  class QuadTangle {
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

    bool m_fitted;

  public:
    QuadTangle();
    QuadTangle(const std::vector<float>& points, bool prev_fitted=false);
    QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3);
    inline bool IsFitted() const { return m_fitted; }
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
  private:
    void swap(float *a, float *b);
    float find_angle(float x, float y, float cx, float cy);

  protected:
    inline void compute_central_point();
    void sort_points();
    virtual bool Fit(const std::vector<float>& points) = 0;

  };

  class CornerQuadTangle : public QuadTangle {
  public:
    CornerQuadTangle();
    CornerQuadTangle(const std::vector<float>& points, bool prev_fitted=false);
  protected:
    bool Fit(const std::vector<float>& points);
  };

  class PolygonQuadTangle : public QuadTangle {
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
     * mid is also an iterator pointing at the start of the list 2 window (inclusive) this must be one place in the list past that pointed to by reverse_mid
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
      

  public:
    PolygonQuadTangle();
    PolygonQuadTangle(const std::vector<float>& points, bool prev_fitted=false);
  protected:
    bool Fit(const std::vector<float>& points);
  };
}


#else
namespace Total {
  class QuadTangle;
}
#endif//QUADTANGLE_GUARD
