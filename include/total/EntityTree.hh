/**
 * $Header$
 */

#ifndef ENTITY_TREE_GUARD
#define ENTITY_TREE_GUARD

#include <list>

#include <total/Config.hh>
#include <total/entities/Entity.hh>

namespace Total {

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
