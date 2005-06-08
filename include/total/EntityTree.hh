/**
 * $Header$
 */

#ifndef ENTITY_TREE_GUARD
#define ENTITY_TREE_GUARD

#include <list>
#include <iostream>

#include <total/Config.hh>
#include <total/Entity.hh>
#include <total/ComposeEntity.hh>

namespace Total {

  template<class Entity>
  class TreeNode {
  public:

    virtual Entity* GetNode() = 0;
    virtual const Entity* GetNode() const = 0;
    virtual TreeNode<Entity>* AddChild() = 0;
    virtual ~TreeNode() {};
    virtual bool IsValid() = 0;
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
    
    bool IsValid() { return m_node.IsValid(); }
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


  template<class TypeList> class TreeHelper : public TreeNode<typename TypeList::Head>, public TreeHelper<typename TypeList::Tail> {};
  template<> class TreeHelper<TypeListEOL> {};

  template<class C>
  class Tree<ComposedEntity<C> > : public TreeHelper<typename ComposedEntity<C>::Typelist> {
  private:
    ComposedEntity<C> m_node;
    std::list<Tree<ComposedEntity<C> >* > m_children;

  public:

    ~Tree() {
      for(typename std::list<Tree<ComposedEntity<C> >* >::const_iterator i = m_children.begin();
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
    
    bool IsValid() { return m_node.IsPipelineValid(); }
    /**
     * \todo does nothing at the moment
     */
    void SetValid(bool valid) { }

    ComposedEntity<C>* GetNode() {
      return &m_node;
    }

    const ComposedEntity<C>* GetNode() const {
      return &m_node;
    }

    std::list<Tree<ComposedEntity<C> >*>& GetChildren() { return m_children; }
    const std::list<Tree<ComposedEntity<C> >*>& GetChildren() const { return m_children; }

  };

}
#endif//ENTITY_TREE_GUARD
