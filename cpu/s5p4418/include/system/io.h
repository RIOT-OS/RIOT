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

#ifndef __IO_H__
#define __IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

static inline uint8_t __read8(uint32_t addr)
{
    return( *((volatile uint8_t *)(addr)) );
}

static inline uint16_t __read16(uint32_t addr)
{
    return( *((volatile uint16_t *)(addr)) );
}

static inline uint32_t __read32(uint32_t addr)
{
    return( *((volatile uint32_t *)(addr)) );
}

static inline uint64_t __read64(uint32_t addr)
{
    return( *((volatile uint64_t *)(addr)) );
}

static inline void __write8(uint32_t addr, uint8_t value)
{
    *((volatile uint8_t *)(addr)) = value;
}

static inline void __write16(uint32_t addr, uint16_t value)
{
    *((volatile uint16_t *)(addr)) = value;
}

static inline void __write32(uint32_t addr, uint32_t value)
{
    *((volatile uint32_t *)(addr)) = value;
}

static inline void __write64(uint32_t addr, uint64_t value)
{
    *((volatile uint64_t *)(addr)) = value;
}

#define read8(a)        __read8(a)
#define read16(a)       __read16(a)
#define read32(a)       __read32(a)
#define read64(a)       __read64(a)

#define write8(a, v)    __write8((a), (v))
#define write16(a, v)   __write16((a), (v))
#define write32(a, v)   __write32((a), (v))
#define write64(a, v)   __write64((a), (v))

#define readl(a)        read32(a)
#define writel(v, a)    write32((a), (v))

#define IO_ADDRESS(addr) (addr)

#ifdef __cplusplus
}
#endif

#endif /* __IO_H__ */
