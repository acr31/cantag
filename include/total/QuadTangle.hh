/*
 * $Header$ 
 */
#ifndef QUADTANGLE_GUARD
#define QUADTANGLE_GUARD

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/Camera.hh>

#include <vector>

namespace Total {
  /**
   * A class to fit points to a quadtangle and encapsulate the result
   *
   * \todo regression test give it sets of points we see if it fits them properly
   */
  class QuadTangle {
  private:
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
    inline void compute_central_point();
    void sort_points();
    void swap(float *a, float *b);
    float find_angle(float x, float y, float cx, float cy);
    void CornerFit(const std::vector<float>& points);

  };
}
#else
namespace Total {
  class QuadTangle;
}
#endif//QUADTANGLE_GUARD
