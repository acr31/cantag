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

#ifndef CORRESPONDANCES_GUARD
#define CORRESPONDANCES_GUARD

#include <cantag/Config.hh>
#include <cantag/entities/TransformEntity.hh>

#include <list>

namespace Cantag {
  /**
   * The Correspondances class is used to maintain a set of tuples mapping between the
   * TransformEntities of sighted tags and some knowledge from a
   * TagDictionary.  The transform entities are stored by value but the
   * ElementTypes that you have provided from a TagDictionary are stored
   * by reference (ownership is not taken).
   */
  template<class ElementType>
  class Correspondances {
  private:
    std::list<std::pair<const TransformEntity, const ElementType*> > m_list;
  public:
    Correspondances() : m_list() {};
    void Put(const TransformEntity& te, const ElementType* element) { m_list.push_back(std::pair<const TransformEntity,const ElementType*>(te,element)); }
    const std::list<std::pair<const TransformEntity,const ElementType*> >& GetValues() const { return m_list; }
  };
}
  
#endif//CORRESPONDANCES_GUARD
  
