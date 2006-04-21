/*
  Copyright (C) 2006 Andrew C. Rice

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

#ifndef AGGREGATION_GUARD
#define AGGREGATION_GUARD

#include <vector>
#include <functional>

#include <cantag/Config.hh>

namespace Cantag {
    template<class T>
    class AggregateMean {
    private:
	T m_total;
	int m_count;
    public:
	AggregateMean() : m_total(0), m_count(0) {};
	void operator()(const T& val) { m_total += val; ++m_count; }
	T operator()() const { 
	    assert(m_count > 0);
	    return m_total / (T)m_count; 
	}
    };
    
    template<class T>
    class AggregateMedian {
    private:
	std::vector<T> m_values;
    public:
	AggregateMedian() : m_values() {};
	void operator()(const T& val) { m_values.push_back(val); }
	T operator()() const { 
	    assert(m_values.size() > 0);
	    return m_values[m_values.size() / 2]; 
	}
    };

    template<class T, class Comp>
    class AggregateExtrema {
    private:
	T m_value;
	bool m_valid;
    public:
	AggregateExtrema() : m_valid(false) {};
	void operator()(const T& val) { 
	    if (!m_valid || Comp()(val,m_value)) { m_value = val; m_valid = true; }
	}
	T operator()() const { 
	    assert(m_valid); 
	    return m_value;
	}
    };

    template<class T>
    class AggregateMax : public AggregateExtrema<T,std::greater<T> > {};

    template<class T>
    class AggregateMin : public AggregateExtrema<T,std::less<T> > {};
}
#endif//AGGREGATION_GUARD
