/**
 * $Header$
 */

#ifndef MATRIX_TAG_GUARD
#define MATRIX_TAG_GUARD

#include <total/Config.hh>
#include <total/Image.hh>
#include <total/Tag.hh>
#include <total/Coder.hh>
#include <total/QuadTangle.hh>
#include <total/QuadTangleTransform.hh>
#include <total/Camera.hh>
#include <total/ShapeChain.hh>
#include <total/findtransform.hh>
#include <total/ShapeTree.hh>

#include <bitset>

#ifdef TEXT_DEBUG
# define MATRIX_TAG_DEBUG
# define MATRIX_TAG_DEBUG_POINTS
#endif

#ifdef IMAGE_DEBUG
static int debug_matrix_image_counter = 0;
#endif

namespace Total {
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
   * \todo regression test with gl harness
   */ 
  template<int SIZE, class Quad>
  class MatrixTag : public virtual Tag< ShapeChain<Quad>, SIZE*SIZE - (SIZE*SIZE % 2) >, public virtual Coder<SIZE*SIZE - (SIZE*SIZE % 2)>, protected virtual QuadTangleTransform {
  private:
    float m_cell_width;
    float m_cell_width_2;
    float *m_cells_corner;

  public:
    MatrixTag();
  
    virtual ~MatrixTag();

    virtual void Draw2D(Image& image, CyclicBitSet<SIZE*SIZE - (SIZE*SIZE % 2)>& tag_data) const;

    virtual LocatedObject<SIZE*SIZE-(SIZE*SIZE % 2)>* GetTransform(typename ShapeTree< ShapeChain<Quad> >::Node* node, 
								   const Camera& camera, const Image& image) const;
    virtual bool DecodeNode(LocatedObject<SIZE*SIZE-(SIZE*SIZE % 2)>* lobj,
			    const Camera& camera, const Image& image) const;

    /**
     * \todo implement CheckTransform
     */
    virtual bool CheckTransform(const LocatedObject<SIZE*SIZE-(SIZE*SIZE % 2)>* lobj, typename ShapeTree<ShapeChain<Quad> >::Node* node) const { return true; }

    /**
     * \todo implement CheckDecode
     */
    virtual bool CheckDecode(const LocatedObject<SIZE*SIZE-(SIZE*SIZE % 2)>* lobj, const Camera& camera, const Image& image) const { return true; }

    virtual int PayloadRotation(float angle) const {
      int quad = (int)round(angle/2/PI * 4);
      return quad* (SIZE*SIZE-(SIZE*SIZE %2)) / 4;
    }

  };

