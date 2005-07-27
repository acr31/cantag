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
 *
 * $Header$
 *
 * \todo update this for the general coding framework.  Reincorporate ian's error correction and mindecode stuff.
 */
#ifndef GF4_CODER_GUARD
#define GF4_CODER_GUARD

#include <cantag/Config.hh>
#include <cantag/coders/Coder.hh>
#include <cantag/GF4Poly.hh>
#include <cmath>

namespace Cantag {
class GF4Coder : public Coder {
private:
  int m_symbol_range;
  int m_symbol_count;
  unsigned long long m_accum_code;
  int m_current_chunk;
  int m_base;
  
  GF4Poly m_genpoly;
  GF4Poly m_encoded;

  int m_maxerrors;

public:
  GF4Coder(int symbol_range,int symbol_count);
  
  virtual unsigned long long Encode(unsigned long long value);
  virtual unsigned long long Decode(unsigned long long value);
};
}
#endif//GF4_CODER_GUARD
