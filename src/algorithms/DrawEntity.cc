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

namespace Cantag {

  bool DrawEntityImage::operator()(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) const {    
    for(int i=m_roi.minx;i<m_roi.maxx;++i) {
      for(int j=m_roi.miny;j<m_roi.maxy;++j) {
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

  bool DrawEntityContour::operator()(ContourEntity& contour) const {
    for(unsigned int i=0;i<contour.GetPoints().size();i+=2) {
      const float x0 = m_roi.ScaleX(contour.GetPoints()[i],m_image.GetWidth());
      const float y0 = m_roi.ScaleY(contour.GetPoints()[i+1],m_image.GetHeight());
      const float x1 = m_roi.ScaleX(contour.GetPoints()[(i+2) % contour.GetPoints().size()],m_image.GetWidth());
      const float y1 = m_roi.ScaleY(contour.GetPoints()[(i+3) % contour.GetPoints().size()],m_image.GetHeight());
      m_image.DrawLine(x0,y0,x1,y1,0,1);
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
			   0,1);
    
    m_image.DrawLine((int)m_roi.ScaleX(pts[8],m_image.GetWidth()),(int)m_roi.ScaleY(pts[9],m_image.GetHeight()),
		     (int)m_roi.ScaleX(pts[10],m_image.GetWidth()),(int)m_roi.ScaleY(pts[11],m_image.GetHeight()),
		     0,1);
    m_image.DrawLine((int)m_roi.ScaleX(pts[8],m_image.GetWidth()),(int)m_roi.ScaleY(pts[9],m_image.GetHeight()),
		     (int)m_roi.ScaleX(pts[12],m_image.GetWidth()),(int)m_roi.ScaleY(pts[13],m_image.GetHeight()),
		     0,1);
		     
    return true;    
  }
}

