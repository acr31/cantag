/**
 * $Header$
 */

#ifndef CONTOUR_FOLLOWER_TREE_GUARD
#define CONTOUR_FOLLOWER_TREE_GUARD

#include <total/Config.hh>
#include <total/MonochromeImage.hh>
#include <total/EntityTree.hh>
#include <total/entities/ContourEntity.hh>
#include <total/Function.hh>
#include <total/ContourRestrictions.hh>

namespace Total {
  class ContourFollowerTree : public Function1<MonochromeImage,TreeNode<ContourEntity> >{
  private:
    /**
     * An array of lookup values for each pixel.  The low two bits
     * indicate if the pixel has been visited before.  The third bit
     * indicates if it is a left or right pixel and the remaining bits
     * are the NBD.
     *
     * If our flag is 1 then the pixel has been visited before if the
     * lowest bit is set, if our flag is 2 then the pixel has been
     * visited before if the second bit is set.  When we raster across
     * the image we clear the values in the bit we are not using ready
     * for the next pass.
     */
    mutable unsigned int* m_working_store;
    mutable int m_image_width;
    mutable int m_image_height;
    mutable int m_flag;

    int m_offset_x[8];
    int m_offset_y[8];

    enum PixelType { EXIT_PIXEL=0, ENTRY_PIXEL=1};

    const ContourRestrictions& m_constraints;

  public:
    ContourFollowerTree(const ContourRestrictions& constraints);
    ~ContourFollowerTree();
    bool operator()(const MonochromeImage& image, TreeNode<ContourEntity>& result) const;
    
  private:
    int FollowContour(const MonochromeImage& image, // the image
		      int start_x,  int start_y, // the start co-ordinates (must lie on contour)
		      int start_position,  // the position in the 8-connected region to start searching from
		      const int nbd,  // the NBD to mark this contour with
		      ContourEntity* result // the entity to store the result in
		      ) const ;

    /**
     * Find this pixel in the working store.  Check if it has been
     * visited before, if it has then return the NBD else return -1.
     * Also update the pixel store to clear out the visit bit when we
     * are using the other flag
     */
    int AssessPixel(int x, int y) const {
      const int index = x+m_image_width*y;
      m_working_store[index] &= ~(1-m_flag);
      if (m_working_store[index] & m_flag) {
	return m_working_store[index] >> 3;
      }
      else {
	return -1;
      }
    }
    

    void SetVisited(int x, int y, int nbd, PixelType t) const {
      m_working_store[x+m_image_width*y] = nbd << 3 | t << 2 | m_flag;
    }
  };
}
#endif//CONTOUR_FOLLOWER_TREE_GUARD
