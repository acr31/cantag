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


/**
 * A scene graph.  This class maintains a logical view of the current
 * image in terms of a hierachy of recognised shapes.
 */
template<class S>
class SceneGraph {
private:
  SceneGraphNode<S>* m_root;

  CvPoint m_points[MAXLENGTH];
  float m_fpoints[MAXLENGTH*2];
  SceneGraphNode<S>* m_parents[MAXDEPTH];

public:
  /**
   * Create a scene graph.
   */ 
  SceneGraph() : m_root(NULL) {};

  ~SceneGraph() {
    if (m_root != NULL) {
      delete(m_root);
    }
  }
  
  /**
   * Update the scene graph with the given image
   */
  void Update(const Image& image, const Camera& camera) {
    IplImage *copy = cvCloneImage(image.m_image); // the find contours process changes the image ;-(
    
    CvMemStorage* store = cvCreateMemStorage(0);
    CvSeq* root;
    cvFindContours(copy,store,&root,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_NONE);
    cvReleaseImage(&copy);

#ifdef IMAGE_DEBUG
    IplImage *debug0 = cvCloneImage(image.m_image);
    cvConvertScale(debug0,debug0,0.5,128);
#endif

    CvTreeNodeIterator treeiter;
    cvInitTreeNodeIterator(&treeiter,root,MAXDEPTH);
    for(int i=0;i<MAXDEPTH;i++) {
      m_parents[i] = NULL;
    }
    m_parents[0] = new SceneGraphNode<S>();
    m_root = m_parents[0];
    do {
      CvSeq *c = (CvSeq*)treeiter.node;
      if ((c != NULL) && (fabs(cvContourArea(c,CV_WHOLE_SEQ))>10)) {
#ifdef IMAGE_DEBUG
	// draw found contours
	cvDrawContours(debug0,c,0,0,0,2,8);
#endif

#ifdef SCENE_GRAPH_DEBUG
	PROGRESS("Found contour at level "<< treeiter.level);
#endif
	int count = c->total;
	if (count > MAXLENGTH) { 
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("Truncated length "<<count << " to "<< MAXLENGTH);
#endif
	  count = MAXLENGTH; 
	}
	cvCvtSeqToArray( c, m_points , cvSlice(0,count));
	int pointer = 0;
	for( int pt = 0; pt < count; pt++ ) {
	  m_fpoints[pointer++] = (float)m_points[pt].x;
	  m_fpoints[pointer++] = (float)m_points[pt].y;	
	}
	camera.ImageToNPCF(m_fpoints,count);
	SceneGraphNode<S>* next = new SceneGraphNode<S>(m_fpoints,count);

	if (next->GetShapes().IsChainFitted()) {
	  // now add this node as a child of its parent.  Its parent is
	  // the node stored in index level-1 in the parents array. 
	  bool added = false;
	  for(int i=treeiter.level;i>=0;i--) {
	    if (m_parents[i] != NULL) {
	      m_parents[i]->AddChild(next);
#ifdef SCENE_GRAPH_DEBUG
	      PROGRESS("Added node to parent at level "<<i);
#endif
	      added = true;
	      break;
	    }
	  }
	  m_parents[treeiter.level+1] = next;
	}
	else {
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("This contour does not match any shape in the chain---discarding it.");
	  delete next;
#endif
	}
      }
    }
    while(cvNextTreeNode(&treeiter));
#ifdef IMAGE_DEBUG
    cvSaveImage("debug-scenegraphcontours.jpg",debug0);
    cvReleaseImage(&debug0);
#endif
    cvReleaseMemStorage(&store);
  }
  
  /**
   * Return the root node for this graph
   */
  SceneGraphNode<S>* GetRootNode() const { return m_root; };
};

#endif//SCENE_GRAPH_GUARD

