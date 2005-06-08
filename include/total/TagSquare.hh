/**
 * $Header$
 */

#ifndef TAG_SQUARE_GUARD
#define TAG_SQUARE_GUARD

#undef SQUARE_DEBUG_POINTS

namespace Total {

  template<int SIZE>
  class TagSquare {
  public:
    static const int PayloadSize = SIZE*SIZE - ((SIZE*SIZE)%2);

  private:
    const float m_cell_width;
    const float m_cell_width_2;
    float m_cells_corner[SIZE*SIZE*2];

  public:
    TagSquare();
    
    inline float GetXSamplePoint(int cell_number) const {
      return m_cells_corner[2*cell_number]+m_cell_width_2;
    }
    
    inline float GetYSamplePoint(int cell_number) const {
      return m_cells_corner[2*cell_number+1]+m_cell_width_2;
    }

  };

  template<int SIZE> TagSquare<SIZE>::TagSquare() : m_cell_width(2.f/(SIZE+2)), m_cell_width_2(1.f/(SIZE+2)) {
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
    int triangle_size = SIZE*SIZE / 2;
    int position = 0;
    for(int height = 0; height < SIZE/2; height++) {
      for(int i=height;i<SIZE-1-height;i++) {
	m_cells_corner[position] = 2*(float)(i+1)/(SIZE+2) - 1;
	m_cells_corner[position+1] = 2*(float)(height+1)/(SIZE+2) - 1;

	m_cells_corner[position+triangle_size] = 2*(float)(SIZE-height)/(SIZE+2) - 1;
	m_cells_corner[position+triangle_size+1] = 2*(float)(i+1)/(SIZE+2) - 1;

	m_cells_corner[position+triangle_size*2] = 2*(float)(SIZE-i)/(SIZE+2) - 1;
	m_cells_corner[position+triangle_size*2+1] = 2*(float)(SIZE-height)/(SIZE+2) - 1;

	m_cells_corner[position+triangle_size*3] = 2*(float)(height+1)/(SIZE+2) - 1;
	m_cells_corner[position+triangle_size*3+1] = 2*(float)(SIZE-i)/(SIZE+2) - 1;

	position+=2;
      }
    }
    
#ifdef SQUARE_DEBUG_POINTS
    for (int i=0;i<SIZE*SIZE - (SIZE*SIZE % 2);i++) {
      std::cout << m_cells_corner[2*i] << " " << m_cells_corner[2*i+1] << std::endl;
    }
#endif
  }

}
#endif//TAG_SQUARE_GUARD
