/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Implementation of Zynq7000 interrupts
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include "zynq_interrupts.h"

#include <stdint.h>
#include <stddef.h>
#include "zynq7000.h"

#define MAX_NUM_INTERRUPTS 		95
#define INT_MASK				0x3FF

#define GIC_CPU_BASEADDR			0xF8F00100U
#define GIC_DIST_BASEADDR			0xF8F01000U

/* GIC CPU offsets */
#define GIC_CPU_CONTROL					0
#define GIC_CPU_PRI_MASK				0x4
#define GIC_CPU_BIN_POINT				0x8
#define GIC_CPU_INT_ACK					0xC
#define GIC_CPU_END_OF_INT				0x10
#define GIC_CPU_RUNNING_PRI				0x14
#define GIC_CPU_HIGHEST_PENDING			0x18
#define GIC_CPU_NON_SECURE_BIN_POINT	0x1C

/* GIC DIST offsets */
#define GIC_DIST_CONTROL				0
#define GIC_DIST_SET_ENABLE				0x100
#define GIC_DIST_CLEAR_ENABLE			0x180
#define GIC_DIST_INT_PRI				0x400
#define GIC_DIST_PROCESSOR_TARGET		0x800
#define GIC_DIST_CONFIGURATION          0xC00

#define write_gic_cpu_reg(offset, data) \
(*(volatile uint32_t *) (GIC_CPU_BASEADDR + offset) = data)

#define read_gic_cpu_reg(offset) \
(*(volatile uint32_t *) (GIC_CPU_BASEADDR + offset))

#define write_gic_dist_reg(offset, data) \
(*(volatile uint32_t *) (GIC_DIST_BASEADDR + offset) = data)

#define read_gic_dist_reg(offset) \
(*(volatile uint32_t *) (GIC_DIST_BASEADDR + offset))

#define DEFAULT_PRIORITY_MASK	0x0UL
#define DEFAULT_CPU_MASK		0x01010101UL

typedef struct
{
    interrupt_handler cb;
    void *arg;
} int_handle_t;

int_handle_t handlers[MAX_NUM_INTERRUPTS];

int enable_interrupt_vector (uint8_t interrupt_id);
int disable_interrupt_vector (uint8_t interrupt_id);
static void interrupts_dist_init (void);
static void interrupts_cpu_init (void);
void set_interrupt_type (uint8_t interrupt_id, uint8_t interrupt_type);

static void interrupts_dist_init (void) {
    uint8_t int_id;

    /* disable GIC distributor while we make changes */
    write_gic_dist_reg(GIC_DIST_CONTROL, 0);

    /* TODO: set trigger mode for SPI interrupts. */

    /* set priority */
    for (int_id = 0; int_id < MAX_NUM_INTERRUPTS; int_id += 4) {
        write_gic_dist_reg(GIC_DIST_INT_PRI + int_id, DEFAULT_PRIORITY_MASK);
    }

    /* set CPU interface for SPI interrupts so both CPUs can access */
    for (int_id = 32; int_id < MAX_NUM_INTERRUPTS; int_id += 4) {
        write_gic_dist_reg(GIC_DIST_PROCESSOR_TARGET + int_id, DEFAULT_CPU_MASK);
    }

    /* disable all interrupts */
    write_gic_dist_reg(GIC_DIST_CLEAR_ENABLE, 0xFFFFFFFF);
    write_gic_dist_reg(GIC_DIST_CLEAR_ENABLE + 4, 0xFFFFFFFF);
    write_gic_dist_reg(GIC_DIST_CLEAR_ENABLE + 8, 0xFFFFFFFF);

    /* enable GIC distributor */
    write_gic_dist_reg(GIC_DIST_CONTROL, 0x3);
}

static void interrupts_cpu_init (void) {
    /* set priority mask */
    write_gic_cpu_reg(GIC_CPU_PRI_MASK, 0xff);

    /* setup control register to enable interrupts */
    write_gic_cpu_reg(GIC_CPU_CONTROL, 7);
}

void init_interrupts (void) {
    for (int i = 0; i < MAX_NUM_INTERRUPTS; i++) {
        handlers[i].cb = NULL;
    }
    interrupts_dist_init ();
    interrupts_cpu_init ();
}

