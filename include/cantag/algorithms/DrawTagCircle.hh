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

#ifndef DRAW_TAG_CIRCLE_GUARD
#define DRAW_TAG_CIRCLE_GUARD

#include <cantag/Config.hh>
#include <cantag/entities/DecodeEntity.hh>
#include <cantag/Function.hh>
#include <cantag/Image.hh>
#include <cantag/TagCircle.hh>

namespace Cantag {
  
  /**
   * Algorithm for drawing a circular tag to an image. This is a
   * helper object which you can create using the inline template
   * function DrawTagCircle which will infer template parameters for
   * you.
   */
  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT = 5>
  class DrawTagCircleObj : public Function1<DecodeEntity<RING_COUNT*SECTOR_COUNT>, Image<Colour::Grey> > {
  private:
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    static const int PayloadSize = SECTOR_COUNT*RING_COUNT;

    typedef typename DecodeEntity<PayloadSize>::Data DecodeData; // needed to convince the compiler to parse this file

  public:
    DrawTagCircleObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec) : m_tagspec(tagspec) {}
    
    bool operator()(const DecodeEntity<RING_COUNT*SECTOR_COUNT>& code, Image<Colour::Grey>& image) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool DrawTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(const DecodeEntity<RING_COUNT*SECTOR_COUNT>& code, Image<Colour::Grey>& image) const {
    if (code.GetPayloads().size() == 0) return false;

    typename std::vector<DecodeData*>::const_iterator i = code.GetPayloads().begin();
    DecodeData* chosen = *i;

    for(;i!=code.GetPayloads().end();++i) {
      DecodeData* candidate = *i;
      if (chosen->confidence < candidate->confidence) chosen = candidate;
    }

    // Work from the outside inwards
    
    int x0 = image.GetWidth()/2;
    int y0 = image.GetHeight()/2;
    int size = (image.GetWidth() < image.GetHeight()) ? image.GetWidth()/2 : image.GetHeight()/2;
    
    bool setscale = false;
    float scalefactor = 0;

    float bullseye_outer_radius = m_tagspec.GetBullseyeOuterEdge();
    float bullseye_inner_radius = m_tagspec.GetBullseyeInnerEdge();
    float data_outer_radius = m_tagspec.GetDataOuterEdge();
    float data_inner_radius = m_tagspec.GetDataInnerEdge();
    
    if (bullseye_outer_radius > data_outer_radius) {
      scalefactor = (float)size/bullseye_outer_radius;
      setscale = true;
      image.DrawFilledCircle(x0,y0,scalefactor*bullseye_outer_radius,COLOUR_BLACK);
    }
  
    if (bullseye_inner_radius > data_outer_radius) {
      if (!setscale) {
	scalefactor = (float)size/bullseye_inner_radius;
	setscale=true;
      }
      image.DrawFilledCircle(x0,y0,scalefactor*bullseye_inner_radius,COLOUR_WHITE);
    }

    if (!setscale) {
      scalefactor = (float)size/m_tagspec.GetDataRingOuterEdge(RING_COUNT-1);
    }

    int pointer = SECTOR_COUNT*RING_COUNT-1;
    for(int i=0;i<SECTOR_COUNT;++i) {
      for(int j=0;j<RING_COUNT;++j) {
	int colour = (chosen->payload)[pointer] ? COLOUR_BLACK : COLOUR_WHITE;
	// or pick the colour based on which sector we are encoding - useful for debugging
	//int colour = (int)((float)(pointer) / (float)(RING_COUNT*SECTOR_COUNT) * 128)+128;
	image.DrawSector(x0,y0,scalefactor*m_tagspec.GetDataRingOuterEdge(RING_COUNT-j-1),m_tagspec.GetReadAngle(SECTOR_COUNT-i-1),m_tagspec.GetReadAngle(SECTOR_COUNT-i),colour);
	pointer--;
      }
    }

    if (data_inner_radius != 0) {
      image.DrawFilledCircle(x0,y0,data_inner_radius*scalefactor,
			     (bullseye_inner_radius < data_inner_radius &&
			      bullseye_outer_radius > data_inner_radius) ? COLOUR_BLACK : COLOUR_WHITE);
    }
  
    if (bullseye_outer_radius < data_inner_radius) {
      image.DrawFilledCircle(x0,y0,bullseye_outer_radius*scalefactor, COLOUR_BLACK);
    }
  
    if (bullseye_inner_radius < data_inner_radius) {
      image.DrawFilledCircle(x0,y0,bullseye_inner_radius*scalefactor,COLOUR_WHITE);
    }

    return true;
  }

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  inline
  DrawTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> DrawTagCircle(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec) {
    return DrawTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec);
  }

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  inline
  DrawTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> DrawTag(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec) {
    return DrawTagCircleObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec);
  }
}

#endif//DRAW_TAG_CIRCLE_GUARD
