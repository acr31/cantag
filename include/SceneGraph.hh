/**
 * $Header$
 */

#ifndef SCENE_GRAPH_GUARD
#define SCENE_GRAPH_GUARD

#include <ShapeChain.hh>
#include <Ellipse.hh>
#include <SceneGraphNode.hh>
#include <map>

#undef SCENE_GRAPH_DEBUG


#define MAXLENGTH 10000
#define MAXDEPTH 20
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

struct ContourStatistics {
  int length;
  int min_x;
  int max_x;
  int min_y;
  int max_y;
  bool convex;
};

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
  void Update(Image& image, const Camera& camera);
  
  /**
   * Return the root node for this graph
   */
  inline SceneGraphNode<S,PAYLOAD_SIZE>* GetRootNode() const { return m_root; };


private:

  /**
   * Follows the edge that starts at (start_x,start_y) and stores the
   * points found in the points_buffer.  The passed NBD is used to mark
   * the contour in the image.
   *
   * If more than maxcount points are found the list is truncated.
   *
   * The algorithm works by examining the 8-connected region around the current contour pixel
   *
   *   +---+---+---+
   *   | 7 | 6 | 5 |
   *   +---+---+---+
   *   | 0 | X | 4 |
   *   +---+---+---+
   *   | 1 | 2 | 3 |
   *   +---+---+---+
   *
   *  We start at position 0 and scan counterclockwise looking for a
   *  1-element (that lies on the contour).  When we find it we store
   *  the position in the array and begin searching again with the newly
   *  found 1-element at the centre of the region.  The search resumes
   *  after the position of the old contour pixel.
   *
   *  This algorithm searches for white objects on a black background!
   *
   *  Contour statistics are accumulated during the walk
   *  1) number of points
   *  2) contour length - if we select the next point in the contour
   *     from the 4- region (cells 0,2,4,6) we add one to the length, if we
   *     select from the other cells we add sqrt(2).  We approximate this
   *     with integer arithmetic by adding 32 for every 4-connected point
   *     and 45 (sqrt(2)*32 = 45.255) for every other point and then
   *     dividing by 32 (or left shifting 5) at the end.
   *  3) bounding box
   *  4) convexity
   */
  int FollowContour(Image& image, // the image to track the contour in (will be altered)
		    unsigned char* data_pointer,
		    int start_x,  int start_y, // the start position (must lie on contour)
		    float* points_buffer,  // the buffer to store the points
		    const int maxcount, // maximum number of points to return (buffer size must be twice this)
		    ContourStatistics& statistics,   // contour statistics structure (can be NULL)
		    int position,  // the position in the 8-connected region to start searching from
		    int nbd  // the NBD to mark this contour with
		    );
  enum bordertype_t { OUTER_BORDER = 1, HOLE_BORDER = 0};
  struct Contour {
    int parent_id;
    SceneGraphNode<S,PAYLOAD_SIZE>* node;
    bordertype_t bordertype;

    Contour() {};
    Contour(const Contour& c) : parent_id(c.parent_id), node(c.node), bordertype(c.bordertype) {};
    Contour(SceneGraphNode<S,PAYLOAD_SIZE>* in_node,bordertype_t border) : node(in_node), bordertype(border) {};
    Contour(int in_parent_id, SceneGraphNode<S,PAYLOAD_SIZE>* in_node,bordertype_t border) : parent_id(in_parent_id), node(in_node), bordertype(border) {};
  };

  

};


template<class S,int PAYLOAD_SIZE> SceneGraph<S,PAYLOAD_SIZE>::SceneGraph() : m_root(new SceneGraphNode<S,PAYLOAD_SIZE>()) {
  store = cvCreateMemStorage(0);
}

template<class S,int PAYLOAD_SIZE> SceneGraph<S,PAYLOAD_SIZE>::~SceneGraph() {
  cvReleaseMemStorage(&store);
  delete m_root;
}

