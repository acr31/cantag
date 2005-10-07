/*
  Copyright (C) 2005 Andrew C. Rice

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

#ifndef ACCUMULATECORRESPONDANCE_GUARD
#define ACCUMULATECORRESPONDANCE_GUARD

#include <set>

#include <cantag/Config.hh>
#include <cantag/Function.hh>
#include <cantag/entities/TransformEntity.hh>
#include <cantag/entities/DecodeEntity.hh>
#include <cantag/Correspondances.hh>
#include <cantag/TagDictionary.hh>

namespace Cantag {

  template<int PAYLOAD_SIZE>
  class AccumulateCorrespondances : public Function<TL1(TransformEntity),TL1(DecodeEntity<PAYLOAD_SIZE>)> {
  private:
    Correspondances<LocationElement>& m_corr;
    Transform& m_average;
    const TagDictionary<PAYLOAD_SIZE,TL1(LocationElement)>& m_dictionary;

  public:
    AccumulateCorrespondances(Correspondances<LocationElement>& corr, Transform& average_transform, const TagDictionary<PAYLOAD_SIZE,TL1(LocationElement,PoseElement,SizeElement)>& dictionary) : m_corr(corr), m_average(average_transform), m_dictionary(dictionary) {}
    bool operator()(const TransformEntity& trans, DecodeEntity<PAYLOAD_SIZE>& decode) const;
  };

  template<int PAYLOAD_SIZE> bool AccumulateCorrespondances<PAYLOAD_SIZE>::operator()(const TransformEntity& trans, DecodeEntity<PAYLOAD_SIZE>& decode) const {
    // ROB
    const LocationElement* lookup = m_dictionary.GetInformation((*(decode.GetPayloads().begin()))->payload);
    if (lookup)
      m_corr.Put(trans,lookup);
  }
}
#endif//ACCUMULATECORRESPONDANCE_GUARD
