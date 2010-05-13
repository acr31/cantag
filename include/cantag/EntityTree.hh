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

#include <vector>
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

    bool IsValid() const {
      return GetNode()->IsValid() && IsTreeValid();
    }

    void SetValid(bool valid) {
      GetNode()->SetValid(valid);
      SetTreeValid(valid);
    }
    
    virtual void Reset() = 0;
    virtual TreeNode<Entity>* NextChild() = 0;
    virtual bool HasNext() = 0 ;

  protected:
    virtual void SetTreeValid(bool valid) = 0;
    virtual bool IsTreeValid() const = 0;
  };
  
  template<class C>
  class Tree : public TreeNode<C> {
  };

}
#endif//ENTITY_TREE_GUARD