template<class S,int PAYLOAD_SIZE> void SceneGraph<S,PAYLOAD_SIZE>::Update(Image& image, const Camera& camera) {

  unsigned char* data_pointer = image.GetDataPointer();
  int image_width = image.GetWidth();
  int image_height = image.GetHeight();
  int image_width_1 = image_width-1;

  // our frame border is a hole border.  We write zero's around the
  // edge of the image to make sure that our border follower never
  // goes out of range
  for(int i=0;i<image_width;++i) {
    *data_pointer = 0;
    data_pointer++;
  }
  for(int i=0;i<image_height-2;++i) {
    *data_pointer = 0;
    data_pointer+=image_width_1;
    *data_pointer = 0;
    ++data_pointer;
  }
  for(int i=0;i<image_width;++i) {
    *data_pointer = 0;
    ++data_pointer;
  }  

#ifdef IMAGE_DEBUG
  image.Save("debug-scenegraph-borders.bmp");
  Image debug(image.GetWidth(),image.GetHeight());
#endif

  std::map<int,Contour> node_hash;
  delete m_root;
  m_root = new SceneGraphNode<S,PAYLOAD_SIZE>();
  node_hash[1] = Contour(m_root,HOLE_BORDER);

  float points_buffer[MAXLENGTH*2];
  data_pointer = image.GetDataPointer();
  int NBD = 2;
  for(int raster_y=0;raster_y < image_height; ++raster_y) {
    int LNBD = 1; // we've just "seen" the frame border so set the last seen border id to match
    ++data_pointer; // exclude the first pixel on the line
    for(int raster_x=1;raster_x < image_width_1;++raster_x, ++data_pointer) {
      if (*data_pointer) {  // this pixel is a 1-element or it has been visited before
	const int cNBD = *data_pointer>>1;
	int contour_length;
	bordertype_t border_type;
	ContourStatistics contour_statistics;;
	if (!cNBD && !(*(data_pointer-1))) { // this pixel has not been seen before and the previous pixel is a 0-element
	  border_type = OUTER_BORDER;	  
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("Found outer border.  Following from " << raster_x << "," << raster_y);
#endif
	  contour_length = FollowContour(image, data_pointer, raster_x, raster_y, points_buffer, MAXLENGTH, contour_statistics,0, NBD);
	} 
	else if ((*data_pointer & 0x1) && !(*(data_pointer+1))) { // this pixel has not been seen before or it is not an exit pixel, and the next pixel is a 0-element
	  border_type = HOLE_BORDER;
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("Found hole border.  Following from " << raster_x << "," << raster_y);
#endif
	  contour_length = FollowContour(image, data_pointer, raster_x, raster_y, points_buffer, MAXLENGTH, contour_statistics,4, NBD);
	}
	else if (cNBD && (*data_pointer & 0x1)) { // this pixel has been seen before and it is not an exit pixel
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("Updating LNBD to " << cNBD);
#endif
	  LNBD = cNBD;
	  continue;
	}
	else {
	  continue;
	}

	// now decide the parent of this border
	    
	// NewBorder    LNBDType   Parent
	// --------------------------------------
	// OUTER        OUTER      Parent of LNBD
	// OUTER        HOLE       LNBD
	// HOLE         OUTER      LNBD
	// HOLE         HOLE       Parent of LNBD

	const int parentNBD = border_type == node_hash[LNBD].bordertype ?  node_hash[LNBD].parent_id : LNBD;
#ifdef IMAGE_DEBUG
	debug.DrawPolygon(points_buffer,contour_length,0,1);
#endif
	SceneGraphNode<S,PAYLOAD_SIZE>* next_node;
	if (contour_length > 10) {
	  camera.ImageToNPCF(points_buffer,contour_length);
	  next_node = new SceneGraphNode<S,PAYLOAD_SIZE>(points_buffer,contour_length);
	  if (next_node->GetShapes().IsChainFitted()) {
	    node_hash[parentNBD].node->AddChild(next_node);
	  }
	  else {
	    delete next_node;
	    next_node = node_hash[parentNBD].node;
	  }
	}
	else {
	  next_node = node_hash[parentNBD].node;
	}
	node_hash[NBD] = Contour(parentNBD,next_node,border_type);

	NBD = (NBD + 1) & 0x7F;
	if (NBD==0) { ++NBD; }	 

      }
      else { // this is a 0-element

      }
    }
    ++data_pointer; // exclude the last pixel on the line
  }

#ifdef IMAGE_DEBUG
  debug.Save("debug-scenegraph-contours.bmp");
#endif
}


