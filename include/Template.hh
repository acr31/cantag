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
  float m_stddev;
  char* m_filename;

public:
  Template(char* filename, int size, int subsample);
  ~Template();
  float Correlate(Image* image, QuadTangle2D* l);
  bool operator<(const Template& t);
private:
  void calculate_mean_stddev(unsigned char* values, float* stddev, float* mean);
};


#endif//TEMPLATE_GUARD
