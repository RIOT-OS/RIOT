/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qemu-i386
 * @{
 *
 * @file
 * @brief       CPU-specific defines for qemu-i386
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef QEMU_I386_CPU_CONF_H
#define QEMU_I386_CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME: This file is just a filler. The numbers are entirely random ... */

#define THREAD_STACKSIZE_DEFAULT                 (8192)
#define THREAD_STACKSIZE_IDLE                    (8192)
#define THREAD_EXTRA_STACKSIZE_PRINTF            (8192)
#define THREAD_EXTRA_STACKSIZE_PRINTF_FLOAT      (8192)
#define THREAD_STACKSIZE_MINIMUM                 (8192)
#define ISR_STACKSIZE                            (0)

#ifdef __cplusplus
}
#endif

#endif /* QEMU_I386_CPU_CONF_H */

/** @} */
