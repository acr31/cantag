/**
 * $Header$
 */

#ifndef SHAPE_TREE_GUARD
#define SHAPE_TREE_GUARD

#include <total/Config.hh>
#include <total/ContourTree.hh>
#include <total/Socket.hh>
#include <set>

namespace Total {
  /**
   * Takes a ContourTree and builds a tree of matched shapes
   */
  template<class S>
  class ShapeTree {

  public:
    struct Node {
      const ContourTree::Contour* source_contour;
      S matched;
      std::vector<Node*> children;
      Node(const ContourTree::Contour* contour) : source_contour(contour), matched(contour->points) {}
      Node(const std::vector<float>& points) : source_contour(NULL), matched(points) {}
      Node() : matched() {}
      ~Node() {
	for(typename std::vector<Node*>::const_iterator i = children.begin();
	    i != children.end();
	    ++i) {
	  delete *i;
	}
      }

      Node(Socket& socket);
      int Save(Socket& socket) const;
      bool Check(std::set<const ContourTree::Contour*>& checked) const;
    };

  private:
    Node m_root_node;
    int m_node_count;
    void walk_tree(Node* current, const ContourTree::Contour* contour);
    bool CheckUnmatchedContours(const ContourTree::Contour* contour, const std::set<const ContourTree::Contour*>& checked) const;
  
  public:
    /**
     * Walk the contour tree starting at this contour attempting to
     * match the shape chain against it
     */
    ShapeTree(const ContourTree::Contour& contour);
    Node* GetRootNode() { return &m_root_node; }
    const Node* GetRootNode() const { return &m_root_node; }

    int Save(Socket& socket) const;
    ShapeTree(Socket& socket);

    /**
     * Check to see if all the shapes in this shape tree match valid contour.
     * If check_negative is true we also check all the remaining contours to see
     * that no shape will match them
     */
    bool Check(const ContourTree& contour, bool check_negative) const;
    inline int GetNodeCount() const { return m_node_count;}
  };

  template<class S> ShapeTree<S>::ShapeTree(const ContourTree::Contour& contour) : m_root_node(),m_node_count(0) {
    walk_tree(&m_root_node,&contour);
  }

  template<class S> void ShapeTree<S>::walk_tree(Node* current, const ContourTree::Contour* contour) {
    // try to match this contour using 
    if (!contour->weeded) { // && contour->bordertype == ContourTree::OUTER_BORDER) {
      Node* n = new Node(contour);    
      m_node_count++;
      if (n->matched.IsChainFitted()) {
	current->children.push_back(n);
	current = n;
      }
      else {
	delete n;
	m_node_count--;
      }
    }
    for(std::vector<ContourTree::Contour*>::const_iterator i = contour->children.begin() ;
	i != contour->children.end();
	++i) {
      walk_tree(current,*i);
    }  
  };

  template<class S> bool ShapeTree<S>::Node::Check(std::set<const ContourTree::Contour*>& checked) const {
    if (source_contour) {
      checked.insert(source_contour);
      if (!matched.Check(source_contour->points)) return false;
    }

    for(typename std::vector<Node*>::const_iterator i = children.begin(); i!=children.end();++i) {
      if (!(*i)->Check(checked)) { return false; }
    }
    return true;
  }

  template<class S> bool ShapeTree<S>::Check(const ContourTree& contour, bool check_negative) const {
    // walk over the shape tree checking that the contour stored matches the shape using the error of fit function
    // add the contour pointer to a set of checked contours.
    std::set<const ContourTree::Contour*> checked;
    if (!m_root_node.Check(checked)) return false;

    if (!check_negative) return true;
    // then we need to check that all the remaining contours do not match a shape
    return CheckUnmatchedContours(contour.GetRootContour(),checked);
  }

  template<class S> bool ShapeTree<S>::CheckUnmatchedContours(const ContourTree::Contour* contour, const std::set<const ContourTree::Contour*>& checked) const {
    if (!contour->weeded && checked.count(contour) == 0) {
      Node n(contour->points);
      // if this node is fitted then we FAIL because its not in the shape tree.
      if (n.matched.IsChainFitted()) return false;
    }
    for(std::vector<ContourTree::Contour*>::const_iterator i = contour->children.begin();
	i!=contour->children.end();
	++i) {
      if (!CheckUnmatchedContours(*i,checked)) { return false; }
    }
    return true;
  }

  template<class S> int ShapeTree<S>::Node::Save(Socket& socket) const {
    int count = matched.Save(socket);
    count += socket.Send((int)children.size());
    for(typename std::vector<Node*>::const_iterator i = children.begin(); i!=children.end(); ++i) {
      count += (*i)->Save(socket);
    }
    return count;
  }

  template<class S> ShapeTree<S>::Node::Node(Socket& socket) : matched(socket) {
    int count = socket.RecvInt();
    for(int i=0;i<count;++i) {
      children.push_back(new Node(socket));
    }
  }

  template<class S> int ShapeTree<S>::Save(Socket& socket) const {
    return m_root_node.Save(socket);
  }

  template<class S> ShapeTree<S>::ShapeTree(Socket& socket) : m_root_node(socket) {}
}
#endif//SHAPE_TREE_GUARD
