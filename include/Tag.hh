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
  
  std::bitset<PAYLOAD_SIZE>* m_must_match;
  
  Tag() : m_must_match(NULL) {}

  virtual void Draw2D(Image& image, const std::bitset<PAYLOAD_SIZE>& tag_data)= 0;
  void WalkSceneGraph(SceneGraphNode<C>* root_node, const Camera& camera, const Image& image) {
    // walk the tree finding all the tags
    DecodeNode(root_node,camera,image);
    for(typename std::vector<SceneGraphNode<C>* >::iterator step = root_node->GetChildren().begin(); 
	step != root_node->GetChildren().end(); 
	step++) {
      if (!(*step)->IsInspected()) {
	WalkSceneGraph(*step,camera,image);
      }
    }
  }

  virtual void DecodeNode(SceneGraphNode<C>* node, const Camera& camera, const Image& image) =0;
};

#endif//TAG_GUARD
