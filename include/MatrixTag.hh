/**
 * $Header$
 */

#ifndef MATRIX_TAG_GUARD
#define MATRIX_TAG_GUARD

#include <Config.hh>
#include <Image.hh>
#include <Tag.hh>
#include <Coder.hh>
#include <QuadTangle.hh>
#include <Camera.hh>
#include <ShapeChain.hh>
#include <findtransform.hh>

#define MATRIX_TAG_DEBUG
#define MATRIX_TAG_DEBUG_POINTS


/**
 * An implementation of the Matrix tag found in
 *
 * @InProceedings{ip:apchi:rekimoto98,
 *  author        = "Jun Rekimoto",
 *  title         = "Matrix: A Realtime Object Identification and Registration Method for Augmented Reality",
 *  booktitle     = "Proceedings of Asia Pacific Computer Human Interaction",
 *  year          = "1998",
 * }
 */ 
template<int SIZE>
class MatrixTag : public virtual Tag< ShapeChain<QuadTangle>, SIZE*SIZE - (SIZE*SIZE % 2) >, protected virtual Coder<SIZE*SIZE - (SIZE*SIZE % 2)> {
 private:
  int m_length;
  float m_cell_width;
  float m_cell_width_2;
  float *m_cells_corner;

 public:
  MatrixTag() :
    m_length(SIZE*SIZE - (SIZE*SIZE % 2)),
    m_cell_width(1/(float)(SIZE+2)),
    m_cell_width_2(0.5/(float)(SIZE+2))
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
      m_cells_corner = new float[SIZE*SIZE*2];
      int triangle_size = SIZE*SIZE / 2;
      int position = 0;
      for(int height = 0; height < SIZE/2; height++) {
	for(int i=height;i<SIZE-1-height;i++) {
	  m_cells_corner[position] = (float)(i+1)/(SIZE+2);
	  m_cells_corner[position+1] = (float)(height+1)/(SIZE+2);

	  m_cells_corner[position+triangle_size] = (float)(SIZE-height)/(SIZE+2);
	  m_cells_corner[position+triangle_size+1] = (float)(i+1)/(SIZE+2);

	  m_cells_corner[position+triangle_size*2] = (float)(SIZE-i)/(SIZE+2);
	  m_cells_corner[position+triangle_size*2+1] = (float)(SIZE-height)/(SIZE+2);

	  m_cells_corner[position+triangle_size*3] = (float)(height+1)/(SIZE+2);
	  m_cells_corner[position+triangle_size*3+1] = (float)(SIZE-i)/(SIZE+2);

	  position+=2;
	}
      }
    
#ifdef MATRIX_TAG_DEBUG_POINTS
      for (int i=0;i<m_length;i++) {
	std::cout << m_cells_corner[2*i] << " " << m_cells_corner[2*i+1] << std::endl;
      }
#endif
    }

  virtual ~MatrixTag() {}

  virtual void Draw2D(Image& image, unsigned long long code) {
#ifdef MATRIX_TAG_DEBUG
    PROGRESS("Draw2D called");
#endif
    int x0 = 0;
    int y0 = 0;
    int x1 = image.GetWidth();
    int y1 = 0;
    int x2 = image.GetWidth();
    int y2 = image.GetHeight();
    int x3 = 0;
    int y3 = image.GetHeight();

    int size = image.GetWidth() < image.GetHeight() ? image.GetWidth() : image.GetHeight();

    image.DrawFilledQuadTangle(x0, y0,
			       x1, y1,
			       x2, y2,
			       x3, y3,COLOUR_BLACK);
			  
    // now draw the code
    unsigned long long value = EncodeTag(code);
    float projX0, projY0;
    float projX1, projY1;
    float projX2, projY2;
    float projX3, projY3;
    for(int i=0;i<m_length;i++) {
      int u0 = (int)(m_cells_corner[2*i]*(float)size);
      int v0 = (int)(m_cells_corner[2*i+1]*(float)size);
      int u1 = (int)((m_cells_corner[2*i]+m_cell_width)*(float)size);
      int v1 = (int)(m_cells_corner[2*i+1]*(float)size);
      int u2 = (int)((m_cells_corner[2*i]+m_cell_width)*(float)size);
      int v2 = (int)((m_cells_corner[2*i+1]+m_cell_width)*(float)size);
      int u3 = (int)(m_cells_corner[2*i]*(float)size);
      int v3 = (int)((m_cells_corner[2*i+1]+m_cell_width)*(float)size);
      int colour = (value & 1) == 1 ? COLOUR_BLACK : COLOUR_WHITE;
      //int colour = (float)i/(float)m_length * 255;
      image.DrawFilledQuadTangle(u0,v0,
				 u1,v1,
				 u2,v2,
				 u3,v3,
				 colour);
      value >>=1;
    }
  }
  virtual void DecodeNode(SceneGraphNode< ShapeChain<QuadTangle> >* node, const Camera& camera, const Image& image) {
#ifdef MATRIX_TAG_DEBUG
    PROGRESS("Decode node called");
#endif

    const QuadTangle quad = node->GetShapes().GetShape();

    if (!quad.IsFitted()) {
#ifdef MATRIX_TAG_DEBUG
      PROGRESS("Ignoring unfitted quadtangle");
#endif
      return;
    }
#ifdef MATRIX_TAG_DEBUG
    PROGRESS("QuadTangle: " << 
	     "("<<quad.GetX0()<<","<<quad.GetY0()<<"),"<<
	     "("<<quad.GetX1()<<","<<quad.GetY1()<<"),"<<
	     "("<<quad.GetX2()<<","<<quad.GetY2()<<"),"<<
	     "("<<quad.GetX3()<<","<<quad.GetY3()<<"),");	   
#endif

    // extract its pose
    float transform[16];
  
    GetTransform(quad,transform);

    float projX, projY;
    unsigned long long code = 0;
    // iterate over the tag reading each section
    for(int i=m_length-1;i>=0;i--) {
      float pts[] = { m_cells_corner[2*i]+m_cell_width_2,
		      m_cells_corner[2*i+1]+m_cell_width_2 };
      ApplyTransform(transform,pts[0],pts[1],pts,pts+1);
      camera.NPCFToImage(pts,1);
      bool sample = image.Sample(pts[0],pts[1]) > 128;
      code <<= 1;
      code |= sample ? 1 : 0;
    }

#ifdef IMAGE_DEBUG
    Image debug0(image);
    debug0.ConvertScale(-1,255);
    debug0.ConvertScale(0.5,128);
    for(int i=0;i<SIZE+2;i++) {
      float pts[] = { 0,
		      (float)i/(float)(SIZE+2),
		      1,
		      (float)i/(float)(SIZE+2),
		      (float)i/(float)(SIZE+2),
		      0,
		      (float)i/(float)(SIZE+2),
		      1 };
      ApplyTransform(transform,pts,4);
      camera.NPCFToImage(pts,4);
      debug0.DrawLine(pts[0],pts[1],pts[2],pts[3],COLOUR_BLACK,1);
      debug0.DrawLine(pts[4],pts[5],pts[6],pts[7],COLOUR_BLACK,1);
    }
  
    for(int i=m_length-1;i>=0;i--) {
      float pts[] = { m_cells_corner[2*i]+m_cell_width_2,
		      m_cells_corner[2*i+1]+m_cell_width_2 };
      ApplyTransform(transform,pts[0],pts[1],pts,pts+1);
      camera.NPCFToImage(pts,1);
      // pick the colour to be the opposite of the sampled point so we can see the dot
      int colour = image.Sample(pts[0],pts[1]) < 128 ? COLOUR_BLACK:COLOUR_WHITE; // our debug image is inverted 255 : 0;
      // or pick the colour to be on a gradient so we see the order it samples in
      //int colour = (int)((double)k/(double)m_sector_count*255);
      debug0.DrawPoint(pts[0],pts[1],colour,4);
    }

    debug0.Save("debug-decode.jpg");
#endif
    unsigned long long result = DecodeTag(code);
    LocatedObject* lobj = node->GetLocatedObject();
    for(int i=0;i<16;i++) {
      lobj->transform[i] = transform[i];
    }	
    node->SetInspected();
  }    
};

#endif//MATRIX_TAG_GUARD
