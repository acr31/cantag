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

#ifndef DRAW_TAG_SQUARESVG_GUARD
#define DRAW_TAG_SQUARESVG_GUARD

#include <ostream>

#include <cantag/Config.hh>
#include <cantag/entities/DecodeEntity.hh>
#include <cantag/Function.hh>
#include <cantag/TagSquare.hh>
#include <cantag/SpeedMath.hh>

namespace Cantag {
  
  /**
   * Algorithm for drawing a square tag to an SVG file. This is a
   * helper object which you can create using the inline template
   * function DrawTagSquare which will infer template parameters for
   * you.
   */
  template<int SIZE>
  class DrawTagSquareSVGObj : public Function<TL0,TL1(DecodeEntity<SIZE*SIZE - (SIZE*SIZE)%2>)> {
  private:
    const TagSquare<SIZE>& m_tagspec;
    std::ostream& m_ostream;

    static const int PayloadSize = TagSquare<SIZE>::PayloadSize;

    typedef typename DecodeEntity<PayloadSize>::Data DecodeData; // needed to convince the compiler to parse this file

  public:
    DrawTagSquareSVGObj(const TagSquare<SIZE>& tagspec,std::ostream& os) : m_tagspec(tagspec),m_ostream(os) {}
    
    bool operator()(DecodeEntity<PayloadSize>& code) const;
  };

  template<int SIZE> bool DrawTagSquareSVGObj<SIZE>::operator()(DecodeEntity<PayloadSize>& code) const {
    if (code.GetPayloads().size() == 0) return false;

    typename std::vector<DecodeData*>::const_iterator i = code.GetPayloads().begin();
    DecodeData* chosen = *i;

    for(;i!=code.GetPayloads().end();++i) {
      DecodeData* candidate = *i;
      if (chosen->confidence < candidate->confidence) chosen = candidate;
    }


    int cellsize = 20;
    
    int size = cellsize * SIZE + 2*cellsize;
    m_ostream << "<g>"<<std::endl;
    m_ostream << "<rect x='0' y='0' width='" << size << "' height='" << size << "' fill='black'/>" << std::endl;
    
    int ptr = 0;
    for(int col=0;col<SIZE;++col) {
      for(int row=0;row<SIZE;++row) {
	int u0 = (col+1)*cellsize;
	int v0 = (row+1)*cellsize;

	const char* colour = chosen->payload[m_tagspec.GetCellNumber(row,col)] ? "#000000" : "#FFFFFF";
	// this one just draws a greyscale range starting at black and going to white
	//Pixel<Pix::Fmt::Grey8> colour((int)((float)ptr/(float)(SIZE*SIZE - (SIZE*SIZE % 2)) * 128)+128);

	int cellwidth = cellsize;
	int cellheight = cellsize;

	if (row < SIZE - 1 && chosen->payload[m_tagspec.GetCellNumber(row+1,col)] == chosen->payload[m_tagspec.GetCellNumber(row,col)]) {
	  m_ostream << "<rect x='" << u0 << "' y='" << v0 << "' width='" << cellwidth << "' height='" << (cellheight+cellsize) << "' fill='" << colour << "'/>" << std::endl;
	}
	
	if (col < SIZE - 1 && chosen->payload[m_tagspec.GetCellNumber(row,col+1)] == chosen->payload[m_tagspec.GetCellNumber(row,col)]) {
	  m_ostream << "<rect x='" << u0 << "' y='" << v0 << "' width='" << (cellwidth+cellsize) << "' height='" << cellheight << "' fill='" << colour << "'/>" << std::endl;
	}

	m_ostream << "<rect x='" << u0 << "' y='" << v0 << "' width='" << cellwidth << "' height='" << cellheight << "' fill='" << colour << "'/>" << std::endl;
	
	++ptr;
      }
    }
    m_ostream << "</g>"<<std::endl;

    return true;
  }

  template<int SIZE>
  inline
  DrawTagSquareSVGObj<SIZE> DrawTagSquareSVG(const TagSquare<SIZE>& tagspec,std::ostream& os) {
    return DrawTagSquareSVGObj<SIZE>(tagspec,os);
  }

  template<int SIZE>
  inline
  DrawTagSquareSVGObj<SIZE> DrawTagSVG(const TagSquare<SIZE>& tagspec,std::ostream& os) {
    return DrawTagSquareSVGObj<SIZE>(tagspec,os);
  }
}

#endif//DRAW_TAG_SQUARESVG_GUARD
