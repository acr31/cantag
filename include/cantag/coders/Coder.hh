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

#ifndef CODER_GUARD
#define CODER_GUARD

#include <cantag/Config.hh>
#include <cantag/CyclicBitSet.hh>

namespace Cantag {

  template <int PAYLOAD_SIZE>
  class Coder {
  public:  
    enum { PayloadSize = PAYLOAD_SIZE };
  
    /**
     * Alters the passed payload inorder to encode it.
     *
     * Returns false if the value could not be encoded for some reason.
     */
    virtual bool EncodePayload(CyclicBitSet<PAYLOAD_SIZE>& data) const = 0;

    
    /**
     * Decode the passed value in place.
     *
     * Returns the number of rotations required to orient the code
     * correctly or -1 if there was some kind of failure
     *
     */
    virtual int DecodePayload(CyclicBitSet<PAYLOAD_SIZE>& data) const =0;

    /**
     * Return true if this code is error correcting or false if it only
     * detects errors.
     */
    virtual bool IsErrorCorrecting() const = 0;

    /**
     * Return the number of bits per symbol.  The code could be rotated
     * by any multiple of the symbol size.
     */
    virtual int GetSymbolSize() const = 0;

    /**
     * Return the codes minimum hamming distance in bits.  This is the
     * minimum number of bits that you need to change to go from one
     * code word to another one.
     */
    virtual int GetHammingDistanceBits() const = 0;

    /**
     * Return the codes minimum hamming distance in bits.  This is the
     * minimum number of symbols that you need to change to go from one
     * code word to another one.
     */
    virtual int GetHammingDistanceSymbols() const = 0;

    /**
     * Encode the provided tag_data and then decode it again.  Any parts
     * of the original tag_data that could not be encoded will thus be
     * thrown away and you are left with the value that you can actually
     * encode on the tag.  Returns true if the process is successful or
     * false if this payload could not be encoded or decoded
     * successfully.
     */
    bool EncodedValue(CyclicBitSet<PAYLOAD_SIZE>& tag_data) const;

  };

  template<int PAYLOAD_SIZE> bool Coder<PAYLOAD_SIZE>::EncodedValue(CyclicBitSet<PAYLOAD_SIZE>& tag_data) const {
    if (!EncodePayload(tag_data)) {
      return false;
    }
  
    if (DecodePayload(tag_data) == -1) {
      return false;
    }

    return true;
  }
}
#endif//CODER_GUARD
