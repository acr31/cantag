/**
 * An implementation of Template based tags found in ARToolKit
 *
 * $Header$
 *
 * $Log$
 * Revision 1.4  2004/02/16 08:02:03  acr31
 * *** empty log message ***
 *
 * Revision 1.3  2004/02/03 16:24:56  acr31
 * various function signature changes and use of __FILE__ and __LINE__ in debug macros
 *
 * Revision 1.2  2004/02/03 07:48:25  acr31
 * added template tag
 *
 * Revision 1.1  2004/02/01 21:29:53  acr31
 * added template tags initial implementation
 *
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

class TemplateTag : public virtual Tag<QuadTangle2D> {
private:
  std::vector<Template*> m_templates;

public:
  TemplateTag(char* tagdirectory, int size, int subsample);
  virtual ~TemplateTag();
  virtual void Draw2D(Image* image, unsigned long long code, int black, int white);
  virtual unsigned long long Decode(Image *image, Camera* camera, const QuadTangle2D *l);
};

#endif//TEMPLATE_TAG_GUARD