template<class S,int PAYLOAD_SIZE> int SceneGraph<S,PAYLOAD_SIZE>::FollowContour(Image& image, unsigned char* data_pointer, int start_x, int start_y, float* points_buffer, const int maxcount, ContourStatistics& statistics, int start_position, const int nbd) {
  int image_width = image.GetWidth();

  //   +---+---+---+
  //   | 7 | 6 | 5 |
  //   +---+---+---+
  //   | 0 |   | 4 |
  //   +---+---+---+
  //   | 1 | 2 | 3 |
  //   +---+---+---+
  const int offset_x[] = { -1,-1,0,1,1,1,0,-1 };
  const int offset_y[] = { 0,1,1,1,0,-1,-1,-1 };

  const int offset[] = {-1,                //  0 
			image_width-1,     //  1
			image_width,       //  2
			image_width+1,     //  3
			1,                 //  4
			1-image_width,     //  5
			-image_width,      //  6
			-image_width-1};   //  7


  // data_pointer is the current focus of the search region
  // sample_pointer is the current read point
  unsigned char* sample_pointer;

  // position is our current index into the search region
  int position = start_position;

  // contour_0 is the first pixel in the contour
  unsigned char* contour_0 = data_pointer;

  // pointer is the index we last wrote to in the points_buffer
  int pointer = -1;

  int NBD_shift = nbd << 1;

  do {
    position = (position - 1) & 0x7;
    sample_pointer = data_pointer+offset[position];
    if (*sample_pointer) { break; }
  }
  while (position != start_position);

#ifdef SCENE_GRAPH_DEBUG
  PROGRESS("Finished clockwise scan at " << position << " (started:" << start_position << ")");
#endif

  if (position == start_position) {
    // one pixel contour
    *data_pointer = NBD_shift;
    points_buffer[0] = start_x;
    points_buffer[1] = start_y;
#ifdef SCENE_GRAPH_DEBUG
    PROGRESS("Found 1 pixel contour starting from "<< start_x << "," << start_y);
#endif
    return 1;
  }
  else {
    position = (position + 1) & 0x7;

    // contour_n is the last pixel in the contour
    unsigned char* contour_n = sample_pointer;

    bool cell4_is_0 = false; // will be set to true when we search a region if we pass cell4 and cell4 is a 0-element
    
    statistics.length = 0; // the contour length
    statistics.min_x = start_x; // bounding box
    statistics.max_x = start_x; // bounding box
    statistics.min_y = start_y; // bounding box
    statistics.max_y = start_y; // bounding box
    statistics.convex = true; // true if this contour is convex

    while(pointer < MAXLENGTH*2) {
      sample_pointer = data_pointer+offset[position];
      if (*sample_pointer) {
#ifdef SCENE_GRAPH_DEBUG
	PROGRESS("Found 1-pixel at position " << position << " around " << start_x << "," << start_y);
#endif
	// we now need to mark this pixel
	// 1) if the pixel sample_x+1,sample_y (cell 4) is a 0-element and we
	// have examined it whilst looking for this 1-element then this
	// is an exit pixel.  Write (NBD,r).
	if (cell4_is_0) {
	  *data_pointer = NBD_shift;
	}
	// 2) else if sample_x,sample_y is unmarked write
	// (NBD,l).
	else if (!(*sample_pointer & ~0x1)) {
	  *data_pointer = NBD_shift | 0x1;
	}

	// update the length
	statistics.length += (position & 0x1 ? 45 : 32);
	// update the bounding box
	if (start_x < statistics.min_x) { statistics.min_x = start_x; }
	else if (start_x > statistics.max_x) { statistics.max_x = start_x; }
	if (start_y < statistics.min_y) { statistics.min_y = start_y; }
	else if (start_y > statistics.max_y) { statistics.max_y = start_y; }

	// check the stopping condition
	if ((pointer > 4) && 
	    (data_pointer == contour_n) &&
	    (sample_pointer = contour_0)) {
	  statistics.length >>= 5;
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("Found " << (pointer>>1) << " pixel contour starting from "<< points_buffer[0] << "," << points_buffer[1]);
#endif

	  return pointer>>1;;
    	}

	// store this point in the pixel chain and update the start position
	points_buffer[++pointer] = start_x;
	points_buffer[++pointer] = start_y;
	start_x += offset_x[position];
	start_y += offset_y[position];
	data_pointer = sample_pointer;

	// reset the search value for cell 4
	cell4_is_0=false;

	// if we find the 1-element at position n anti-clockwise then we
	// need to shift the region to be centered on the new point and
	// resume searching one place after the previous central point
	// i.e. if we find a point at 3 we need to resume searching from
	// 0 (one place past the old centre at 7)
	
	//    +---+---+---+---+         +---+---+---+---+   
	//    | 7 | 6 | 5 |   |         |   |   |   |   |
	//    +---+---+---+---+         +---+---+---+---+   
	//    | 0 |   | 4 |   |         |   |_7_| 6 | 5 |
	//    +---+---+---+---+   -->   +---+---+---+---+   
	//    | 1 | 2 | X |   |         |   | 0 | X | 4 |
	//    +---+---+---+---+         +---+---+---+---+   
	//    |   |   |   |   |         |   | 1 | 2 | 3 |
	//    +---+---+---+---+         +---+---+---+---+   
	
	//  Found posn (anti)       |  Resume posn (anti)
	//  -----------------------------------------------
	//          0               |         5
	//          1               |         6
	//          2               |         7
	//          3               |         0
	//          4               |         1
	//          5               |         2
	//          6               |         3
	//          7               |         4
	//
	position = (position+5) & 0x7;
      }
      else {  // pixel is a 0-element
	if (position == 4) {  // if we are at cell4 (this is a 0-element) then set the cell4_is_0 flag
	  cell4_is_0 = true; 
	}
	
	// advance the position
	position = (position+1) & 0x7;
      }

    }

    statistics.length >>= 5;
    return (pointer-3)/2;
  }
}


#endif//SCENE_GRAPH_GUARD

