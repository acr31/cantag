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
#include "Rectangle2D.hh"

#undef FILENAME
#define FILENAME "MatrixTag.hh"

template<class C>
class MatrixTag : public Tag<Rectangle2D> {
private:
  int m_length;
  C m_coder;
  int m_size;
  float m_cell_width;
  float m_cell_width_2;
  float *m_cells_corner;

public:
  MatrixTag(int size) :
    m_length(size*size - (size*size % 2)),
    m_coder(2,size*size - (size*size % 2)),
    m_size(size),
    m_cell_width(1/(float)(size+2)),
    m_cell_width_2(0.5/(float)(size+2))
  {

    /* we read the tag in triangles:
     *
     *    1  2  3  4  5  6  7 17       1  2  3  4  5  6 13
     *   55  8  9 10 11 12 24 18      43  7  8  9 10 19 14
     *   54 60 13 14 15 29 25 19      42 47 11 12 23 20 15
     *   53 59 63 16 32 30 26 20      41 46 49    24 21 16
     *   52 58 62 64 48 31 27 21      40 45 48 37 36 22 17
     *   51 57 61 47 46 45 28 22      39 44 35 34 33 32 18
     *   50 56 44 43 42 41 40 23      38 30 29 28 27 26 25
     *   49 39 38 37 36 35 34 33
     *
     * This way the only possible ways of reading the code are rotations of each other
     *
     * If the tag has an odd number of elements then the middle
     * element would be unread.  So we leave it leave it unused.  The
     * other option is to read it 4 times at the top of each triangle.
     *
     */
    m_cells_corner = new float[m_size*m_size];
    int triangle_size = m_size*m_size / 2;
    int position = 0;
    for(int height = 0; height < m_size/2; height++) {
      for(int i=height;i<m_size-1-height;i++) {
	m_cells_corner[position] = (float)(i+1)/(m_size+2);
	m_cells_corner[position+1] = (float)(height+1)/(m_size+2);

	m_cells_corner[position+triangle_size] = (float)(m_size-height)/(m_size+2);
	m_cells_corner[position+triangle_size+1] = (float)(i+1)/(m_size+2);

	m_cells_corner[position+triangle_size*2] = (float)(m_size-i)/(m_size+2);
	m_cells_corner[position+triangle_size*2+1] = (float)(m_size-height)/(m_size+2);

	m_cells_corner[position+triangle_size*3] = (float)(height+1)/(m_size+2);
	m_cells_corner[position+triangle_size*3+1] = (float)(m_size-i)/(m_size+2);
	position+=2;
      }
    }
    
    /**
       print co-ordinates for GNUPLOT
    for (int i=0;i<m_length;i++) {
      std::cout << m_cells_corner[2*i] << " " << m_cells_corner[2*i+1] << std::endl;
    }
    */
  }

  virtual ~MatrixTag() {
  }

  
  virtual void Draw2D(Image* image, const Rectangle2D *l, unsigned long long code, int black, int white) {
    PROGRESS("Drawing tag " <<
	     "(" << l->m_x0 << "," << l->m_y0 << ") " <<
	     "(" << l->m_x1 << "," << l->m_y1 << ") " <<
	     "(" << l->m_x2 << "," << l->m_y2 << ") " <<
	     "(" << l->m_x3 << "," << l->m_y3 << ")");
    DrawFilledQuadTangle(image,
			 l->m_x0, l->m_y0,
			 l->m_x1, l->m_y1,
			 l->m_x2, l->m_y2,
			 l->m_x3, l->m_y3,
			 black);

    // now draw the code
    m_coder.Set(code);
    float projX0, projY0;
    float projX1, projY1;
    float projX2, projY2;
    float projX3, projY3;
    for(int i=0;i<m_length;i++) {
      l->ProjectPoint(m_cells_corner[2*i],m_cells_corner[2*i+1], &projX0, &projY0);
      l->ProjectPoint(m_cells_corner[2*i]+m_cell_width,m_cells_corner[2*i+1], &projX1, &projY1);
      l->ProjectPoint(m_cells_corner[2*i]+m_cell_width,m_cells_corner[2*i+1]+m_cell_width, &projX2, &projY2);
      l->ProjectPoint(m_cells_corner[2*i],m_cells_corner[2*i+1]+m_cell_width, &projX3, &projY3);
      DrawFilledQuadTangle(image,
			   (int)projX0, (int)projY0,
			   (int)projX1, (int)projY1,
			   (int)projX2, (int)projY2,
			   (int)projX3, (int)projY3,
			   m_coder.NextChunk() == 1 ? black : white);
    }
  }


  virtual unsigned long long Decode(Image *image, const Rectangle2D *l) { 
#ifdef IMAGE_DEBUG
    Image* debug0 = cvCloneImage(image);
    cvConvertScale(debug0,debug0,0.5,128);    
#endif
    float projX, projY;
    m_coder.Reset();
    // iterate over the tag reading each section
    for(int i=0;i<m_length;i++) {
      l->ProjectPoint(m_cells_corner[2*i]+m_cell_width_2,m_cells_corner[2*i+1]+m_cell_width_2, &projX, &projY);
      try {
	m_coder.LoadChunk( (SampleImage(image,(int)projX,(int)projY) < 128) ? 1 : 0);
      }
      catch (Coder::InvalidSymbol &e) {
#ifdef IMAGE_DEBUG
	cvSaveImage("debug-decode0.jpg",debug0);
	cvReleaseImage(&debug0);
#endif
	throw e;
      }
#ifdef IMAGE_DEBUG
      if (i==1) {
	cvLine(debug0,cvPoint((int)projX,(int)projY),cvPoint((int)projX,(int)projY),255,8);
	cvLine(debug0,cvPoint((int)projX,(int)projY),cvPoint((int)projX,(int)projY),255-10*i,6);
      }
      else {
	cvLine(debug0,cvPoint((int)projX,(int)projY),cvPoint((int)projX,(int)projY),255,4);
	cvLine(debug0,cvPoint((int)projX,(int)projY),cvPoint((int)projX,(int)projY),255-10*i,3);
      }
#endif	
    }
    
#ifdef IMAGE_DEBUG
    cvSaveImage("debug-decode0.jpg",debug0);
    cvReleaseImage(&debug0);
#endif
    return m_coder.Decode();
  }

};
#endif//MATRIX_TAG_GUARD
