/**
 * $Header$
 */

#include <total/ContourTree.hh>

#undef CONTOUR_TREE_DEBUG
#undef  CONTOUR_TREE_DEBUG_SAVE

namespace Total {
#ifdef IMAGE_DEBUG
  static Image* debug_image;
#endif

  ContourTree::ContourTree(Image& image) : m_root_contour(NULL), m_contour_count(0)
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
      *data_pointer &= 0xFC;
      data_pointer++;
    }
    for(int i=0;i<image_height-2;++i) {
      *data_pointer &= 0xFC;
      data_pointer+=image_width_1;
      *data_pointer &= 0xFC;
      ++data_pointer;
    }
    for(int i=0;i<image_width;++i) {
      *data_pointer &= 0xFC;
      ++data_pointer;
    }  

#ifdef IMAGE_DEBUG
    debug_image= new Image(image.GetWidth(),image.GetHeight());
#endif

    m_root_contour = new Contour(1);
    m_contour_count++;
    m_root_contour->weeded = true;
    Contour* current = m_root_contour;
    current->bordertype=HOLE_BORDER;
    node_hash[1] = current;
    data_pointer = image.GetDataPointer();
    int NBD = 2;
    node_hash[NBD] = new Contour(NBD);
    m_contour_count++;
    current = node_hash[NBD];
  
    // we're going to mark the image with contour id's as we find them
    // the LSB will indicate if we have a exit (0) or an entry pixel (1)
    // the next bit will be 1 to indicate that we've been here before
    // (note that we only ever mark things that are 1-pixels)

    for(int raster_y=0;raster_y < image_height; ++raster_y) {
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Updating LNBD to 1 (row start)");
#endif
      unsigned int LNBD = 1; // we've just "seen" the frame border so set the last seen border id to match
      data_pointer = image.GetRow(raster_y);
      ++data_pointer; // exclude the first pixel on the line
      for(int raster_x=1;raster_x < image_width_1;++raster_x, ++data_pointer) {
	if (*data_pointer & 3) {  // this pixel is a 1-element or it has been visited before
	  const int cNBD = *data_pointer & 2;   // the second pixel will be 1 if we've seen it before and 0 if not
	  const int previous_is_1 = *(data_pointer-1) & 3;
	  const int next_is_1 = *(data_pointer+1) & 3;
	  if (cNBD) { // this pixel has been seen before
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Updating LNBD to " << nbd_store[raster_x+raster_y*image.GetWidth()]);
#endif
	    LNBD = nbd_store[raster_x+raster_y*image.GetWidth()];
	    continue;
	  }	

	  int contour_length;
	  ContourStatistics contour_statistics;
	  if (!cNBD && !previous_is_1) { // this pixel has not been seen before and the previous pixel is a 0-element
	    current->bordertype = OUTER_BORDER;	  
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Found outer border.  Following from " << raster_x << "," << raster_y);
#endif
	    contour_length = FollowContour(image, data_pointer, raster_x, raster_y, current->points, contour_statistics,0, NBD,nbd_store);
	  } 
	  else if (!cNBD && !next_is_1) { // this pixel has not been seen before or it is not an exit pixel, and the next pixel is a 0-element
	    current->bordertype = HOLE_BORDER;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Found hole border.  Following from " << raster_x << "," << raster_y);
#endif
	    contour_length = FollowContour(image, data_pointer, raster_x, raster_y, current->points, contour_statistics,4, NBD,nbd_store);
	  }
	  else {
	    continue;
	  }

	  //	  if (contour_length < 20) { current->weeded = true; m_contour_count--;}
	
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
	  NBD = NBD + 1;
	  //	if (NBD==0) { ++NBD; }	 

	
	  current = new Contour(NBD);
	  m_contour_count++;
	  node_hash[NBD] = current;
	
	}
	else { // this is a 0-element
	
	}
      }
      ++data_pointer; // exclude the last pixel on the line
    }
    delete current;
    m_contour_count--;
#ifdef IMAGE_DEBUG
    debug_image->Save("debug-contourtree-contours.pnm");
#endif
    delete[] nbd_store;
  }

  ContourTree::ContourTree(const ContourTree& tree) : m_root_contour(new Contour(*(tree.m_root_contour))) {}


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

    // find the first point that we will start searching from
    do {
      position = (position - 1) & 0x7;
      sample_pointer = data_pointer+offset[position];
      if (*sample_pointer & 0x1) { break; }
    }
    while (position != start_position);

#ifdef CONTOUR_TREE_DEBUG
    PROGRESS("Finished clockwise scan at " << position << " (started:" << start_position << ")");
