/**
 * $Header$
 *
 */
#ifndef TAG_GUARD
#define TAG_GUARD

#include <Config.hh>
#include <Image.hh>
#include <Camera.hh>
#include <ShapeTree.hh>
#include <vector>
#include <CyclicBitSet.hh>
#include <WorldState.hh>

/**
 * Abstract superclass for all tags.
 */
template<class C, int PAYLOAD_SIZE>
class Tag {
public:
  typedef C TagShapeType;
  static const int TagPayloadSize = PAYLOAD_SIZE;
  typedef CyclicBitSet<PAYLOAD_SIZE> TagPayloadType;

  TagPayloadType* m_must_match;
  
  Tag() : m_must_match(NULL) {}

  /**
   * Draw this tag onto the image object provided
   */
  virtual void Draw2D(Image& image, TagPayloadType& tag_data) const = 0;
    
  /**
   * Decode the passed node. Return true if you find a valid tag there or false otherwise
   */
  virtual LocatedObject<PAYLOAD_SIZE>* DecodeNode(typename ShapeTree<C>::Node* node, 
						  const Camera& camera, const Image& image) const =0;

  void WalkGraph(typename ShapeTree<C>::Node* root_node, WorldState<PAYLOAD_SIZE>* worldstate, const Camera& camera, const Image& image) const;
  
};

template<class C, int PAYLOAD_SIZE> void Tag<C,PAYLOAD_SIZE>::WalkGraph(typename ShapeTree<C>::Node* root_node, WorldState<PAYLOAD_SIZE>* worldstate, const Camera& camera, const Image& image) const {
  PROGRESS("Walk graph");
  // walk the tree finding all the tags
  // if we find a Tag then decode node returns true so we know not to inspect any child contours
  if (root_node->matched.IsFitted()) {
    PROGRESS("Founnd matched node");
    LocatedObject<PAYLOAD_SIZE>* lobj = DecodeNode(root_node,camera,image);
    if (lobj) {      
      worldstate->Add(lobj);
    }
  }
  
  for(typename std::vector<typename ShapeTree<C>::Node* >::iterator step = root_node->children.begin(); 
      step != root_node->children.end(); 
      step++) {
    WalkGraph(*step,worldstate, camera,image);
  }
}

#endif//TAG_GUARD
