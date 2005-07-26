/**
 * $Header$
 */

#ifndef CONTOUR_RESTRICTIONS_GUARD
#define CONTOUR_RESTRICTIONS_GUARD

class ContourRestrictions {
private:
  int m_minContourLength;
  int m_minContourWidth;
  int m_minContourHeight;    

public:

  ContourRestrictions(int minlength, int minwidth, int minheight) : m_minContourLength(minlength),m_minContourWidth(minwidth),m_minContourHeight(minheight) {}
  
  inline int GetMinContourLength() const { return m_minContourLength; }
  inline int GetMinContourWidth() const { return m_minContourWidth; }
  inline int GetMinContourHeight() const { return m_minContourHeight; }

  inline bool CheckDimensions(int length, int width, int height) const {
    return length > m_minContourLength && width > m_minContourWidth && height > m_minContourHeight;
  }
};

#endif//CONTOUR_RESTRICTIONS_GUARD
