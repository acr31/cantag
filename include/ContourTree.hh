/**
 * $Header$
 */

#ifndef CONTOUR_TREE_GUARD
#define CONTOUR_TREE_GUARD

#include <Config.hh>
#include <Image.hh>
#include <vector>
#include <map>

/**
 * A tree of matched contours.  Create an instance of this class from
 * an image and a set of contour "weeding" constraints.  A weeding
 * constraint defines whether the contour should be considered a
 * candidate for further processing.
 */ 
class ContourTree {
public:
  enum topology_t {ALL,CONCAVE_ONLY, CONVEX_ONLY};
  enum bordertype_t { OUTER_BORDER = 1, HOLE_BORDER = 0};

  struct Contour {
    int nbd;
    bordertype_t bordertype;
    int parent_id;
    std::vector<float> points;
    std::vector<Contour*> children;
    Contour(int id) : nbd(id),parent_id(id) {}
  };

  struct ContourConstraint {
    int minLength;
    int maxLength;
    topology_t toplogy;
    int minArea;
    int maxArea;
    ContourConstraint() {}
  };

  struct ContourStatistics {
    int length;
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    bool convex;
    bool concave;
    ContourStatistics() : length(0), min_x(100000), max_x(-100000),min_y(100000),max_y(-1000000),convex(true),concave(true) {}
  };

private:
  Image& m_image;
  std::vector<ContourConstraint>& m_constraints;
  std::map<int,Contour*> m_node_hash;
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
                    std::vector<float>& points,  // the buffer to store the points
                    ContourStatistics& statistics,   // contour statistics structure (can be NULL)
                    int position,  // the position in the 8-connected region to start searching from
                    int nbd,  // the NBD to mark this contour with
		    unsigned int* nbd_store
                    );
public:

  ContourTree(Image& image, std::vector<ContourConstraint>& constraints);
  inline Contour* GetRootContour() { return m_node_hash[1]; }
  ~ContourTree();
};

#endif//CONTOUR_TREE_GUARD
