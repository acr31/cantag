/**
 * 
 * $Header$
 *
 * $Log$
 * Revision 1.5  2004/02/16 08:02:03  acr31
 * *** empty log message ***
 *
 * Revision 1.4  2004/02/06 21:11:44  acr31
 * adding ellipse fitting
 *
 * Revision 1.3  2004/02/03 16:25:11  acr31
 * more work on template tags and some function signature changes
 *
 * Revision 1.2  2004/02/03 07:48:31  acr31
 * added template tag
 *
 */
#include <Config.hh>
#include <TemplateTag.hh>

#include <sys/types.h>
#include <dirent.h>
#include <cstdio>

TemplateTag::TemplateTag(char* tagdirectory, int size, int subsample) : m_templates() {

  // build the list of templates 
  DIR *dir = opendir(tagdirectory);
  if (dir) {
    dirent* ptr;
    while(ptr = readdir(dir)) {
      if (strncmp(ptr->d_name,".",1)) {
	char filename[255];
	snprintf(filename,255,"%s/%s",tagdirectory,ptr->d_name);
	m_templates.push_back(new Template(filename,size,subsample));
      }
    }    
  }
  std::sort(m_templates.begin(),m_templates.end());
};

TemplateTag::~TemplateTag() {
  
  for(std::vector<Template*>::const_iterator i=m_templates.begin();i!=m_templates.end();i++) {
    delete *i;
  }

};

void TemplateTag::Draw2D(Image* image, unsigned long long code, int black, int white) {
  if (code > m_templates.size()) {
    code =0;
  }

  DrawFilledQuadTangle(image,
		       0,0,
		       image->width,0,
		       image->width,image->height,
		       0,image->height,
		       black);
  (m_templates.at(code))->Draw2D(image,black,white);
}

unsigned long long TemplateTag::Decode(Image* image, Camera* camera, const QuadTangle2D *l) {
  float best_val;
  int best_index = -1;
  int index =0;
  
  for(std::vector<Template*>::const_iterator i=m_templates.begin();i!=m_templates.end();i++) {
    float val = (*i)->Correlate(image,l);
    if ((best_index ==-1) ||
	(best_val < val)) {
      best_index = index;
      best_val = val;
    }
    PROGRESS("Template "<<index<<" ("<<(*i)->m_filename<<") has correlation value "<<val);
    index++;
  }
  PROGRESS("Best choice was template "<<best_index);
  return best_index;
}
