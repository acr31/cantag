/**
 * $Header$
 */

#ifndef SCENE_GRAPH_NODE_GUARD
#define SCENE_GRAPH_NODE_GUARD

#include <vector>
#include <LocatedObject.hh>

/**
 * A shape in the scene graph
 */
template<class S>
class SceneGraphNode {
private:
  /**
   * Set to true if this node has already been inspected by the tag matcher
   */
  bool m_inspected;

  /**
   * Set to point to the located object if one was found or NULL otherwise.
   */
  LocatedObject* m_located;

  /**
   * A ShapeChain for matching points and holding the results
   */
  S m_matcher;

  SceneGraphNode<S>* m_next;

  std::vector<SceneGraphNode<S>* > m_children;

public:
  SceneGraphNode(float* points, int numpoints) : m_inspected(false), m_located(NULL), m_matcher(points,numpoints), m_children() {};

  SceneGraphNode() : m_inspected(false), m_located(NULL), m_matcher(NULL,0), m_children() {};

  ~SceneGraphNode() {
    for(typename std::vector< SceneGraphNode<S>* >::iterator i = m_children.begin();
	i != m_children.end();
	i++) {
      delete (*i);
    }
  }

  /**
   * Compare this node with the given node.  Return true if they
   * represent the same shape.
   */
  inline bool Compare(const SceneGraphNode<S>& node) const {
    return m_matcher.Compare(node.m_matcher);
  }

  /**
   * Refresh the located object because it hasn't moved
   */
  inline void RefreshLocatedObject(Image* image) {
    m_located.Refresh(image);
  }

  /**
   * Replace the current located object with a new one
   */
  inline LocatedObject* GetLocatedObject() {
    if (m_located == NULL) {
      m_located = new LocatedObject();
    }
    return m_located;
  }

  /**
   * Remove the current located object
   */
  inline void ClearLocatedObject() {
    delete(m_located);
  }

  /**
   * Return true if this node has already been inspected
   */
  inline bool IsInspected() { return m_inspected; }

  /**
   * Set this node as inspected
   */ 
  inline bool SetInspected() { m_inspected = true; }

  inline void AddChild(SceneGraphNode<S>* child) {
    m_children.push_back(child);
  }

  inline std::vector<SceneGraphNode<S>* >& GetChildren() {
    return m_children;
  }

  inline const S& GetShapes() const {
    return m_matcher;
  }
};

#endif//SCENE_GRAPH_NODE_GUARD
