/**
 * $Header$
 *
 */
#ifndef TAG_GUARD
#define TAG_GUARD

#include "Config.hh"
#include "Drawing.hh"
#include <Camera.hh>
#include <SceneGraphNode.hh>
#include <vector>

template<class C>
class Tag {
public:
  virtual void Draw2D(Image* image, unsigned long long code, int black, int white)= 0;
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
