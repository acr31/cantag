#ifndef GAUSSIAN_ELIMINATION_GUARD
#define GAUSSIAN_ELIMINATION_GUARD

void solve_simultaneous(double* X, double** A, double* R, int size);
void invert_matrix(double** A, double **R, int size);
void predivide(double** A, double **B, int size, int cols);
#endif//GAUSSIAN_ELIMINATION_GUARD
