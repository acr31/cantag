#ifndef GAUSSIAN_ELIMINATION_GUARD
#define GAUSSIAN_ELIMINATION_GUARD


void solve_simultaneous(float* X, float** A, float* R, int size);
void invert_matrix(float** A, float **R, int size);
void predivide(float** A, float **B, int size, int cols);
#endif//GAUSSIAN_ELIMINATION_GUARD
