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
 * Contains code by Ian Caulkin
 *
 * $Header$
 *
 */

#include <total/coders/GF4Coder.hh>

namespace Total {
  /**
   * Initialise genpoly to 404429. This is
   *
   * X^9+x*X^8+x*X^6+x*X^5+(x+1)X^4+(x+1)X^3+(x+1)X^1+1
   */
  GF4Coder::GF4Coder(int symbol_range,  int symbol_count) :
    m_symbol_range(symbol_range),
    m_symbol_count(symbol_count), 
    m_genpoly(404429), 
    m_encoded(0), 
    m_maxerrors(0),
    m_current_chunk(0) {
    assert(symbol_range == 4);
    assert(symbol_count < 21);
  }

  unsigned long long GF4Coder::Encode(unsigned long long value) {
    PROGRESS("Encoder set with value "<<value);
    m_encoded = GF4Poly(value)*m_genpoly;
    PROGRESS("Encoded value is "<<m_encoded.GetValue());
    return m_encoded.GetValue();

  }

  unsigned long long GF4Coder::Decode(unsigned long long value)  { 
    GF4Poly enc(value);
    PROGRESS("Encoded value = "<<enc.GetValue());
    for(int i=0;i<m_symbol_count;i++) {
      GF4Poly syndrome = enc % m_genpoly;
      PROGRESS("Syndrome is " << syndrome.GetValue());
      if (syndrome == (unsigned long long int)0) {
	PROGRESS("Syndrome is 0 - we have a match");
	return (enc / m_genpoly).GetValue();
      }
      enc.Rotate(1,m_symbol_count);
    }
    PROGRESS("We failed to find a valid orientation for this code - throwing InvalidCode.");
    throw InvalidCode();
  }

}
