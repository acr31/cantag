/**
 * $Header$
 */
#include <ImageSegmentor.hh>

#include <iostream>
/**
 *  Start following a contour adjoining an arbitrary 0-element start_x-1,start_y
 *
 *  Search for a 1-element counterclockwise starting from the 0-element in the region of start_x,start_y
 *  Once a 1-element is found repeat the search from that position
 */

int ImageSegmentor::FollowContour(Image& image, int start_x, int start_y, float* points_buffer, const int maxcount) {
  const int offset_x[] = {-1,-1,0,1,1,1,0,-1};
  const int offset_y[] = {0,1,1,1,0,-1,-1,-1};

  points_buffer[0] = start_x;
  points_buffer[1] = start_y;
  int position = 0;
  int comparison_pointer = 0;
  int pointer = 2;
  while(pointer < maxcount*2) {
    int sample_x = start_x+offset_x[position];
    int sample_y = start_y+offset_y[position];
    if (image.SampleNoCheck(sample_x,sample_y) == 0) {
	points_buffer[pointer++] = start_x = sample_x;
	points_buffer[pointer++] = start_y = sample_y;
	position+=5;
    }
    else {
      position++;
    }
    position%=8;	

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
