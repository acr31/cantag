#ifndef TEMPLATE_GUARD
#define TEMPLATE_GUARD

#include <Config.hh>
#include <Drawing.hh>
#include <QuadTangle2D.hh>

class Template {
private:
  float m_average;
  unsigned int m_size;
  unsigned int m_subsample;
  uchar* m_values;
  float m_sigma;
  Image* m_original;

public:
  char* m_filename;
  Template(char* filename, int size, int subsample);
  ~Template();
  float Correlate(Image* image, const QuadTangle2D* l) const;
  float Correlate(const Template* template) const;
  bool operator<(const Template& t);
  void Draw2D(Image* image,int white, int black);
private:
  void calculate_mean_sigma(unsigned char* values, float* stddev, float* mean) const;
  float Correlate(const unsigned char* values) const;
};


#endif//TEMPLATE_GUARD
