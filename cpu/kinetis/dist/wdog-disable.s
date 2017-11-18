/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* GCC ARM assembler */

    .text
    .thumb
    .align 2

/* Public functions declared in this file */
    .global  wdog_disable

.section .text.wdog_disable
.thumb_func
.func
wdog_disable:
    movw r0, #0x200e
    movw r1, #0xc520
    movt r0, #0x4005
    strh r1, [r0, #0]
    movw r1, #0xd928
    strh r1, [r0, #0]
    movw r0, #0x2000
    movs r1, #0xd2
    movt r0, #0x4005
    strh r1, [r0, #0]
    bkpt #0
.endfunc
