/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/01/21 11:55:07  acr31
 * added keywords for substitution
 *
 */
std::ostream& operator<<(std::ostream& s, const Exception& z) {
  return z.Print(s);
}
