/*
 * Copyright (C) 2024 Lukas R. Jackson, Vanguard Defense International
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Basic arithmetic operations
 *
 * @author      Lukas R. Jackson <LukasJacksonEG@gmail.com>
 *
 * @}
 */

#include "include/matrix.h"
#include <stdio.h>

int main(void)
{
    int A[ROWS_A][COLS_A] = {{1, 2, 3}, {4, 5, 6}};
    int B[ROWS_B][COLS_B] = {{7, 8}, {9, 10}, {11, 12}};
    int result[ROWS_A][COLS_B];

    matrix_multiply(A, B, result);

    printf("Matrix A:\n");
    print_matrix(ROWS_A, COLS_A, A);

    printf("\nMatrix B:\n");
    print_matrix(ROWS_B, COLS_B, B);

    printf("\nResult of multiplication:\n");
    print_matrix(ROWS_A, COLS_B, result);

    return 0;
}
