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
 * Galois Field(4) coder.  
 */

#ifndef GF4_CODER_GUARD
#define GF4_CODER_GUARD

#include <cantag/Config.hh>
#include <cantag/GF4Poly.hh>
#include <cantag/CyclicBitSet.hh>

namespace Cantag {
  template<int BIT_COUNT, int GRANULARITY>
  class GF4Coder : public Coder<BIT_COUNT> {
  public:
    typedef GF4Coder<BIT_COUNT,GRANULARITY> CoderType;

  private:
    GF4Poly m_genpoly;
    
  public:
    GF4Coder();

    virtual bool IsErrorCorrecting() const;
    virtual int GetSymbolSize() const;
    virtual int GetHammingDistanceBits() const;
    virtual int GetHammingDistanceSymbols() const;


    virtual int DecodePayload(CyclicBitSet<BIT_COUNT>& data) const;
    virtual bool EncodePayload(CyclicBitSet<BIT_COUNT>& data) const;
  };

  template<int BIT_COUNT,int GRANULARITY> GF4Coder<BIT_COUNT,GRANULARITY>::GF4Coder() {}
  template<int BIT_COUNT,int GRANULARITY> bool GF4Coder<BIT_COUNT,GRANULARITY>::IsErrorCorrecting() const { return false; }
  template<int BIT_COUNT,int GRANULARITY> int GF4Coder<BIT_COUNT,GRANULARITY>::GetSymbolSize() const { return 2; }
  template<int BIT_COUNT,int GRANULARITY> int GF4Coder<BIT_COUNT,GRANULARITY>::GetHammingDistanceBits() const { }
  template<int BIT_COUNT,int GRANULARITY> int GF4Coder<BIT_COUNT,GRANULARITY>::GetHammingDistanceSymbols() const { }
  template<int BIT_COUNT,int GRANULARITY> int GF4Coder<BIT_COUNT,GRANULARITY>::DecodePayload(CyclicBitSet<BIT_COUNT>& data) const {}
  template<int BIT_COUNT,int GRANULARITY> bool GF4Coder<BIT_COUNT,GRANULARITY>::EncodePayload(CyclicBitSet<BIT_COUNT>& data) const {}

}
#endif//GF4_CODER_GUARD
