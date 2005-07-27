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

#ifndef RAW_CODER_GUARD
#define RAW_CODER_GUARD

#include <cantag/Config.hh>
#include <cantag/coders/Coder.hh>
#include <cantag/CyclicBitSet.hh>

namespace Cantag {

  /**
   * A raw coding scheme.  Lays out the code given - no error detection.
   */
  template<int BIT_COUNT, int SYMBOLSIZE>
  class RawCoder : public virtual Coder<BIT_COUNT> {
  public:
    virtual bool IsErrorCorrecting() const { return false; }
    virtual int GetSymbolSize() const { return 1; }
    virtual int GetHammingDistanceBits() const { return 1; }
    virtual int GetHammingDistanceSymbols() const { return 1; }

    virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const { return true; }

    virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  };

  template<int BIT_COUNT,int SYMBOLSIZE> int RawCoder<BIT_COUNT,SYMBOLSIZE>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {
    int rotation = data.MinRotate(SYMBOLSIZE) * SYMBOLSIZE;
    return rotation;
  }
}
#endif//RAW_CODER_GUARD
