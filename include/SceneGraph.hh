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
#undef SCENE_GRAPH_DEBUG
#undef ADD_CONTOUR_NOISE

#ifdef ADD_CONTOUR_NOISE
# include <boost/random.hpp>
#endif

/**
 * A scene graph.  This class maintains a logical view of the current
 * image in terms of a hierachy of recognised shapes.
 *
 * \todo square matching is currently requiring that you change
 * APPROX_NONE to APPROX_SIMPLE.  Fix the square fitter.
 *
 * \todo has a memory leak. we throw away scenegraphnodes without
 * deleteing them
 *
 * \todo regression test: create dummy images with squares etc in them
 * and try to match them
 */
template<class S,int PAYLOAD_SIZE>
class SceneGraph {
private:
  SceneGraphNode<S,PAYLOAD_SIZE>* m_root;

  CvPoint m_points[MAXLENGTH];
  float m_fpoints[MAXLENGTH*2];
  SceneGraphNode<S,PAYLOAD_SIZE>* m_parents[MAXDEPTH];

#ifdef ADD_CONTOUR_NOISE
  boost::normal_distribution<float> m_normal_dist;
  boost::rand48 m_rand_generator;
  boost::variate_generator<boost::rand48&, boost::normal_distribution<float> > m_normal;
#endif

public:
  /**
   * Create a scene graph.
   */ 
  SceneGraph() : m_root(NULL) 
#ifdef ADD_CONTOUR_NOISE
		 ,m_normal_dist(0,1),
		 m_rand_generator((unsigned long long)time(0)),
		 m_normal(m_rand_generator,m_normal_dist)
#endif
{};

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
    cvFindContours(copy,store,&root,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
    cvReleaseImage(&copy);

#ifdef IMAGE_DEBUG
    IplImage *debug0 = cvCloneImage(image.m_image);
    cvConvertScale(debug0,debug0,0.5,128);
#endif

    for(int i=0;i<MAXDEPTH;i++) {
      m_parents[i] = NULL;
    }
    m_parents[0] = new SceneGraphNode<S,PAYLOAD_SIZE>();
    m_root = m_parents[0];

    CvTreeNodeIterator treeiter;
    cvInitTreeNodeIterator(&treeiter,root,MAXDEPTH);

    do {
      CvSeq *c = (CvSeq*)treeiter.node;

      if ((c != NULL) && (fabs(cvContourArea(c,CV_WHOLE_SEQ))>100)) {
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
#ifdef ADD_CONTOUR_NOISE
	  float xoff = m_normal();
	  float yoff = m_normal();
	  //	  PROGRESS("x    " << m_fpoints[pointer-2]);
	  //	  PROGRESS("y    " << m_fpoints[pointer-1]);
	  //	  PROGRESS("xoff " << xoff);
	  //	  PROGRESS("yoff " << yoff);

	  m_fpoints[pointer-2] += xoff;
	  m_fpoints[pointer-1] += yoff;
#endif	
	}
	camera.ImageToNPCF(m_fpoints,count);
	SceneGraphNode<S,PAYLOAD_SIZE>* next = new SceneGraphNode<S,PAYLOAD_SIZE>(m_fpoints,count);

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
  SceneGraphNode<S,PAYLOAD_SIZE>* GetRootNode() const { return m_root; };
};

#endif//SCENE_GRAPH_GUARD

