/**
 * $Header$
 */

#include <map>

#include <total/algorithms/ContourFollowerTree.hh>

#define CONTOUR_TREE_DEBUG

namespace Total {

  ContourFollowerTree::ContourFollowerTree(const ContourRestrictions& constraint) : m_working_store(NULL), m_image_width(-1),m_image_height(-1),m_constraints(constraint) {
    m_offset_x[0] = -1;
    m_offset_x[1] = -1;
    m_offset_x[2] = 0;
    m_offset_x[3] = 1;
    m_offset_x[4] = 1;
    m_offset_x[5] = 1;
    m_offset_x[6] = 0;
    m_offset_x[7] = -1;
    
    m_offset_y[0] = 0;
    m_offset_y[1] = 1;
    m_offset_y[2] = 1;
    m_offset_y[3] = 1;
    m_offset_y[4] = 0;
    m_offset_y[5] = -1;
    m_offset_y[6] = -1;
    m_offset_y[7] = -1;
  }

  ContourFollowerTree::~ContourFollowerTree() {
    if (m_working_store) delete[] m_working_store;
  }

  bool ContourFollowerTree::operator()(const MonochromeImage& image, TreeNode<ContourEntity>& result) const {
    if (m_image_width != image.GetWidth() ||
	m_image_height != image.GetHeight()) {
      if (m_working_store) delete[] m_working_store;
      m_image_width = image.GetWidth();
      m_image_height = image.GetHeight();
      m_working_store = new unsigned int[m_image_width*m_image_height];     
      memset(m_working_store,0,m_image_width*m_image_height*sizeof(unsigned int));
      m_flag = 1;
    }

    std::map<int,TreeNode<ContourEntity>*> node_hash;
    
    TreeNode<ContourEntity>* root_contour = &result;
    root_contour->GetNode()->SetNBD(1);
    root_contour->GetNode()->SetBorderType(ContourEntity::HOLE_BORDER);
    root_contour->GetNode()->SetParentNBD(1);
    node_hash[1] = root_contour;
    int NBD = 2;
    
    // we're going to mark the image with contours as we find them
    // the LSB will indicate if we have a exit (0) or an entry pixel (1)
    // the next bit will be 1 to indicate that we've been here before
    // (note that we only ever mark things that are 1-pixels)

    for(int raster_y=1;raster_y < m_image_height-1; ++raster_y) {
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Updating LNBD to 1 (row start)");
#endif
      unsigned int LNBD = 1; // we've just "seen" the frame border so set the last seen border id to match
      for(int raster_x=1;raster_x < m_image_width - 1;++raster_x) {
	const int pixel_nbd = AssessPixel(raster_x,raster_y);       
	const bool previously_visited = pixel_nbd != -1;
	const int region_value = image.GetPixel3(raster_x,raster_y);
	PROGRESS("Assess " << raster_x << " " << raster_y << " " << m_working_store[raster_x+raster_y*m_image_height] << " " << pixel_nbd << " " << region_value);
	if (region_value & MonochromeImage::CENTRE_PIXEL || 
	    previously_visited) {  // this pixel is a 1-element or it has been visited before
	  if (previously_visited) { // this pixel has been seen before
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Updating LNBD to " << pixel_nbd);
#endif
	    LNBD = pixel_nbd;
	    continue;
	  }	

	  // if we get this far the pixel has not been seen before

	  const bool previous_is_1 = region_value & MonochromeImage::LEFT_PIXEL;
	  const bool next_is_1 = region_value & MonochromeImage::RIGHT_PIXEL;

	  bool need_follow = false;
	  ContourEntity::bordertype_t border_type;
	  if (!previous_is_1) { // this pixel has not been seen before and the previous pixel is a 0-element
	    border_type = ContourEntity::OUTER_BORDER;	  
	    need_follow = true;
	  } 
	  else if (!next_is_1) { // this pixel has not been seen before or it is not an exit pixel, and the next pixel is a 0-element
	    border_type = ContourEntity::HOLE_BORDER;
	    need_follow = true;
	  }

	  if (need_follow) {
	    // now decide the parent of this border
	    
	    // NewBorder    LNBDType   Parent
	    // --------------------------------------
	    // OUTER        OUTER      Parent of LNBD
	    // OUTER        HOLE       LNBD
	    // HOLE         OUTER      LNBD
	    // HOLE         HOLE       Parent of LNBD
	    int parent_id = border_type == node_hash[LNBD]->GetNode()->GetBorderType() ?  node_hash[LNBD]->GetNode()->GetParentNBD() : LNBD;
	    TreeNode<ContourEntity>* parent = node_hash[parent_id];
	    TreeNode<ContourEntity>* child = parent->AddChild();	    
	    ContourEntity* node = child->GetNode();
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Following contour from "<< raster_x << "," << raster_y);
#endif
	    FollowContour(image, raster_x, raster_y, border_type == ContourEntity::OUTER_BORDER ? 0:4, NBD,node);
	    //	    node->SetContourFitted(node->GetLength() > 100);
	    node->SetParentNBD(parent_id);
	    node->SetNBD(NBD);
	    node->SetBorderType(border_type);
	    node->SetValid(m_constraints.CheckDimensions(node->GetLength(),node->GetWidth(),node->GetHeight()));
	    node_hash[NBD] = child;
	    ++NBD;
	  }
	}
	else { // this is a 0-element	
	}
      }
    }
    return true;
  }

