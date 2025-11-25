/*
 * Copyright (C) 2023 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           RISCV PMP implementation
 *
 * @author          Bennet Blischke
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "cpu_conf.h"
#include "vendor/riscv_csr.h"
#include "pmp.h"

#if NUM_PMP_ENTRIES != 16 && NUM_PMP_ENTRIES != 64
#error "Only 16 or 64 PMP entries allowed."
#endif

#define _WRITE_PMPCFG(REG, VALUE) write_csr(REG, VALUE)
#define _READ_PMPCFG(REG) read_csr(REG)
#define WRITE_PMPCFG(REG, VALUE) _WRITE_PMPCFG(CSR_PMPCFG0 + REG, VALUE)
#define READ_PMPCFG(REG) _READ_PMPCFG(CSR_PMPCFG0 + REG)

/* These shifts are needed as the PMP Address registers stores
 * 34 Bit addresses with 4 byte alignment in a 32 bit register
 */
#define _WRITE_PMPADDR(REG, VALUE) write_csr(REG, VALUE >> 2)
#define _READ_PMPADDR(REG) (read_csr(REG) << 2)
#define WRITE_PMPADDR(REG, VALUE) _WRITE_PMPADDR(CSR_PMPADDR0 + REG, VALUE)
#define READ_PMPADDR(REG) _READ_PMPADDR(CSR_PMPADDR0 + REG)

#define _NAPOT_base(addr) (addr & (addr + 1))
#define _NAPOT_end(addr) (addr | (addr + 1))

/* The assembly instructions for reading riscv CSRs, encode the CSR-address
 * in the immediate field which is not changeable at run-time.
 * Hence it is needed to generate all possible register accesses at build-time.
 * Settled for switch-case as they are easiest to read, understand and maintain.
 */
void write_pmpcfg(uint8_t reg_num, uint32_t value)
{
    assert(reg_num < NUM_PMP_ENTRIES / 4);
    switch (reg_num) {
    case 0: WRITE_PMPCFG(0, value); break;
    case 1: WRITE_PMPCFG(1, value); break;
    case 2: WRITE_PMPCFG(2, value); break;
    case 3: WRITE_PMPCFG(3, value); break;
#if NUM_PMP_ENTRIES == 64
    case 4: WRITE_PMPCFG(4, value); break;
    case 5: WRITE_PMPCFG(5, value); break;
    case 6: WRITE_PMPCFG(6, value); break;
    case 7: WRITE_PMPCFG(7, value); break;
    case 8: WRITE_PMPCFG(8, value); break;
    case 9: WRITE_PMPCFG(9, value); break;
    case 10: WRITE_PMPCFG(10, value); break;
    case 11: WRITE_PMPCFG(11, value); break;
    case 12: WRITE_PMPCFG(12, value); break;
    case 13: WRITE_PMPCFG(13, value); break;
    case 14: WRITE_PMPCFG(14, value); break;
    case 15: WRITE_PMPCFG(15, value); break;
#endif
    }
}

uint32_t read_pmpcfg(uint8_t reg_num)
{
    assert(reg_num < NUM_PMP_ENTRIES / 4);
    switch (reg_num) {
    case 0: return READ_PMPCFG(0);
    case 1: return READ_PMPCFG(1);
    case 2: return READ_PMPCFG(2);
    case 3: return READ_PMPCFG(3);
#if NUM_PMP_ENTRIES == 64
    case 4: return READ_PMPCFG(4);
    case 5: return READ_PMPCFG(5);
    case 6: return READ_PMPCFG(6);
    case 7: return READ_PMPCFG(7);
    case 8: return READ_PMPCFG(8);
    case 9: return READ_PMPCFG(9);
    case 10: return READ_PMPCFG(10);
    case 11: return READ_PMPCFG(11);
    case 12: return READ_PMPCFG(12);
    case 13: return READ_PMPCFG(13);
    case 14: return READ_PMPCFG(14);
    case 15: return READ_PMPCFG(15);
#endif
    }
    return 0;
}

