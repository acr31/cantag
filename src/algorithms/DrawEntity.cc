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

#include <cantag/algorithms/DrawEntity.hh>

#include <algorithm>
using std::max;
using std::min;
#include <iostream>
using std::cout;
using std::endl;

namespace Cantag {

  bool DrawEntityImage::operator()(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) const {
    for(float i=m_roi.minx;i<m_roi.maxx;++i) {
      for(float j=m_roi.miny;j<m_roi.maxy;++j) {
	m_image.DrawPixel(m_roi.ScaleX(i,m_image.GetWidth()),m_roi.ScaleY(j,m_image.GetHeight()),image.Sample(i,j));
      }
    }
    return true;
  }

  bool DrawEntityMonochrome::operator()(MonochromeImage& monochrome) const {
    for(int i=0;i<monochrome.GetWidth();++i) {
      for(int j=0;j<monochrome.GetHeight();++j) {
	m_image.DrawPixel(m_roi.ScaleX(i,m_image.GetWidth()),m_roi.ScaleY(j,m_image.GetHeight()),monochrome.GetPixel(i,j) ? 255 : 0);
      }
    }
    return true;
  }

  bool DrawEntityHoughHPS::operator()(HoughEntity& hough_entity) const {
    m_image.DrawPoint(int(hough_entity.GetAngle() * 180 / DBL_PI + 360 - m_start_angle) % 360,
		      int(hough_entity.GetPerpendicularDistance()),
		      Pixel<Pix::Fmt::Grey8>(min(float(255), 256 * hough_entity.GetAccumulator() / max(1, m_normalisation_factor))),
		      1 /*, m_roi*/);
    return true;
  }

  bool DrawEntityHoughCPS::operator()(HoughEntity& hough_entity) const {
    m_image.DrawPolarLine(hough_entity.GetPerpendicularDistance(), hough_entity.GetAngle(),
			  COLOUR_BLACK, 1 /*, m_roi*/);
    return true;
  }

  bool DrawEntityContour::operator()(ContourEntity& contour) const {
    // There should be an even number of points.
    assert(contour.GetPoints().size() % 2 == 0);

    for(unsigned int i=0;i<contour.GetPoints().size();i+=2) {
      m_image.DrawLine(contour.GetPoints()[i],
		       contour.GetPoints()[i+1],
		       contour.GetPoints()[(i+2) % contour.GetPoints().size()],
		       contour.GetPoints()[(i+3) % contour.GetPoints().size()],
		       COLOUR_BLACK, 1, m_roi);
    }
    return true;
  }

  bool DrawEntityConvexHull::operator()(const ContourEntity& contour, ConvexHullEntity& convex_hull) const {
    const std::vector<int>& indices = convex_hull.GetIndices();
    const std::vector<float>& points = contour.GetPoints();

    std::vector<float> hull_points;
    for (int i = 0; i < indices.size(); ++i) {
      int index = indices[i];
      hull_points.push_back(points[2 * index]);
      hull_points.push_back(points[2 * index + 1]);
    }

    for(unsigned int i=0;i<hull_points.size();i+=2) {
      m_image.DrawLine(hull_points[i],
		       hull_points[i+1],
		       hull_points[(i+2) % hull_points.size()],
		       hull_points[(i+3) % hull_points.size()],
		       COLOUR_BLACK, 1, m_roi);
    }

    return true;
  }

  bool DrawEntityTransform::operator()(TransformEntity& transform) const {
    float pts[] = {-1,-1,
		   -1,1,
		   1,1,
		   1,-1,
		   0,0,
		   1,0,
		   0,0.5		   
    };
    transform.GetPreferredTransform()->Apply(pts,7);
    m_camera.NPCFToImage(pts,7);
    m_image.DrawQuadTangle((int)m_roi.ScaleX(pts[0],m_image.GetWidth()),(int)m_roi.ScaleY(pts[1],m_image.GetHeight()),
			   (int)m_roi.ScaleX(pts[2],m_image.GetWidth()),(int)m_roi.ScaleY(pts[3],m_image.GetHeight()),
			   (int)m_roi.ScaleX(pts[4],m_image.GetWidth()),(int)m_roi.ScaleY(pts[5],m_image.GetHeight()),
			   (int)m_roi.ScaleX(pts[6],m_image.GetWidth()),(int)m_roi.ScaleY(pts[7],m_image.GetHeight()),
			   COLOUR_BLACK,1);
    
    m_image.DrawLine((int)m_roi.ScaleX(pts[8],m_image.GetWidth()),(int)m_roi.ScaleY(pts[9],m_image.GetHeight()),
		     (int)m_roi.ScaleX(pts[10],m_image.GetWidth()),(int)m_roi.ScaleY(pts[11],m_image.GetHeight()),
		     COLOUR_BLACK,1);
    m_image.DrawLine((int)m_roi.ScaleX(pts[8],m_image.GetWidth()),(int)m_roi.ScaleY(pts[9],m_image.GetHeight()),
		     (int)m_roi.ScaleX(pts[12],m_image.GetWidth()),(int)m_roi.ScaleY(pts[13],m_image.GetHeight()),
		     COLOUR_BLACK,1);
		     
    return true;    
  }
}

