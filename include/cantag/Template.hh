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

#ifndef TEMPLATE_GUARD
#define TEMPLATE_GUARD

#include <cantag/Config.hh>
#include <cantag/Image.hh>
#include <cantag/Camera.hh>

namespace Cantag {

  /**
   * A template code.  Handles correlation with other templates and the
   * like.
   *
   * \todo{regression test: create various images which we expect various correlation coefficients from and try them}
   */
  class Template {
  private:
    float m_average;
    unsigned int m_size;
    unsigned int m_subsample;
    unsigned char* m_values;
    float m_sigma;
    Image* m_original;

  public:
    char* m_filename;
    Template(char* filename, int size, int subsample);
    ~Template();
    float Correlate(const Image& image, const float transform[16], const Camera& camera) const;
    float Correlate(const Template& t) const;
    bool operator<(const Template& t);
    void Draw2D(Image& image);
  private:
    void calculate_mean_sigma(const unsigned char* values, float* stddev, float* mean) const;
    float Correlate(const unsigned char* values) const;
  };
}

#endif//TEMPLATE_GUARD
