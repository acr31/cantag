/**
 * $Header$
 *
 */

#ifndef RING_TAG_GUARD
#define RING_TAG_GUARD

#include "Config.hh"
#include "Drawing.hh"
#include <Tag.hh>
#include "Coder.hh"
#include <Camera.hh>
#include <ShapeChain.hh>
#include <Ellipse.hh>

class RingTag : public virtual Tag< ShapeChain<Ellipse> >, protected virtual Coder {
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
  virtual void DecodeNode(SceneGraphNode< ShapeChain<Ellipse> >* node, const Camera& camera, const Image& image);
  //  virtual unsigned long long Decode(Image *image, Camera* camera, const Ellipse2D *l);

private:
  void draw_circle(Image* image, const Camera& camera, float l[16], double radius);
  void draw_read(const Image& image, const Camera& camera, float l[16], int i);
};
#endif//RING_TAG_GUARD
