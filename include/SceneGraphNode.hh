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

public:
  
  SceneGraphNode(float* points, int numpoints);
  SceneGraphNode();
  ~SceneGraphNode();
  
  /**
   * Compare this node with the given node.  Return true if they
   * represent the same shape.
   */
  inline bool Compare(const SceneGraphNode<S,PAYLOAD_SIZE>& node) const;

  /**
   * Refresh the located object because it hasn't moved
   */
  inline void RefreshLocatedObject(Image* image);

  /**
   * Return the  current located object or create one and return that
   */
  inline LocatedObject<PAYLOAD_SIZE>* GetLocatedObject();

  /**
   * Remove the current located object
   */
  inline void ClearLocatedObject();

  /**
   * Return true if this node has already been inspected
   */
  inline bool IsInspected() const;

  /**
   * Return true if this node has a located object
   */
  inline bool IsLocated() const;

  /**
   * Set this node as inspected
   */ 
  inline void SetInspected();

  inline void AddChild(SceneGraphNode<S,PAYLOAD_SIZE>* child);

  inline bool HasChildren() const;

  inline std::vector<SceneGraphNode<S,PAYLOAD_SIZE>* >& GetChildren();

  inline const S& GetShapes() const;

  LocatedObject<PAYLOAD_SIZE>* Find(const CyclicBitSet<PAYLOAD_SIZE>& code);
};

template<class S, int PAYLOAD_SIZE> SceneGraphNode<S,PAYLOAD_SIZE>::SceneGraphNode(float* points, int numpoints) : m_inspected(false), m_located(NULL), m_matcher(points,numpoints), m_children() {};

template<class S, int PAYLOAD_SIZE> SceneGraphNode<S,PAYLOAD_SIZE>::SceneGraphNode() : m_inspected(false), m_located(NULL), m_matcher(), m_children() {};

template<class S, int PAYLOAD_SIZE> SceneGraphNode<S,PAYLOAD_SIZE>::~SceneGraphNode() {
  for(typename std::vector< SceneGraphNode<S,PAYLOAD_SIZE>* >::iterator i = m_children.begin();
      i != m_children.end();
      i++) {
    delete (*i);
  }
  if (m_located != NULL) {
    delete m_located;
  }
}

template<class S, int PAYLOAD_SIZE> bool SceneGraphNode<S,PAYLOAD_SIZE>::Compare(const SceneGraphNode<S,PAYLOAD_SIZE>& node) const {
  return m_matcher.Compare(node.m_matcher);
}

template<class S, int PAYLOAD_SIZE> void SceneGraphNode<S,PAYLOAD_SIZE>::RefreshLocatedObject(Image* image) {
  m_located.Refresh(image);
}

template<class S, int PAYLOAD_SIZE> bool SceneGraphNode<S,PAYLOAD_SIZE>::IsLocated() const {
  return (m_located != NULL);
}

template<class S, int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>* SceneGraphNode<S,PAYLOAD_SIZE>::GetLocatedObject() {
  if (m_located == NULL) {
    m_located = new LocatedObject<PAYLOAD_SIZE>();
  }
  return m_located;
}

template<class S, int PAYLOAD_SIZE> void SceneGraphNode<S,PAYLOAD_SIZE>::ClearLocatedObject() {
  delete(m_located);
}

template<class S, int PAYLOAD_SIZE> bool SceneGraphNode<S,PAYLOAD_SIZE>::IsInspected() const { return m_inspected; }

template<class S, int PAYLOAD_SIZE> void SceneGraphNode<S,PAYLOAD_SIZE>::SetInspected() { m_inspected = true; }

template<class S, int PAYLOAD_SIZE> void SceneGraphNode<S,PAYLOAD_SIZE>::AddChild(SceneGraphNode<S,PAYLOAD_SIZE>* child) {
  m_children.push_back(child);
}

template<class S, int PAYLOAD_SIZE> bool SceneGraphNode<S,PAYLOAD_SIZE>::HasChildren() const {
  return !m_children.empty();
}

template<class S, int PAYLOAD_SIZE> std::vector<SceneGraphNode<S,PAYLOAD_SIZE>* >& SceneGraphNode<S,PAYLOAD_SIZE>::GetChildren() {
  return m_children;
}

template<class S, int PAYLOAD_SIZE> const S& SceneGraphNode<S,PAYLOAD_SIZE>::GetShapes() const {
  return m_matcher;
} 

template<class S, int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>* SceneGraphNode<S,PAYLOAD_SIZE>::Find(const CyclicBitSet<PAYLOAD_SIZE>& code) {
  if ((m_located != NULL) &&
      (m_located->tag_code.get()) &&
      *(m_located->tag_code) == code) {
    return m_located;
  }
    
  for(typename std::vector<SceneGraphNode<S,PAYLOAD_SIZE>*>::iterator step = GetChildren().begin(); 
      step != GetChildren().end(); 
      step++) {
    LocatedObject<PAYLOAD_SIZE>* res = (*step)->Find(code);
    if (res != NULL) {
      return res;
    }
  }
  return NULL;
}

#endif//SCENE_GRAPH_NODE_GUARD
