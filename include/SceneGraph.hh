/**
 * $Header$
 */

#ifndef SCENE_GRAPH_GUARD
#define SCENE_GRAPH_GUARD

#include <ShapeChain.hh>
#include <Ellipse.hh>
#include <SceneGraphNode.hh>

#define MAXLENGTH 10000
#define MAXDEPTH 20
#define SCENE_GRAPH_DEBUG
#define MINCONTOUR_AREA 100
/**
 * A scene graph.  This class maintains a logical view of the current
 * image in terms of a hierachy of recognised shapes.
 *
 * \todo square matching is currently requiring that you change
 * APPROX_NONE to APPROX_SIMPLE.  Fix the square fitter.
 *
 * \todo regression test: create dummy images with squares etc in them
 * and try to match them
 *
 * \todo stop depending on opencv's stuff either implement my own contour follower or move it to the image class
 */
template<class S,int PAYLOAD_SIZE>
class SceneGraph {
private:
  SceneGraphNode<S,PAYLOAD_SIZE>* m_root;
  CvMemStorage* store;

public:
  /**
   * Create a scene graph.
   */ 
  SceneGraph() : m_root(new SceneGraphNode<S,PAYLOAD_SIZE>()) {
    store = cvCreateMemStorage(0);
  };

  ~SceneGraph() {
    cvReleaseMemStorage(&store);
    delete m_root;
  }
  /**
   * Update the scene graph with the given image
   */
  void Update(const Image& image, const Camera& camera) {
    IplImage *copy = cvCloneImage(image.m_image); // the find contours process changes the image ;-(
    CvSeq* root;
    cvFindContours(copy,store,&root,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_NONE);
    cvReleaseImage(&copy);

#ifdef IMAGE_DEBUG
    IplImage *debug0 = cvCloneImage(image.m_image);
    cvConvertScale(debug0,debug0,0.5,128);
#endif

    // this array of pointers to nodes will be used to keep track of
    // the current parent node
    SceneGraphNode<S,PAYLOAD_SIZE>* parents[MAXDEPTH] = {0};
    delete m_root;
    m_root = new SceneGraphNode<S,PAYLOAD_SIZE>();
    parents[0] = m_root;

    CvTreeNodeIterator treeiter;
    cvInitTreeNodeIterator(&treeiter,root,MAXDEPTH);

    do {
      CvSeq *c = (CvSeq*)treeiter.node;

      if ((c != NULL) && (fabs(cvContourArea(c,CV_WHOLE_SEQ))>MINCONTOUR_AREA)) {
#ifdef SCENE_GRAPH_DEBUG
	PROGRESS("Found contour at level "<< treeiter.level);
#endif
	int count = c->total;
	int total_count = count;
	int step = 1;
	// halve the length of the contour until it fits in MAXLENGTH,
	// increase step in tandem so we dont just chop off a load of
	// points at the end
	while (count > MAXLENGTH) { 
	  count>>=1;
	  step<<=1;
	}

#ifdef SCENE_GRAPH_DEBUG
	PROGRESS("Using "<<count<<" points out of " << total_count << " with steps of " << step);
#endif

	// copy the points out of the sequence into an array so we can use them
	float fpoints[MAXLENGTH*2];
	CvSeqReader reader;
	cvStartReadSeq( c, &reader, 0 );
	int fpoints_pointer = 0;
	for(int pt=0;pt<c->total;pt++) {
	  CvPoint val;
	  CV_READ_SEQ_ELEM(val,reader);
	  if (pt % step == 0) {
	    fpoints[fpoints_pointer++] = (float)val.x;
	    fpoints[fpoints_pointer++] = (float)val.y;	  
	  }
	}

	// normalise the image points
	camera.ImageToNPCF(fpoints,count);

	SceneGraphNode<S,PAYLOAD_SIZE>* next = new SceneGraphNode<S,PAYLOAD_SIZE>(fpoints,count);

	if (next->GetShapes().IsChainFitted()) {
#ifdef IMAGE_DEBUG
	  // draw found contours
	  cvDrawContours(debug0,c,0,0,0,2,8);
#endif
	  // now add this node as a child of its parent.  Its parent is
	  // the node stored in index level-1 in the parents array. 
	  for(int i=treeiter.level;i>=0;i--) {
	    if (parents[i] != NULL) {
	      parents[i]->AddChild(next);
#ifdef SCENE_GRAPH_DEBUG
	      PROGRESS("Added node to parent at level "<<i);
#endif
	      break;
	    }
	  }
	  parents[treeiter.level+1] = next;
	}
	else {
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("This contour does not match any shape in the chain---discarding it.");
#endif
	  delete next;
	}
      }
    }
    while(cvNextTreeNode(&treeiter));
#ifdef IMAGE_DEBUG
    cvSaveImage("debug-scenegraphcontours.jpg",debug0);
    cvReleaseImage(&debug0);
#endif
   
  }
  
