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
  float min_width;

    float sample_strength;
    bool sample_strength_valid;

    float correct_transform_error;
    bool error_valid;
    std::vector<float> incorrect_transform_errors;

  Result() : valid(false), not_visible(false),error_valid(false) {}
  Result(float a, float d, int b) : angle_error(a), distance_error(d),bit_error(b), valid(true),not_visible(false),error_valid(false), sample_strength_valid(false) {}

  void Update(float a, float b, int c, float d) {
    if (!valid || bit_error > c) {
      angle_error = a;
      distance_error = b;
      bit_error = c;
      max_distance = d;
      valid = true;
    }
  }

  void SetSignalStrength(float w, float i) {
    signal_strength = w;
    min_width = i;
  }

    void SetCorrectTransformError(float v) {
	correct_transform_error = v;
	error_valid=true;
    }
    void AddIncorrectTransformError(float v) {
	incorrect_transform_errors.push_back(v);
    }

    void SetSampleStrength(float s) {
	sample_strength = s;
	sample_strength_valid = true;
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
struct Container {
    const Cantag::SignalStrengthEntity* se;
    const Cantag::MaxSampleStrengthEntity* me;
    const Cantag::TransformEntity* te;
    const Cantag::DecodeEntity<PAYLOAD_SIZE>* de;
    
    Container(const Cantag::SignalStrengthEntity* cse,    
	      const Cantag::MaxSampleStrengthEntity* cme,
	      const Cantag::TransformEntity* cte,
	      const Cantag::DecodeEntity<PAYLOAD_SIZE>* cde) : se(cse),me(cme),te(cte),de(cde) {};
};

template<int PAYLOAD_SIZE>
struct AddLocatedObject : public Cantag::Function<TL3(Cantag::SignalStrengthEntity,Cantag::TransformEntity,Cantag::DecodeEntity<PAYLOAD_SIZE>),TL1(Cantag::MaxSampleStrengthEntity)> {
    std::vector<Container<PAYLOAD_SIZE> >& m_list;

    AddLocatedObject(std::vector<Container<PAYLOAD_SIZE> >& lists) : m_list(lists) {}
  
    bool operator()(const Cantag::SignalStrengthEntity& ce, 
		    const Cantag::TransformEntity& te, 
		    const Cantag::DecodeEntity<PAYLOAD_SIZE>& de, 
		    Cantag::MaxSampleStrengthEntity& me) const {
	m_list.push_back(Container<PAYLOAD_SIZE>(&ce,&me,&te,&de));
	return true;
    }
};


#endif//FUNCTION_GUARD
