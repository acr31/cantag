/**
 * $Header$
 */

#ifndef SHAPE_TREE_GUARD
#define SHAPE_TREE_GUARD

#include <Config.hh>
#include <ContourTree.hh>

#ifdef HAVE_BOOST_ARCHIVE
#include <boost/serialization/access.hpp>
#include <boost/serialization/level.hpp>
#endif

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
#ifdef HAVE_BOOST_ARCHIVE
  private:
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive & ar, const unsigned int version);
#endif
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

#ifdef HAVE_BOOST_ARCHIVE
public:
  ShapeTree() {}
private:
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive & ar, const unsigned int version);
#endif
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

#ifdef HAVE_BOOST_ARCHIVE

//BOOST_CLASS_TRACKING(ShapeTree, boost::serialization::track_never);
namespace boost { 
  namespace serialization {
    template<class S>
    struct tracking_level<ShapeTree<S> >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<track_never> type;
      BOOST_STATIC_CONSTANT(
			    enum tracking_type, 
			    value = static_cast<enum tracking_type>(type::value)
			    );
    };
  } // serialization
} // boost

//BOOST_CLASS_IMPLEMENTATION(ShapeTree, boost::serialization::object_serializable);
namespace boost { 
  namespace serialization {
    template<class S>
    struct implementation_level<ShapeTree<S> >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<object_serializable> type;
      BOOST_STATIC_CONSTANT(
			    enum level_type,
			    value = static_cast<enum level_type>(type::value)
			    );
    };
  } // serialization
} // boost

//BOOST_CLASS_TRACKING(ShapeTree::Node, boost::serialization::track_never);
namespace boost { 
  namespace serialization {
    template<class S>
    struct tracking_level<typename ShapeTree<S>::Node >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<track_never> type;
      BOOST_STATIC_CONSTANT(
			    enum tracking_type, 
			    value = static_cast<enum tracking_type>(type::value)
			    );
    };
  } // serialization
} // boost

//BOOST_CLASS_IMPLEMENTATION(ShapeTree::Node, boost::serialization::object_serializable);
namespace boost { 
  namespace serialization {
    template<class S>
    struct implementation_level<typename ShapeTree<S>::Node >
    {
      typedef mpl::integral_c_tag tag;
      typedef mpl::int_<object_serializable> type;
      BOOST_STATIC_CONSTANT(
			    enum level_type,
			    value = static_cast<enum level_type>(type::value)
			    );
    };
  } // serialization
} // boost


template<class S> template<class Archive> void ShapeTree<S>::Node::serialize(Archive & ar, const unsigned int version) {
  ar & matched;
  ar & children;  
}

template<class S> template<class Archive> void ShapeTree<S>::serialize(Archive & ar, const unsigned int version) {
  ar & m_root_node;
}
#endif

#endif//SHAPE_TREE_GUARD
