std::ostream& operator<<(std::ostream& s, const Exception& z) {
  return z.Print(s);
}
