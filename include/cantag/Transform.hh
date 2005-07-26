/**
 * $Header$
 */

#include <cantag/Config.hh>

namespace Cantag {
  
  /**
   * A class to represent a transformation from camera co-ordinates to
   * object co-ordinates
   */
  class Transform {
  private:
    float m_transform[16];
    float m_confidence;
    
  public:
    
    Transform(float confidence);
    Transform(float* transform, float confidence);
    
    inline float& operator[](int index) { return m_transform[index]; }
    inline float operator[](int index) const { return m_transform[index]; }
    
    inline float getConfidence() { return m_confidence;} 
    inline void setConfidence(float confidence) { m_confidence = confidence; }
    
    void Apply(float x, float y, float* resx, float* resy) const;
    void Apply(float x, float y, float z,float* resx, float* resy) const;
    void Apply(float* points, int num_points) const;
    
    /**
     * Rotate the tag's object co-ordinates by a specific angle. Takes
     * the sine and cos of this angle as arguments.
     */
    void Rotate(float cos,float sin);
  };
}
