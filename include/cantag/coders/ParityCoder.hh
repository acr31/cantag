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

#ifndef PARITY_CODER_GUARD
#define PARITY_CODER_GUARD

#include <cantag/Config.hh>
#include <cantag/coders/Coder.hh>
#include <cantag/CyclicBitSet.hh>

#define PARITY_DEBUG

namespace Cantag {

  /**
   * A parity coding scheme.  Lays out the code with a parity bit at the end.
   */
  template<int BIT_COUNT>
  class ParityCoder : public virtual Coder<BIT_COUNT> {
  public:
    virtual bool IsErrorCorrecting() const;
    virtual int GetSymbolSize() const;
    virtual int GetHammingDistanceBits() const;
    virtual int GetHammingDistanceSymbols() const;

    virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const;

    virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  };

  template<int BIT_COUNT> bool ParityCoder<BIT_COUNT>::IsErrorCorrecting() const { return false; }
  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::GetSymbolSize() const { return 1; }
  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::GetHammingDistanceBits() const { return 2; }
  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::GetHammingDistanceSymbols() const { return 2; }

  template<int BIT_COUNT> bool ParityCoder<BIT_COUNT>::EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    bool parity = true;
    for(unsigned int i=0;i<BIT_COUNT-1;i++) {
      if (data[i]) { 
	parity = !parity;
      }
    }
    data.Set(BIT_COUNT-1,!parity);
    return true;
  }

  template<int BIT_COUNT> int ParityCoder<BIT_COUNT>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    int rotation = data.MinRotate();
    bool parity = true;    
    for(unsigned int i=0;i<BIT_COUNT;i++) {
      if (data[i]) {
	parity = !parity;
      }
    }
    data.Set(BIT_COUNT-1,false);

#ifdef PARITY_DEBUG
    PROGRESS("Parity is "<<parity);
#endif 
    
    if (parity) {
      return rotation;
    }
    else {
      data.SetInvalid();
      return -1;
    }
  }
}
#endif//PARITY_CODER_GUARD
