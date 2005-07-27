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

#ifndef COMPOSE_ENTITY_GUARD
#define COMPOSE_ENTITY_GUARD

#include <iostream>
#include <cantag/entities/Entity.hh>
#include <cantag/TemplateUtils.hh>
#include <cantag/Function.hh>

namespace Cantag {

  template<class List>
  class TEntity : public List::Head, public TEntity<typename List::Tail> {
  private:
    typedef TEntity<typename List::Tail> RecSuper;
    template<class CopyTail> TEntity(const TEntity<TypeList<typename List::Head, CopyTail> >& copyme) : 
      List::Head(copyme), RecSuper(copyme) {};

    TEntity(const TEntity<TypeList<typename List::Head, TypeListEOL> >& copyme) : List::Head(copyme), RecSuper() {};
    
  public:
    TEntity() : List::Head(), RecSuper() {};
  };

  template<> class TEntity<TypeListEOL> {
  public:
    TEntity() {};
  };

  template<class List>
  class ComposedEntity : public TEntity<List> {
  private:
    /**
     * Will be set to the index of the highest sucessfully completed stage
     */
    int m_progress;
    

    template<class WorkingList, class Dummy = WorkingList>
    class Check {
    public:
      inline static bool CheckValid(int index, const ComposedEntity& me) {
	if (index == 1) {
	  return (static_cast<const typename WorkingList::Head&>(me)).IsValid(); }
	else {
	  return Check<typename WorkingList::Tail>::CheckValid(index-1,me);
	}
      }
    };

    template<class Dummy>
    class Check<TypeListEOL,Dummy> {
    public:
      inline static bool CheckValid(int index, const ComposedEntity& me) { return false; }
    };

  public:
    typedef List Typelist;

    ComposedEntity() : TEntity<List>(), m_progress(-1) {}

    inline bool IsPipelineValid() const {
      if (m_progress == -1) return true;
      return Check<List>::CheckValid(m_progress,*this);
    }

    template<class L, class Algorithm> friend bool Apply(ComposedEntity<L>& entity,Algorithm& algorithm);
  };

  
}
#endif//COMPOSE_ENTITY_GUARD
