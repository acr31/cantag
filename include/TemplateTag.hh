/**
 * $Header$
 */

#ifndef TEMPLATE_TAG_GUARD
#define TEMPLATE_TAG_GUARD

#include <Config.hh>
#include <Drawing.hh>
#include <Tag.hh>
#include <QuadTangle2D.hh>
#include <Template.hh>
#include <Coder.hh>
#include <vector>
#include <Camera.hh>

/**
 * An implementation of Template based tags found in ARToolKit
 *
 * \todo Needs updating and testing
 */
class TemplateTag : public virtual Tag< ShapeChain<QuadTangle>,16> {
private:
  std::vector<Template*> m_templates;

public:
  TemplateTag(char* tagdirectory, int size, int subsample);
  virtual ~TemplateTag();
  virtual void Draw2D(Image& image, const std::bitset<16>& tag_data) const;
  virtual bool DecodeNode(SceneGraphNode< ShapeChain<QuadTangle> >* node, const Camera& camera, const Image& image);
};

#endif//TEMPLATE_TAG_GUARD
