/**
 * $Header$
 */

#include <ContourTree.hh>

#undef CONTOUR_TREE_DEBUG
#undef  CONTOUR_TREE_DEBUG_SAVE
Image* debug_image;

ContourTree::ContourTree(Image& image, std::vector<ContourConstraint>& constraints) : m_root_contour(NULL)
{
  unsigned int* nbd_store = new unsigned int[image.GetWidth()*image.GetHeight()];
  std::map<int,Contour*> node_hash;
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
  debug_image= new Image(image.GetWidth(),image.GetHeight());
#endif

  m_root_contour = new Contour(1);
  m_root_contour->weeded = true;
  Contour* current = m_root_contour;
  current->bordertype=HOLE_BORDER;
  node_hash[1] = current;
  data_pointer = image.GetDataPointer();
  int NBD = 2;
  node_hash[NBD] = new Contour(NBD);
  current = node_hash[NBD];
  
  // we're going to mark the image with contour id's as we find them
  // the LSB will indicate if we have a exit or an entry pixel
  // the remainder is the NBD
  // (note that we only ever mark things that are 1-pixels)

  for(int raster_y=0;raster_y < image_height; ++raster_y) {
#ifdef CONTOUR_TREE_DEBUG
    PROGRESS("Updating LNBD to 1 (row start)");
#endif
    unsigned int LNBD = 1; // we've just "seen" the frame border so set the last seen border id to match
    data_pointer = image.GetRow(raster_y);
    ++data_pointer; // exclude the first pixel on the line
    for(int raster_x=1;raster_x < image_width_1;++raster_x, ++data_pointer) {
      //      PROGRESS("SCAN " << raster_x << "," << raster_y << " " << *data_pointer << " " << image.Sample(raster_x,raster_y));
      if (*data_pointer) {  // this pixel is a 1-element or it has been visited before
	const int cNBD = *data_pointer>>1;   // left shift to discard the pixel value (or exit/entry flag if already marked)
	if (cNBD) { // this pixel has been seen before and it is not an exit pixel
	  //	if (cNBD && (*data_pointer & 0x1)) { // this pixel has been seen before and it is not an exit pixel
#ifdef CONTOUR_TREE_DEBUG
	  //	  PROGRESS("Updating LNBD to " << cNBD);
	  PROGRESS("Updating LNBD to " << nbd_store[raster_x+raster_y*image.GetWidth()]);
#endif
	  //	  LNBD = cNBD;
	  LNBD = nbd_store[raster_x+raster_y*image.GetWidth()];
	}
	
	int contour_length;
	ContourStatistics contour_statistics;;
	if (!cNBD && !(*(data_pointer-1))) { // this pixel has not been seen before and the previous pixel is a 0-element
	  current->bordertype = OUTER_BORDER;	  
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Found outer border.  Following from " << raster_x << "," << raster_y);
#endif
	  contour_length = FollowContour(image, data_pointer, raster_x, raster_y, current->points, contour_statistics,0, NBD,nbd_store);
	} 
	else if ((*data_pointer & 0x1) && !(*(data_pointer+1))) { // this pixel has not been seen before or it is not an exit pixel, and the next pixel is a 0-element
	  current->bordertype = HOLE_BORDER;
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Found hole border.  Following from " << raster_x << "," << raster_y);
#endif
	  contour_length = FollowContour(image, data_pointer, raster_x, raster_y, current->points, contour_statistics,4, NBD,nbd_store);
	}
	else {
	  continue;
	}

	if (contour_length < 20) { current->weeded = true; }
	
	// now decide the parent of this border
	
	// NewBorder    LNBDType   Parent
	// --------------------------------------
	// OUTER        OUTER      Parent of LNBD
	// OUTER        HOLE       LNBD
	// HOLE         OUTER      LNBD
	// HOLE         HOLE       Parent of LNBD
	current->parent_id = current->bordertype == node_hash[LNBD]->bordertype ?  node_hash[LNBD]->parent_id : LNBD;
#ifdef IMAGE_DEBUG
	if (!current->weeded) {
	  int size = current->points.size();
	  float* pointsarray = new float[size];
	  int ptr = 0;
	  for(std::vector<float>::const_iterator i = current->points.begin();i!=current->points.end(); ++i) {
	    pointsarray[ptr++] = *i;
	  }
	  debug_image->DrawPolygon(pointsarray,size/2,0,1);
	  delete[] pointsarray;
	}
#endif
	
	if (current->parent_id != NBD) {
	  node_hash[current->parent_id]->children.push_back(current);
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Adding contour "<<NBD <<" as child of " << current->parent_id);
#endif
	}
	
	//	NBD = (NBD + 1) & 0x7F;
	NBD = (NBD + 1);
	//	if (NBD==0) { ++NBD; }	 
	
	current = new Contour(NBD);
	node_hash[NBD] = current;
	
      }
      else { // this is a 0-element
	
      }
    }
    ++data_pointer; // exclude the last pixel on the line
  }
  
#ifdef IMAGE_DEBUG
  debug_image->Save("debug-contourtree-contours.bmp");
#endif
  delete[] nbd_store;
}