  template<int SIZE,class Quad> MatrixTag<SIZE,Quad>::MatrixTag() :
    m_cell_width(2/(float)(SIZE+2)),
    m_cell_width_2(1/(float)(SIZE+2))
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
    
#ifdef MATRIX_TAG_DEBUG_POINTS
    for (int i=0;i<SIZE*SIZE - (SIZE*SIZE % 2);i++) {
      std::cout << m_cells_corner[2*i] << " " << m_cells_corner[2*i+1] << std::endl;
    }
#endif
  }

  template<int SIZE,class Quad> MatrixTag<SIZE,Quad>::~MatrixTag() {
    delete[] m_cells_corner;
  }

  template<int SIZE,class Quad> void MatrixTag<SIZE,Quad>::Draw2D(Image& image, CyclicBitSet<SIZE*SIZE - (SIZE*SIZE % 2)>& tag_data) const {
#ifdef MATRIX_TAG_DEBUG
    PROGRESS("Draw2D called");
#endif
    int x0 = 0;
    int y0 = 0;
    int x1 = image.GetWidth()-1;
    int y1 = 0;
    int x2 = image.GetWidth()-1;
    int y2 = image.GetHeight()-1;
    int x3 = 0;
    int y3 = image.GetHeight()-1;

    int size = (image.GetWidth() < image.GetHeight() ? image.GetWidth()-1 : image.GetHeight()-1)/2;

    image.DrawFilledQuadTangle(x0, y0,
			       x1, y1,
			       x2, y2,
			       x3, y3,COLOUR_BLACK);

    // now draw the code
    EncodePayload(tag_data);
    for(int i=0;i<SIZE*SIZE - (SIZE*SIZE % 2);i++) {
      int u0 = (int)((m_cells_corner[2*i]+1)*(float)size);
      int v0 = (int)((m_cells_corner[2*i+1]+1)*(float)size);

      int u1 = (int)((m_cells_corner[2*i]+m_cell_width+1)*(float)size);
      int v1 = (int)((m_cells_corner[2*i+1]+1)*(float)size);

      int u2 = (int)((m_cells_corner[2*i]+m_cell_width+1)*(float)size);
      int v2 = (int)((m_cells_corner[2*i+1]+m_cell_width+1)*(float)size);

      int u3 = (int)((m_cells_corner[2*i]+1)*(float)size);
      int v3 = (int)((m_cells_corner[2*i+1]+m_cell_width+1)*(float)size);

      int colour = tag_data[i] ? COLOUR_BLACK : COLOUR_WHITE;
      //int colour = (int)((float)i/(float)(SIZE*SIZE - (SIZE*SIZE % 2)) * 128)+128;
      image.DrawFilledQuadTangle(u0,v0,
				 u1,v1,
				 u2,v2,
				 u3,v3,
				 colour);
    }
    /* draw a false ordering for pretty picture
       int counter = 0;
       for(int i=0;i<SIZE;i++) {
       for(int j=0;j<SIZE;j++) {
       int u0 = (int)((float)(i+1)/(SIZE+2)*size);
       int v0 = (int)((float)(j+1)/(SIZE+2)*size);
       int u1 = (int)((float)(i+2)/(SIZE+2)*size);
       int v1 = (int)((float)(j+1)/(SIZE+2)*size);
       int u2 = (int)((float)(i+2)/(SIZE+2)*size);
       int v2 = (int)((float)(j+2)/(SIZE+2)*size);
       int u3 = (int)((float)(i+1)/(SIZE+2)*size);
       int v3 = (int)((float)(j+2)/(SIZE+2)*size);
       int colour = (int)((float)counter/SIZE/SIZE * 128)+128;
       counter++;
       image.DrawFilledQuadTangle(u0,v0,
       u1,v1,
       u2,v2,
       u3,v3,
       colour);
       }
       }
    */
  }


  template<int SIZE,class Quad> LocatedObject<SIZE*SIZE-(SIZE*SIZE%2)>* MatrixTag<SIZE,Quad>::GetTransform(typename ShapeTree< ShapeChain<Quad> >::Node* node, const Camera& camera, const Image& image) const {
#ifdef MATRIX_TAG_DEBUG
    PROGRESS("Decode node called");
#endif

    const Quad quad = node->matched.GetShape();

    if (!quad.IsFitted()) return false;

#ifdef MATRIX_TAG_DEBUG
    PROGRESS("QuadTangle: " << 
	     "("<<quad.GetX0()<<","<<quad.GetY0()<<"),"<<
	     "("<<quad.GetX1()<<","<<quad.GetY1()<<"),"<<
	     "("<<quad.GetX2()<<","<<quad.GetY2()<<"),"<<
	     "("<<quad.GetX3()<<","<<quad.GetY3()<<"),");	   
#endif

    // extract its pose
    float transform[16];
  
    TransformQuadTangle(quad,transform);

    LocatedObject<SIZE*SIZE - (SIZE*SIZE % 2)>* lobj = new LocatedObject<SIZE*SIZE - (SIZE*SIZE % 2)>();
    lobj->LoadTransform(transform,1,camera);
  
    return lobj;
  }

  template<int SIZE,class Quad> bool MatrixTag<SIZE,Quad>::DecodeNode( LocatedObject<SIZE*SIZE-(SIZE*SIZE%2)>* lobj, const Camera& camera, const Image& image) const {
  
    float* transform= lobj->transform;
    CyclicBitSet<SIZE*SIZE - (SIZE*SIZE % 2)>* read_code = new CyclicBitSet<SIZE*SIZE - (SIZE*SIZE % 2)>();
    // iterate over the tag reading each section
    for(int i=0;i<SIZE*SIZE - (SIZE*SIZE % 2);i++) {
      float pts[] = { m_cells_corner[2*i]+m_cell_width_2,
		      m_cells_corner[2*i+1]+m_cell_width_2 };
      ApplyTransform(transform,pts[0],pts[1],pts,pts+1);
      camera.NPCFToImage(pts,1);
      bool sample = (image.Sample(pts[0],pts[1]) & 0x1) != 0;
      (*read_code)[i] = sample;
    }

#ifdef IMAGE_DEBUG
    Image debug0(image);
    debug0.Mask(1);
    debug0.ConvertScale(50,0);
    debug0.ConvertScale(-1,255);
    for(int i=-SIZE-2;i<=SIZE+2;i+=2) {
      float pts[] = { -1,
		      (float)i/(float)(SIZE+2),
		      1,
		      (float)i/(float)(SIZE+2),
		      (float)i/(float)(SIZE+2),
		      -1,
		      (float)i/(float)(SIZE+2),
		      1 };
      ApplyTransform(transform,pts,4);
      camera.NPCFToImage(pts,4);
      debug0.DrawLine(pts[0],pts[1],pts[2],pts[3],COLOUR_BLACK,1);
      debug0.DrawLine(pts[4],pts[5],pts[6],pts[7],COLOUR_BLACK,1);
    }
  
    for(int i=0;i<SIZE*SIZE - (SIZE*SIZE % 2);i++) {
      float pts[] = { m_cells_corner[2*i]+m_cell_width_2,
		      m_cells_corner[2*i+1]+m_cell_width_2 };
      ApplyTransform(transform,pts[0],pts[1],pts,pts+1);
      camera.NPCFToImage(pts,1);
      // pick the colour to be the opposite of the sampled point so we can see the dot
      int colour = image.Sample(pts[0],pts[1]) & 1 ? COLOUR_BLACK:COLOUR_WHITE; // our debug image is inverted 255 : 0;
      // or pick the colour to be on a gradient so we see the order it samples in
      //int colour = (int)((double)i/(double)(SIZE*SIZE - (SIZE*SIZE % 2))*255);
      debug0.DrawPoint(pts[0],pts[1],colour,4);
    }

    char filename[256];
    snprintf(filename,255,"debug-decode-%0.5d.pnm",debug_matrix_image_counter++);
    filename[255]=0;
    debug0.Save(filename);
#endif

    int rotation = DecodePayload(*read_code);
    const int cells_per_quadrant = (SIZE*SIZE - (SIZE*SIZE % 2))/4;
    if ((rotation >= 0)  && (rotation % cells_per_quadrant == 0)) {
#ifdef MATRIX_TAG_DEBUG
      PROGRESS("Found code " << *read_code << " rotation is " << rotation);
#endif	

      // find out the angle of rotation by looking at the number of bits
      // the code is rotated by divide this by the number of cells in
      // each quarter i.e. if we have a 6x6 square then there are 9
      // cells per quadrant

      int angle = rotation / cells_per_quadrant;
      int cosa[] = {1,0,-1,0};
      int sina[] = {0,1,0,-1};

      int rotation[16] = { cosa[angle], -sina[angle],0,0,
			   sina[angle], cosa[angle],0,0,
			   0,0,1,0,
			   0,0,0,1};
      
      float finaltrans[16] = {0};
      // premultiply rotations by transforms
      for(int row=0;row<4;row++) {
	for(int col=0;col<4;col++) {
	  for(int k=0;k<4;k++) {
	    finaltrans[row*4+col] += transform[row*4+k] * rotation[k*4+col];
	  }
	}
      }

      lobj->LoadTransform(finaltrans,1,camera);
      lobj->tag_codes.push_back(read_code);
      return true;
    }    
    else {
      delete read_code;
      return false;
    }
  }

}
#endif//MATRIX_TAG_GUARD
