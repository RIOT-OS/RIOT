/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * CPU-specific defines for qemu-i386.
 *
 * @ingroup x86
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef __RIOT__BOARDS__QEMU_I386__CPU_CONF__H
#define __RIOT__BOARDS__QEMU_I386__CPU_CONF__H

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME: This file is just a filler. The numbers are entirely random ... */

#define KERNEL_CONF_STACKSIZE_DEFAULT      (8192)
#define KERNEL_CONF_STACKSIZE_IDLE         (8192)
#define KERNEL_CONF_STACKSIZE_PRINTF       (8192)
#define KERNEL_CONF_STACKSIZE_PRINTF_FLOAT (8192)

#define MINIMUM_STACK_SIZE                 (8192)

#define UART0_BUFSIZE                      (16)

#define F_CPU (1000000) /* This value is unused in x86 */

#ifdef __cplusplus
}
#endif

#endif

/** @} */