int ContourTree::FollowContour(Image& image, // the image to track the contour in (will be altered)
			       unsigned char* data_pointer,
			       int start_x,  int start_y, // the start position (must lie on contour)
			       std::vector<float>& points,  // the buffer to store the points
			       ContourStatistics& statistics,   // contour statistics structure (can be NULL)
			       int start_position,  // the position in the 8-connected region to start searching from
			       const int nbd,  // the NBD to mark this contour with
			       unsigned int* nbd_store
			       ) {
  int image_width = image.GetWidth();
  int image_width_step = image.GetWidthStep();
  
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
			image_width_step-1,     //  1
			image_width_step,       //  2
			image_width_step+1,     //  3
			1,                 //  4
			1-image_width_step,     //  5
			-image_width_step,      //  6
			-image_width_step-1};   //  7


  // data_pointer is the current focus of the search region
  // sample_pointer is the current read point
  unsigned char* sample_pointer;

  // position is our current index into the search region
  int position = start_position;

  // contour_0 is the first pixel in the contour
  const unsigned char* contour_0 = data_pointer;

  const int NBD_shift = 1 << 1;

  do {
    position = (position - 1) & 0x7;
    sample_pointer = data_pointer+offset[position];
    if (*sample_pointer) { break; }
  }
  while (position != start_position);

#ifdef CONTOUR_TREE_DEBUG
  PROGRESS("Finished clockwise scan at " << position << " (started:" << start_position << ")");
