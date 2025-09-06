/*
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level EBI (External BUS Interface) driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 * https://www.avrfreaks.net/forum/xmega-ebi-and-sram
 * https://www.avrfreaks.net/forum/xmega-au-four-port-ebi
 * https://community.atmel.com/forum/location-variable-specified-address
 * @}
 */
#include <avr/io.h>

#include "assert.h"
#include "periph_conf.h"
#include "cpu_pm.h"
#include "cpu_ebi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void ebi_init(void) __attribute__((naked, section(".init1"), used));

/**
 * @brief Set up the I/O ports for use by the EBI.
 *
 * @note In SDRAM mode the \a sram_ale and \a lpc_ale parameters are ignored
 *       by the hardware.
 */
void ebi_init(void)
{
    EBI_CS_t *cs;
    uint8_t mode;
    uint8_t sd_ctrl = 0;
    uint8_t i;
    uint8_t expand_sram = 0;

    /*
     * This is a mandatory configuration. Whowever, to complete disable module
     * just configure it as:
     *
     * static const ebi_conf_t ebi_config = { 0 };
     *
     * or, for a temporary disable, set addr_bits to 0 at periph_conf.h:
     *
     * .addr_bits = 0,
     */
    if (ebi_config.addr_bits == 0) {
        return;
    }

    /*
     * Set address and control lines as outputs, and active-low control lines
     * initially high.
     */
    if (ebi_config.flags & EBI_PORT_SDRAM) {
        /* With SDRAM, the configuration is fairly fixed. */
        PORTH.OUT = 0x0f;
        PORTH.DIR = 0xff;
        PORTJ.DIR = 0xf0;
        PORTK.DIR = 0xff;
    } else {
        uint8_t ale_mask = ebi_config.sram_ale | ebi_config.lpc_ale;
        uint8_t port_mask;

        /*
         * Set PORTH initial state, set WE and CAS/RE high by default.
         * Set chip selects high by default if enabled.
         */
        port_mask = 0x03;
        if (ebi_config.flags & EBI_PORT_CS0) {
            port_mask |= 0x10;
        }
        if (ebi_config.flags & EBI_PORT_CS1) {
            port_mask |= 0x20;
        }
        if (ebi_config.flags & EBI_PORT_CS2) {
            port_mask |= 0x40;
        }
        if (ebi_config.flags & EBI_PORT_CS3) {
            port_mask |= 0x80;
        }
        PORTH.OUT = port_mask;

        /*
         * Set PORTH direction, enable WE, CAS/RE and RAS/ALE1 to
         * output by default. Set chip select direction if enabled.
         */
        port_mask = 0x07;

        /* If two latches are in use, enable the ALE2 pin as well. */
        if (ale_mask & 0x02) {
            port_mask |= 0x08;
        }
        if (ebi_config.flags & EBI_PORT_CS0 || ebi_config.addr_bits > 16) {
            port_mask |= 0x10;
        }
        if (ebi_config.flags & EBI_PORT_CS1 || ebi_config.addr_bits > 17) {
            port_mask |= 0x20;
        }
        if (ebi_config.flags & EBI_PORT_CS2 || ebi_config.addr_bits > 18) {
            port_mask |= 0x40;
        }
        if (ebi_config.flags & EBI_PORT_CS3 || ebi_config.addr_bits > 19) {
            port_mask |= 0x80;
        }
        PORTH.DIR = port_mask;

        /*
         * PORTJ is always used for data, direction and value is controlled by
         * the EBI module.
         */

        /* PORTK is only used in 3-port mode */
        if (ebi_config.flags & EBI_PORT_3PORT) {
            port_mask = 0x00;

            if (ebi_config.flags & EBI_PORT_SRAM) {
                /*
                 * Bits 0..7 go here, so if we have 8 lines or more, enable all
                 * lines. Otherwise, enable as many as we need.
                 */
                if (ebi_config.addr_bits < 8) {
                    port_mask = (1 << ebi_config.addr_bits) - 1;
                }
                else {
                    port_mask = 0xff;
                }
            }
            else {
                /*
                 * Bits 8..15 go here, so if we have less than 16 address lines,
                 * disable the ones that we don't need. If we have 8 lines or
                 * less, disable all address lines on this port.
                 */
                if (ebi_config.addr_bits <= 8) {
                    port_mask = 0x00;
                }
                else if (ebi_config.addr_bits < 16) {
                    port_mask = (1 << (ebi_config.addr_bits - 8)) - 1;
                }
                else {
                    port_mask = 0xff;
                }
            }

            PORTK.DIR = port_mask;
        }
    }

    if (ebi_config.flags & EBI_PORT_3PORT) {
        mode = EBI_IFMODE_3PORT_gc;
    }
    else {
        mode = EBI_IFMODE_2PORT_gc;
    }

    if (ebi_config.sram_ale == 1) {
        mode |= EBI_SRMODE_ALE1_gc;
    }
    else if (ebi_config.sram_ale == 2) {
        mode |= EBI_SRMODE_ALE12_gc;
    }
    else {
        mode |= EBI_SRMODE_NOALE_gc;
    }

    if (ebi_config.lpc_ale > 0) {
        mode |= (ebi_config.lpc_ale << EBI_LPCMODE_gp);
    }

    if (ebi_config.sdram.cas_latency == EBI_SDRAM_CAS_LAT_3CLK) {
        sd_ctrl |= EBI_SDCAS_bm;
    }

    if (ebi_config.sdram.row_bits == EBI_SDRAM_ROW_BITS_12) {
        sd_ctrl |= EBI_SDROW_bm;
    }

    /* Enable EBI periph clock */
    PR.PRGEN &= ~PR_EBI_bm;

    /* 8-bit SDRAM requires 4-port EBI, which we don't have. */
    EBI.CTRL       = EBI_SDDATAW_4BIT_gc
                   | mode;
    EBI.SDRAMCTRLA = sd_ctrl
                   | ebi_config.sdram.column_bits;
    EBI.SDRAMCTRLB = ebi_config.sdram.ld_mode_dly
                   | ebi_config.sdram.row_cycle_dly
                   | ebi_config.sdram.row_prechage_dly;
    EBI.SDRAMCTRLC = ebi_config.sdram.write_recovery_dly
                   | ebi_config.sdram.exit_self_rfsh_dly
                   | ebi_config.sdram.row_to_column_dly;
    EBI.REFRESH    = ebi_config.sdram.refresh_period & 0x0FFF;
    EBI.INITDLY    = ebi_config.sdram.init_dly & 0x3FFF;

    /* IRQ are disabled here */
    cs = (EBI_CS_t *)&EBI.CS0;
    for (i = 0; i < PERIPH_EBI_MAX_CS; i++) {
        if (ebi_config.cs[i].mode != EBI_CS_MODE_DISABLED_gc &&
            ebi_config.cs[i].mode != EBI_CS_MODE_SDRAM_gc) {

            /* Configure */
            cs[i].CTRLA    = ebi_config.cs[i].space;
            cs[i].CTRLB    = ebi_config.cs[i].wait;
            cs[i].BASEADDR = ((ebi_config.cs[i].address >> 8) & 0xfff0);

            /* Enable */
            cs[i].CTRLA    = ebi_config.cs[i].space | ebi_config.cs[i].mode;

            if (ebi_config.cs[i].address == 0) {
                expand_sram = 1;
            }
        }
    }

    /**
     * Only CS[3] can be configured as SDRAM.
     * CS structure is little bit different too.
     */
    if (ebi_config.cs[3].mode == EBI_CS_MODE_SDRAM_gc) {
        cs[3].CTRLA    = ebi_config.cs[3].space;
        cs[3].CTRLB    = ebi_config.sdram.mode
                       | (ebi_config.sdram.refresh ? EBI_CS_SDSREN_bm : 0);
        cs[3].BASEADDR = ((ebi_config.cs[3].address >> 8) & 0xfff0);

        cs[3].CTRLA    = ebi_config.cs[3].space | ebi_config.cs[3].mode;

        if (ebi_config.cs[3].address == 0) {
            expand_sram = 1;
        }

        while (!(cs[3].CTRLB & EBI_CS_SDINITDONE_bm)) {};
    }

    if (expand_sram > 0) {
        /**
         * @brief Set new Stack Pointer
         */
        __asm__ volatile (
            "out  __SP_L__,     %A[stack]          \n\t"
            "out  __SP_H__,     %B[stack]          \n\t"
            : /* no output */
            : [stack]         "r"(RAM_LEN)
            : "memory"
        );
    };
}

