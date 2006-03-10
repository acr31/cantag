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

template<class TagType>
struct Chooser {};

template<int RING_COUNT,int SECTOR_COUNT,int READ_COUNT>
struct Chooser<Cantag::TagCircle<RING_COUNT,SECTOR_COUNT,READ_COUNT> > {
  static void CreateCode(Cantag::CyclicBitSet<RING_COUNT*SECTOR_COUNT>& code) {
    bool value = true;
    int ptr = 0;
    for(int i=0;i<SECTOR_COUNT;++i) {
      for(int j=0;j<RING_COUNT;++j) {
	code.Set(ptr++,value);
	value ^= true;      
      }
      value ^= true;
    }
  }
};

template<int EDGE_CELLS>
struct Chooser<Cantag::TagSquare<EDGE_CELLS> > {
  enum { PayloadSize = EDGE_CELLS * EDGE_CELLS - (EDGE_CELLS * EDGE_CELLS % 2) };
  static void CreateCode(Cantag::CyclicBitSet<PayloadSize>& code) {
    bool value = true;
    int ptr = 0;
    for(int i=0;i<PayloadSize;++i) {
      code.Set(ptr++,value);
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
    Cantag::DecodeEntity<TagType::PayloadSize> d;
    typename Cantag::DecodeEntity<TagType::PayloadSize>::Data* data = d.Add();
    Chooser<typename TagType::SpecType>::CreateCode(data->payload);
    
    RunTest<TagType> r(size,fov,d);
    
    try {
      r.ExecuteBatch(std::cout,typeid(TagType).name());
    }
    catch (const char* exception) {
      std::cerr << "Caught exception: " << exception<< std::endl;
    }    
    Executor<typename List::Tail>::Execute();
  }  

  static void ExecuteSingle(float x0, float y0, float z0, float theta, float phi) {
    const float fov = 70.f;
    const int size = 600;
    Cantag::DecodeEntity<TagType::PayloadSize> d;
    typename Cantag::DecodeEntity<TagType::PayloadSize>::Data* data = d.Add();
    Chooser<typename TagType::SpecType>::CreateCode(data->payload);
    
    RunTest<TagType> r(size,fov,d);
    
    try {
      r.ExecuteSingle(std::cout,x0,y0,z0,theta,phi,typeid(TagType).name());
    }
    catch (const char* exception) {
      std::cerr << "Caught exception: " << exception<< std::endl;
    }    
    Executor<typename List::Tail>::ExecuteSingle(x0,y0,z0,theta,phi);
  }
};

template<>
struct Executor<Cantag::TypeListEOL> {
  static void Execute() {};
  static void ExecuteSingle(float x0,float y0,float z0,float theta,float phi) {};
};


//typedef AllTags TagList;
//typedef CircleTags TagList;
//typedef TL1(SquareCornerProj36) TagList;
//typedef TL4(SquareCornerProj36,SquareRegressConvexHullProj36,SquareCornerSpaceSearch36,SquareRegressConvexHullSpaceSearch36) TagList;
typedef TL1(SquareCornerProj36) TagList;

int main(int argc,char* argv[]) {
  std::cout.precision(15);

  if (argc == 1) {
    Executor<TagList>::Execute();
  }
  else {
    float theta = atof(argv[1]);
    float phi = atof(argv[2]);
    float x0 = atof(argv[3]);
    float y0 = atof(argv[4]);
    float z0 = atof(argv[5]);

    Executor<TagList>::ExecuteSingle(x0,y0,z0,theta,phi);

  }
}