#endif

  if (position == start_position) {
    // one pixel contour
    *data_pointer = NBD_shift;
    nbd_store[start_x+image_width*start_y] = nbd;
    points.push_back(start_x);
    points.push_back(start_y);
#ifdef CONTOUR_TREE_DEBUG
    PROGRESS("Found 1 pixel contour starting from "<< start_x << "," << start_y);
#endif
#ifdef IMAGE_DEBUG
#ifdef CONTOUR_TREE_DEBUG_SAVE
    debug_image->DrawPixel(start_x,start_y,COLOUR_BLACK);
    debug_image->Save("debug-contourtree-contours.bmp");
#endif
#endif
    return 1;
  }
  else {
    position = (position + 1) & 0x7;

    // contour_n is the last pixel in the contour
    const unsigned char* contour_n = sample_pointer;

    bool cell4_is_0 = false; // will be set to true when we search a region if we pass cell4 and cell4 is a 0-element
    
    statistics.length = 0; // the contour length
    statistics.min_x = start_x; // bounding box
    statistics.max_x = start_x; // bounding box
    statistics.min_y = start_y; // bounding box
    statistics.max_y = start_y; // bounding box
    statistics.convex = true; // true if this contour is convex

    while(1) {
      sample_pointer = data_pointer+offset[position];
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Searching position " << position << " value is " << (int)*sample_pointer);
#endif
      if (*sample_pointer) {
#ifdef CONTOUR_TREE_DEBUG
	PROGRESS("Found 1-pixel at position " << position << " around " << start_x << "," << start_y);
#endif
#ifdef IMAGE_DEBUG
#ifdef CONTOUR_TREE_DEBUG_SAVE
	debug_image->DrawPixel(start_x,start_y,128);
	debug_image->Save("debug-contourtree-contours.bmp");
#endif
#endif
	// we now need to mark this pixel
	// 1) if the pixel sample_x+1,sample_y (cell 4) is a 0-element and we
	// have examined it whilst looking for this 1-element then this
	// is an exit pixel.  Write (NBD,r).
	if (cell4_is_0) {
	  *data_pointer = NBD_shift & 0xFF;
	  nbd_store[start_x+image_width*start_y] = nbd;
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Marked  exit " << start_x << "," << start_y << " with " << NBD_shift);
#endif
	}
	// 2) else if sample_x,sample_y is unmarked write
	// (NBD,l).
	//	else if (!(*sample_pointer & ~0x1)) {
	else if (!(*data_pointer & ~0x1)) {
	  *data_pointer = (NBD_shift | 0x1) & 0xFF;
	  nbd_store[start_x+image_width*start_y] = nbd;
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Marked " << start_x << "," << start_y << " with " << (NBD_shift | 0x1));
#endif
	}

	// update the length
	statistics.length += (position & 0x1 ? 45 : 32);
	// update the bounding box
	if (start_x < statistics.min_x) { statistics.min_x = start_x; }
	else if (start_x > statistics.max_x) { statistics.max_x = start_x; }
	if (start_y < statistics.min_y) { statistics.min_y = start_y; }
	else if (start_y > statistics.max_y) { statistics.max_y = start_y; }

	// check the stopping condition
	if ((points.size() > 4) && // i.e. we have seen more than two pixels
	    (data_pointer == contour_n) &&
	    (sample_pointer == contour_0)) {
	  statistics.length >>= 5;
#ifdef CONTOUR_TREE_DEBUG
	  PROGRESS("Found " << (points.size()>>1) << " pixel contour starting from "<< points[0] << "," << points[1]);
#endif
#ifdef IMAGE_DEBUG
#ifdef CONTOUR_TREE_DEBUG_SAVE
	  debug_image->DrawPixel(start_x,start_y,0);		  
	  debug_image->Save("debug-contourtree-contours.bmp");
#endif
#endif
	  return points.size()>>1;
    	}

	// store this point in the pixel chain and update the start position
	points.push_back(start_x);
	points.push_back(start_y);
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
    return (points.size()-3)/2;
  }
}

ContourTree::~ContourTree() {
  if (m_root_contour != NULL) {
    delete m_root_contour;
  }
}

void ContourTree::ImageToNPCF(const Camera& camera) {
  ImageToNPCF(camera,GetRootContour());
}

void ContourTree::ImageToNPCF(const Camera& camera, Contour* current) {
  camera.ImageToNPCF(current->points);
  for(std::vector<Contour*>::const_iterator i = current->children.begin();
      i != current->children.end();
      ++i) {
    ImageToNPCF(camera,*i);
  }
}

ContourTree::Contour::~Contour() {
  for(std::vector<Contour*>::const_iterator i = children.begin();
      i!=children.end();
      ++i) {
    //delete *i;
  }
}

ContourTree::ContourTree(Socket& socket) {
  m_root_contour = new Contour(socket);
}

void ContourTree::Save(Socket& socket) {
  if (m_root_contour != NULL) {
    m_root_contour->Save(socket);
  }
}

ContourTree::Contour::Contour(Socket& socket) : points() {
  nbd = socket.RecvInt();
  border_type = socket.RecvInt();
  parent_id = socket.RecvInt();
  socket.Recv(points);
  int count = socket.RecvInt();
  for(int i=0;i<count;++i) {
    children.push_back(new Contour(socket));
  }
}

ContourTree::Contour::Save(Socket& socket) {
  if (!weeded) {
    socket.Send(nbd);
    socket.Send(border_type);
    socket.Send(parent_id);
    socket.Send(points);
  }
  int counter = 0;
  for(std::vector<float>::const_iterator i = children.begin(); i!= children.end(); ++i) {
    if (!(*i)->weeded) counter++;
  }
  socket.Send(counter);
  for(std::vector<float>::const_iterator i = children.begin(); i!= children.end(); ++i) {
    (*i)->Send(socket);
  }
}
