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

#include <iostream>

#include <Cantag.hh>
// this file includes the definition of the type of tag used in the
// samples.  This is then typedef'd to be of type TagType which is
// used below
#include "TagDef.hh"
#include "RunTest.hh"

typedef CircleInnerLSFull36 TagType;

using namespace Cantag;

template<class TagType>
struct Chooser {};

template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
struct Chooser<TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT> > {
  static void CreateCode(CyclicBitSet<RING_COUNT*SECTOR_COUNT>& code) {
    bool value = true;
    int ptr = 0;
    for(int i=0;i<SECTOR_COUNT;++i) {
      for(int j=0;j<RING_COUNT;++j) {
	code[ptr++] = value;
	value ^= true;      
      }
      value ^= true;
    }
  }
};

template<int EDGE_CELLS>
struct Chooser<TagSquare<EDGE_CELLS> > {
  enum { PayloadSize = EDGE_CELLS * EDGE_CELLS - (EDGE_CELLS * EDGE_CELLS % 2) };
  static void CreateCode(CyclicBitSet<PayloadSize>& code) {
    bool value = true;
    int ptr = 0;
    for(int i=0;i<PayloadSize;++i) {
      code[ptr++] = value;
      value ^= true;      
    }
  }
};

template<class List> 
struct Executor {
  typedef typename List::Head TagType;
  static void Execute() {
    const float fov = 70.f;
    const int size = 600;
    DecodeEntity<TagType::PayloadSize> d;
    typename DecodeEntity<TagType::PayloadSize>::Data* data = d.Add();
    Chooser<typename TagType::SpecType>::CreateCode(data->payload);
    
    RunTest<TagType> r(size,fov,d);
    
    try {
      r.ExecuteBatch(std::cout,1,1,180,180,50,typeid(TagType).name());
    }
    catch (const char* exception) {
      std::cerr << "Caught exception: " << exception<< std::endl;
    }    
    Executor<typename List::Tail>::Execute();
  }  
};

template<>
struct Executor<Cantag::TypeListEOL> {
  static void Execute() {};
};

int main(int argc,char* argv[]) {
  
  Executor<AllTags>::Execute();
}

