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
class MatrixTag : public virtual Tag< ShapeChain<QuadTangle> >, protected virtual Coder {
private:
  int m_length;
  int m_size;
  float m_cell_width;
  float m_cell_width_2;
  float *m_cells_corner;

public:
  MatrixTag(int size);
  virtual ~MatrixTag();
  virtual void Draw2D(Image& image, unsigned long long code);
  virtual void DecodeNode(SceneGraphNode< ShapeChain<QuadTangle> >* node, const Camera& camera, const Image& image);
};
#endif//MATRIX_TAG_GUARD
