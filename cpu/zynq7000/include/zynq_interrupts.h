/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_zynq7000
 * @{
 *
 * @file            interrupts.h
 * @brief           Interrupt logic
 *
 * @author          Simon Vincent <simon.vincent@xsilon.com>
 */

#ifndef __ZYNQ_INTERRUPTS_H
#define __ZYNQ_INTERRUPTS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*interrupt_handler)(void *arg);

#define INT_HIGH_LEVEL 1
#define INT_RISING_EDGE 3

void init_interrupts(void);

int register_interrupt_handler(uint8_t interrupt_id, uint8_t interrupt_type, interrupt_handler handler, void *arg);

int unregister_interrupt_handler(uint8_t interrupt_id);

void handle_interrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZYNQ_INTERRUPTS_H */
/** @} */
