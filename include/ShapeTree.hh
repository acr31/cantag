/**
 * $Header$
 */

#ifndef SHAPE_TREE_GUARD
#define SHAPE_TREE_GUARD

#include <Config.hh>
#include <ContourTree.hh>
#include <Socket.hh>
/**
 * Takes a ContourTree and builds a tree of matched shapes
 */
template<class S>
class ShapeTree {

public:
  struct Node {
    S matched;
    std::vector<Node*> children;
    Node(const std::vector<float>& points) : matched(points) {}
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

  };

private:
  Node m_root_node;
  void walk_tree(Node* current, const ContourTree::Contour* contour);

#ifdef IMAGE_DEBUG
  Image* debug_image;
#endif
public:
  /**
   * Walk the contour tree starting at this contour attempting to
   * match the shape chain against it
   */
  ShapeTree(const ContourTree::Contour& contour);
  Node* GetRootNode() { return &m_root_node; }

  int Save(Socket& socket) const;
  ShapeTree(Socket& socket);
};

template<class S> ShapeTree<S>::ShapeTree(const ContourTree::Contour& contour) : m_root_node() {
#ifdef IMAGE_DEBUG
  debug_image = new Image(1000,1000);
#endif
  walk_tree(&m_root_node,&contour);
#ifdef IMAGE_DEBUG
  debug_image->Save("debug-shapetree.bmp");
  delete debug_image;
#endif
}

template<class S> void ShapeTree<S>::walk_tree(Node* current, const ContourTree::Contour* contour) {
  // try to match this contour using 
  if (!contour->weeded) {
    Node* n = new Node(contour->points);    
    if (n->matched.IsChainFitted()) {
#ifdef IMAGE_DEBUG
      n->matched.DrawChain(*debug_image);
#endif
      current->children.push_back(n);
      current = n;
    }
    else {
      delete n;
    }
  }
  for(std::vector<ContourTree::Contour*>::const_iterator i = contour->children.begin() ;
      i != contour->children.end();
      ++i) {
    walk_tree(current,*i);
  }  
};


template<class S> int ShapeTree<S>::Node::Save(Socket& socket) const {
  int count = matched.Save(socket);
  count += socket.Send((int)children.size());
  for(typename std::vector<Node*>::const_iterator i = children.begin(); i!=children.end(); ++i) {
    count += (*i)->Save(socket);
  }
  return count;
}

template<class S> ShapeTree<S>::Node::Node(Socket& socket) : matched(socket) {
  std::cout << "Loading root node " << std::endl;
  int count = socket.RecvInt();
  for(int i=0;i<count;++i) {
    children.push_back(new Node(socket));
  }
}

template<class S> int ShapeTree<S>::Save(Socket& socket) const {
  return m_root_node.Save(socket);
}

template<class S> ShapeTree<S>::ShapeTree(Socket& socket) : m_root_node(socket) {}

#endif//SHAPE_TREE_GUARD
