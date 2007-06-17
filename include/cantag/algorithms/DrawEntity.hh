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
#include <cantag/TagCircle.hh>
#include <cantag/TagSquare.hh>
#include <cantag/entities/ContourEntity.hh>
#include <cantag/entities/ConvexHullEntity.hh>
#include <cantag/entities/HoughEntity.hh>
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Camera.hh>
#include <cantag/Image.hh>
#include <cantag/ROI.hh>

#include <map>
using std::multimap;

namespace Cantag {

  class CANTAG_EXPORT DrawEntityImage : public Function<TL0,TypeList<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>,TypeListEOL> > {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const ROI m_roi;
  public:
    DrawEntityImage(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {};
    DrawEntityImage(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {};
    bool operator()(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) const;
  };

  class CANTAG_EXPORT DrawEntityMonochrome : public Function<TL0,TL1(MonochromeImage)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const ROI m_roi;
  public:
    DrawEntityMonochrome(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {};
    DrawEntityMonochrome(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {};
    bool operator()(MonochromeImage& monimage) const ;    
  };

  /** HPS = Hough parameter space */
  class CANTAG_EXPORT DrawEntityHoughHPS : public Function<TL0, TL1(HoughEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    float m_normalisation_factor;
    int m_start_angle;
    const ROI m_roi;
  public:
    DrawEntityHoughHPS(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, float normalisation_factor, int start_angle, ROI roi) : m_image(image), m_normalisation_factor(normalisation_factor), m_start_angle(start_angle), m_roi(roi) {};
    DrawEntityHoughHPS(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, float normalisation_factor, int start_angle) : m_image(image), m_normalisation_factor(normalisation_factor), m_start_angle(start_angle), m_roi(0,image.GetWidth(),0,image.GetHeight()) {};
    bool operator()(HoughEntity& hough_entity) const;
  };

  /** CPS = Cartesian parameter space */
  class CANTAG_EXPORT DrawEntityHoughCPS : public Function<TL0, TL1(HoughEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>* m_original;
    //const ROI m_roi;
    multimap<float, HoughEntity> m_acc_lines;
  public:
    //DrawEntityHoughCPS(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {};
    //DrawEntityHoughCPS(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {};
    DrawEntityHoughCPS(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Image<Pix::Sze::Byte1, Pix::Fmt::Grey8>* original = NULL) : m_image(image), m_original(original) {};
    bool operator()(HoughEntity& hough_entity);
    void Draw() const;
  };

  class CANTAG_EXPORT DrawEntityContour : public Function<TL0,TL1(ContourEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const ROI m_roi;
  public:
    DrawEntityContour(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {}
    DrawEntityContour(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {}
    bool operator()(ContourEntity& contourentity) const ;
  };

  class CANTAG_EXPORT DrawEntityConvexHull : public Function<TL1(ContourEntity), TL1(ConvexHullEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const ROI m_roi;
  public:
    DrawEntityConvexHull(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {}
    DrawEntityConvexHull(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {}
    bool operator()(const ContourEntity& contour, ConvexHullEntity& convex_hull) const;
  };

  template<class Shape>
  class DrawEntityShape : public Function<TL0,TL1(ShapeEntity<Shape>)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const Camera& m_camera;
    const ROI m_roi;
  public:
    DrawEntityShape(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image,const Camera& camera,ROI roi) : m_image(image), m_camera(camera),m_roi(roi) {}
    DrawEntityShape(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image,const Camera& camera) : m_image(image), m_camera(camera),m_roi(0,image.GetWidth(),0,image.GetHeight()) {}
    bool operator()(ShapeEntity<Shape>& contourentity) const ;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  class DrawEntitySampleCircleObj : public Function<TL0,TL1(TransformEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const Camera& m_camera;
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    const ROI m_roi;
  public:
    DrawEntitySampleCircleObj(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, ROI roi) : m_image(image), m_camera(camera), m_tagspec(tagspec), m_roi(roi) {}
    DrawEntitySampleCircleObj(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec) : m_image(image), m_camera(camera), m_tagspec(tagspec),m_roi(0,image.GetWidth(),0,image.GetHeight()) {}
    bool operator()(TransformEntity& transform) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool DrawEntitySampleCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(TransformEntity& transform) const {
    const Transform* i = transform.GetPreferredTransform();
    if (i) {
      int readindex = READ_COUNT/2;
      for(int j=0;j<SECTOR_COUNT;++j) {
	// read a chunk by sampling each ring and shifting and adding
	for(int k=0;k<RING_COUNT;++k) {
	  float tpt[]=  {  m_tagspec.GetXSamplePoint(readindex,RING_COUNT - 1 - k),
			   m_tagspec.GetYSamplePoint(readindex,RING_COUNT - 1 - k) };
	  i->Apply(tpt[0],tpt[1],tpt,tpt+1);
	  m_camera.NPCFToImage(tpt,1);
	  int coordx = m_roi.ScaleX(tpt[0],m_image.GetWidth());
	  int coordy = m_roi.ScaleY(tpt[1],m_image.GetHeight());	  
	  m_image.DrawPixel(coordx,coordy,0);
	}
	readindex+=READ_COUNT;
	readindex %= SECTOR_COUNT * READ_COUNT;
      }      
    }
    return true;
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  DrawEntitySampleCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> DrawEntitySample(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, ROI roi) { 
    return DrawEntitySampleCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(image,camera,tagspec,roi);
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  DrawEntitySampleCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> DrawEntitySample(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec) { 
    return DrawEntitySampleCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(image,camera,tagspec);
  }

  template<int EDGE_CELLS>
  class DrawEntitySampleSquareObj : public Function<TL0,TL1(TransformEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const Camera& m_camera;
    const TagSquare<EDGE_CELLS>& m_tagspec;
    const ROI m_roi;
  public:
    DrawEntitySampleSquareObj(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagSquare<EDGE_CELLS>& tagspec, ROI roi) : m_image(image), m_camera(camera), m_tagspec(tagspec), m_roi(roi) {}
    DrawEntitySampleSquareObj(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagSquare<EDGE_CELLS>& tagspec) : m_image(image), m_camera(camera), m_tagspec(tagspec),m_roi(0,image.GetWidth(),0,image.GetHeight()) {}
    bool operator()(TransformEntity& transform) const;
  };

  
  template<int EDGE_CELLS> bool DrawEntitySampleSquareObj<EDGE_CELLS>::operator()(TransformEntity& transform) const {
    const Transform* i = transform.GetPreferredTransform();
    const int payloadsize = EDGE_CELLS*EDGE_CELLS - (EDGE_CELLS*EDGE_CELLS %2);
    if (i) {
      for(int j=0;j<payloadsize;j++) {
	float pts[] = { m_tagspec.GetXSamplePoint(j),
			m_tagspec.GetYSamplePoint(j) };
	i->Apply(pts[0],pts[1],pts,pts+1);
	m_camera.NPCFToImage(pts,1);
	int coordx = m_roi.ScaleX(pts[0],m_image.GetWidth());
	int coordy = m_roi.ScaleY(pts[1],m_image.GetHeight());	  
	m_image.DrawPixel(coordx,coordy,0);

	// use this one to show the sample order
	//m_image.DrawPixel(coordx,coordy, j*255/payloadsize);
      }
    }
    return true;
  }

  template<int EDGE_CELLS>
  inline
  DrawEntitySampleSquareObj<EDGE_CELLS> DrawEntitySample(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagSquare<EDGE_CELLS>& tagspec, ROI roi) { 
    return DrawEntitySampleSquareObj<EDGE_CELLS>(image,camera,tagspec,roi);
  }

  template<int EDGE_CELLS>
  inline
  DrawEntitySampleSquareObj<EDGE_CELLS> DrawEntitySample(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagSquare<EDGE_CELLS>& tagspec) { 
    return DrawEntitySampleSquareObj<EDGE_CELLS>(image,camera,tagspec);
  }

  class CANTAG_EXPORT DrawEntityTransform : public Function<TL0,TL1(TransformEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const Camera& m_camera;
    const ROI m_roi;
  public:
    DrawEntityTransform(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, ROI roi) : m_image(image), m_camera(camera),m_roi(roi) {}
    DrawEntityTransform(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera) : m_image(image), m_camera(camera),m_roi(0,image.GetWidth(),0,image.GetHeight()) {}
    bool operator()(TransformEntity& transform) const;
  };

  template<class Shape> bool DrawEntityShape<Shape>::operator()(ShapeEntity<Shape>& shape) const {
    std::vector<float> points;
    shape.GetShape()->Draw(points);
    //    m_camera.NPCFToImage(points);
    std::vector<float>::const_iterator i = points.begin();
    float firstx = *(i++);
    float firsty = *(i++);
    float currentx = firstx;
    float currenty = firsty;
    while(i != points.end()) {
      const float x = *(i++);
      const float y = *(i++);      
      m_image.DrawLine(m_roi.ScaleX(currentx,m_image.GetWidth()),m_roi.ScaleY(currenty,m_image.GetHeight()),
		       m_roi.ScaleX(x,m_image.GetWidth()),m_roi.ScaleY(y,m_image.GetHeight()),
		       0,1);
      currentx = x;
      currenty = y;
    }
    m_image.DrawLine(m_roi.ScaleX(currentx,m_image.GetWidth()),m_roi.ScaleY(currenty,m_image.GetHeight()),
		     m_roi.ScaleX(firstx,m_image.GetWidth()),m_roi.ScaleY(firsty,m_image.GetHeight()),
		     0,1);
    return true;    
  }

}

#endif
