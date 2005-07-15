/**
 * $Header$
 */

#ifndef TRANSFORM_ROTATE_TO_PAYLOAD_GUARD
#define TRANSFORM_ROTATE_TO_PAYLOAD_GUARD

#include <list>
#include <cmath>
#include <iostream>

#include <total/Config.hh>
#include <total/TagSpec.hh>
#include <total/Camera.hh>
#include <total/MonochromeImage.hh>
#include <total/entities/Entity.hh>
#include <total/CyclicBitSet.hh>

namespace Total {
  /**
   * Rotate the transformation to the angle as represented by the
   * number of bits of rotation in the payload.  If the transformation
   * doesn't have a corresponding decode then leave it alone.  This is
   * a helper object you can construct using the inlined template
   * function TransformRotateToPayload which will infer template
   * parameters
   */
  template<int PAYLOAD_SIZE>
  class TransformRotateToPayloadObj : public Function1<DecodeEntity<PAYLOAD_SIZE>,TransformEntity> {
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
      float cos,sin;
      if (data->confidence > 0.f) {
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
