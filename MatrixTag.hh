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
class MatrixTag : virtual Tag<Rectangle2D> {
private:
  C m_coder;
  int m_size;
  float *m_cells_bottom_left;

public:
  MatrixTag(int size) :
    m_coder(2,size*size),
    m_size(size+2) {
    
    // we draw an outer border of black cells
    // ie (0,_) = black
    //    (size+1,_) = black
    //    (_,0) = black
    //    (_,size+1) = black
    //
    // This leaves a space of size x size cells in the middle for the code
    int arraysize = m_size*m_size;
    m_cells_bottom_left = new float[arraysize];
    
    // our first cell starts at position 0,0 and is
    // m_cells_bottom_left[0],m_cells_bottom_left[1]

    // the bottom right cell is at 1,0 and is
    // m_cells_bottom_left[2*size-2],m_cells_bottom_left[2*size-1]
    
    // the top left cell is at 0,1 and is
    // m_cells_bottom_left[2*size * (size-1)],m_cells_bottom_left[2*size * (size-1)+1]

    // the top right cell is at 1,1 and is
    // m_cells_bottom_left[2*size * size-2],m_cells_bottom_left[2*size * size-1]

    for(int i=0;i<m_size;i++) {
      for(int j=0;j<m_size;j++) {
	m_cells_bottom_left[i*2*m_size + 2*j] = (float)j/(float)m_size;
	m_cells_bottom_left[i*2*m_size + 2*j+1] = (float)i/(float)m_size;
	std::cout << "("<<(float)j/(float)m_size << "," << (float)i/(float)m_size <<") ";
      }
      std::cout << std::endl;
    }
  }

  virtual ~MatrixTag() {
    delete[] m_cells_bottom_left;
  }
  
  virtual void Draw2D(Image* image, const Rectangle2D *l, unsigned long code, int black, int white) {
    PROGRESS("Drawing tag (" << l->m_x0 << "," << l->m_y0 << ") (" << l->m_x1 << "," << l->m_y1 << ") (" << l->m_x2 << "," << l->m_y2 << ") (" << l->m_x3 << ","<< l->m_y3<<")");
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
    for(int i=1;i<m_size-1;i++) {
      for(int j=1;j<m_size-1;j++) {
	l->ProjectPoint(m_cells_bottom_left[i*m_size*2+2*j],m_cells_bottom_left[i*m_size*2+2*j+1], &projX0, &projY0);
	l->ProjectPoint(m_cells_bottom_left[(i+1)*m_size*2+2*j],m_cells_bottom_left[(i+1)*m_size*2+2*j+1], &projX1, &projY1);
	l->ProjectPoint(m_cells_bottom_left[(i+1)*m_size*2+2*(j+1)],m_cells_bottom_left[(i+1)*m_size*2+2*(j+1)+1], &projX2, &projY2);
	l->ProjectPoint(m_cells_bottom_left[i*m_size*2+2*(j+1)],m_cells_bottom_left[i*m_size*2+2*(j+1)+1], &projX3, &projY3);
	DrawFilledQuadTangle(image,
			     (int)projX0, (int)projY0,
			     (int)projX1, (int)projY1,
			     (int)projX2, (int)projY2,
			     (int)projX3, (int)projY3,
			     m_coder.NextChunk() == 1 ? black : white);
      }
    }
  }

  virtual unsigned long Decode(Image *image, const Rectangle2D *l) { return 0; }

};
#endif//MATRIX_TAG_GUARD
