/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file        dev_uid_stm32f10x.c
 * @brief       Implementation of the unique device ID feature
 *              for STM32F10x MCUs.
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "dev_uid.h"


/* STM32F10x MCUs offer a 12-byte (96-bit) Unique IDentifier
   (stored in read-only, special-purpose MCU registers) */

/* Fixed address for UID registers in STM32F10x microcontrollers. */
#define UNIQUE_DEV_ID_REG_BASE  (0x1FFFF7E8)


/* Define the pointer to this device's UID */
const dev_uid_t const * dev_uid
     = (const dev_uid_t * const) UNIQUE_DEV_ID_REG_BASE;

/* Print function for device UIDs */
void print_dev_uid(const dev_uid_t const * uid)
{
    /* STM32F1xx UIDs are on 12 bytes */
    printf("Device UID: Ox");
    for (int i = 0; i < 12; i++) {
        printf(" %2.2x", uid->byte[i]);
    }
    printf("\n");
}

/* "Digest" device ID, as computed by HiKoB for IoT-LAB M3 motes
 *  in their OpenLAB framework. */
uint16_t get_short_dev_id(void)
{
    uint16_t res = dev_uid->byte[10];
    res <<= 7;
    res  |= dev_uid->byte[8];
    res <<= 8;
    res  |= dev_uid->byte[9];
    return res;
}

