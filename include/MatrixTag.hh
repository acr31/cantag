/**
 * An implementation of the Matrix tag found in
 *
 * @InProceedings{ip:apchi:rekimoto98,
 *  author        = "Jun Rekimoto",
 *  title         = "Matrix: A Realtime Object Identification and Registration Method for Augmented Reality",
 *  booktitle     = "Proceedings of Asia Pacific Computer Human Interaction",
 *  year          = "1998",
 * }
 * 
 *
 * $Header$
 *
 * $Log$
 * Revision 1.8  2004/02/16 08:02:02  acr31
 * *** empty log message ***
 *
 * Revision 1.7  2004/02/03 16:24:55  acr31
 * various function signature changes and use of __FILE__ and __LINE__ in debug macros
 *
 * Revision 1.6  2004/02/01 14:25:33  acr31
 * moved Rectangle2D to QuadTangle2D and refactored implementations around
 * the place
 *
 * Revision 1.5  2004/01/30 16:54:17  acr31
 * changed the Coder api -reimplemented various bits
 *
 * Revision 1.4  2004/01/30 08:05:23  acr31
 * changed rectangle2d to use gaussian elimination
 *
 * Revision 1.3  2004/01/29 12:47:14  acr31
 * added method for drawing unfilled quadtangles
 *
 * Revision 1.2  2004/01/27 18:06:58  acr31
 * changed inheriting classes to inherit publicly from their parents
 *
 * Revision 1.1  2004/01/25 14:54:37  acr31
 * moved over to automake/autoconf build system
 *
 * Revision 1.3  2004/01/24 17:53:22  acr31
 * Extended TripOriginalCoder to deal with base 2 encodings.  MatrixTag
 * implementation now works.
 *
 * Revision 1.2  2004/01/23 22:36:39  acr31
 * added a cyclic reading and writing scheme
 *
 * Revision 1.1  2004/01/23 18:18:11  acr31
 * added Matrix Tag and a test program and the beginning of the CRC coder
 *
 */

#ifndef MATRIX_TAG_GUARD
#define MATRIX_TAG_GUARD

#include "Config.hh"
#include "Drawing.hh"
#include "Tag.hh"
#include "Coder.hh"
#include "QuadTangle2D.hh"
#include <Camera.hh>

class MatrixTag : public virtual Tag<QuadTangle2D>, protected virtual Coder {
private:
  int m_length;
  int m_size;
  float m_cell_width;
  float m_cell_width_2;
  float *m_cells_corner;

public:
  MatrixTag(int size);
  virtual ~MatrixTag();
  virtual void Draw2D(Image* image, unsigned long long code, int black, int white);
  virtual unsigned long long Decode(Image *image, Camera* camera, const QuadTangle2D *l);
};
#endif//MATRIX_TAG_GUARD
