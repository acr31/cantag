/*
  Copyright (C) 2006 Andrew C. Rice

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

#ifndef DRAW_TAG_CIRCLESVG_GUARD
#define DRAW_TAG_CIRCLESVG_GUARD

#include <ostream>

#include <cantag/Config.hh>
#include <cantag/entities/DecodeEntity.hh>
#include <cantag/Function.hh>
#include <cantag/TagCircle.hh>

namespace Cantag {
  
  /**
   * Algorithm for drawing a circular tag to an SVG file. This is a
   * helper object which you can create using the inline template
   * function DrawTagCircle which will infer template parameters for
   * you.
   */
  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT = 5>
  class DrawTagCircleSVGObj : public Function<TL0,TL1(DecodeEntity<RING_COUNT*SECTOR_COUNT>) > {
  private:
    const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& m_tagspec;
    std::ostream& m_ostream;

    static const int PayloadSize = SECTOR_COUNT*RING_COUNT;
    typedef typename DecodeEntity<PayloadSize>::Data DecodeData; // needed to convince the compiler to parse this file
    
  public:
    DrawTagCircleSVGObj(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec,std::ostream& os) : m_tagspec(tagspec),m_ostream(os) {}
    
    bool operator()(DecodeEntity<RING_COUNT*SECTOR_COUNT>& code) const;
  };

  template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT> bool DrawTagCircleSVGObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>::operator()(DecodeEntity<RING_COUNT*SECTOR_COUNT>& code) const {
    if (code.GetPayloads().size() == 0) return false;

    typename std::vector<DecodeData*>::const_iterator i = code.GetPayloads().begin();
    DecodeData* chosen = *i;
    for(;i!=code.GetPayloads().end();++i) {
      DecodeData* candidate = *i;
      if (chosen->confidence < candidate->confidence) chosen = candidate;
    }

    // Work from the outside inwards
    
    int size = 100;
    
    bool setscale = false;
    float scalefactor = 0;

    float bullseye_outer_radius = m_tagspec.GetBullseyeOuterEdge();
    float bullseye_inner_radius = m_tagspec.GetBullseyeInnerEdge();
    float data_outer_radius = m_tagspec.GetDataOuterEdge();
    float data_inner_radius = m_tagspec.GetDataInnerEdge();
    
    if (bullseye_outer_radius > data_outer_radius) {
      scalefactor = (float)size/bullseye_outer_radius;
      setscale = true;
      m_ostream << "<circle cx='0' cy='0' r='" << scalefactor*bullseye_outer_radius << "' fill='black'/>" << std::endl;
    }
    
    if (bullseye_inner_radius > data_outer_radius) {
      if (!setscale) {
	scalefactor = (float)size/bullseye_inner_radius;
	setscale=true;
      }
      m_ostream << "<circle cx='0' cy='0' r='" << scalefactor*bullseye_inner_radius << "' fill='white'/>" << std::endl;
    }

    if (!setscale) {
      scalefactor = (float)size/m_tagspec.GetDataRingOuterEdge(RING_COUNT-1);
    }

    int pointer = 0;
    for(int i=0;i<SECTOR_COUNT;++i) {
      for(int j=0;j<RING_COUNT;++j) {
	const char* colour = (chosen->payload)[pointer] ? "#000000" : "#FFFFFF";
	// or pick the colour based on which sector we are encoding - useful for debugging
	//Pixel<Pix::Fmt::Grey8> colour((int)((float)(pointer) / (float)(RING_COUNT*SECTOR_COUNT) * 128)+128);
	float x0 = cos(m_tagspec.GetReadAngle(i)) * scalefactor * m_tagspec.GetDataRingInnerEdge(RING_COUNT-j-1);
	float y0 = sin(m_tagspec.GetReadAngle(i)) * scalefactor * m_tagspec.GetDataRingInnerEdge(RING_COUNT-j-1);
	float x1 = cos(m_tagspec.GetReadAngle(i)) * scalefactor * m_tagspec.GetDataRingOuterEdge(RING_COUNT-j-1);
	float y1 = sin(m_tagspec.GetReadAngle(i)) * scalefactor * m_tagspec.GetDataRingOuterEdge(RING_COUNT-j-1);
	float x2 = cos(m_tagspec.GetReadAngle(i+1)) * scalefactor * m_tagspec.GetDataRingOuterEdge(RING_COUNT-j-1);
	float y2 = sin(m_tagspec.GetReadAngle(i+1)) * scalefactor * m_tagspec.GetDataRingOuterEdge(RING_COUNT-j-1);
	float x3 = cos(m_tagspec.GetReadAngle(i+1)) * scalefactor * m_tagspec.GetDataRingInnerEdge(RING_COUNT-j-1);
	float y3 = sin(m_tagspec.GetReadAngle(i+1)) * scalefactor * m_tagspec.GetDataRingInnerEdge(RING_COUNT-j-1);
	float iradius = m_tagspec.GetDataRingInnerEdge(RING_COUNT-j-1)*scalefactor;
	float oradius = m_tagspec.GetDataRingOuterEdge(RING_COUNT-j-1)*scalefactor;

	m_ostream << "<path d='M " << x0 << "," << y0 << 
	  " L " << x1 << "," << y1 << 
	  " A " << oradius << " " << oradius << " 0 0 1 " << x2 << "," << y2 <<
	  " L " << x3 << "," << y3 << 
	  " A " << iradius << " " << iradius << " 0 0 0 " << x0 << "," << y0 <<
	  " z' fill='" << colour << "'/>" << std::endl;
	++pointer;
      }
    }

    if (data_inner_radius != 0) {
      const char* colour = (bullseye_inner_radius < data_inner_radius &&
			    bullseye_outer_radius > data_inner_radius) ? "#000000" : "#FFFFFF";
      m_ostream << "<circle cx='0' cy='0' r='" << scalefactor*data_inner_radius << "' fill='" << colour << "'/>" << std::endl;
      
    }
  
    if (bullseye_outer_radius < data_inner_radius) {
      m_ostream << "<circle cx='0' cy='0' r='" << scalefactor*bullseye_outer_radius << "' fill='black'/>" << std::endl;
    }
  
    if (bullseye_inner_radius < data_inner_radius) {
      m_ostream << "<circle cx='0' cy='0' r='" << scalefactor*bullseye_inner_radius << "' fill='white'/>" << std::endl;
    }

    return true;
  }

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  inline
  DrawTagCircleSVGObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> DrawTagCircleSVG(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec, std::ostream& os) {
    return DrawTagCircleSVGObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,os);
  }

  template<int RING_COUNT, int SECTOR_COUNT, int READ_COUNT>
  inline
  DrawTagCircleSVGObj<RING_COUNT,SECTOR_COUNT,READ_COUNT> DrawTagSVG(const TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT>& tagspec,std::ostream& os) {
    return DrawTagCircleSVGObj<RING_COUNT,SECTOR_COUNT,READ_COUNT>(tagspec,os);
  }
}

#endif//DRAW_TAG_CIRCLESVG_GUARD
