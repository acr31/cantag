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
 * Revision 1.1  2004/02/01 14:26:48  acr31
 * Implementations for Matrixtag and ringtag
 *
 *
 */

#include <Config.hh>
#include <MatrixTag.hh>
#include <Drawing.hh>
#include <Tag.hh>
#include <Coder.hh>
#include <QuadTangle2D.hh>

#undef FILENAME
#define FILENAME "MatrixTag.cc"

MatrixTag::MatrixTag(int size) :
  m_length(size*size - (size*size % 2)),
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
  m_cells_corner = new float[m_size*m_size*2];
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

MatrixTag::~MatrixTag() {}

void MatrixTag::Draw2D(Image* image, const QuadTangle2D *l, unsigned long long code, int black, int white) {
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
  unsigned long long value = EncodeTag(code);
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
			 (value & 1) == 1 ? black : white);
    value >>=1;
  }
}


unsigned long long MatrixTag::Decode(Image *image, const QuadTangle2D *l) { 
#ifdef IMAGE_DEBUG
  Image* debug0 = cvCloneImage(image);
  cvConvertScale(debug0,debug0,0.5,128);    
  DrawQuadTangle(debug0,l,0,3);
    
  for(int i=0;i<m_size+2;i++) {
    float dX,dY;
    l->ProjectPoint((float)i/(m_size+2),0,&dX,&dY);
    CvPoint p1 = cvPoint((int)dX,(int)dY);
    l->ProjectPoint((float)i/(m_size+2),1,&dX,&dY);
    CvPoint p2 = cvPoint((int)dX,(int)dY);

    l->ProjectPoint(0,(float)i/(m_size+2),&dX,&dY);
    CvPoint p3 = cvPoint((int)dX,(int)dY);
    l->ProjectPoint(1,(float)i/(m_size+2),&dX,&dY);
    CvPoint p4 = cvPoint((int)dX,(int)dY);

    cvLine(debug0,p1,p2,0,1);
    cvLine(debug0,p3,p4,0,1);      
  }

#endif


  float projX, projY;
  unsigned long long code = 0;
  // iterate over the tag reading each section
  for(int i=m_length-1;i>=0;i--) {
    l->ProjectPoint(m_cells_corner[2*i]+m_cell_width_2,m_cells_corner[2*i+1]+m_cell_width_2, &projX, &projY);
    bool sample = SampleImage(image,(int)projX,(int)projY) > 128;
    code <<= 1;
    code |= sample ? 1 : 0;
#ifdef IMAGE_DEBUG
    cvLine(debug0,cvPoint((int)projX,(int)projY),cvPoint((int)projX,(int)projY),0,5);
#endif	
  }    


#ifdef IMAGE_DEBUG
  cvSaveImage("debug-decode.jpg",debug0);
  cvReleaseImage(&debug0);
#endif

  return DecodeTag(code);
}

