/*
 * Copyright (C) 2023 Lukas R. Jackson, Vanguard Defense International
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#ifndef INCLUDE_MATRIX_H
#define INCLUDE_MATRIX_H

/**
 * @defgroup  matrix multiplication 
 * @brief  Function declarations for matrix multiplication.
 * @{
 */
/**
 * @file    matrix.h
 * @author  Lukas R. Jackson <LukasJacksonEG@gmail.com>
 */

/**
 * @brief Number of rows in matrix A.
 */
#define ROWS_A 2

/**
 * @brief Number of columns in matrix A.
 */
#define COLS_A 3

/**
 * @brief Number of rows in matrix B.
 */
#define ROWS_B 3

/**
 * @brief Number of columns in matrix B.
 */
#define COLS_B 2

/**
 * @brief Multiply two matrices.
 * This function multiplies two matrices A and B and stores the result in the
 * provided result matrix.
 * @param A The first matrix of size ROWS_A x COLS_A.
 * @param B The second matrix of size ROWS_B x COLS_B.
 * @param result The result matrix of size ROWS_A x COLS_B.
 */
void matrix_multiply(int A[ROWS_A][COLS_A], int B[ROWS_B][COLS_B], int result[ROWS_A][COLS_B]);

/**
 * @brief Print a matrix.
 * This function prints the elements of the provided matrix to the standard output.
 * @param rows The number of rows in the matrix.
 * @param cols The number of columns in the matrix.
 * @param matrix The matrix to be printed.
 */
void print_matrix(int rows, int cols, int matrix[rows][cols]);

/** @} */

#endif /* INCLUDE_MATRIX_H */
