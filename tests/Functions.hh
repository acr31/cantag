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

#include <Cantag.hh>

#ifndef FUNCTION_GUARD
#define FUNCTION_GUARD

struct Result {
  float angle_error;
  float distance_error;
  int bit_error;
  bool valid;
  bool not_visible;
  float min_distance;
  float max_distance;

  float signal_strength;
  int min_width;

  Result() : valid(false), not_visible(false) {}
  Result(float a, float d, int b) : angle_error(a), distance_error(d),bit_error(b), valid(true),not_visible(false) {}

  void Update(float a, float b, int c, float d) {
    if (!valid || bit_error > c) {
      angle_error = a;
      distance_error = b;
      bit_error = c;
      max_distance = d;
      valid = true;
    }
  }

  void SetSignalStrength(float w,int i) {
    signal_strength = w;
    min_width = i;
  }
};



struct FindContour : public Cantag::Function<TL0,TL1(Cantag::ContourEntity)> {
  const Cantag::ContourEntity& m_reference;
  FindContour(const Cantag::ContourEntity& ref) : m_reference(ref) {};
  bool operator()(Cantag::ContourEntity& tocheck) const {
    
    if ((fabs(m_reference.GetMinX() - tocheck.GetMinX()) < 5) &&
	(fabs(m_reference.GetMinY() - tocheck.GetMinY()) < 5) &&
	(fabs(m_reference.GetMaxX() - tocheck.GetMaxX()) < 5) &&
	(fabs(m_reference.GetMaxY() - tocheck.GetMaxY()) < 5)) {
      return true;
    }
    else {
      return false;
    }
  }
};

template<int PAYLOAD_SIZE>
struct AddLocatedObject : public Cantag::Function<TL2(Cantag::SignalStrengthEntity,Cantag::TransformEntity),TL1(Cantag::DecodeEntity<PAYLOAD_SIZE>)> {
  std::vector<std::pair<const Cantag::SignalStrengthEntity*, std::pair<const Cantag::TransformEntity*,const Cantag::DecodeEntity<PAYLOAD_SIZE>*> > >& m_list;

  AddLocatedObject(std::vector<std::pair<const Cantag::SignalStrengthEntity*,std::pair<const Cantag::TransformEntity*,const Cantag::DecodeEntity<PAYLOAD_SIZE>*> > >& lists) : m_list(lists) {};
  
  bool operator()(const Cantag::SignalStrengthEntity& ce, const Cantag::TransformEntity& te, Cantag::DecodeEntity<PAYLOAD_SIZE>& de) const {
    m_list.push_back(std::pair<const Cantag::SignalStrengthEntity*,std::pair<const Cantag::TransformEntity*,const Cantag::DecodeEntity<PAYLOAD_SIZE>*> >(&ce,std::pair<const Cantag::TransformEntity*,const Cantag::DecodeEntity<PAYLOAD_SIZE>*>(&te,&de)));
    return true;
  }
};


#endif//FUNCTION_GUARD
