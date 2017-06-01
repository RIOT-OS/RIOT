/*
 * Copyright (C) 2017 PDTECTH Co., LTD
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __ARM32_CP15_H__
#define __ARM32_CP15_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void     __irq_enable(void);
void     __irq_disable(void);
void     __fiq_enable(void);
void     __fiq_disable(void);
void     __icache_enable(void);
void     __icache_disable(void);
int      __icache_status(void);
void     __dcache_enable(void);
void     __dcache_disable(void);
int      __dcache_status(void);
void     __wbuffer_enable(void);
void     __wbuffer_disable(void);
int      __wbuffer_status(void);
void     __mmu_enable(void);
void     __mmu_disable(void);
int      __mmu_status(void);
void     __vic_enable(void);
void     __vic_disable(void);
int      __vic_status(void);
void     __branch_enable(void);
void     __branch_disable(void);
int      __branch_status(void);
void     __ttb_set(uint32_t base);
uint32_t __ttb_get(void);
void     __domain_set(uint32_t domain);
uint32_t __domain_get(void);
void     __tlb_invalidate(void);
uint32_t __get_cpuid(void);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_CP15_H__ */
