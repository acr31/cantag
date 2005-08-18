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

#ifndef DRAW_ENTITY_GUARD
#define DRAW_ENTITY_GUARD

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Camera.hh>
#include <cantag/Image.hh>


namespace Cantag {

  class DrawEntityContour : public Function<TL0,TL1(ContourEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
  public:
    DrawEntityContour(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image) {}
    bool operator()(ContourEntity& contourentity) const ;
  };

  template<class Shape>
  class DrawEntityShape : public Function<TL0,TL1(ShapeEntity<Shape>)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const Camera& m_camera;
  public:
    DrawEntityShape(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image,const Camera& camera) : m_image(image), m_camera(camera) {}
    bool operator()(ShapeEntity<Shape>& contourentity) const ;
  };

  class DrawEntityTransform : public Function<TL0,TL1(TransformEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const Camera& m_camera;
  public:
    DrawEntityTransform(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera) : m_image(image), m_camera(camera) {}
    bool operator()(TransformEntity& transform) const;
  };

  template<class Shape> bool DrawEntityShape<Shape>::operator()(ShapeEntity<Shape>& shape) const {
    std::vector<int> points;
    shape.GetShape()->Draw(points,m_camera);
    for(std::vector<int>::const_iterator i = points.begin();
	i != points.end();
	++i) {
      const int x = *i;
      ++i;
      const int y = *i;
      
      m_image.DrawPixel(x,y,0);
    }  
    return true;    
  }



}

#endif