uint16_t hugemem_read16(const hugemem_ptr_t from)
{
    uint16_t value;

    __asm__ volatile (
        "movw   r30,        %A[from]           \n\t"
        "out    %[rampz],   %C[from]           \n\t"
        "ld     %A[dest],    Z+                \n\t"
        "ld     %B[dest],    Z                 \n\t"
        "out    %[rampz],    __zero_reg__      \n\t"
        : [dest]    "=r"(value)
        : [from]    "r"(from),
          [rampz]   "i"(&RAMPZ)
        : "r30", "r31"
    );

    return value;
}

void hugemem_write16(hugemem_ptr_t to, uint16_t val)
{
    __asm__ volatile (
        "movw   r30,        %A[to]             \n\t"
        "out    %[rampz],   %C[to]             \n\t"
        "st     Z+,         %A[val]            \n\t"
        "st     Z,          %B[val]            \n\t"
        "out    %[rampz],    __zero_reg__      \n\t"
        : /* no output */
        : [to]      "r"(to),
          [val]     "r"(val),
          [rampz]   "i"(&RAMPZ)
        : "r30", "r31"
    );
}

uint32_t hugemem_read32(const hugemem_ptr_t from)
{
    uint32_t value;

    __asm__ volatile (
        "movw   r30,        %A[from]           \n\t"
        "out    %[rampz],   %C[from]           \n\t"
        "ld     %A[dest],    Z+                \n\t"
        "ld     %B[dest],    Z+                \n\t"
        "ld     %C[dest],    Z+                \n\t"
        "ld     %D[dest],    Z                 \n\t"
        "out    %[rampz],    __zero_reg__      \n\t"
        : [dest]    "=r"(value)
        : [from]    "r"(from),
          [rampz]   "i"(&RAMPZ)
        : "r30", "r31"
    );

    return value;
}

