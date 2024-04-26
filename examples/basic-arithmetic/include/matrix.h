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
 * @defgroup    matrix multiplication 
 * @brief       Function declarations for matrix multiplication.
 * @{
 */

/**
 * @file        matrix.h
 * @author      Lukas R. Jackson <LukasJacksonEG@gmail.com>
 */

#define ROWS_A 2
#define COLS_A 3
#define ROWS_B 3
#define COLS_B 2

void matrix_multiply(int A[ROWS_A][COLS_A], int B[ROWS_B][COLS_B], int result[ROWS_A][COLS_B]);

void print_matrix(int rows, int cols, int matrix[rows][cols]);

/** @} */

#endif /* INCLUDE_MATRIX_H */
