#include "Tag.hh"

Tag::Tag() {}

std::ostream& operator<<(std::ostream& s, const Tag& z) {
  return z.Print(s);
}
