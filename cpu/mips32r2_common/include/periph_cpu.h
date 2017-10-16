/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* This file must exist to get timer code to build */

/* No peripherals I/O via JTAG or Bootloader using UHI */

/*
 * Note mips32r2_common can be selected in its own right as a CPU
 * for testing on PFGA systems (like BOARD=mips-malta) with limited
 * or no peripherals
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
