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
      char filename[255];
      snprintf(filename,255,"%s/%s",tagdirectory,ptr->d_name);
      m_templates.push_back(new Template(filename,size,subsample));
      free(ptr);
    }    
  }
  std::sort(m_templates.begin(),m_templates.end());
};

TemplateTag::~TemplateTag() {
  
  for(std::vector<Template*>::const_iterator i=m_templates.begin();i!=m_templates.end();i++) {
    delete *i;
  }

};

void TemplateTag::Draw2D(Image* image, const QuadTangle2D *l, unsigned long long code, int black, int white) {

}

unsigned long long TemplateTag::Decode(Image* image, const QuadTangle2D *l) {
  return 0;
}
