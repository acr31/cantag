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

  /**
   * An implementation of Suzuki's topological structural analysis algorithm
   *
   * @Article{a:cvgip85:suzuki,
   *  author        = "S. Suzuki and K. Abe",
   *  title         = "Topological Structural Analysis of Digitized Binary Images by Border Following",
   *  journal       = "Computer Vision, Graphics, and Image Processing",
   *  year          = "1985",
   *  volume        = "30",
   *  number        = "1",
   *  pages         = "32--46"}
   *
   */
  void FollowContours(const Image& image) {

    int NBD = 1;
    int LNBD;

    for(int raster_y = 0; raster_y < image.GetHeight(); raster_y++) {
      LNBD=1;      
      for(int raster_x = 1; raster_x < image.GetWidth(); raster_x++) {
	unsigned char image_value = image.GetPixelNoCheck(raster_x,raster_y);
	int follow_x;
	int follow_y;
	if (image_value == 1 && image.GetPixelNoCheck(raster_x-1,raster_y) == 0) {
	  // this is the border following starting point of an outer border
	  NBD++;
	  
	  follow_x = raster_x-1;
	  follow_y = raster_y;
	  
	  // identify the parent of the current borde
	  
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
};

#endif//SCENE_GRAPH_GUARD

