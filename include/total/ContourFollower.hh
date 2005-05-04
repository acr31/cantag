/**
 * $Header$
 */

#ifndef CONTOUR_FOLLOWER_GUARD
#define CONTOUR_FOLLOWER_GUARD

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/Entity.hh>

#include <map>

#undef CONTOUR_TREE_DEBUG

namespace Total {
  class ContourFollower {
  private:
    int* m_nbd_store;
    int m_offset_x[8];
    int m_offset_y[8];
    int m_offset[8];
    const int m_image_width;
    const int m_image_height;
    const int m_image_width_bytes;
    
  public:
    /**
     * Create the contour follower suitable for dealing with images of the given size.
     */
    ContourFollower(int image_width, int image_height, int image_width_step);
    
    ~ContourFollower() {
      delete[] m_nbd_store;
    }
    
    /**
     * Find all the contours in the image and store them in a tree of
     * entities and return a pointer to the root element
     */
    template<class E> E* FollowContours(Image& image);

  private:
    int FollowContour(unsigned char* data_pointer, // the current position in the image
		      int start_x,  int start_y, // the start co-ordinates (must lie on contour)
		      int start_position,  // the position in the 8-connected region to start searching from
		      int nbd,  // the NBD to mark this contour with
		      ContourEntity* result // the entity to store the result in
		      );       
  };
  
  template<class E> E* ContourFollower::FollowContours(Image& image) {
    // our frame border is a hole border.  We write zero's around the
    // edge of the image to make sure that our border follower never
    // goes out of range
    for(int i=0;i<m_image_height;++i) {
      unsigned char* data_pointer = image.GetRow(i);
      *data_pointer &= 0xFC;
      if (i == 0 || i == m_image_height-1) {
	for(int j=0;j<m_image_width-2;++j) {
	  *data_pointer &= 0xFC;
	  data_pointer++;
	}
      }
      else {
	data_pointer+=m_image_width-1;
      }
      *data_pointer &= 0xFC;
    }
    std::map<int,E*> node_hash;

    E* root_contour = new E();
    root_contour->nbd = 1;
    root_contour->bordertype = ContourEntity::HOLE_BORDER;
    node_hash[1] = root_contour;
    int NBD = 2;
    E* current = new E();
    current->nbd = NBD;
    node_hash[NBD] = current;
    
    // we're going to mark the image with contours as we find them
    // the LSB will indicate if we have a exit (0) or an entry pixel (1)
    // the next bit will be 1 to indicate that we've been here before
    // (note that we only ever mark things that are 1-pixels)

    for(int raster_y=0;raster_y < m_image_height; ++raster_y) {
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Updating LNBD to 1 (row start)");
#endif
      unsigned int LNBD = 1; // we've just "seen" the frame border so set the last seen border id to match
      unsigned char* data_pointer = image.GetRow(raster_y);
      ++data_pointer; // exclude the first pixel on the line
      for(int raster_x=1;raster_x < m_image_width - 1;++raster_x, ++data_pointer) {
	if (*data_pointer & 3) {  // this pixel is a 1-element or it has been visited before
	  const int cNBD = *data_pointer & 2;   // the second pixel will be 1 if we've seen it before and 0 if not
	  const int previous_is_1 = *(data_pointer-1) & 3;
	  const int next_is_1 = *(data_pointer+1) & 3;
	  if (cNBD) { // this pixel has been seen before
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Updating LNBD to " << m_nbd_store[raster_x+raster_y*image.GetWidth()]);
#endif
	    LNBD = m_nbd_store[raster_x+raster_y*image.GetWidth()];
	    continue;
	  }	

	  if (!cNBD && !previous_is_1) { // this pixel has not been seen before and the previous pixel is a 0-element
	    current->bordertype = ContourEntity::OUTER_BORDER;	  
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Found outer border.  Following from " << raster_x << "," << raster_y);
#endif
	    FollowContour(data_pointer, raster_x, raster_y, 0, NBD,current);
	  } 
	  else if (!cNBD && !next_is_1) { // this pixel has not been seen before or it is not an exit pixel, and the next pixel is a 0-element
	    current->bordertype = ContourEntity::HOLE_BORDER;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Found hole border.  Following from " << raster_x << "," << raster_y);
#endif
	    FollowContour(data_pointer, raster_x, raster_y,4, NBD,current);
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
	  current->parent_id = current->bordertype == node_hash[LNBD]->bordertype ?  node_hash[LNBD]->parent_id : LNBD;
	  current->m_contourFitted = true;
	  current->SetStage((ContourEntity*)NULL);
	  if (current->parent_id != NBD) {
	    node_hash[current->parent_id]->AddChild(current);
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Adding contour "<<NBD <<" as child of " << current->parent_id);
#endif
	  }
	
	  NBD = NBD + 1;
	
	  current = new E();
	  current->nbd = NBD;
	  node_hash[NBD] = current;	
	}
	else { // this is a 0-element
	
	}
      }
      ++data_pointer; // exclude the last pixel on the line
    }
    delete current;
    return root_contour;
  }
}
#endif//CONTOUR_FOLLOWER_GUARD
