/**
 * $Header$
 *
 * $Log$
 * Revision 1.5  2004/02/03 16:24:56  acr31
 * various function signature changes and use of __FILE__ and __LINE__ in debug macros
 *
 * Revision 1.4  2004/02/01 14:25:33  acr31
 * moved Rectangle2D to QuadTangle2D and refactored implementations around
 * the place
 *
 * Revision 1.3  2004/01/30 16:54:17  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.2  2004/01/27 18:06:58  acr31
 * changed inheriting classes to inherit publicly from their parents
 *
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.9  2004/01/24 19:29:24  acr31
 * removed ellipsetoxy and put the project method in Ellipse2D objects
 *
 * Revision 1.8  2004/01/24 10:33:35  acr31
 * changed unsigned long to unsigned long long
 *
 * Revision 1.7  2004/01/23 16:08:55  acr31
 * remove spurious #include
 *
 * Revision 1.6  2004/01/23 16:03:51  acr31
 * moved CircularTag back to Tag - but its now templated on the type of tag - ellipse or rectangle
 *
 * Revision 1.5  2004/01/23 12:05:48  acr31
 * moved Tag to CircularTag in preparation for Squaretag
 *
 * Revision 1.4  2004/01/23 11:57:08  acr31
 * moved Location2D to Ellipse2D in preparation for Square Tags
 *
 * Revision 1.3  2004/01/21 13:41:36  acr31
 * added pose from circle to triptest - (pose from circle is unstable at the moment)
 *
 * Revision 1.2  2004/01/21 11:52:29  acr31
 * added keywords
 *
 */

#ifndef RING_TAG_GUARD
#define RING_TAG_GUARD

#include "Config.hh"
#include "Drawing.hh"
#include "Tag.hh"
#include "Coder.hh"
#include "Ellipse2D.hh"

#undef FILENAME
#define FILENAME "RingTag.hh"

class RingTag : public virtual Tag<Ellipse2D>, protected virtual Coder {
private:
  int m_ring_count;
  int m_sector_count;
  float m_bullseye_inner_radius;
  float m_bullseye_outer_radius;
  float m_data_inner_radius;
  float m_data_outer_radius;
  float *m_data_ring_outer_radii;
  float *m_data_ring_centre_radii;
  float *m_sector_angles;
  float *m_read_angles;

public:
  RingTag(int ring_count,
	  int sector_count,
	  float bullseye_inner_radius,
	  float bullseye_outer_radius,
	  float data_inner_radius,
	  float data_outer_radius);
  virtual ~RingTag();
  virtual void Draw2D(Image* image,  unsigned long long code, int black, int white);
  virtual unsigned long long Decode(Image *image, const Ellipse2D *l);
};
#endif//RING_TAG_GUARD
