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
 $Header$
 */

#ifndef TAGDEF_GUARD
#define TAGDEF_GUARD

#include <Cantag.hh>

template<int RINGS, int SECTORS>
struct TestCircleCoder {
  typedef typename Cantag::ParityCoder<RINGS*SECTORS> ParityCoder;
#ifdef HAVE_GMPXX_H
  typedef typename Cantag::TripOriginalCoder<RINGS*SECTORS,RINGS,2> TripCoder;
#endif
  typedef typename Cantag::SCCCoder<RINGS,SECTORS,5,1> SCCCoder;  /* try rings = 5, sectors = 31 */
  typedef ParityCoder Coder;
};

struct CircleInnerTrip : public Cantag::TagCircle<2,16>, public TestCircleCoder<2,16>::Coder {
  CircleInnerTrip() : Cantag::TagCircle<2,16>(0.272727f,0.454545f,0.5454545f,1.0f) {}
};

template<int RINGS, int SECTORS>
struct CircleInnerFixed : public Cantag::TagCircle<RINGS,SECTORS>, public TestCircleCoder<RINGS,SECTORS>::Coder {
  CircleInnerFixed() : Cantag::TagCircle<RINGS,SECTORS>(0.2f,0.4f,0.6f,1.0f) {}
};

template<int RINGS, int SECTORS>
struct CircleSplitFixed : public Cantag::TagCircle<RINGS,SECTORS>, public TestCircleCoder<RINGS,SECTORS>::Coder {
  CircleSplitFixed() : Cantag::TagCircle<RINGS,SECTORS>(0.2f,1.0f,0.4f,0.8f) {}
};

template<int RINGS, int SECTORS>
struct CircleOuterFixed : public Cantag::TagCircle<RINGS,SECTORS>, public TestCircleCoder<RINGS,SECTORS>::Coder {
  CircleOuterFixed() : Cantag::TagCircle<RINGS,SECTORS>(0.8f,1.0f,0.2f,0.6f) {}
};

template<int RINGS, int SECTORS>
struct CircleInnerOpt : public Cantag::TagCircleInner<RINGS,SECTORS>, public TestCircleCoder<RINGS,SECTORS>::Coder {};

template<int RINGS, int SECTORS>
struct CircleSplitOpt : public Cantag::TagCircleSplit<RINGS,SECTORS>, public TestCircleCoder<RINGS,SECTORS>::Coder {};

template<int RINGS, int SECTORS>
struct CircleOuterOpt : public Cantag::TagCircleOuter<RINGS,SECTORS>, public TestCircleCoder<RINGS,SECTORS>::Coder {};


template<int SIZE>
struct TestSquareCoder {
  enum { PAYLOAD_SIZE = SIZE*SIZE - (SIZE*SIZE % 2) };
  typedef typename Cantag::CRCSymbolChunkCoder<PAYLOAD_SIZE,4,PAYLOAD_SIZE/4> CRCCoder;

  typedef CRCCoder Coder;
};

template<int SIZE>
struct Square : public Cantag::TagSquare<SIZE>, public TestSquareCoder<SIZE>::Coder {};


typedef CircleSplitOpt<6,88> CircleTagType;
typedef Square<23> SquareTagType;
typedef CircleTagType TagType;

#endif//TAGDEF_GUARD
