#ifndef GAUSSIAN_MATRIX
#define GAUSSIAN_MATRIX

#include <Config.hh>

class GaussianMatrix {
private:
  float** m_values;
  int m_size;

public:
  GaussianMatrix(float** values, int size);
  ~GaussianMatrix();
  int FindBestCoefficient(int row, int startcolumn);
  void SwapColumns(int c1, int c2);
  void ScaleRow(int row, float factor, int startcolumn);
  void SubtractRow(int fromrow, int row, float lambda, int startcolumn);
  void Print();
};

#endif//GAUSSIAN_MATRIX