  int ContourFollowerTree::FollowContour(const MonochromeImage& image, // the current image
					 int start_x,  int start_y, // the start co-ordinates (must lie on contour)
					 int start_position,  // the position in the 8-connected region to start searching from
					 const int nbd,  // the NBD to mark this contour with
					 ContourEntity* result // the entity to store the result in
					 ) const {

    result->SetStart(start_x,start_y);
    // start_x,start_y is the current focus of the search region
    // sample_x,sample_y is the current read point
    int sample_x;
    int sample_y;

    // position is our current index into the search region
    int position = start_position;

    // contour_0 is the first pixel in the contour
    const int contour_0_x = start_x;
    const int contour_0_y = start_y;

    // find the first point that we will start searching from
    do {
      position = (position - 1) & 0x7;
      sample_x = start_x+m_offset_x[position];
      sample_y = start_y+m_offset_y[position];
      PROGRESS("Trying " << sample_x << " " << sample_y);
      if (image.GetPixel(sample_x,sample_y)) { break; }
    }
    while (position != start_position);

#ifdef CONTOUR_TREE_DEBUG
    PROGRESS("Finished clockwise scan at " << position << " (started:" << start_position << ")");
#endif

    // check if we have a 1-pixel contour
    if (position == start_position) {
      SetVisited(start_x,start_y,nbd,EXIT_PIXEL);
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Found 1 pixel contour starting from "<< start_x << "," << start_y);
#endif
      return 1;
    }
    else {
      position = (position + 1) & 0x7;
      int move_position = -1;
      // contour_n is the last point in the contour
      const int contour_n_x = sample_x;
      const int contour_n_y = sample_y;

      bool cell4_is_0 = false; // will be set to true when we search a region if we pass cell4 and cell4 is a 0-element
      while(1) {
	sample_x = start_x + m_offset_x[position];
	sample_y = start_y + m_offset_y[position];
#ifdef CONTOUR_TREE_DEBUG
	PROGRESS("Searching position " << position << " value is " << image.GetPixel(sample_x,sample_y));
#endif
	if (image.GetPixel(sample_x,sample_y)) {
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Found 1-pixel at position " << position << " around " << start_x << "," << start_y);
#endif
	  // we now need to mark this pixel
	  // 1) if the pixel sample_x+1,sample_y (cell 4) is a 0-element and we
	  // have examined it whilst looking for this 1-element then this
	  // is an exit pixel.  Write (NBD,r).
	  if (cell4_is_0) {
	    SetVisited(start_x,start_y,nbd,EXIT_PIXEL);
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Marked  exit " << start_x << "," << start_y);
#endif
	  }
	  // 2) else if sample_x,sample_y is unmarked write
	  // (NBD,l).
	  else if (AssessPixel(start_x,start_y) == -1) {
	    SetVisited(start_x,start_y,nbd,ENTRY_PIXEL);
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Marked " << start_x << "," << start_y);
#endif
	  }
	  
	  // store this point in the pixel chain and update the start position
	  result->AddPoint(move_position);

	  // check the stopping condition
	  if ((result->GetNumPoints() > 4) && // i.e. we have seen more than two pixels
	      (start_x == contour_n_x) &&
	      (start_y == contour_n_y) &&	      
	      (sample_x == contour_0_x) &&
	      (sample_y == contour_0_y)) {
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Found " << (result->GetNumPoints()>>1) << " pixel contour");
#endif

	    return result->GetNumPoints()>>1;
	  }

	  start_x = sample_x;
	  start_y = sample_y;
	  move_position = position;

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

#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Contour length = " << (result->GetNumPoints()-3/2));
#endif
      return (result->GetNumPoints()-3)/2;
    }
  }
}
