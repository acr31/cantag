/**
 * $Header$
 */

#ifndef CONTOUR_TREE_GUARD
#define CONTOUR_TREE_GUARD

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/Camera.hh>
#include <total/Socket.hh>

#include <vector>
#include <map>

namespace Total {

  /**
   * A tree of matched contours.  Create an instance of this class from
   * an image and a set of contour "weeding" constraints.  A weeding
   * constraint defines whether the contour should be considered a
   * candidate for further processing.
   */ 
  class ContourTree {
  public:
    enum topology_t {ALL,CONCAVE_ONLY, CONVEX_ONLY};
    enum bordertype_t { UNKNOWN = 2, OUTER_BORDER = 1, HOLE_BORDER = 0};

    /**
     * A data structure for holding the contour itself
     */
    struct Contour {
      /**
       * The unique contour id
       */
      int nbd;
      
      /**
       * The type of the contour (UKNOWN, OUTER_BORDER, HOLE_BORDER)
       */
      bordertype_t bordertype;

      /**
       * The contour id of the parent contour
       */
      int parent_id;

      /**
       * The points on the contour
       */
      std::vector<float> points;

      /**
       * The children contours - a child contour is completely contained within the parent contour
       */
      std::vector<Contour*> children;

      /**
       * Indicate if this contour has been eliminated from
       * consideration in future stages because it failed to match a
       * contour constraint.
       */
      bool weeded;

      /**
       * Create a new contour with the given contour id
       */ 
      Contour(int id) : nbd(id),bordertype(UNKNOWN),parent_id(id),points(),children(),weeded(false) {}

      /**
       * Create this contour with data from the network
       */
      Contour(Socket& socket);

      /**
       * Copy construct this contour (deep copy so all children contours are copied too)
       */
      Contour(const Contour& contour);

      /**
       * Delete this contour and all children contours
       */
      ~Contour();

      /**
       * Save this contour over the network
       */
      int Save(Socket& socket) const;

    };
  
    /**
     * A datastructure representing constraints to check as we accumulate contours
     * 
     * \todo currently unused
     */
    struct ContourConstraint {
      int minLength;
      int maxLength;
      topology_t toplogy;
      int minArea;
      int maxArea;
      ContourConstraint() {}
    };

    /**
     * A datastructure to represent statistics about the contour as we walk around it
     *
     * \todo currently unused
     */
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
    Contour* m_root_contour;
    int m_contour_count;

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

    /**
     * Convert this contour to NCPF and then recursively convert its children contours
     */
    void ImageToNPCF(const Camera& camera, Contour* contour);
    bool CheckImageToNPCF(const Camera& camera, const Contour* current, const Contour* evidence_current) const;
  public:
    
    /**
     * Construct a contour tree from the proffered image
     */
    ContourTree(Image& image);

    /**
     * Construct a contour tree from the network
     */
    ContourTree(Socket& socket);

    /**
     * Copy construct (deep copy) this contour tree
     */
    ContourTree(const ContourTree& tree);

    /**
     * Convert the pixels in the contours in this tree to NPCF co-ordinates
     */
    void ImageToNPCF(const Camera& camera);

    /**
     * Validate the ImageToNPCF stage
     */
    bool CheckImageToNPCF(const ContourTree& evidence, const Camera& camera) const;

    /**
     * Return a pointer to the root contour in the tree
     */
    inline Contour* GetRootContour() { return m_root_contour; }

    /**
     * Return a const pointer to the root contour in the tree
     */
    inline const Contour* GetRootContour() const { return m_root_contour; }

    /**
     * Return the number of contours in the tree
     */
    inline int GetContourCount() const { return m_contour_count; }

    /**
     * Delete this tree and all the contours within it
     */
    ~ContourTree();

    /**
     * Send this contour tree over the network using this socket
     */
    int Save(Socket& socket) const;

  };
}
#endif//CONTOUR_TREE_GUARD
