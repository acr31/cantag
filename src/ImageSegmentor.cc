/**
 * $Header$
 */
#include <ImageSegmentor.hh>

#include <iostream>

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
int ImageSegmentor::FollowContour(Image& image, int start_x, int start_y, float* points_buffer, const int maxcount, int nbd) {
  // these two arrays store the offsets for each of the eight regions
  // around the target pixel
  const int offset_x[] = {-1,-1,0,1,1,1,0,-1};
  const int offset_y[] = {0,1,1,1,0,-1,-1,-1};

  points_buffer[0] = start_x;
  points_buffer[1] = start_y;
  int position = 0;
  int previous_position = 0;
  int comparison_pointer = 0;
  int pointer = 2;

  int length = 0;
  int min_x = start_x;
  int max_x = start_x;
  int min_y = start_y;
  int max_y = start_y;
  bool convex = true;
  enum direction_t = {CLOCKWISE,ANTI_CLOCKWISE};
  direction_t direction = ANTI_CLOCKWISE;

  while(pointer < maxcount*2) {
    int sample_x = start_x+offset_x[position];
    int sample_y = start_y+offset_y[position];
    if (image.SampleNoCheck(sample_x,sample_y)) {
      image.DrawPixel(sample_x,sample_y, nbd<<1 | 0x1);
      points_buffer[pointer++] = start_x = sample_x;
      points_buffer[pointer++] = start_y = sample_y;

      // update the length
      length+= (position%2 == 0 ? 32 : 45);

      // update the bounding box
      if (start_x < min_x) { min_x = start_x; }
      else if (start_x > max_x) { max_x = start_x; }
      if (start_y < min_y) { min_y = start_y; }
      else if (start_y > max_y) { max_y = start_y; }

      if (previous_position != position) {
	// we've made a turn
	if (position = 
      }

      // adding 5 to our position (mod 8) will put our start
      // position one further round than the previous central point.
      position+=5;
    }
    else {
      position++;
    }
    position%=8;	
    
    // if our current pixel matches the _second_ pixel found in the
    // chain and the previous pixel we found matches the first then we
    // are done.  we have to wait until pointer>4 in order to make
    // sure we dont immediatly stop.  I wonder if the compiler will
    // notice it can unroll this loop a couple of times and remove
    // this comparison?
    if ((pointer > 4) && 
	(points_buffer[2] == sample_x) &&
	(points_buffer[3] == sample_y) &&
	(points_buffer[0] == points_buffer[pointer-4]) &&
	(points_buffer[1] == points_buffer[pointer-3])) {
      break;
    }
  }
  return (pointer-4)/2;
}


// length
// area
// convexity
// bounding rectangle

// store contours in hash table
