/*
 * Copyright (C) 2023 Lukas R. Jackson, Vanguard Defense International
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup     Matrix multiplication
 * @brief        Definitions for matrix multiplication functions
 * @{
 * @file         matrix.c
 * @author       Lukas R. Jackson <LukasJacksonEG@gmail.com>
 *
 */

#include "include/matrix.h"
#include <stdio.h>

/**
 * @brief Multiply two matrices.
 * This function multiplies two matrices A and B and stores the result in the
 * provided result matrix.
 * @param A The first matrix of size ROWS_A x COLS_A.
 * @param B The second matrix of size ROWS_B x COLS_B.
 * @param result The result matrix of size ROWS_A x COLS_B.
 */
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

/**
 * @brief Print a matrix.
 * This function prints the elements of the provided matrix to the standard output.
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * @param matrix The matrix to be printed.
 */
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

