/*
 * SPDX-FileCopyrightText: 2018 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
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
