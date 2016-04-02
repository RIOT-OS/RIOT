/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "cpu.h"

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           Implementation of stack smashing protection helper functions used by GCC's -fstack-protector
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

void *__stack_chk_guard = 0;

void __stack_chk_guard_setup(void)
{
    unsigned char *p;
    p = (unsigned char *) &__stack_chk_guard;

    /* TODO: This should be replaced by a random number to use as a canary value */
    p[0] = 0;
    p[1] = 0;
    p[2] = '\n';
    p[3] = 255;
}

/*
 * Arrange so that the __stack_chk_guard_setup function is called during
 * early init.
 */
void __attribute__((section(".preinit_array")))(*preinit__stack_chk_guard_setup[])(void) = {__stack_chk_guard_setup};

/**
 * @brief Handler for stack smashing protection failure.
 *
 * This is called if the SSP checks fail, which means that the stack has been
 * corrupted.
 */
void __attribute__((noreturn)) __stack_chk_fail(void)
{
    __asm__ volatile ("bkpt #1");

    while (1);
}
/** @} */