void hugemem_write32(hugemem_ptr_t to, uint32_t val)
{
    __asm__ volatile (
        "movw   r30,        %A[to]             \n\t"
        "out    %[rampz],   %C[to]             \n\t"
        "st     Z+,         %A[val]            \n\t"
        "st     Z+,         %B[val]            \n\t"
        "st     Z+,         %C[val]            \n\t"
        "st     Z,          %D[val]            \n\t"
        "out    %[rampz],    __zero_reg__      \n\t"
        : /* no output */
        : [to]      "r"(to),
          [val]     "r"(val),
          [rampz]   "i"(&RAMPZ)
        : "r30", "r31"
    );
}

void hugemem_read_block(void *to, const hugemem_ptr_t from, size_t size)
{
    if (size > 0) {
        __asm__ volatile (
            "movw   r30,         %A[from]      \n\t"
            "out    %[rampz],    %C[from]      \n\t"
            "get_%=:                           \n\t"
            "ld     __tmp_reg__,  Z+           \n\t"
            "st     X+,           __tmp_reg__  \n\t"
            "sbiw   %A[size],     1            \n\t"
            "brne get_%=                       \n\t"
            "out    %[rampz],     __zero_reg__ \n\t"
            : [to]      "+x"(to),
              [size]    "+w"(size)
            : [from]    "r"(from),
              [rampz]   "i"(&RAMPZ)
            : "r30", "r31"
        );
    }
}

void hugemem_write_block(hugemem_ptr_t to, const void *from, size_t size)
{
    if (size > 0) {
        __asm__ volatile (
            "movw   r30,         %A[from]      \n\t"
            "out    %[rampz],    %C[from]      \n\t"
            "put_%=:                           \n\t"
            "ld     __tmp_reg__,  X+           \n\t"
            "st     Z+,           __tmp_reg__  \n\t"
            "sbiw   %A[size],     1            \n\t"
            "brne put_%=                       \n\t"
            "out    %[rampz],     __zero_reg__ \n\t"
            : [from]    "+x"(from),
              [size]    "+w"(size)
            : [to]      "r"(to),
              [rampz]   "i"(&RAMPZ)
            : "r30", "r31"
        );
    }
}
