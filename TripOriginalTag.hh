#ifndef TRIP_ORIGINAL_TAG_GUARD
#define TRIP_ORIGINAL_TAG_GUARD

#include <vector>
#include <cv.h>


#include "Tag.hh"

/**
 * Encapsulates all the properties of a Trip original style tag
 *
 * The detection method is:
 *
 * Convert image to grayscale
 * Adaptive threshold
 * Contour following
 * Identifying concentric ellipses
 * Decoding tag name
 *
 * The coding system is:
 *
 * One sector all black for synchronization
 * Then some number of checksum sectors
 * Then remaining data sectors
 * Each sector encodes 2^(RING_COUNT-1) values - all set is not allowed
 *
 */

/* The number of data rings */
#define RING_COUNT 2

/* The total number of sectors including synchronization and checksum */
#define SECTOR_COUNT 16

/* The total number of angles to try when looking for the sync sector */
#define SYNC_COUNT 24

/* The number of checksum sectors that follow the sync sector */
#define CHECKSUM_COUNT 2

class TripOriginalTag : virtual Tag {
private:
  /* The angle of each sector - there should be SECTOR_COUNT of
     these */
  static const double sector_angles[]; 
  /* The angles to try looking for a synchronization sector - there
     should be SYNC_COUNT of these */
  static const double sync_angles[];
  
  /* The radii for each ring - there should be RING_COUNT of these.
     The system multiplies the radius of the outer bullseye ring by
     each radius to find the radius of the ring in question  */   
  static const double radii_outer[];
  static const double radii_centre[];
  static const double radii_inner[];

public:
  TripOriginalTag(CvBox2D ellipse, long code);
  static void Process(IplImage *image,std::vector<Tag*> *result);
  virtual std::ostream& Print(std::ostream& s) const;
  virtual void Show(IplImage *image);
  virtual void Synthesize(IplImage *image,int white,int black);
  
private:
  CvBox2D m_ellipse;
  unsigned long m_code;
};


#endif//TRIP_ORIGINAL_TAG_GUARD
