/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:53:35  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.3  2004/01/23 22:36:12  acr31
 * Added missing include for Exception.hh
 *
 * Revision 1.2  2004/01/21 11:55:07  acr31
 * added keywords for substitution
 *
 */
#include <Exception.hh>

std::ostream& operator<<(std::ostream& s, const Exception& z) {
  return z.Print(s);
}
