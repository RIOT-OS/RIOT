/**
 * @defgroup    boards_pttu PTTU
 * @ingroup     boards
 * @brief       Support for the PTTU board
 * @{
 *
 * @file
 * @brief       Basic definitions for the PTTU board
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>

#include "lpc2387.h"
#include "cpu-conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VICIntEnClear VICIntEnClr

void init_clks1(void);
void init_clks2(void);
void bl_init_clks(void);

typedef uint8_t radio_packet_length_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif // __BOARD_H