void write_pmpaddr(uint8_t reg_num, uint32_t value)
{
    assert(reg_num < NUM_PMP_ENTRIES);
    switch (reg_num) {
    case 0: WRITE_PMPADDR(0, value); break;
    case 1: WRITE_PMPADDR(1, value); break;
    case 2: WRITE_PMPADDR(2, value); break;
    case 3: WRITE_PMPADDR(3, value); break;
    case 4: WRITE_PMPADDR(4, value); break;
    case 5: WRITE_PMPADDR(5, value); break;
    case 6: WRITE_PMPADDR(6, value); break;
    case 7: WRITE_PMPADDR(7, value); break;
    case 8: WRITE_PMPADDR(8, value); break;
    case 9: WRITE_PMPADDR(9, value); break;
    case 10: WRITE_PMPADDR(10, value); break;
    case 11: WRITE_PMPADDR(11, value); break;
    case 12: WRITE_PMPADDR(12, value); break;
    case 13: WRITE_PMPADDR(13, value); break;
    case 14: WRITE_PMPADDR(14, value); break;
    case 15: WRITE_PMPADDR(15, value); break;
#if NUM_PMP_ENTRIES == 64
    case 16: WRITE_PMPADDR(16, value); break;
    case 17: WRITE_PMPADDR(17, value); break;
    case 18: WRITE_PMPADDR(18, value); break;
    case 19: WRITE_PMPADDR(19, value); break;
    case 20: WRITE_PMPADDR(20, value); break;
    case 21: WRITE_PMPADDR(21, value); break;
    case 22: WRITE_PMPADDR(22, value); break;
    case 23: WRITE_PMPADDR(23, value); break;
    case 24: WRITE_PMPADDR(24, value); break;
    case 25: WRITE_PMPADDR(25, value); break;
    case 26: WRITE_PMPADDR(26, value); break;
    case 27: WRITE_PMPADDR(27, value); break;
    case 28: WRITE_PMPADDR(28, value); break;
    case 29: WRITE_PMPADDR(29, value); break;
    case 30: WRITE_PMPADDR(30, value); break;
    case 31: WRITE_PMPADDR(31, value); break;
    case 32: WRITE_PMPADDR(32, value); break;
    case 33: WRITE_PMPADDR(33, value); break;
    case 34: WRITE_PMPADDR(34, value); break;
    case 35: WRITE_PMPADDR(35, value); break;
    case 36: WRITE_PMPADDR(36, value); break;
    case 37: WRITE_PMPADDR(37, value); break;
    case 38: WRITE_PMPADDR(38, value); break;
    case 39: WRITE_PMPADDR(39, value); break;
    case 40: WRITE_PMPADDR(40, value); break;
    case 41: WRITE_PMPADDR(41, value); break;
    case 42: WRITE_PMPADDR(42, value); break;
    case 43: WRITE_PMPADDR(43, value); break;
    case 44: WRITE_PMPADDR(44, value); break;
    case 45: WRITE_PMPADDR(45, value); break;
    case 46: WRITE_PMPADDR(46, value); break;
    case 47: WRITE_PMPADDR(47, value); break;
    case 48: WRITE_PMPADDR(48, value); break;
    case 49: WRITE_PMPADDR(49, value); break;
    case 50: WRITE_PMPADDR(50, value); break;
    case 51: WRITE_PMPADDR(51, value); break;
    case 52: WRITE_PMPADDR(52, value); break;
    case 53: WRITE_PMPADDR(53, value); break;
    case 54: WRITE_PMPADDR(54, value); break;
    case 55: WRITE_PMPADDR(55, value); break;
    case 56: WRITE_PMPADDR(56, value); break;
    case 57: WRITE_PMPADDR(57, value); break;
    case 58: WRITE_PMPADDR(58, value); break;
    case 59: WRITE_PMPADDR(59, value); break;
    case 60: WRITE_PMPADDR(60, value); break;
    case 61: WRITE_PMPADDR(61, value); break;
    case 62: WRITE_PMPADDR(62, value); break;
    case 63: WRITE_PMPADDR(63, value); break;
#endif
    }
}

