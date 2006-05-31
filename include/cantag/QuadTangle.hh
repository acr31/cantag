/*
  Copyright (C) 2004 Andrew C. Rice & Robert K. Harle

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

  Email: acr31@cam.ac.uk  rkh23@cam.ac.uk
*/

/*
 * $Header$ 
 */
#ifndef QUADTANGLE_GUARD
#define QUADTANGLE_GUARD

#include <cantag/Config.hh>
#include <cantag/Image.hh>
#include <cantag/Camera.hh>

#include <vector>
#include <list>

namespace Cantag {

  /**
   * A class to fit points to a quadtangle and encapsulate the result
   *
   * \todo regression test give it sets of points we see if it fits them properly
   */
  class CANTAG_EXPORT QuadTangle{
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

    float m_fit_error;


    /**
     * The index into the contour for the first corner point - the x co-ordinate is at m_index_0 * 2 and then y at m_index_x * 2 +1
     */
    int m_index_0;

    /**
     * The index into the contour for the second corner point
     */
    int m_index_1;

    /**
     * The index into the contour for the thrid corner point
     */
    int m_index_2;

    /**
     * The index into the contour for the fourth corner point
     */
    int m_index_3;
    

  public:
    QuadTangle();
    QuadTangle(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3, int index_0, int index_1, int index_2, int index_3);

    void Set(float x0, float y0,float x1, float y1,float x2, float y2,float x3, float y3, int index_0, int index_1, int index_2, int index_3);
    
    bool Compare(const QuadTangle& o) const;
    void Draw(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera) const;
    
    inline void SetFitError(float fit_error) { m_fit_error = fit_error; }

    inline float GetFitError() const { return m_fit_error; }


    /**
     * Generate the points for this quadtangle and store the alternate
     * x and y in the given vector.  The parameter mid_points
     * specifies the number of points to generate between the corners
     * of the quadtangle - normally zero is sufficient because the
     * corners bound the straight lines which are invariant under
     * perspective transform.
     */
    void Draw(std::vector<float>& points, short mid_points = 0) const;

    inline float GetX0() const { return m_x0; }
    inline float GetY0() const { return m_y0; }
    inline float GetX1() const { return m_x1; }
    inline float GetY1() const { return m_y1; }
    inline float GetX2() const { return m_x2; }
    inline float GetY2() const { return m_y2; }
    inline float GetX3() const { return m_x3; }
    inline float GetY3() const { return m_y3; }

    inline int GetIndex0() const { return m_index_0; }
    inline int GetIndex1() const { return m_index_1; }
    inline int GetIndex2() const { return m_index_2; }
    inline int GetIndex3() const { return m_index_3; }

    bool CheckError(const std::vector<float>& points, float threshold) const;

    /**
     * This function returns a good starting guess for the normal
     * vector of the plane in variable n
     */
    bool EstimatePoseQuadrant(float n[3]) const;
  private:
    void swap(float *a, float *b);
    float find_angle(float x, float y, float cx, float cy);
    void Interpolate(std::vector<float>& points, float start, float end, float steps, short index) const;

  protected:
    inline void compute_central_point();
    void sort_points();
  };
}
#else
namespace Cantag {
  class QuadTangle;
}
#endif//QUADTANGLE_GUARD
