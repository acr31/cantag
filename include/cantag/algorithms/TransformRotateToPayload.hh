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

#ifndef TRANSFORM_ROTATE_TO_PAYLOAD_GUARD
#define TRANSFORM_ROTATE_TO_PAYLOAD_GUARD

#include <list>
#include <cmath>

#include <cantag/Config.hh>
#include <cantag/TagSpec.hh>
#include <cantag/Camera.hh>
#include <cantag/MonochromeImage.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/CyclicBitSet.hh>

namespace Cantag {
  /**
   * Rotate the transformation to the angle as represented by the
   * number of bits of rotation in the payload.  If the transformation
   * doesn't have a corresponding decode then leave it alone.  This is
   * a helper object you can construct using the inlined template
   * function TransformRotateToPayload which will infer template
   * parameters
   */
  template<int PAYLOAD_SIZE>
  class TransformRotateToPayloadObj : public Function<TL1(DecodeEntity<PAYLOAD_SIZE>),TL1(TransformEntity)> {
  private:
    typedef typename DecodeEntity<PAYLOAD_SIZE>::Data DecodeData;

    const TagSpec<PAYLOAD_SIZE>& m_tagspec;
  public:
    TransformRotateToPayloadObj(const TagSpec<PAYLOAD_SIZE>& tagspec) : m_tagspec(tagspec) {}
    bool operator()(const DecodeEntity<PAYLOAD_SIZE>& image, TransformEntity& transform) const;
  };

  template<int PAYLOAD_SIZE> bool TransformRotateToPayloadObj<PAYLOAD_SIZE>::operator()(const DecodeEntity<PAYLOAD_SIZE>& decode, TransformEntity& transform) const {
    if (!transform.IsValid()) return false;
    bool return_value = false;
    typename std::vector<DecodeData*>::const_iterator i = decode.GetPayloads().begin();
    std::list<Transform*>::iterator j = transform.GetTransforms().begin();
    for(;i != decode.GetPayloads().end() && j != transform.GetTransforms().end();++i,++j) {
      DecodeData* data = *i;
      if (data->confidence > 0.f) {
	float cos,sin;
	m_tagspec.GetCellRotation(data->bits_rotation,cos,sin);
	(*j)->Rotate(cos,sin);
	return_value = true;
      }
    }
    return return_value;
  }

  template<int PAYLOAD_SIZE>
  inline
  TransformRotateToPayloadObj<PAYLOAD_SIZE> TransformRotateToPayload(const TagSpec<PAYLOAD_SIZE>& tagspec) {
    return TransformRotateToPayloadObj<PAYLOAD_SIZE>(tagspec);
  }
}
#endif//TRANSFORM_ROTATE_TO_PAYLOAD_GUARD
