/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/23 12:05:47  acr31
 * moved Tag to CircularTag in preparation for Squaretag
 *
 * Revision 1.3  2004/01/21 11:55:08  acr31
 * added keywords for substitution
 *
 */
#include "CircularTag.hh"

CircularTag::CircularTag() {}

std::ostream& operator<<(std::ostream& s, const CircularTag& z) {
  return z.Print(s);
}

unsigned long CircularTag::GetCode() const {
  return m_code;
}

std::ostream& CircularTag::print(std::ostream& s) const {
  s << m_code;
  return s;
}
