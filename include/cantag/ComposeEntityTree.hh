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

#ifndef COMPOSE_ENTITY_TREE_GUARD
#define COMPOSE_ENTITY_TREE_GUARD

#include <vector>

#include <cantag/Config.hh>
#include <cantag/entities/Entity.hh>
#include <cantag/EntityTree.hh>
#include <cantag/ComposeEntity.hh>

namespace Cantag {    
    template<class TypeList> class TreeHelper : public TreeNode<typename TypeList::Head>, public TreeHelper<typename TypeList::Tail> {};
    template<> class TreeHelper<TypeListEOL> {};

    template<class C>
    class Tree<ComposedEntity<C> > : public TreeHelper<typename ComposedEntity<C>::Typelist> {
    private:
      ComposedEntity<C> m_node;
      std::vector<Tree<ComposedEntity<C> >* > m_children;
      typename std::vector<Tree<ComposedEntity<C> >* >::iterator m_children_iterator;
	
    public:

      void Reset() {
	m_children_iterator = m_children.begin();
      }

      bool HasNext() {
	return m_children_iterator != m_children.end();
      }

      Tree<ComposedEntity<C> >* NextChild() {
	if (m_children_iterator != m_children.end()) {
	  return *(m_children_iterator++);
	}
	else {
	  return NULL;
	}
      }
	
	~Tree() {
	    for(typename std::vector<Tree<ComposedEntity<C> >* >::const_iterator i = m_children.begin();
		i != m_children.end();
		++i) {
		delete *i;
	    }
	}
	
	Tree<ComposedEntity<C> >* AddChild() {
	    Tree<ComposedEntity<C> >* result = new Tree<ComposedEntity<C> >();
	    m_children.push_back(result);
	    return result;
	}

      bool IsPipelineValid() const { return m_node.IsPipelineValid(); }

      bool IsTreeValid() const { return m_node.IsPipelineValid(); }
      void SetTreeValid(bool valid) { 
	m_node.SetValid(valid); 
      }

	ComposedEntity<C>* GetNode() {
	    return &m_node;
	}
	
	const ComposedEntity<C>* GetNode() const {
	    return &m_node;
	}
	
	std::vector<Tree<ComposedEntity<C> >*>& GetChildren() { return m_children; }
	const std::vector<Tree<ComposedEntity<C> >*>& GetChildren() const { return m_children; }

      void SetProgress(int progress) { m_node.SetProgress(progress); }
      int GetProgress() { return m_node.GetProgress(); }
	
    };
    
}
#endif//COMPOSE_ENTITY_TREE_GUARD
