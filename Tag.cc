#include "Tag.hh"

Tag::Tag() {}

std::ostream& operator<<(std::ostream& s, const Tag& z) {
  return z.Print(s);
}

unsigned long Tag::GetCode() const {
  return m_code;
}

std::ostream& Tag::print(std::ostream& s) const {
  s << m_code;
  return s;
}