uint32_t read_pmpaddr(uint8_t reg_num)
{
    assert(reg_num < NUM_PMP_ENTRIES);
    switch (reg_num) {
    case 0: return READ_PMPADDR(0);
    case 1: return READ_PMPADDR(1);
    case 2: return READ_PMPADDR(2);
    case 3: return READ_PMPADDR(3);
    case 4: return READ_PMPADDR(4);
    case 5: return READ_PMPADDR(5);
    case 6: return READ_PMPADDR(6);
    case 7: return READ_PMPADDR(7);
    case 8: return READ_PMPADDR(8);
    case 9: return READ_PMPADDR(9);
    case 10: return READ_PMPADDR(10);
    case 11: return READ_PMPADDR(11);
    case 12: return READ_PMPADDR(12);
    case 13: return READ_PMPADDR(13);
    case 14: return READ_PMPADDR(14);
    case 15: return READ_PMPADDR(15);
#if NUM_PMP_ENTRIES == 64
    case 16: return READ_PMPADDR(16);
    case 17: return READ_PMPADDR(17);
    case 18: return READ_PMPADDR(18);
    case 19: return READ_PMPADDR(19);
    case 20: return READ_PMPADDR(20);
    case 21: return READ_PMPADDR(21);
    case 22: return READ_PMPADDR(22);
    case 23: return READ_PMPADDR(23);
    case 24: return READ_PMPADDR(24);
    case 25: return READ_PMPADDR(25);
    case 26: return READ_PMPADDR(26);
    case 27: return READ_PMPADDR(27);
    case 28: return READ_PMPADDR(28);
    case 29: return READ_PMPADDR(29);
    case 30: return READ_PMPADDR(30);
    case 31: return READ_PMPADDR(31);
    case 32: return READ_PMPADDR(32);
    case 33: return READ_PMPADDR(33);
    case 34: return READ_PMPADDR(34);
    case 35: return READ_PMPADDR(35);
    case 36: return READ_PMPADDR(36);
    case 37: return READ_PMPADDR(37);
    case 38: return READ_PMPADDR(38);
    case 39: return READ_PMPADDR(39);
    case 40: return READ_PMPADDR(40);
    case 41: return READ_PMPADDR(41);
    case 42: return READ_PMPADDR(42);
    case 43: return READ_PMPADDR(43);
    case 44: return READ_PMPADDR(44);
    case 45: return READ_PMPADDR(45);
    case 46: return READ_PMPADDR(46);
    case 47: return READ_PMPADDR(47);
    case 48: return READ_PMPADDR(48);
    case 49: return READ_PMPADDR(49);
    case 50: return READ_PMPADDR(50);
    case 51: return READ_PMPADDR(51);
    case 52: return READ_PMPADDR(52);
    case 53: return READ_PMPADDR(53);
    case 54: return READ_PMPADDR(54);
    case 55: return READ_PMPADDR(55);
    case 56: return READ_PMPADDR(56);
    case 57: return READ_PMPADDR(57);
    case 58: return READ_PMPADDR(58);
    case 59: return READ_PMPADDR(59);
    case 60: return READ_PMPADDR(60);
    case 61: return READ_PMPADDR(61);
    case 62: return READ_PMPADDR(62);
    case 63: return READ_PMPADDR(63);
#endif
    }
    return 0;
}

uint8_t get_pmpcfg(uint8_t entry)
{
    return (read_pmpcfg(entry / 4) >> (entry % 4) * 8) & 0xFF;
}

void set_pmpcfg(uint8_t entry, uint8_t value)
{
    uint32_t cur_value = read_pmpcfg(entry / 4);
    uint32_t mask = 0xff << (entry % 4) * 8;

    cur_value &= ~mask;
    cur_value |= value << (entry % 4) * 8;
    write_pmpcfg(entry / 4, cur_value);
}

void print_pmpcfg(uint8_t entry)
{
    uint8_t cfg = get_pmpcfg(entry);
    uint32_t start = 0;
    uint32_t stop = 0;
    char *mode = "OFF  ";

    switch (cfg & PMP_A) {
    case PMP_TOR:
        mode = "TOR  ";
        start = (entry > 0) ? read_pmpaddr(entry - 1) : 0;
        stop = read_pmpaddr(entry);
        break;
    case PMP_NA4:
        mode = "NA4  ";
        start = read_pmpaddr(entry);
        stop = start + 3;
        break;
    case PMP_NAPOT:
        mode = "NAPOT";
        /* Flipp last two bits in NAPOT mode */
        uint32_t _tmp = read_pmpaddr(entry) | 0x03;
        start = _NAPOT_base(_tmp);
        stop = _NAPOT_end(_tmp);
        break;
    }
    printf("pmp%02dcfg: %c %c%c%c %s 0x%08x - 0x%08x\n",
           entry,
           (cfg & PMP_L) ? 'L' : '-',
           (cfg & PMP_R) ? 'R' : '-',
           (cfg & PMP_W) ? 'W' : '-',
           (cfg & PMP_X) ? 'X' : '-',
           mode,
           (unsigned int)start,
           (unsigned int)stop
           );
}
