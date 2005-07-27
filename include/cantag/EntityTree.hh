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

#ifndef ENTITY_TREE_GUARD
#define ENTITY_TREE_GUARD

#include <list>

#include <cantag/Config.hh>
#include <cantag/entities/Entity.hh>

namespace Cantag {

  template<class Entity>
  class TreeNode {
  public:
    virtual Entity* GetNode() = 0;
    virtual const Entity* GetNode() const = 0;
    virtual TreeNode<Entity>* AddChild() = 0;
    virtual ~TreeNode() {};
    virtual bool IsValid() const = 0;
    virtual void SetValid(bool valid) = 0;
  };
  
  template<class C>
  class Tree : public TreeNode<C> {
  private:
    C m_node;
    std::list<Tree<C>*> m_children;
  public:

    ~Tree() {
      for(typename std::list<Tree<C>*>::const_iterator i = m_children.begin();
	  i != m_children.end();
	  ++i) {
	delete *i;
      }
    }

    Tree<C>* AddChild() {
      Tree<C>* result = new Tree<C>();
      m_children.push_back(result);
      return result;
    }

    C* GetNode() {
      return &m_node;
    }

    const C* GetNode() const {
      return &m_node;
    }
    
    bool IsValid() const { return m_node.IsValid(); }
    void SetValid(bool valid) { m_node.SetValid(valid); }

    int GetSize() { 
      int count = m_node.IsValid() ? 1 : 0;
      for(typename std::list<Tree<C>*>::const_iterator i = m_children.begin();
	  i != m_children.end();
	  ++i) {
	count += (*i)->GetSize();
      }
      return count;
    };

    std::list<Tree<C>*>& GetChildren() { return m_children; }
    const std::list<Tree<C>*>& GetChildren() const { return m_children; }
  };

}
#endif//ENTITY_TREE_GUARD
