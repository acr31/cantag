/**
 * $Header$
 */

#ifndef SCENE_GRAPH_GUARD
#define SCENE_GRAPH_GUARD

#include <ShapeChain.hh>
#include <Ellipse.hh>
#include <SceneGraphNode.hh>
#include <CyclicBitSet.hh>

#define MAXLENGTH 10000
#define MAXDEPTH 20
#undef SCENE_GRAPH_DEBUG
#define MINCONTOUR_AREA 10
#define MINCONTOUR_LENGTH 10

int debug_contour_count = 0;

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
  SceneGraph();
  
  ~SceneGraph();

  /**
   * Update the scene graph with the given image
   */
  void Update(const Image& image, const Camera& camera);
  
  /**
   * Return the root node for this graph
   */
  inline SceneGraphNode<S,PAYLOAD_SIZE>* GetRootNode() const;

  /**
   * Return the first tag found in the scene graph that has this code
   * or NULL if none do.
   */
  inline LocatedObject<PAYLOAD_SIZE>* Find(const CyclicBitSet<PAYLOAD_SIZE>& code );
};

template<class S,int PAYLOAD_SIZE> SceneGraph<S,PAYLOAD_SIZE>::SceneGraph() : m_root(new SceneGraphNode<S,PAYLOAD_SIZE>()) { 
  store = cvCreateMemStorage(0);
}

template<class S,int PAYLOAD_SIZE> SceneGraph<S,PAYLOAD_SIZE>::~SceneGraph() {
  cvReleaseMemStorage(&store);
  if (m_root != NULL) {
    delete m_root;
  }
}

template<class S, int PAYLOAD_SIZE> void SceneGraph<S,PAYLOAD_SIZE>::Update(const Image& image, const Camera& camera) {
  CvSeq* root;
  cvFindContours(image.m_image,store,&root,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_NONE);

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

    if (c != NULL) {
      int contour_length = c->total;
      int contour_area  = (int)fabs(cvContourArea(c,CV_WHOLE_SEQ));
      if ((contour_length > MINCONTOUR_LENGTH) &&
	  (contour_area > MINCONTOUR_AREA)) {
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
  }
  while(cvNextTreeNode(&treeiter));
#ifdef IMAGE_DEBUG
  char filename[256];
  snprintf(filename,255,"camgrab-%d.bmp",debug_contour_count++);
  filename[255]=0;
  //    cvSaveImage(filename,debug0);
  cvReleaseImage(&debug0);
#endif
}
  

template<class S,int PAYLOAD_SIZE> SceneGraphNode<S,PAYLOAD_SIZE>* SceneGraph<S,PAYLOAD_SIZE>::GetRootNode() const {
  return m_root;
}

template<class S,int PAYLOAD_SIZE> LocatedObject<PAYLOAD_SIZE>* SceneGraph<S,PAYLOAD_SIZE>::Find(const CyclicBitSet<PAYLOAD_SIZE>& code) {
    return m_root->Find(code);
}


#endif//SCENE_GRAPH_GUARD

  
