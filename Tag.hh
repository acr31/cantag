#ifndef TAG_GUARD
#define TAG_GUARD

#include <cv.h>
#include <ostream>

class Tag {
public:
  Tag();
  virtual void Show(IplImage *image) =0;
  virtual std::ostream& Print(std::ostream& s) const=0;
};

std::ostream& operator<<(std::ostream& s, const Tag& z);

#endif//TAG_GUARD
