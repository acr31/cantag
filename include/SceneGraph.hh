/**
 * $Header$
 */

#ifndef SCENE_GRAPH_GUARD
#define SCENE_GRAPH_GUARD

#include <ShapeChain.hh>
#include <Ellipse.hh>
#include <SceneGraphNode.hh>
#include <map>

#define SCENE_GRAPH_DEBUG


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
#ifdef SCENE_GRAPH_DEBUG
  PROGRESS("Updating Scene Graph");
#endif
      // a lookup from contour IDs to the shape assigned to them
    std::map<int,Contour> node_hash;
    
    delete m_root;
    m_root = new SceneGraphNode<S,PAYLOAD_SIZE>();

    // the outer border (the frame of the image) is considered to have
    // id 1 and be of type HOLE_BORDER
    node_hash[1] = Contour(m_root,HOLE_BORDER);

    // we mark the contours in the image with the following coding
    // bit 1 = set if this is an exit pixel (corresponds to -NBD in the paper)
    // bit 2-8 = NBD

#ifdef IMAGE_DEBUG
    Image debug0(image);
    debug0.ConvertScale(0.5,128);
#endif

    float points_buffer[MAXLENGTH*2];

    int NBD = 2;  // the next border ID to issue is stored in NBD

    for(int raster_y=0;raster_y < image.GetHeight(); ++raster_y) {
      int LNBD = 1; // we've just "seen" the frame border so set the last seen border id to match
      for(int raster_x=1;raster_x < image.GetWidth()-1;++raster_x) {

	// the sampled value with either be:
	//  0  => 0-element
	//  1  => 1-element that we havn't visited before
	//  >1 => 1-element that we have visited before.  In this case the low bit will be set if this is an exit pixel and the remaining bits encode the NBD

	const int current = image.SampleNoCheck(raster_x,raster_y);
	const int current_nbd = current >> 1;
	const bool current_not_visited = current_nbd == 0;  // this will be true if this is an element that has not been visited before
	const bool current_exit_pixel = current_nbd && (current & 0x1);

	if (current) { // the current pixel is a 1-element (possibly visited before)
#ifdef SCENE_GRAPH_DEBUG
	  PROGRESS("Pixel value is " << (int)current);
#endif
	  const int previous = image.SampleNoCheck(raster_x-1,raster_y);
	  const int next = image.SampleNoCheck(raster_x+1,raster_y);
	  int contour_length;
	  bordertype_t border_type;
	  ContourStatistics st;
	  if (current_not_visited && !previous) { // the previous pixel is a 0-element
#ifdef SCENE_GRAPH_DEBUG
	    PROGRESS("Found start point for outer border at " <<raster_x << "," << raster_y);
#endif	    
	    border_type = OUTER_BORDER;
	    contour_length = FollowContour(image, raster_x, raster_y, points_buffer, MAXLENGTH, st,0, NBD);
	  }
	  else if ((!current_exit_pixel || current_not_visited) && !next) { // the next element is a 0-element 
#ifdef SCENE_GRAPH_DEBUG
	    PROGRESS("Found start point for hole border " <<raster_x << "," << raster_y);
#endif	    
	    contour_length = FollowContour(image,raster_x,raster_y,points_buffer,MAXLENGTH,st,4,NBD);
	    border_type = HOLE_BORDER;
	    if (!current_not_visited) {
	      LNBD = current_nbd;
	    }
	  }
	  else {  // otherwise we just carry on looking
	    contour_length =0;
	  }
	  
	  if (contour_length > 0) {
	    
	    // now decide the parent of this border
	    
	    // NewBorder    LNBDType   Parent
	    // OUTER        OUTER      Parent of LNBD
	    // OUTER        HOLE       LNBD
	    // HOLE         OUTER      LNBD
	    // HOLE         HOLE       Parent of LNBD
	    
#ifdef SCENE_GRAPH_DEBUG
	    PROGRESS("LNBD is " << LNBD);
	    PROGRESS("OuterBorder(LNBD) " << node_hash[LNBD].bordertype);
	    PROGRESS("NBD  is " << NBD);
	    PROGRESS("OuterBorder(NBD) " << border_type);
#endif
	    
	    int parent_id;
	    if ( border_type ^ node_hash[LNBD].bordertype ) {
	      parent_id = LNBD;
	    }
	    else {
	      parent_id = node_hash[LNBD].parent_id;
	    }
	    
#ifdef SCENE_GRAPH_DEBUG
	    PROGRESS("parent_id is " << parent_id);
#endif
	    	    
	    // filter this contour

	    if ((contour_length > 40) &&
		(st.length > 40) &&
		(st.max_x - st.min_x > 20) &&
		(st.max_y - st.min_y > 20)) {
#ifdef SCENE_GRAPH_DEBUG
	      PROGRESS("Contour is a valid candidate for matching");
#endif
#ifdef IMAGE_DEBUG
	      debug0.DrawPolygon(points_buffer,contour_length,0,1);
	    //	    debug0.Save("temp.bmp");
	    //	    exit(-1);
#endif

	      // now attempt to fit a shape to this border
	      
	      // if we succeed then create a new scene graph node and insert
	      // it into the node_hash for this NBD also add this new scene
	      // graph node as a child of its parent SceneGraphNode which
	      // will be pointed to by its parent contour ID below
	      
	      // if we fail then insert another entry in the node_hash for
	      // the parent SceneGraphNode using this NBD
	      camera.ImageToNPCF(points_buffer,contour_length);
	      SceneGraphNode<S,PAYLOAD_SIZE>* next_node = new SceneGraphNode<S,PAYLOAD_SIZE>(points_buffer,contour_length);
	      if (next_node->GetShapes().IsChainFitted()) {
#ifdef SCENE_GRAPH_DEBUG
		PROGRESS("Shape matches!");
#endif
		node_hash[parent_id].node->AddChild(next_node);
		node_hash[NBD] = Contour(parent_id,next_node,border_type);					
		
		++NBD;
		NBD &= 127;
		if (NBD==0) { ++NBD; }	      
	      }
	      else {
		delete next_node;
		node_hash[NBD] = Contour(parent_id,node_hash[parent_id].node,border_type);
	      }	    
	    }
	    else {
	      node_hash[NBD] = Contour(parent_id,node_hash[parent_id].node,border_type);
	    }
	  }

	  if (!current_not_visited) {
#ifdef SCENE_GRAPH_DEBUG
	    PROGRESS("Current node has been visited, setting LNBD to "<< (int)current_nbd);
#endif
	    LNBD = current_nbd;
	  }
	}  
      }
      
    }
