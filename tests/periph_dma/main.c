/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Dummy test application for DMA peripheral
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <stdio.h>

int main(void)
{
    /*
    This test is checking that DMA on UART is not broken when stdio UART is
    configured with DMA.
    Note that this message is also printed on 'normal' stdio UART (without DMA).
    */
    puts("DMA is working");

    return 0;
}
