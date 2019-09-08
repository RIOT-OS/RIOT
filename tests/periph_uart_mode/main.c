/*
 * Copyright (C) 2018 Sparkmeter
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Manual test application for UART mode on CPUs with only one UART
 *
 * @author      Ben Postman <ben.l.postman@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "periph/uart.h"
#include "stdio_uart.h"
#include "xtimer.h"

/* Number of different options for each mode parameter */
#define DATA_BIT_OPTIONS (4)
#define PARITY_OPTIONS (5)
#define STOP_BIT_OPTIONS (2)
#define TOTAL_OPTIONS (DATA_BIT_OPTIONS * PARITY_OPTIONS * STOP_BIT_OPTIONS)

/* Character positions in the mode string */
#define DATA_BIT_POS (0)
#define PARITY_POS (1)
#define STOP_BIT_POS (2)

/* Length of the printable mode string, includes null terminator */
#define MODE_STR_LEN (4)

/* UART_DEV is always 0 since this test is for device with 1 UART */
#define _UART_DEV (UART_DEV(0))

/* Delay between each mode, makes parsing results easier */
#ifndef DELAY_US
#define DELAY_US 50000
#endif


/* Stores each mode string for printing at the end of the test */
static char mode_strings[TOTAL_OPTIONS][MODE_STR_LEN];


static void _get_mode(const uart_data_bits_t data_bits,
                      const uart_parity_t parity, const uart_stop_bits_t stop_bits, char* mode_str) {
    switch (data_bits) {
        case UART_DATA_BITS_5:
            mode_str[DATA_BIT_POS] = '5';
            break;
        case UART_DATA_BITS_6:
            mode_str[DATA_BIT_POS] = '6';
            break;
        case UART_DATA_BITS_7:
            mode_str[DATA_BIT_POS] = '7';
            break;
        case UART_DATA_BITS_8:
            mode_str[DATA_BIT_POS] = '8';
            break;
        default:
            break;
    }

    switch (parity) {
        case UART_PARITY_NONE:
            mode_str[PARITY_POS] = 'N';
            break;
        case UART_PARITY_EVEN:
            mode_str[PARITY_POS] = 'E';
            break;
        case UART_PARITY_ODD:
            mode_str[PARITY_POS] = 'O';
            break;
        case UART_PARITY_MARK:
            mode_str[PARITY_POS] = 'M';
            break;
        case UART_PARITY_SPACE:
            mode_str[PARITY_POS] = 'S';
            break;
        default:
            break;
    }

    switch (stop_bits) {
        case UART_STOP_BITS_1:
            mode_str[STOP_BIT_POS] = '1';
            break;
        case UART_STOP_BITS_2:
            mode_str[STOP_BIT_POS] = '2';
            break;
        default:
            break;
    }

    mode_str[MODE_STR_LEN - 1] = '\0';
}


int main(void)
{
    int8_t status;

    /* Stores the result for each mode */
    bool results[TOTAL_OPTIONS];

    /* Results index that is used to associate mode_strings and results */
    uint8_t ridx = 0;

    /* Use arrays so that each option can be easily iterated over */
    const uart_data_bits_t data_bits[DATA_BIT_OPTIONS] = {
        UART_DATA_BITS_5,
        UART_DATA_BITS_6,
        UART_DATA_BITS_7,
        UART_DATA_BITS_8
    };

    const uart_parity_t parity[PARITY_OPTIONS] = {
        UART_PARITY_NONE,
        UART_PARITY_EVEN,
        UART_PARITY_ODD,
        UART_PARITY_MARK,
        UART_PARITY_SPACE
    };

    const uart_stop_bits_t stop_bits[STOP_BIT_OPTIONS] = {
        UART_STOP_BITS_1,
        UART_STOP_BITS_2
    };

    /* Test each permutation */
    for (int didx = 0; didx < 4; ++didx) {
        for (int pidx = 0; pidx < 5; ++pidx) {
            for (int sidx = 0; sidx < 2; ++sidx) {
                /* Initialize stdio to get printf */
                stdio_init();

                status = uart_mode(_UART_DEV, data_bits[didx],
                                   parity[pidx], stop_bits[sidx]);
                _get_mode(data_bits[didx], parity[pidx],
                          stop_bits[sidx], mode_strings[ridx]);

                if (status == UART_OK) {
                    results[ridx] = true;
                    printf("%s\n", mode_strings[ridx]);
                    xtimer_usleep(DELAY_US);
                }
                else {
                    results[ridx] = false;
                }

                ridx++;
            }
        }
    }

    /* Reset mode and print results */
    stdio_init();
    uart_mode(_UART_DEV, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
    for (int i = 0; i < TOTAL_OPTIONS; ++i) {
        if (results[i] == true) {
            printf("%s: Supported\n", mode_strings[i]);
        }
        else {
            printf("%s: Unsupported\n", mode_strings[i]);
        }
    }

    return 0;
}