#ifdef IMAGE_DEBUG
      debug0.Save("debug-contours.bmp");
#endif
      
}
  
template<class S,int PAYLOAD_SIZE> int SceneGraph<S,PAYLOAD_SIZE>::FollowContour(Image& image, int start_x, int start_y, float* points_buffer, const int maxcount, ContourStatistics& statistics, int position, const int nbd) {
#ifdef SCENE_GRAPH_DEBUG
  PROGRESS("Following contour");
#endif	 

  //  points_buffer[0] = start_x;
  //  points_buffer[1] = start_y;
  int pointer = -1;  // our index into the points buffer
  int image_width = image.GetWidth();
  uchar* data_pointer = image.GetDataPointer() + start_x + start_y *image_width;

  // first scan around clockwise from the requested position looking for a 1-element
  const int offset_clockwise[] = { -1,-image_width-1,-image_width,1-image_width,1,1+image_width,image_width,image_width-1};
  bool found = false;
  uchar* sample_pointer;
  for(int i=0;i<8;i++) {
    sample_pointer = data_pointer + offset_clockwise[position];
    if (*sample_pointer) {
      found = true;
      break;
    }
    position++;
    position &= 0x7;
  }

  if (!found) {
#ifdef SCENE_GRAPH_DEBUG
    PROGRESS("Failed to find any adjoining 1-elements - returning single pixel contour");
#endif
    // if we dont find a 1-element then record the nbd in this pixel
    // and that it is an exit pixel and then return this single pixel contour
    *sample_pointer = nbd<<1 | 0x1;
    return 0;
  }

  // if we found the 1-element a position n clockwise then we resume
  // searching anti-clockwise from the position 1 after the found position

  //    +---+---+---+---+         +---+---+---+---+   
  //    | 1 | 2 | 3 |   |         | 7 | 6 | 5 |   |
  //    +---+---+---+---+         +---+---+---+---+   
  //    | 0 |   | 4 |   |         | 0 |   |_4_|   |
  //    +---+---+---+---+   -->   +---+---+---+---+   
  //    | 7 | 6 | X |   |         | 1 | 2 | X |   |
  //    +---+---+---+---+         +---+---+---+---+   
  //    |   |   |   |   |         |   |   |   |   |
  //    +---+---+---+---+         +---+---+---+---+   
  //
  //  Found posn (clockwise)  |  Resume posn (anti)
  //  -----------------------------------------------
  //          0               |         1
  //          1               |         0
  //          2               |         7
  //          3               |         6
  //          4               |         5
  //          5               |         4
  //          6               |         3
  //          7               |         2
  //
  // i.e. the restart position is:  9 - found % 8  
  position = (9- position) % 0x7;

  
  // these two arrays store the offsets for each of the eight regions
  // around the target pixel
  const int offset[] = {-1,image_width-1,image_width,image_width+1,1,1-image_width,-image_width,-image_width-1};
  const int offset_x[] = { -1, -1, 0, 1,1,1,0,-1};
  const int offset_y[] = { 0, 1,1,1,0,-1,-1,-1};
  
  bool cell4_is_0 = false; // will be set to true when we search a region if we pass cell4 and cell4 is a 0-element

  statistics.length = 0; // the contour length
  statistics.min_x = start_x; // bounding box
  statistics.max_x = start_x; // bounding box
  statistics.min_y = start_y; // bounding box
  statistics.max_y = start_y; // bounding box
  statistics.convex = true; // true if this contour is convex

  int nbd_shifted = nbd<<1;

  while(pointer < MAXLENGTH) {
    // work out the pixel co-ordinates for the position of interest
    sample_pointer = data_pointer+offset[position];
    int sample = *sample_pointer;
    if (sample) {  // if the pixel is a 1-element
#ifdef SCENE_GRAPH_DEBUG
      PROGRESS("Found 1-element at position " << position);
#endif
      // we now need to mark this pixel
      // 1) if the pixel sample_x+1,sample_y (cell 4) is a 0-element and we
      // have examined it whilst looking for this 1-element then this
      // is an exit pixel.  Write (NBD,r).
      if (cell4_is_0) {
	*data_pointer = nbd_shifted | 0x1;
      }
      // 2) else if sample_x,sample_y is unmarked write
      // (NBD,l).
      else if (!(sample>>1)) {
	*data_pointer = nbd_shifted;
      }

      // update the length
      statistics.length += (position & 0x1 ? 45 : 32);
      // update the bounding box
      if (start_x < statistics.min_x) { statistics.min_x = start_x; }
      else if (start_x > statistics.max_x) { statistics.max_x = start_x; }
      if (start_y < statistics.min_y) { statistics.min_y = start_y; }
      else if (start_y > statistics.max_y) { statistics.max_y = start_y; }

      //      if (previous_position != position) {
      // we've made a turn
      //	if (position = 
      //      }

      // store this point in the pixel chain and update the start position
      points_buffer[++pointer] = start_x;
      points_buffer[++pointer] = start_y;
      start_x+= offset_x[position];
      start_y+= offset_y[position];
      data_pointer = sample_pointer;
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
    else {  // the pixel is a 0-element
#ifdef SCENE_GRAPH_DEBUG
      PROGRESS("Pixel is 0-element.  Advancing search position " << position);
#endif
      if (position == 4) {  // if we are at cell4 (this is a 0-element) then set the cell4_is_0 flag
	cell4_is_0 = true; 
      }

      // advance the position
      position = (position+1) & 0x7;
    }
    
    // if our current pixel matches the _second_ pixel found in the
    // chain and the previous pixel we found matches the first then we
    // are done.  we have to wait until pointer>4 in order to make
    // sure we dont immediatly stop.  I wonder if the compiler will
    // notice it can unroll this loop a couple of times and remove
    // this comparison?
    if ((pointer > 4) && 
	(points_buffer[2] == points_buffer[pointer-1]) &&
	(points_buffer[3] == points_buffer[pointer]) &&
	(points_buffer[0] == points_buffer[pointer-3]) &&
	(points_buffer[1] == points_buffer[pointer-2])) {
      break;
    }
  }
  
  // discard the last two pixels because they are overlap on the
  // beginning of the chain

#ifdef SCENE_GRAPH_DEBUG
  PROGRESS("Returning contour of length " << (pointer-3)/2);
#endif	 

  statistics.length >>= 5;
  return (pointer-3)/2;
}

#endif//SCENE_GRAPH_GUARD

