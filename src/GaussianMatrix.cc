#include <Config.hh>
#include <GaussianMatrix.hh>

#undef FILENAME
#define FILENAME "GaussianMatrix.cc"

GaussianMatrix::GaussianMatrix(float** values,int size) : m_values(values), m_size(size) {}

GaussianMatrix::~GaussianMatrix() {}

/*
 * Find the largest magnitude co-efficient in this row (start
 * searching from the given column).  If we have a 1 then use that
 * though.
 */
int GaussianMatrix::FindBestCoefficient(int row, int startcolumn) {
  float max = fabs(m_values[startcolumn][row]);
  int maxcol = startcolumn;
  if (max == 1) { 
    return startcolumn;
  }
  for(int i=startcolumn+1;i<m_size;i++) {
    if (m_values[i][row] == 1) {
      return i;
    }
    
    if (fabs(m_values[i][row]) > max) {
      max = fabs(m_values[i][row]);
      maxcol = i;
    }    
  }

  if (max==0) {
    // THROW SINGULAR MATRIX
  }
  return maxcol;
}

void GaussianMatrix::SwapColumns(int c1, int c2) {
  float* p = m_values[c1];
  m_values[c1] = m_values[c2];
  m_values[c2] = p;
}

void GaussianMatrix::ScaleRow(int row, float factor, int startcolumn) {
  for(int i=startcolumn;i<m_size;i++) {
    m_values[i][row] *= factor;
  }
}

/**
 * [row] -= lambda*[fromrow]
 */
void GaussianMatrix::SubtractRow(int fromrow, int row, float lambda, int startcolumn) {
  for(int i=startcolumn;i<m_size;i++) {
    m_values[i][row] -= lambda*m_values[i][fromrow];
  }
}

void GaussianMatrix::Print() {
  std::cout << std::endl << "[";

  for(int i=0;i<m_size;i++) {
    for(int j=0;j<m_size;j++) {
      std::cout << m_values[j][i] << "\t";
    }
    std::cout << ";" << std::endl;
  }
  std::cout << std::endl;
  
}
