/**
 * $Header$
 */

#ifndef MONOCHROME_IMAGE_GUARD
#define MONOCHROME_IMAGE_GUARD

#include <cassert>
#include <total/Entity.hh>
namespace Total {
  /**
   * \todo pack the data into ints
   */
  class MonochromeImage : public Entity {
  public:
    enum {
      LEFT_PIXEL = 0x1,
      CENTRE_PIXEL = 0x2,
      RIGHT_PIXEL = 0x4
    };
  private:
    unsigned int m_width;
    unsigned int m_height;
    bool* m_data;    

  public:
    MonochromeImage(unsigned int width, unsigned int height);
    ~MonochromeImage();
    void Save(const char* filename) const;

    inline bool GetPixel(unsigned int x, unsigned int y) const {
      assert(x<m_width);
      assert(y<m_height);
      return m_data[x+m_width*y];
    }

    inline bool GetPixel(int x, int y) const {
      return GetPixel((unsigned int)x,(unsigned int)y);
    }

    inline bool GetPixel(float x, float y) const {
      return GetPixel((unsigned int)x,(unsigned int)y);
    }
    
    inline void SetPixel(unsigned int x, unsigned int y, bool value) {
      m_data[x+m_width*y] = value;
    }

    inline int GetWidth() const { return m_width; }

    inline int GetHeight() const { return m_height; }

    inline int GetPixel3(unsigned int x,unsigned int y) const{
      return GetPixel(x-1,y) | (GetPixel(x,y) << 1) | (GetPixel(x+1,y)<< 2);
    }
  };
}
#endif//IMAGE_GUARD