#endif

    // check if we have a 1-pixel contour
    if (position == start_position) {
      *data_pointer |= 2;
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

      // contour_n is the last point in the contour
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
	if (*sample_pointer & 3) {
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
	    *data_pointer &= 0xFE;
	    *data_pointer |= 2;
	    nbd_store[start_x+image_width*start_y] = nbd;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Marked  exit " << start_x << "," << start_y << " with 2");
#endif
	  }
	  // 2) else if sample_x,sample_y is unmarked write
	  // (NBD,l).
	  else if (!(*data_pointer & 2)) {
	    *data_pointer |= 3;
	    nbd_store[start_x+image_width*start_y] = nbd;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Marked " << start_x << "," << start_y << " with 3");
#endif
	  }
	  
	  // update the length
	  statistics.length += (position & 0x1 ? 45 : 32);
	  // update the bounding box
	  if (start_x < statistics.min_x) { statistics.min_x = start_x; }
	  else if (start_x > statistics.max_x) { statistics.max_x = start_x; }
	  if (start_y < statistics.min_y) { statistics.min_y = start_y; }
	  else if (start_y > statistics.max_y) { statistics.max_y = start_y; }


	  // store this point in the pixel chain and update the start position
	  points.push_back(start_x);
	  points.push_back(start_y);
	  start_x += offset_x[position];
	  start_y += offset_y[position];

	  // check the stopping condition
	  if ((points.size() > 4) && // i.e. we have seen more than two pixels
	      (data_pointer == contour_n) &&
	      (sample_pointer == contour_0)) {
	    statistics.length >>= 5;
#ifdef CONTOUR_TREE_DEBUG
	    PROGRESS("Found " << (points.size()>>1) << " pixel contour starting from "<< points[0] << "," << points[1]);
#endif
#if defined(IMAGE_DEBUG) and defined(CONTOUR_TREE_DEBUG_SAVE)
	    debug_image->DrawPixel(start_x,start_y,0);		  
	    debug_image->Save("debug-contourtree-contours.bmp");
#endif
	    return points.size()>>1;
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

      statistics.length >>= 5;
#ifdef CONTOUR_TREE_DEBUG
      PROGRESS("Contour length = " << (points.size()-3/2));
#endif
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

  bool ContourTree::CheckImageToNPCF(const ContourTree& evidence, const Camera& camera) const {
    return CheckImageToNPCF(camera,GetRootContour(),evidence.GetRootContour());
  }


  bool ContourTree::CheckImageToNPCF(const Camera& camera, const Contour* current, const Contour* evidence_current) const {
    if (current->points.size() != evidence_current->points.size()) return false; 
    if (current->children.size() != evidence_current->children.size()) return false;

    std::vector<float> pointscopy = current->points;
    camera.NPCFToImage(pointscopy);
    int size = current->points.size();
    for(int i=0;i<size;++i) {
      if (fabs(pointscopy[i]-evidence_current->points[i]) > 0.5) return false;
    }

    std::vector<Contour*>::const_iterator i = current->children.begin();
    std::vector<Contour*>::const_iterator j = evidence_current->children.begin();
    for(;
	i != current->children.end() && j != evidence_current->children.end();
	++i,++j) {
      if (!CheckImageToNPCF(camera,*i,*j)) return false;
    }
    return true;
  }

  ContourTree::Contour::~Contour() {
    for(std::vector<Contour*>::const_iterator i = children.begin();
	i!=children.end();
	++i) {
      delete *i;
    }
  }

  ContourTree::ContourTree(Socket& socket) {
    m_root_contour = new Contour(socket);
  }

  int ContourTree::Save(Socket& socket) const {
    if (m_root_contour != NULL) {
      return m_root_contour->Save(socket);
    }
    else {
      return 0;
    }
  }

  ContourTree::Contour::Contour(Socket& socket) : points() {
    nbd = socket.RecvInt();
    if (nbd == -1) {
      weeded = true; 
    }
    else {
      weeded = false;
      bordertype = (bordertype_t)socket.RecvInt();
      parent_id = socket.RecvInt();
      socket.Recv(points);
    }
    int count = socket.RecvInt();
    for(int i=0;i<count;++i) {
      children.push_back(new Contour(socket));
    }
  }

  ContourTree::Contour::Contour(const Contour& contour)  : 
    nbd(contour.nbd),
    bordertype(contour.bordertype),
    parent_id(contour.parent_id),
    points(contour.points),
    children(),
    weeded(contour.weeded)
  {
    for(std::vector<Contour*>::const_iterator i = contour.children.begin();
	i!=contour.children.end();
	++i) {
      children.push_back(new Contour(*(*i)));
    }
  }

  int ContourTree::Contour::Save(Socket& socket) const {
    int byte_count = 0;
    if (!weeded) {
      byte_count += socket.Send(nbd);
      byte_count += socket.Send((int)bordertype);
      byte_count += socket.Send(parent_id);
      byte_count += socket.Send(points);
    }
    else {
      byte_count += socket.Send(-1);
    }
    int counter = 0;
    for(std::vector<Contour*>::const_iterator i = children.begin(); i!= children.end(); ++i) {
      if (!(*i)->weeded) counter++;
    }
    byte_count+= socket.Send(counter);
    for(std::vector<Contour*>::const_iterator i = children.begin(); i!= children.end(); ++i) {
      if (!(*i)->weeded) {
	byte_count += (*i)->Save(socket);
      }
    }
    return byte_count;
  }
}
