/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           Compatibility definitions between MK60D10.h and MK60DZ10.h
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef MK60_COMP_H_
#define MK60_COMP_H_

#ifdef __cplusplus
extern "C"
{
#endif

#if K60_CPU_REV == 1

/* Some compatibility defines to minimize the ifdefs needed for the register
 * name changes */

#define SIM_SCGC6_SPI0_MASK SIM_SCGC6_DSPI0_MASK
#define SIM_SCGC6_SPI0_SHIFT SIM_SCGC6_DSPI0_SHIFT

#define MCG_C2_RANGE0_MASK MCG_C2_RANGE_MASK
#define MCG_C5_PRDIV0_MASK MCG_C5_PRDIV_MASK
#define MCG_C6_VDIV0_MASK MCG_C6_VDIV_MASK

#define UART_BASES { UART0, UART1, UART2, UART3, UART4, UART5 }

#define LPTMR0_IRQn LPTimer_IRQn

/* Rev 2.x made the OSC32KSEL field into a bitfield (is a single bit in 1.x) */
#define SIM_SOPT1_OSC32KSEL(a) (SIM_SOPT1_OSC32KSEL_MASK)

#endif /* K60_CPU_REV == 1 */


/* Compatibility defines for compatibility with differing module names between
 * MK60 and MKW22 headers */
#define SIM_SCGC5_LPTMR_MASK SIM_SCGC5_LPTIMER_MASK
#define SIM_SCGC5_LPTMR_SHIFT SIM_SCGC5_LPTIMER_SHIFT

#ifndef OSC0
/* Compatibility definition */
#define OSC0 OSC
#endif
#ifndef MCG_C2_RANGE0
/* Rev 2 parts renamed the parameter RANGE -> RANGE0 */
#define MCG_C2_RANGE0 MCG_C2_RANGE
#endif

#ifdef __cplusplus
}
#endif


#endif /* MK60_COMP_H_ */
/** @} */
