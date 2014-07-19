/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 *
 * @file            cpu-conf.h
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Leon George <l.george@fu-berlin.de>
 */



#ifndef __CPU_CONF_H
#define __CPU_CONF_H

/* The linker script defines a stack-size of 48K (0xC000) */
#define KERNEL_CONF_STACKSIZE_DEFAULT 0xB000

#define KERNEL_CONF_STACKSIZE_PRINTF 0x1000

#define KERNEL_CONF_STACKSIZE_IDLE 0x500

#endif
