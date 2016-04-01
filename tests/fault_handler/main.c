/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       Minimal test application for triggering a hard fault or equivalent
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include <xtimer.h>

#ifndef FORBIDDEN_ADDRESS
/* Many platforms do not allow writes to address 0x00000000 */
#define FORBIDDEN_ADDRESS (0x00000000)
#endif /* !defined(FORBIDDEN_ADDRESS) */
#ifndef INVALID_INSTRUCTION
/* Random garbage may crash the program as well. */
#define INVALID_INSTRUCTION asm volatile (".short 0xdead, 0xbeef, 0xcafe, 0xbabe\n")
#endif /* !defined(INVALID_INSTRUCTION) */

#define PRINT_MACRO(a) PRINT_MACRO2(a)
#define PRINT_MACRO2(a) #a

int main(void)
{
    puts("Fault handler test application");
    printf("This application will crash by attempting to write to address 0x%08x\n", FORBIDDEN_ADDRESS);
    puts("Waiting 1 second before crashing...");
    xtimer_usleep(1000000);
    puts("Write to forbidden address " PRINT_MACRO(FORBIDDEN_ADDRESS));
    *((volatile int *) FORBIDDEN_ADDRESS) = 12345;
    int readback = *((volatile int *) FORBIDDEN_ADDRESS);
    printf("readback: 0x%08x\n", readback);
    puts("We did not expect the application to survive the previous write.");
    puts("Trying to execute an invalid instruction");
    puts(PRINT_MACRO(INVALID_INSTRUCTION));
    INVALID_INSTRUCTION;
    puts("Failed to crash the program, hanging...");
    while(1);
    return 0;
}