  /**
   * Return the root node for this graph
   */
  SceneGraphNode<S,PAYLOAD_SIZE>* GetRootNode() const { return m_root; };


private:

  // maintain a linked list of pointers to open chains
  
  // scan across the line
  // if you see a white to black transition then 
  //    scan accross the linked list of pointers looking for an open chain to attach to
  //    if the item at the head of the list has image column greater than the current position +1 then stop looking
  //    if you find a chain then add this point to it
  //    if you dont find a chain then start a new one and insert it into the list at the current position
  //    every chain you take off the list when looking for the new chain is now closed
  // if you continue to find black pixels, add them to the chain
  // if you find a black to white transition then insert this chain into the list again
  // 

  /*

  void FollowContours(Image& image) {

    // we alter the current image to encode
    // 1 bit of pixel value
    // 1 bit of edge value
    // the contour id
    int contourID = 1;
    int last_contourID;

    // the current scene.  The node at the top of the stack is the
    // current parent shape
    std::stack<SceneGraphNode*> node_stack;
    node_stack.push(m_root);

    for(int raster_y = 0; raster_y < image.GetHeight(); raster_y++) {
      uchar* data = image.GetRow(y);
      int last_contourID = 1;
      unsigned char previous_value = image.GetPixelNoCheck(0,raster_y);

      // if we see an exit pixel that has been coloured as a visit by
      // the parent contour then pop the parent off the stack

      // if we see an entry pixel then it must be either a child of
      // the parent contour or an ignored contour.  If it is a child
      // then push the child onto the stack.  If it is a ignored
      // contour then ignore it.

      // if we see a pixel that is an unmarked edge we pick a new
      // contour id and follow it round, marking it.  An unmarked edge
      // is a transition from black to white.

      // we mark with the new id except where we see an exit pixel
      // (one with a blank pixel on the right of it) 

      // We then try and match the shape, if it matches we add this as
      // a child of the current parent
      
      // Then push it onto the stack 

      // repeat

      // we move a window of 3 pixels across the image
      for(int raster_x = 0; raster_x < image.GetWidth()-3; raster_x++) {
	if (data[1] & 0x2) { // this is a edge pixel
	  if (!data[0] & 0x2) {
	    // this is an entry pixel
	    if (data[1] & ~0x2) {
	      // this is already issued as a contour
	      SceneGraphNode* child = node_stack.top()->GetChildByContourID(data[1] >> 2);
	      if (child != NULL) {
		// this matches a fitted shape
		node_stack.push(child);
	      }
	      else {
		// this doesn't match a fitted shape so ignore it
	      }
	    }
	    else {
	      // this doesn't match an existing contour
	      contourID++;
	      
	    }
	  }

	int follow_x;
	int follow_y;
	if (image_value & 0x1 && !(previous_value & 0x1)) {
	  // this is the border following starting point of an outer border
	  NBD++;
	  
	  follow_x = raster_x-1;
	  follow_y = raster_y;
	  
	  // identify the parent of the current border
	  
	}
	else if (image_value >= 1 && image.GetPixelNoCheck(raster_x+1,raster_y) == 0) {
	  // this is the border following starting point of a hole border
	  NBD++;
	  follow_x = raster_x+1;
	  follow_y = raster_y;
	  LNBD = image_value;
	}
      }
      
    }

  }
  */
};

#endif//SCENE_GRAPH_GUARD

