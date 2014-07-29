/**
 * @defgroup    boards_msba2-common MSB-A2 common
 * @ingroup     boards
 * @brief       Common files for all MSB-A2 based boards
 * @{
 *
 * @files       msba2-common.h
 * @brief       MSB-A2 Common Board Definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef __MSBA2_COMMON_H
#define __MSBA2_COMMON_H


#include <stdint.h>
#include "lpc2387.h"

#define VICIntEnClear VICIntEnClr

static inline void pllfeed(void)
{
    PLLFEED = 0xAA;
    PLLFEED = 0x55;
}

/** @} */
#endif // __MSBA2_COMMON_H