int register_interrupt_handler (uint8_t interrupt_id, uint8_t interrupt_type, interrupt_handler handler,
                            void *arg) {
    if (interrupt_id > MAX_NUM_INTERRUPTS) {
        return -1;
    }

    handlers[interrupt_id].cb = handler;
    handlers[interrupt_id].arg = arg;

    set_interrupt_type (interrupt_id, interrupt_type);
    enable_interrupt_vector (interrupt_id);
    return 0;
}

int unregister_interrupt_handler (uint8_t interrupt_id) {
    if (interrupt_id > MAX_NUM_INTERRUPTS) {
        return -1;
    }

    handlers[interrupt_id].cb = NULL;
    disable_interrupt_vector (interrupt_id);
    return 0;
}

void handle_interrupt (void) {
    uint32_t interrupt_id;

    /* get pending interrupt + acknowledge */
    interrupt_id = read_gic_cpu_reg(GIC_CPU_INT_ACK) & INT_MASK;

    if (interrupt_id > MAX_NUM_INTERRUPTS) {
        goto end_of_int;
    }

    if (handlers[interrupt_id].cb) {
        handlers[interrupt_id].cb (handlers[interrupt_id].arg);
    }

    end_of_int:
    /* write to end of interrupt reg */
    write_gic_cpu_reg(GIC_CPU_END_OF_INT, interrupt_id);
}

void set_interrupt_type (uint8_t interrupt_id, uint8_t interrupt_type) {
    if (interrupt_id > MAX_NUM_INTERRUPTS) {
        return;
    }

    uint32_t mask = (interrupt_type & 0x3) << ((interrupt_id % 16) * 2);
    uint32_t value;

    if (interrupt_id < 32) {
        /* these are hardcoded */
        return;
    }
    else if (interrupt_id < 48) {
        value = read_gic_dist_reg(GIC_DIST_CONFIGURATION + 0x8);
        value |= mask;
        write_gic_dist_reg(GIC_DIST_CONFIGURATION + 0x8, value);
    }
    else if (interrupt_id < 64) {
        value = read_gic_dist_reg(GIC_DIST_CONFIGURATION + 0xC);
        value |= mask;
        write_gic_dist_reg(GIC_DIST_CONFIGURATION + 0xC, value);
    }
    else if (interrupt_id < 80) {
        value = read_gic_dist_reg(GIC_DIST_CONFIGURATION + 0x10);
        value |= mask;
        write_gic_dist_reg(GIC_DIST_CONFIGURATION + 0x10, value);
    }
    else {
        value = read_gic_dist_reg(GIC_DIST_CONFIGURATION + 0x14);
        value |= mask;
        write_gic_dist_reg(GIC_DIST_CONFIGURATION + 0x14, value);
    }
    return;
}

int enable_interrupt_vector (uint8_t interrupt_id) {
    if (interrupt_id > MAX_NUM_INTERRUPTS) {
        return -1;
    }

    uint32_t mask = 1 << (interrupt_id % 32);

    if (interrupt_id < 32) {
        write_gic_dist_reg(GIC_DIST_SET_ENABLE, mask);
    }
    else if (interrupt_id < 64) {
        write_gic_dist_reg(GIC_DIST_SET_ENABLE + 4, mask);
    }
    else {
        write_gic_dist_reg(GIC_DIST_SET_ENABLE + 8, mask);
    }
    return 0;
}

int
disable_interrupt_vector (uint8_t interrupt_id) {
    if (interrupt_id > MAX_NUM_INTERRUPTS) {
        return -1;
    }

    uint32_t mask = 1 << (interrupt_id % 32);

    if (interrupt_id < 32) {
        write_gic_dist_reg(GIC_DIST_CLEAR_ENABLE, mask);
    }
    else if (interrupt_id < 64) {
        write_gic_dist_reg(GIC_DIST_CLEAR_ENABLE + 4, mask);
    }
    else {
        write_gic_dist_reg(GIC_DIST_CLEAR_ENABLE + 8, mask);
    }
    return 0;
}
