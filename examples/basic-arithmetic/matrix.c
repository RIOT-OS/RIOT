#include "include/matrix.h"
#include <stdio.h>

void matrix_multiply(int A[ROWS_A][COLS_A], int B[ROWS_B][COLS_B], int result[ROWS_A][COLS_B])
{
    for (int i = 0; i < ROWS_A; ++i)
    {
        for (int j = 0; j < COLS_B; ++j)
        {
            result[i][j] = 0;
            for (int k = 0; k < COLS_A; ++k)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void print_matrix(int rows, int cols, int matrix[rows][cols])
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
