/**
 * $Header$
 *
 */
#ifndef TAG_GUARD
#define TAG_GUARD

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/Camera.hh>
#include <total/ShapeTree.hh>
#include <total/CyclicBitSet.hh>
#include <total/WorldState.hh>

#include <vector>
#include <map>

namespace Total {
  /**
   * Abstract superclass for all tags.
   *
   * \todo awful inclusion of a mutable temporary store to store the relationship between nodes and locatedobjects - remove it!
   */
  template<class C, int PAYLOAD_SIZE>
  class Tag {
  private:
    mutable std::map<typename ShapeTree<C>::Node*, LocatedObject<PAYLOAD_SIZE>* > temp_store;
    mutable int flag;
  public:
    typedef C TagShapeType;
    static const int TagPayloadSize = PAYLOAD_SIZE;
    typedef CyclicBitSet<PAYLOAD_SIZE> TagPayloadType;

    TagPayloadType* m_must_match;
  
    Tag() :  temp_store(),flag(0),m_must_match(NULL) {}

    virtual ~Tag() {}

    /**
     * Draw this tag onto the image object provided
     */
    virtual void Draw2D(Image& image, TagPayloadType& tag_data) const = 0;
    
    /**
     * Attempt to build a locatedobject for this node by trying to build a 3d transform for mapping onto the tag
     */
    virtual LocatedObject<PAYLOAD_SIZE>* GetTransform(typename ShapeTree<C>::Node* node, 
						      const Camera& camera, const Image& image) const =0;
  
    /**
     * Attempt to decode the tag pointed at by this located object, all
     * valid codes found will be added to the tag_codes of the located
     * object, if there are no valid codes then tag_codes will be
     * unaltered - this typically indicates that this is not actually a
     * tag but just a likely looking shape in the image.
     */
    virtual bool DecodeNode(LocatedObject<PAYLOAD_SIZE>* lobj,
			    const Camera& camera, const Image& image) const =0;

    /**
     * Check that the 3d transform in this located object is consistant with this node from the scene graph
     */
    virtual bool CheckTransform(const LocatedObject<PAYLOAD_SIZE>* lobj, typename ShapeTree<C>::Node* node) const = 0;

    /**
     * Check that the code in this located object is a valid reading
     * from the image. Will return true if this locatedobject has the
     * correct code in it or if this located object has no code in it
     * and the image has no valid code for this tag too.
     */
    virtual bool CheckDecode(const LocatedObject<PAYLOAD_SIZE>* lobj, const Camera& camera, const Image& image) const =0;

    /**
     * Traverse the shapetree calling GetTransform on each node
     */
    void WalkGraph(typename ShapeTree<C>::Node* root_node, WorldState<PAYLOAD_SIZE>* worldstate, const Camera& camera, const Image& image) const;
  
    /**
     * Walk the list of locatedobjects in the scene and call DecodeNode on each one
     */
    void DecodeTags(WorldState<PAYLOAD_SIZE>* worldstate,  const Camera& camera, const Image& image) const;

    /**
     * Check that all the nodes in the shapetree are correctly represented in
     * the worldstate.  This means that all nodes corresponding to located
     * objects should be checked to see that they do indeed correspond (by
     * calling CheckTransform).  If check_negative is true then all nodes not
     * corresponding to located objects are checked to see that they
     * cannot be interpreted to do so.
     */
    bool CheckGraph(typename ShapeTree<C>::Node* root_node, const WorldState<PAYLOAD_SIZE>* worldstate, const Camera& camera, const Image& image, bool check_negative) const;

    /**
     * Check that all the locatedobjects in the worldstate have correctly interpreted data codes
     */
    bool CheckTags(WorldState<PAYLOAD_SIZE>* worldstate,  const Camera& camera, const Image& image, bool check_negative) const;

  };

  template<class C, int PAYLOAD_SIZE> void Tag<C,PAYLOAD_SIZE>::WalkGraph(typename ShapeTree<C>::Node* root_node, WorldState<PAYLOAD_SIZE>* worldstate, const Camera& camera, const Image& image) const {
    int setflag = 0;
    if (!flag) { temp_store.clear(); flag =1; setflag = 1;}
    // walk the tree finding all the tags
    // if we find a Tag then decode node returns true so we know not to inspect any child contours
    if (root_node->matched.IsFitted()) {
      LocatedObject<PAYLOAD_SIZE>* lobj = GetTransform(root_node,camera,image);
      if (lobj) {      
	temp_store[root_node] = lobj;
	worldstate->Add(lobj);
      }
    }
  
    for(typename std::vector<typename ShapeTree<C>::Node* >::iterator step = root_node->children.begin(); 
	step != root_node->children.end(); 
	step++) {
      WalkGraph(*step,worldstate, camera,image);
    }
    if (setflag) flag = 0;
  }

  template<class C, int PAYLOAD_SIZE> void Tag<C,PAYLOAD_SIZE>::DecodeTags(WorldState<PAYLOAD_SIZE>* worldstate, const Camera& camera, const Image& image) const {
    for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = worldstate->GetNodes().begin();
	i != worldstate->GetNodes().end();
	++i) {
      DecodeNode(*i,camera,image);
    }
  }

  /**
   * \todo assumes that everything in temp_store is in the worldstate and vice versa
   */
  template<class C, int PAYLOAD_SIZE> bool Tag<C,PAYLOAD_SIZE>::CheckGraph(typename ShapeTree<C>::Node* root_node, const WorldState<PAYLOAD_SIZE>* worldstate, const Camera& camera, const Image& image, bool check_negative) const {
    if (temp_store.count(root_node) == 1) {
      if (!CheckTransform(temp_store[root_node],root_node)) return false;
    }
    else if (check_negative) {
      LocatedObject<PAYLOAD_SIZE>* lobj = GetTransform(root_node,camera,image);
      if (lobj) { delete lobj; return false; }
    }
    for(typename std::vector<typename ShapeTree<C>::Node* >::iterator step = root_node->children.begin(); 
	step != root_node->children.end(); 
	step++) {
      if (!CheckGraph(*step,worldstate, camera,image,check_negative)) return false;
    }
    return true;
  }

  template<class C, int PAYLOAD_SIZE> bool Tag<C,PAYLOAD_SIZE>::CheckTags(WorldState<PAYLOAD_SIZE>* worldstate,  const Camera& camera, const Image& image,bool check_negative) const {
    for(typename std::vector<LocatedObject<PAYLOAD_SIZE>*>::const_iterator i = worldstate->GetNodes().begin();
	i != worldstate->GetNodes().end();
	++i) {
      if (check_negative || (*i)->tag_codes.size() > 0) {
	if (!CheckDecode(*i,camera,image)) return false;
      }
    }
    return true;
  }

}
#endif//TAG_GUARD
