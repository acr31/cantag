/**
 * An implementation of Template based tags found in ARToolKit
 *
 * $Header$
 *
 * $Log$
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

#undef FILENAME
#define FILENAME "TemplateTag.hh"

class TemplateTag : public virtual Tag<QuadTangle2D>, protected virtual Coder {
private:
  std::vector<Template*> m_templates;

public:
  TemplateTag(char* tagdirectory, int size, int subsample);
  virtual ~TemplateTag();
  virtual void Draw2D(Image* image, const QuadTangle2D *l, unsigned long long code, int black, int white);
  virtual unsigned long long Decode(Image *image, const QuadTangle2D *l);
};

#endif//TEMPLATE_TAG_GUARD
