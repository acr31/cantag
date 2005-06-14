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
  inline void SetContourRestrictions(int minlength, int minwidth, int minheight) { 
    m_minContourLength = minlength; 
    m_minContourWidth = minwidth;
    m_minContourHeight = minheight;
  }
  
  inline int GetMinContourLength() { return m_minContourLength; }
  inline int GetMinContourWidth() { return m_minContourWidth; }
  inline int GetMinContourHeight() { return m_minContourHeight; }

  inline bool CheckDimensions(int length, int width, int height) const {
    return length > m_minContourLength && width > m_minContourWidth && height > m_minContourHeight;
  }
};

#endif//CONTOUR_RESTRICTIONS_GUARD
