/**
 * $Header$
 *
 */
#ifndef TAG_GUARD
#define TAG_GUARD

#include <Config.hh>
#include <Image.hh>
#include <Camera.hh>
#include <SceneGraphNode.hh>
#include <vector>
#include <bitset>

template<class C, int PAYLOAD_SIZE>
class Tag {
public:
  typedef C TagShapeType;
  static const int TagPayloadSize = PAYLOAD_SIZE;
  
  std::bitset<PAYLOAD_SIZE>* m_must_match;
  
  Tag() : m_must_match(NULL) {}

  virtual void Draw2D(Image& image, const std::bitset<PAYLOAD_SIZE>& tag_data) const = 0;
  void WalkSceneGraph(SceneGraphNode<C>* root_node, const Camera& camera, const Image& image) {
    // walk the tree finding all the tags
    // if we find a Tag then decode node returns true so we know not to inspect any child contours
    if (!DecodeNode(root_node,camera,image)) {
      for(typename std::vector<SceneGraphNode<C>* >::iterator step = root_node->GetChildren().begin(); 
	  step != root_node->GetChildren().end(); 
	  step++) {
	if (!(*step)->IsInspected()) {
	  WalkSceneGraph(*step,camera,image);
	}
      }
    }
  }

  /**
   * Decode the passed node. Return true if you find a valid tag there or false otherwise
   */
  virtual bool DecodeNode(SceneGraphNode<C>* node, const Camera& camera, const Image& image) =0;
};

#endif//TAG_GUARD
