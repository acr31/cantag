/**
 * $Header$
 */

#include <total/ContourFollower.hh>

namespace Total {
  ContourFollower::ContourFollower(int image_width, int image_height, int image_width_bytes) :
    m_nbd_store(new int[image_width*image_height]),
    m_image_width(image_width),
    m_image_height(image_height),
    m_image_width_bytes(image_width_bytes) {

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

    m_offset[0] = -1;
    m_offset[1] = image_width_bytes-1;
    m_offset[2] = image_width_bytes;
    m_offset[3] = image_width_bytes+1;
    m_offset[4] = 1;
    m_offset[5] = 1-image_width_bytes;
    m_offset[6] = -image_width_bytes;
    m_offset[7] = -image_width_bytes-1;
  }
  
  int ContourFollower::FollowContour(unsigned char* data_pointer, // the current position in the image
				     int start_x,  int start_y, // the start co-ordinates (must lie on contour)
				     int start_position,  // the position in the 8-connected region to start searching from
				     const int nbd,  // the NBD to mark this contour with
				     ContourEntity* result // the entity to store the result in
				     ) {
    
    // data_pointer is the current focus of the search region
    // sample_pointer is the current read point
    unsigned char* sample_pointer;

    // position is our current index into the search region
    int position = start_position;

    // contour_0 is the first pixel in the contour
    const unsigned char* contour_0 = data_pointer;

    // find the first point that we will start searching from
    do {
      position = (position - 1) & 0x7;
      sample_pointer = data_pointer+m_offset[position];
      if (*sample_pointer & 0x1) { break; }
    }
    while (position != start_position);

#ifdef CONTOUR_TREE_DEBUG
    PROGRESS("Finished clockwise scan at " << position << " (started:" << start_position << ")");
#endif

    // check if we have a 1-pixel contour
    if (position == start_position) {
      *data_pointer |= 2;
      m_nbd_store[start_x+m_image_width*start_y] = nbd;
      result->points.push_back(start_x);
      result->points.push_back(start_y);
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Found 1 pixel contour starting from "<< start_x << "," << start_y);
#endif
      return 1;
    }
    else {
      position = (position + 1) & 0x7;

      // contour_n is the last point in the contour
      const unsigned char* contour_n = sample_pointer;

      bool cell4_is_0 = false; // will be set to true when we search a region if we pass cell4 and cell4 is a 0-element
    
      //      statistics.length = 0; // the contour length
      //      statistics.min_x = start_x; // bounding box
      //      statistics.max_x = start_x; // bounding box
      //      statistics.min_y = start_y; // bounding box
      //      statistics.max_y = start_y; // bounding box
      //      statistics.convex = true; // true if this contour is convex

      while(1) {
	sample_pointer = data_pointer+m_offset[position];
#ifdef CONTOUR_TREE_DEBUG
	PROGRESS("Searching position " << position << " value is " << (int)*sample_pointer);
#endif
	if (*sample_pointer & 3) {
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Found 1-pixel at position " << position << " around " << start_x << "," << start_y);
#endif

	  // we now need to mark this pixel
	  // 1) if the pixel sample_x+1,sample_y (cell 4) is a 0-element and we
	  // have examined it whilst looking for this 1-element then this
	  // is an exit pixel.  Write (NBD,r).
	  if (cell4_is_0) {
	    *data_pointer &= 0xFE;
	    *data_pointer |= 2;
	    m_nbd_store[start_x+m_image_width*start_y] = nbd;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Marked  exit " << start_x << "," << start_y << " with 2");
#endif
	  }
	  // 2) else if sample_x,sample_y is unmarked write
	  // (NBD,l).
	  else if (!(*data_pointer & 2)) {
	    *data_pointer |= 3;
	    m_nbd_store[start_x+m_image_width*start_y] = nbd;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Marked " << start_x << "," << start_y << " with 3");
#endif
	  }
	  
	  // update the length
	  //	  statistics.length += (position & 0x1 ? 45 : 32);
	  // update the bounding box
	  //	  if (start_x < statistics.min_x) { statistics.min_x = start_x; }
	  //	  else if (start_x > statistics.max_x) { statistics.max_x = start_x; }
	  //	  if (start_y < statistics.min_y) { statistics.min_y = start_y; }
	  //	  else if (start_y > statistics.max_y) { statistics.max_y = start_y; }


	  // store this point in the pixel chain and update the start position
	  result->points.push_back(start_x);
	  result->points.push_back(start_y);
	  start_x += m_offset_x[position];
	  start_y += m_offset_y[position];

	  // check the stopping condition
	  if ((result->points.size() > 4) && // i.e. we have seen more than two pixels
	      (data_pointer == contour_n) &&
	      (sample_pointer == contour_0)) {
	    //	    statistics.length >>= 5;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Found " << (result->points.size()>>1) << " pixel contour starting from "<< result->points[0] << "," << result->points[1]);
#endif

	    return result->points.size()>>1;
	  }

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

      //      statistics.length >>= 5;
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Contour length = " << (result->points.size()-3/2));
#endif
      return (result->points.size()-3)/2;
    }
  }
}
