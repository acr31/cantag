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

#ifndef ENCODE_GUARD
#define ENCODE_GUARD

#include <list>

#include <cantag/entities/DecodeEntity.hh>
#include <cantag/coders/Coder.hh>
#include <cantag/CyclicBitSet.hh>
#include <cantag/Function.hh>

namespace Cantag {

  /**
   * Algorithm for encoding tag data
   */
  template<class Coder>
  class Encode : public Function<TL0,TL1(DecodeEntity<Coder::PayloadSize>) >, private Coder {
  private:
    typedef typename DecodeEntity<Coder::PayloadSize>::Data DecodeData; // needed to convince the compiler to parse this file
  public:
    Encode() : Coder() {};

    bool operator()(DecodeEntity<Coder::PayloadSize>& destination) const {
      bool return_result = false;
      for(typename std::vector<DecodeData*>::iterator i = destination.GetPayloads().begin(); 
	  i != destination.GetPayloads().end(); 
	  ++i) {
	DecodeData* data = *i;
	bool result = EncodePayload(data->payload);

	if (result) {
	  data->bits_rotation = 0;
	  data->confidence = 1.f;
	  return_result = true;
	}
	else {
	  data->bits_rotation = -1;
	  data->confidence = 0.f;
	}
      }
      return return_result;
    }    
  };
  
}

#endif//ENCODE_GUARD
