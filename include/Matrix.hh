#ifndef MATRIX_GUARD
#define MATRIX_GUARD

#define CHECKS

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

  
  
};

#endif//MATRIX_GUARD
