#ifndef MATRIX_GUARD
#define MATRIX_GUARD

#include <opencv/cv.h>

template<class T,int ROWS,int COLS>
class Matrix {
private:
  T* m_data;

public:
  Matrix() {
    m_data = new T[ROWS*COLS];
  };
  ~Matrix() {
    delete[] m_data;
  };
  
  T* operator[](unsigned int row) const {
    return m_data+row*COLS;
  }
  
  Matrix<T,ROWS,COLS> operator+(const Matrix<T,ROWS,COLS>& m) const {
    Matrix<T,ROWS,COLS> r;
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	r.m_data[i*COLS+j] = m_data[i*COLS+j]+m.m_data[i*COLS+j];
      }
    }
    return r;
  }
  
  Matrix<T,ROWS,COLS>& operator+=(const Matrix<T,ROWS,COLS>& m) {
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	m_data[i*COLS+j] += m.m_data[i*COLS+j];
      }
    }
    return *this;
  }

  Matrix<T,ROWS,COLS> operator+(const T& m) const {
    Matrix<T,ROWS,COLS> r;
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	r.m_data[i*COLS+j] = m_data[i*COLS+j]+m;
      }
    }
    return r;
  }
  
  Matrix<T,ROWS,COLS>& operator+=(const T& m) {
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	m_data[i*COLS+j] += m;
      }
    }
    return *this;
  }

  Matrix<T,ROWS,COLS> operator-(const Matrix<T,ROWS,COLS>& m) const {
    Matrix<T,ROWS,COLS> r;
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	r.m_data[i*COLS+j] = m_data[i*COLS+j]-m.m_data[i*COLS+j];
      }
    }
    return r;
  }
  
  Matrix<T,ROWS,COLS>& operator-=(const Matrix<T,ROWS,COLS>& m) {
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	m_data[i*COLS+j] -= m.m_data[i*COLS+j];
      }
    }
    return *this;
  }

  Matrix<T,ROWS,COLS> operator-(const T& m) const {
    Matrix<T,ROWS,COLS> r;
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	r.m_data[i*COLS+j] = m_data[i*COLS+j]-m;
      }
    }
    return r;
  }
  
  Matrix<T,ROWS,COLS>& operator-=(const T& m) {
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	r.m_data[i*COLS+j] -= m;
      }
    }
    return *this;
  }

  template<int MCOLS> Matrix<T,ROWS,MCOLS> operator*(const Matrix<T,COLS,MCOLS>& m) const {
    Matrix<T,ROWS,MCOLS> r;
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<MCOLS;j++) {
	r.m_data[i*MCOLS+j]= m_data[i*COLS] * m.m_data[j];  // unroll one iteration to initialise r
	for(int k=1;k<COLS;k++) {
	  r.m_data[i*MCOLS+j] += m_data[i*COLS+k] * m.m_data[k*MCOLS+j];
	}
      }
    }
    return r;
  }

  Matrix<T,ROWS,COLS> operator*(const T& m) const {
    Matrix<T,ROWS,COLS> r;
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	r.m_data[i*COLS+j] = m_data[i*COLS+j]*m;
      }
    }
    return r;
  }

  Matrix<T,ROWS,COLS>& operator*=(const T& m)  {
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	m_data[i*COLS+j]*=m;
      }
    }
    return *this;
  }

  T det() const {
    return m_data[0] * m_data[3] - m_data[1] * m_data[2];
  }

  Matrix<T,COLS,ROWS> inv() const {
    // this is not a valid thing to do
  }

  Matrix<T,COLS,ROWS> transp() const {
    Matrix<T,COLS,ROWS> r;
    for(int i=0;i<ROWS;i++) {
      for(int j=0;j<COLS;j++) {
	r.m_data[i*COLS+j] = m_data[j*COLS+i];
      }
    }   
  }

  void eigenVV(Matrix<T,ROWS,COLS>* vecs, Matrix<T,ROWS,1>* vals) const {
    // this is not a valid thing to do
  }
};

template<> void Matrix<float,ROWS,ROWS>::eigenVV(Matrix<float,ROWS,ROWS>* vecs, Matrix<float,ROWS,ROWS>* vals) const {
  float v[ROWS*ROWS];
  for(int i=0;i<ROWS*ROWS;i++) {
    v.m_data[i] = m_data[i];
  }
  CvMat m1;
  cvInitMatHeader(&m1,ROWS,ROWS,CV_32F,v);
  
  CvMat m2;
  float v2[ROWS*ROWS];
  cvInitMatHeader(&m2,ROWS,ROWS,CV_32F,v2);
  
  CvMat m3;
  float v3[ROWS];
  cvInitMatHeader(&m3,ROWS,1,CV_32F,v3);
  
  cvEigenVV(&m1,&m2,&m3,1E-20);
  
  // swap things around 
  // opencv returns its eigenvectors as rows and values largest first
  // we prefer vectors as columns with smallest value first
  
  vecs.m_data[0] = v2[6]; vecs.m_data[1] = v2[3]; vecs.m_data[2] = v2[0];
  vecs.m_data[3] = v2[7]; vecs.m_data[4] = v2[4]; vecs.m_data[5] = v2[1];
  vecs.m_data[6] = v2[8]; vecs.m_data[7] = v2[5]; vecs.m_data[8] = v2[2];

  vals.m_data[0] = v3[2]; vals.m_data[1] = 0;     vals.m_data[2] = 0;
  vals.m_data[3] = 0;     vals.m_data[4] = v3[1]; vals.m_data[5] = 0;
  vals.m_data[6] = 0;     vals.m_data[7] = 0;     vals.m_data[8] = v3[0]; 

}

template<> Matrix<T,2,2>  Matrix<T,2,2>::inv() const {
  Matrix<T,2,2> r;
  T determinant = det();
  r.m_data[0] = m_data[3] / determinant;
  r.m_data[1] = -m_data[2] / determinant;
  r.m_data[2] = -m_data[1] / determinant;
  r.m_data[3] = m_data[1] / determinant;
}

template<> Matrix<float,COLS,ROWS> Matrix<float,ROWS,COLS>::inv() const {
  CvMat m1;
  cvInitMatHeader(&m1,ROWS,COLS,CV_32F,m1);
  
  Matrix<float,COLS,ROWS> r;
  CvMat m2;
  cvInitMatHeader(&m2,COLS,ROWS,CV_32F,r.m_data);

  cvInvert(&m1,&m2,CV_LU);
  return r;
}

template<> Matrix<double,COLS,ROWS> Matrix<double,ROWS,COLS>::inv() const {
  CvMat m1;
  cvInitMatHeader(&m1,ROWS,COLS,CV_64F,m1);
  
  Matrix<double,COLS,ROWS> r;
  CvMat m2;
  cvInitMatHeader(&m2,COLS,ROWS,CV_64F,r.m_data);

  cvInvert(&m1,&m2,CV_LU);
  return r;
}



#endif//MATRIX_GUARD
