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
#include <cantag/entities/ShapeEntity.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/Camera.hh>
#include <cantag/Image.hh>


namespace Cantag {

  struct ROI {
    int minx;
    int maxx;
    int miny;
    int maxy;

    ROI(int pminx, int pmaxx, int pminy, int pmaxy) : minx(pminx), maxx(pmaxx), miny(pminy), maxy(pmaxy) {};
    int ScaleX(int x, int imageWidth) const { return (x - minx) * imageWidth / (maxx-minx);}
    float ScaleX(float x, int imageWidth) const { return (x - (float)minx) * (float)imageWidth / (float)(maxx-minx); }
    int ScaleY(int y, int imageHeight) const { return (y - miny) * imageHeight / (maxy-miny);}
    float ScaleY(float y, int imageHeight) const { return (y - (float)miny) * (float)imageHeight / (float)(maxy-miny); }
  };

  class DrawEntityImage : public Function<TL0,TypeList<Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>,TypeListEOL> > {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const ROI m_roi;
  public:
    DrawEntityImage(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {};
    DrawEntityImage(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {};
    bool operator()(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) const;
  };

  class DrawEntityMonochrome : public Function<TL0,TL1(MonochromeImage)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const ROI m_roi;
  public:
    DrawEntityMonochrome(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {};
    DrawEntityMonochrome(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {};
    bool operator()(MonochromeImage& monimage) const ;    
  };

  class DrawEntityContour : public Function<TL0,TL1(ContourEntity)> {
  private:
    Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& m_image;
    const ROI m_roi;
  public:
    DrawEntityContour(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, ROI roi) : m_image(image), m_roi(roi) {}
    DrawEntityContour(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image) : m_image(image), m_roi(0,image.GetWidth(),0,image.GetHeight()) {}
    bool operator()(ContourEntity& contourentity) const ;
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
    return DrawEntitySampleSquareObj(image,camera,tags,roi);
  }

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
  inline
  DrawEntitySampleCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> DrawEntitySample(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec) { 
    return DrawEntitySampleSquareObj(image,camera,tags);
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
      }
    }
    return true;
  }

  template<int EDGE_CELLS>
  inline
  DrawEntitySampleSquareObj<EDGE_CELLS> DrawEntitySample(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagSquare<EDGE_CELLS>& tagspec, ROI roi) { 
    return DrawEntitySampleSquareObj(image,camera,tags,roi);
  }

  template<int EDGE_CELLS>
  inline
  DrawEntitySampleSquareObj<EDGE_CELLS> DrawEntitySample(Image<Pix::Sze::Byte1,Pix::Fmt::Grey8>& image, const Camera& camera, const TagSquare<EDGE_CELLS>& tagspec) { 
    return DrawEntitySampleSquareObj(image,camera,tags);
  }

  class DrawEntityTransform : public Function<TL0,TL1(TransformEntity)> {
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
    m_camera.NPCFToImage(points);
    for(std::vector<float>::const_iterator i = points.begin();
	i != points.end();
	++i) {
      const int x = *i;
      ++i;
      const int y = *i;
      
      m_image.DrawPixel(m_roi.ScaleX(x,m_image.GetWidth()),m_roi.ScaleY(y,m_image.GetHeight()),0);
    }  
    return true;    
  }

}

#endif
