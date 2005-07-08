/**
 * $Header$
 */

#ifndef COMPOSE_ENTITY_TREE_GUARD
#define COMPOSE_ENTITY_TREE_GUARD

#include <list>

#include <total/Config.hh>
#include <total/entities/Entity.hh>
#include <total/EntityTree.hh>
#include <total/ComposeEntity.hh>

namespace Total {    
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
#endif//COMPOSE_ENTITY_TREE_GUARD
