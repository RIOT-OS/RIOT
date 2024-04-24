#define ROWS_A 2
#define COLS_A 3
#define ROWS_B 3
#define COLS_B 2

void matrix_multiply(int A[ROWS_A][COLS_A], int B[ROWS_B][COLS_B], int result[ROWS_A][COLS_B]);

void print_matrix(int rows, int cols, int matrix[rows][cols]);
