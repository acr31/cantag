/**
 * $Header$
 *
 * $Log$
 * Revision 1.1  2004/01/25 14:53:34  acr31
 * moved over to autoconf/automake build system
 *
 * Revision 1.3  2004/01/23 11:49:53  acr31
 * Finished integrating the GF4 coder - I've ripped out the error correcting stuff for now and its just looking for a match but it doesn't seem to work.  I need to spend some time to understand the division and mod operations in the GF4Poly to progress
 *
 * Revision 1.2  2004/01/21 11:55:07  acr31
 * added keywords for substitution
 *
 */
#include <Coder.hh>

Coder::ValueTooLarge::ValueTooLarge() {}
std::ostream& Coder::ValueTooLarge::Print(std::ostream& s) const {
  s << "Value is too large for this coding scheme";
  return s;
}


Coder::InvalidCheckSum::InvalidCheckSum() {}
std::ostream& Coder::InvalidCheckSum::Print(std::ostream& s) const {
  s << "Invalid checksum";
  return s;
}

Coder::InvalidSymbol::InvalidSymbol() {}
std::ostream& Coder::InvalidSymbol::Print(std::ostream& s) const {
  s << "Invalid symbol";
  return s;
}

Coder::InvalidCode::InvalidCode() {}
std::ostream& Coder::InvalidCode::Print(std::ostream& s) const {
  s << "Invalid code";
  return s;
}
