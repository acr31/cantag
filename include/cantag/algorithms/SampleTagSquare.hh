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

#ifndef SAMPLE_SQUARE_TAG
#define SAMPLE_SQUARE_TAG

#include <cantag/entities/Entity.hh>
#include <cantag/TagSquare.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/Function.hh>

namespace Cantag {

  template<int EDGE_CELLS>
  class SampleTagSquareObj : public Function<TL2(MonochromeImage,TransformEntity), TL1(DecodeEntity<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)>) > {
  private:
    const TagSquare<EDGE_CELLS>& m_tagspec;
    const Camera& m_camera;
    enum { PayloadSize = EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2) };
  public:
    SampleTagSquareObj(const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera);
    bool operator()(const MonochromeImage& image,const TransformEntity& source, DecodeEntity<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)>& destination) const;
  };

  
  template<int EDGE_CELLS> SampleTagSquareObj<EDGE_CELLS>::SampleTagSquareObj(const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) :
    m_tagspec(tagspec),
    m_camera(camera) {}
  
  template<int EDGE_CELLS> bool SampleTagSquareObj<EDGE_CELLS>::operator()(const MonochromeImage& image, const TransformEntity& source, DecodeEntity<EDGE_CELLS*EDGE_CELLS-(EDGE_CELLS*EDGE_CELLS%2)>& destination) const {


    const Transform* i = source.GetPreferredTransform();

    if (i) {
      typename DecodeEntity<PayloadSize>::Data* payload = destination.Add();

      for(int j=0;j<PayloadSize;j++) {
	 float pts[] = { m_tagspec.GetXSamplePoint(j),
			 m_tagspec.GetYSamplePoint(j) };
	 i->Apply(pts[0],pts[1],pts,pts+1);
	 m_camera.NPCFToImage(pts,1);

	 if (pts[0] < 0 || pts[0] >= image.GetWidth() ||
	     pts[1] < 0 || pts[1] >= image.GetHeight()) { 
	   return false;
	 }
	 bool sample = image.GetPixel(pts[0],pts[1]);
	 (payload->payload)[j] = sample;
      }
      return true;
    }
    return false;
  }

  template<int EDGE_CELLS>
  inline
  SampleTagSquareObj<EDGE_CELLS> SampleTagSquare(const TagSquare<EDGE_CELLS>& tagspec, const Camera& camera) {
    return SampleTagSquareObj<EDGE_CELLS>(tagspec,camera);
  }
}
#endif//SAMPLE_SQUARE_TAG
