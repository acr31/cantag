#include "Coder.hh"

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
