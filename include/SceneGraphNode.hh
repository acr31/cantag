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
template<class S, int PAYLOAD_SIZE>
class SceneGraphNode {
private:
  /**
   * Set to true if this node has already been inspected by the tag matcher
   */
  bool m_inspected;

  /**
   * Set to point to the located object if one was found or NULL otherwise.
   */
  LocatedObject<PAYLOAD_SIZE>* m_located;

  /**
   * A ShapeChain for matching points and holding the results
   */
  S m_matcher;

  SceneGraphNode<S,PAYLOAD_SIZE>* m_next;

  std::vector<SceneGraphNode<S,PAYLOAD_SIZE>* > m_children;

  int m_contour_id;

public:
  SceneGraphNode(float* points, int numpoints) : m_inspected(false), m_located(NULL), m_matcher(points,numpoints), m_children() {};

  SceneGraphNode() : m_inspected(false), m_located(NULL), m_matcher(), m_children() {};

  ~SceneGraphNode() {
    for(typename std::vector< SceneGraphNode<S,PAYLOAD_SIZE>* >::iterator i = m_children.begin();
	i != m_children.end();
	i++) {
      delete (*i);
    }
  }

  inline int GetContourID() const {
    return m_contour_id;
  }

  inline int SetContourID(int contour_id) {
    m_contour_id = contour_id;
  }

  inline SceneGraphNode* GetChildByContourID(int contour_id) {
    for(typename std::vector< SceneGraphNode<S,PAYLOAD_SIZE>* >::iterator i = m_children.begin();
	i != m_children.end();
	i++) {
      if ( (*i)->GetContourID() == contour_id) {
	return *i;
      }
    }
    return NULL;
  }

  /**
   * Compare this node with the given node.  Return true if they
   * represent the same shape.
   */
  inline bool Compare(const SceneGraphNode<S,PAYLOAD_SIZE>& node) const {
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
  inline LocatedObject<PAYLOAD_SIZE>* GetLocatedObject() {
    if (m_located == NULL) {
      m_located = new LocatedObject<PAYLOAD_SIZE>();
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
  inline bool IsInspected() const { return m_inspected; }

  /**
   * Set this node as inspected
   */ 
  inline bool SetInspected() { m_inspected = true; }

  inline void AddChild(SceneGraphNode<S,PAYLOAD_SIZE>* child) {
    m_children.push_back(child);
  }

  inline bool HasChildren() {
    return !m_children.empty();
  }

  inline std::vector<SceneGraphNode<S,PAYLOAD_SIZE>* >& GetChildren() {
    return m_children;
  }

  inline const S& GetShapes() const {
    return m_matcher;
  } 
};

#endif//SCENE_GRAPH_NODE_GUARD
