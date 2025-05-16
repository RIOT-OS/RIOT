/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Register Definitions for the AT86RF215 chip
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */


#include <stdint.h>
#include "vendor/at86rf215.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Radio Frontend registers
 * @{
 */
struct at86rf215_RF_regs {
    uint16_t RG_IRQS;           /**< see datasheet */
    uint16_t RG_IRQM;           /**< see datasheet */
    uint16_t RG_AUXS;           /**< see datasheet */
    uint16_t RG_STATE;          /**< see datasheet */
    uint16_t RG_CMD;            /**< see datasheet */
    uint16_t RG_CS;             /**< see datasheet */
    uint16_t RG_CCF0L;          /**< see datasheet */
    uint16_t RG_CCF0H;          /**< see datasheet */
    uint16_t RG_CNL;            /**< see datasheet */
    uint16_t RG_CNM;            /**< see datasheet */
    uint16_t RG_RXBWC;          /**< see datasheet */
    uint16_t RG_RXDFE;          /**< see datasheet */
    uint16_t RG_AGCC;           /**< see datasheet */
    uint16_t RG_AGCS;           /**< see datasheet */
    uint16_t RG_RSSI;           /**< see datasheet */
    uint16_t RG_EDC;            /**< see datasheet */
    uint16_t RG_EDD;            /**< see datasheet */
    uint16_t RG_EDV;            /**< see datasheet */
    uint16_t RG_RNDV;           /**< see datasheet */
    uint16_t RG_TXCUTC;         /**< see datasheet */
    uint16_t RG_TXDFE;          /**< see datasheet */
    uint16_t RG_PAC;            /**< see datasheet */
    uint16_t RG_PADFE;          /**< see datasheet */
    uint16_t RG_PLL;            /**< see datasheet */
    uint16_t RG_PLLCF;          /**< see datasheet */
    uint16_t RG_TXCI;           /**< see datasheet */
    uint16_t RG_TXCQ;           /**< see datasheet */
    uint16_t RG_TXDACI;         /**< see datasheet */
    uint16_t RG_TXDACQ;         /**< see datasheet */
};
/** @} */

/**
 * @brief    Base Band Controller registers
 * @{
 */
struct at86rf215_BBC_regs {
    uint16_t RG_IRQS;           /**< see datasheet */
    uint16_t RG_FBRXS;          /**< see datasheet */
    uint16_t RG_FBRXE;          /**< see datasheet */
    uint16_t RG_FBTXS;          /**< see datasheet */
    uint16_t RG_FBTXE;          /**< see datasheet */
    uint16_t RG_IRQM;           /**< see datasheet */
    uint16_t RG_PC;             /**< see datasheet */
    uint16_t RG_PS;             /**< see datasheet */
    uint16_t RG_RXFLL;          /**< see datasheet */
    uint16_t RG_RXFLH;          /**< see datasheet */
    uint16_t RG_TXFLL;          /**< see datasheet */
    uint16_t RG_TXFLH;          /**< see datasheet */
    uint16_t RG_FBLL;           /**< see datasheet */
    uint16_t RG_FBLH;           /**< see datasheet */
    uint16_t RG_FBLIL;          /**< see datasheet */
    uint16_t RG_FBLIH;          /**< see datasheet */
    uint16_t RG_OFDMPHRTX;      /**< see datasheet */
    uint16_t RG_OFDMPHRRX;      /**< see datasheet */
    uint16_t RG_OFDMC;          /**< see datasheet */
    uint16_t RG_OFDMSW;         /**< see datasheet */
    uint16_t RG_OQPSKC0;        /**< see datasheet */
    uint16_t RG_OQPSKC1;        /**< see datasheet */
    uint16_t RG_OQPSKC2;        /**< see datasheet */
    uint16_t RG_OQPSKC3;        /**< see datasheet */
    uint16_t RG_OQPSKPHRTX;     /**< see datasheet */
    uint16_t RG_OQPSKPHRRX;     /**< see datasheet */
    uint16_t RG_AFC0;           /**< see datasheet */
    uint16_t RG_AFC1;           /**< see datasheet */
    uint16_t RG_AFFTM;          /**< see datasheet */
    uint16_t RG_AFFVM;          /**< see datasheet */
    uint16_t RG_AFS;            /**< see datasheet */
    uint16_t RG_MACEA0;         /**< see datasheet */
    uint16_t RG_MACEA1;         /**< see datasheet */
    uint16_t RG_MACEA2;         /**< see datasheet */
    uint16_t RG_MACEA3;         /**< see datasheet */
    uint16_t RG_MACEA4;         /**< see datasheet */
    uint16_t RG_MACEA5;         /**< see datasheet */
    uint16_t RG_MACEA6;         /**< see datasheet */
    uint16_t RG_MACEA7;         /**< see datasheet */
    uint16_t RG_MACPID0F0;      /**< see datasheet */
    uint16_t RG_MACPID1F0;      /**< see datasheet */
    uint16_t RG_MACSHA0F0;      /**< see datasheet */
    uint16_t RG_MACSHA1F0;      /**< see datasheet */
    uint16_t RG_MACPID0F1;      /**< see datasheet */
    uint16_t RG_MACPID1F1;      /**< see datasheet */
    uint16_t RG_MACSHA0F1;      /**< see datasheet */
    uint16_t RG_MACSHA1F1;      /**< see datasheet */
    uint16_t RG_MACPID0F2;      /**< see datasheet */
    uint16_t RG_MACPID1F2;      /**< see datasheet */
    uint16_t RG_MACSHA0F2;      /**< see datasheet */
    uint16_t RG_MACSHA1F2;      /**< see datasheet */
    uint16_t RG_MACPID0F3;      /**< see datasheet */
    uint16_t RG_MACPID1F3;      /**< see datasheet */
    uint16_t RG_MACSHA0F3;      /**< see datasheet */
    uint16_t RG_MACSHA1F3;      /**< see datasheet */
    uint16_t RG_AMCS;           /**< see datasheet */
    uint16_t RG_AMEDT;          /**< see datasheet */
    uint16_t RG_AMAACKPD;       /**< see datasheet */
    uint16_t RG_AMAACKTL;       /**< see datasheet */
    uint16_t RG_AMAACKTH;       /**< see datasheet */
    uint16_t RG_FSKC0;          /**< see datasheet */
    uint16_t RG_FSKC1;          /**< see datasheet */
    uint16_t RG_FSKC2;          /**< see datasheet */
    uint16_t RG_FSKC3;          /**< see datasheet */
    uint16_t RG_FSKC4;          /**< see datasheet */
    uint16_t RG_FSKPLL;         /**< see datasheet */
    uint16_t RG_FSKSFD0L;       /**< see datasheet */
    uint16_t RG_FSKSFD0H;       /**< see datasheet */
    uint16_t RG_FSKSFD1L;       /**< see datasheet */
    uint16_t RG_FSKSFD1H;       /**< see datasheet */
    uint16_t RG_FSKPHRTX;       /**< see datasheet */
    uint16_t RG_FSKPHRRX;       /**< see datasheet */
    uint16_t RG_FSKRPC;         /**< see datasheet */
    uint16_t RG_FSKRPCONT;      /**< see datasheet */
    uint16_t RG_FSKRPCOFFT;     /**< see datasheet */
    uint16_t RG_FSKRRXFLL;      /**< see datasheet */
    uint16_t RG_FSKRRXFLH;      /**< see datasheet */
    uint16_t RG_FSKDM;          /**< see datasheet */
    uint16_t RG_FSKPE0;         /**< see datasheet */
    uint16_t RG_FSKPE1;         /**< see datasheet */
    uint16_t RG_FSKPE2;         /**< see datasheet */
    uint16_t RG_PMUC;           /**< see datasheet */
    uint16_t RG_PMUVAL;         /**< see datasheet */
    uint16_t RG_PMUQF;          /**< see datasheet */
    uint16_t RG_PMUI;           /**< see datasheet */
    uint16_t RG_PMUQ;           /**< see datasheet */
    uint16_t RG_CNTC;           /**< see datasheet */
    uint16_t RG_CNT0;           /**< see datasheet */
    uint16_t RG_CNT1;           /**< see datasheet */
    uint16_t RG_CNT2;           /**< see datasheet */
    uint16_t RG_CNT3;           /**< see datasheet */
};
/** @} */

/**
 * @name    sub-GHz Radio Frontend register map
 * @{
 */
static const struct at86rf215_RF_regs RF09_regs = {
    .RG_IRQS   = 0x00,
    .RG_IRQM   = 0x100,
    .RG_AUXS   = 0x101,
    .RG_STATE  = 0x102,
    .RG_CMD    = 0x103,
    .RG_CS     = 0x104,
    .RG_CCF0L  = 0x105,
    .RG_CCF0H  = 0x106,
    .RG_CNL    = 0x107,
    .RG_CNM    = 0x108,
    .RG_RXBWC  = 0x109,
    .RG_RXDFE  = 0x10A,
    .RG_AGCC   = 0x10B,
    .RG_AGCS   = 0x10C,
    .RG_RSSI   = 0x10D,
    .RG_EDC    = 0x10E,
    .RG_EDD    = 0x10F,
    .RG_EDV    = 0x110,
    .RG_RNDV   = 0x111,
    .RG_TXCUTC = 0x112,
    .RG_TXDFE  = 0x113,
    .RG_PAC    = 0x114,
    .RG_PADFE  = 0x116,
    .RG_PLL    = 0x121,
    .RG_PLLCF  = 0x122,
    .RG_TXCI   = 0x125,
    .RG_TXCQ   = 0x126,
    .RG_TXDACI = 0x127,
    .RG_TXDACQ = 0x128,
};
/** @} */

/**
 * @name    2.4 GHz Radio Frontend register map
 * @{
 */
static const struct at86rf215_RF_regs RF24_regs = {
    .RG_IRQS   = 0x01,
    .RG_IRQM   = 0x200,
    .RG_AUXS   = 0x201,
    .RG_STATE  = 0x202,
    .RG_CMD    = 0x203,
    .RG_CS     = 0x204,
    .RG_CCF0L  = 0x205,
    .RG_CCF0H  = 0x206,
    .RG_CNL    = 0x207,
    .RG_CNM    = 0x208,
    .RG_RXBWC  = 0x209,
    .RG_RXDFE  = 0x20A,
    .RG_AGCC   = 0x20B,
    .RG_AGCS   = 0x20C,
    .RG_RSSI   = 0x20D,
    .RG_EDC    = 0x20E,
    .RG_EDD    = 0x20F,
    .RG_EDV    = 0x210,
    .RG_RNDV   = 0x211,
    .RG_TXCUTC = 0x212,
    .RG_TXDFE  = 0x213,
    .RG_PAC    = 0x214,
    .RG_PADFE  = 0x216,
    .RG_PLL    = 0x221,
    .RG_PLLCF  = 0x222,
    .RG_TXCI   = 0x225,
    .RG_TXCQ   = 0x226,
    .RG_TXDACI = 0x227,
    .RG_TXDACQ = 0x228,
};
/** @} */

/**
 * @name    sub-GHz Radio Frontend register map
 * @{
 */
static const struct at86rf215_BBC_regs BBC0_regs = {
    .RG_IRQS       = 0x02,
    .RG_FBRXS      = 0x2000,
    .RG_FBRXE      = 0x27FE,
    .RG_FBTXS      = 0x2800,
    .RG_FBTXE      = 0x2FFE,
    .RG_IRQM       = 0x300,
    .RG_PC         = 0x301,
    .RG_PS         = 0x302,
    .RG_RXFLL      = 0x304,
    .RG_RXFLH      = 0x305,
    .RG_TXFLL      = 0x306,
    .RG_TXFLH      = 0x307,
    .RG_FBLL       = 0x308,
    .RG_FBLH       = 0x309,
    .RG_FBLIL      = 0x30A,
    .RG_FBLIH      = 0x30B,
    .RG_OFDMPHRTX  = 0x30C,
    .RG_OFDMPHRRX  = 0x30D,
    .RG_OFDMC      = 0x30E,
    .RG_OFDMSW     = 0x30F,
    .RG_OQPSKC0    = 0x310,
    .RG_OQPSKC1    = 0x311,
    .RG_OQPSKC2    = 0x312,
    .RG_OQPSKC3    = 0x313,
    .RG_OQPSKPHRTX = 0x314,
    .RG_OQPSKPHRRX = 0x315,
    .RG_AFC0       = 0x320,
    .RG_AFC1       = 0x321,
    .RG_AFFTM      = 0x322,
    .RG_AFFVM      = 0x323,
    .RG_AFS        = 0x324,
    .RG_MACEA0     = 0x325,
    .RG_MACEA1     = 0x326,
    .RG_MACEA2     = 0x327,
    .RG_MACEA3     = 0x328,
    .RG_MACEA4     = 0x329,
    .RG_MACEA5     = 0x32A,
    .RG_MACEA6     = 0x32B,
    .RG_MACEA7     = 0x32C,
    .RG_MACPID0F0  = 0x32D,
    .RG_MACPID1F0  = 0x32E,
    .RG_MACSHA0F0  = 0x32F,
    .RG_MACSHA1F0  = 0x330,
    .RG_MACPID0F1  = 0x331,
    .RG_MACPID1F1  = 0x332,
    .RG_MACSHA0F1  = 0x333,
    .RG_MACSHA1F1  = 0x334,
    .RG_MACPID0F2  = 0x335,
    .RG_MACPID1F2  = 0x336,
    .RG_MACSHA0F2  = 0x337,
    .RG_MACSHA1F2  = 0x338,
    .RG_MACPID0F3  = 0x339,
    .RG_MACPID1F3  = 0x33A,
    .RG_MACSHA0F3  = 0x33B,
    .RG_MACSHA1F3  = 0x33C,
    .RG_AMCS       = 0x340,
    .RG_AMEDT      = 0x341,
    .RG_AMAACKPD   = 0x342,
    .RG_AMAACKTL   = 0x343,
    .RG_AMAACKTH   = 0x344,
    .RG_FSKC0      = 0x360,
    .RG_FSKC1      = 0x361,
    .RG_FSKC2      = 0x362,
    .RG_FSKC3      = 0x363,
    .RG_FSKC4      = 0x364,
    .RG_FSKPLL     = 0x365,
    .RG_FSKSFD0L   = 0x366,
    .RG_FSKSFD0H   = 0x367,
    .RG_FSKSFD1L   = 0x368,
    .RG_FSKSFD1H   = 0x369,
    .RG_FSKPHRTX   = 0x36A,
    .RG_FSKPHRRX   = 0x36B,
    .RG_FSKRPC     = 0x36C,
    .RG_FSKRPCONT  = 0x36D,
    .RG_FSKRPCOFFT = 0x36E,
    .RG_FSKRRXFLL  = 0x370,
    .RG_FSKRRXFLH  = 0x371,
    .RG_FSKDM      = 0x372,
    .RG_FSKPE0     = 0x373,
    .RG_FSKPE1     = 0x374,
    .RG_FSKPE2     = 0x375,
    .RG_PMUC       = 0x380,
    .RG_PMUVAL     = 0x381,
    .RG_PMUQF      = 0x382,
    .RG_PMUI       = 0x383,
    .RG_PMUQ       = 0x384,
    .RG_CNTC       = 0x390,
    .RG_CNT0       = 0x391,
    .RG_CNT1       = 0x392,
    .RG_CNT2       = 0x393,
    .RG_CNT3       = 0x394,
};
/** @} */

/**
 * @name    2.4 GHz Radio Frontend register map
 * @{
 */
static const struct at86rf215_BBC_regs BBC1_regs = {
    .RG_IRQS       = 0x03,
    .RG_FBRXS      = 0x3000,
    .RG_FBRXE      = 0x37FE,
    .RG_FBTXS      = 0x3800,
    .RG_FBTXE      = 0x3FFE,
    .RG_IRQM       = 0x400,
    .RG_PC         = 0x401,
    .RG_PS         = 0x402,
    .RG_RXFLL      = 0x404,
    .RG_RXFLH      = 0x405,
    .RG_TXFLL      = 0x406,
    .RG_TXFLH      = 0x407,
    .RG_FBLL       = 0x408,
    .RG_FBLH       = 0x409,
    .RG_FBLIL      = 0x40A,
    .RG_FBLIH      = 0x40B,
    .RG_OFDMPHRTX  = 0x40C,
    .RG_OFDMPHRRX  = 0x40D,
    .RG_OFDMC      = 0x40E,
    .RG_OFDMSW     = 0x40F,
    .RG_OQPSKC0    = 0x410,
    .RG_OQPSKC1    = 0x411,
    .RG_OQPSKC2    = 0x412,
    .RG_OQPSKC3    = 0x413,
    .RG_OQPSKPHRTX = 0x414,
    .RG_OQPSKPHRRX = 0x415,
    .RG_AFC0       = 0x420,
    .RG_AFC1       = 0x421,
    .RG_AFFTM      = 0x422,
    .RG_AFFVM      = 0x423,
    .RG_AFS        = 0x424,
    .RG_MACEA0     = 0x425,
    .RG_MACEA1     = 0x426,
    .RG_MACEA2     = 0x427,
    .RG_MACEA3     = 0x428,
    .RG_MACEA4     = 0x429,
    .RG_MACEA5     = 0x42A,
    .RG_MACEA6     = 0x42B,
    .RG_MACEA7     = 0x42C,
    .RG_MACPID0F0  = 0x42D,
    .RG_MACPID1F0  = 0x42E,
    .RG_MACSHA0F0  = 0x42F,
    .RG_MACSHA1F0  = 0x430,
    .RG_MACPID0F1  = 0x431,
    .RG_MACPID1F1  = 0x432,
    .RG_MACSHA0F1  = 0x433,
    .RG_MACSHA1F1  = 0x434,
    .RG_MACPID0F2  = 0x435,
    .RG_MACPID1F2  = 0x436,
    .RG_MACSHA0F2  = 0x437,
    .RG_MACSHA1F2  = 0x438,
    .RG_MACPID0F3  = 0x439,
    .RG_MACPID1F3  = 0x43A,
    .RG_MACSHA0F3  = 0x43B,
    .RG_MACSHA1F3  = 0x43C,
    .RG_AMCS       = 0x440,
    .RG_AMEDT      = 0x441,
    .RG_AMAACKPD   = 0x442,
    .RG_AMAACKTL   = 0x443,
    .RG_AMAACKTH   = 0x444,
    .RG_FSKC0      = 0x460,
    .RG_FSKC1      = 0x461,
    .RG_FSKC2      = 0x462,
    .RG_FSKC3      = 0x463,
    .RG_FSKC4      = 0x464,
    .RG_FSKPLL     = 0x465,
    .RG_FSKSFD0L   = 0x466,
    .RG_FSKSFD0H   = 0x467,
    .RG_FSKSFD1L   = 0x468,
    .RG_FSKSFD1H   = 0x469,
    .RG_FSKPHRTX   = 0x46A,
    .RG_FSKPHRRX   = 0x46B,
    .RG_FSKRPC     = 0x46C,
    .RG_FSKRPCONT  = 0x46D,
    .RG_FSKRPCOFFT = 0x46E,
    .RG_FSKRRXFLL  = 0x470,
    .RG_FSKRRXFLH  = 0x471,
    .RG_FSKDM      = 0x472,
    .RG_FSKPE0     = 0x473,
    .RG_FSKPE1     = 0x474,
    .RG_FSKPE2     = 0x475,
    .RG_PMUC       = 0x480,
    .RG_PMUVAL     = 0x481,
    .RG_PMUQF      = 0x482,
    .RG_PMUI       = 0x483,
    .RG_PMUQ       = 0x484,
    .RG_CNTC       = 0x490,
    .RG_CNT0       = 0x491,
    .RG_CNT1       = 0x492,
    .RG_CNT2       = 0x493,
    .RG_CNT3       = 0x494,
};
/** @} */

/**
 * @name    Part Numbers
 * @{
 */
#define AT86RF215_PN    (0x34)  /* sub-GHz & 2.4 GHz */
#define AT86RF215IQ_PN  (0x35)  /* I/Q radio only */
#define AT86RF215M_PN   (0x36)  /* sub-GHz only */
/** @} */

/**
 * @name    SPI command prefixes
 * @{
 */
#define FLAG_WRITE        0x8000
#define FLAG_READ         0x0000
/** @} */

/**
 * @name    Radio Commands written to RF->RG_CMD
 * @{
 */
#define CMD_RF_NOP          0x0
#define CMD_RF_SLEEP        0x1
#define CMD_RF_TRXOFF       0x2
#define CMD_RF_TXPREP       0x3
#define CMD_RF_TX           0x4
#define CMD_RF_RX           0x5
#define CMD_RF_RESET        0x7     /* transceiver reset, the transceiver state
                                       will automatically end up in state TRXOFF */
/** @} */

/**
 * @name    Radio States, read from RF->RG_STATE
 * @{
 */
#define RF_STATE_TRXOFF     0x2     /* Transceiver off, SPI active */
#define RF_STATE_TXPREP     0x3     /* Transmit preparation */
#define RF_STATE_TX         0x4     /* Transmit */
#define RF_STATE_RX         0x5     /* Receive */
#define RF_STATE_TRANSITION 0x6     /* State transition in progress */
#define RF_STATE_RESET      0x7     /* Transceiver is in state RESET or SLEEP */
/** @} */

/** offset (in Hz) for CCF0 in 2.4 GHz mode */
#define CCF0_24G_OFFSET          1500000U

/** The sub-register configures the sampling frequency of the received signal.
 *  Undefined values are mapped to default setting fS=4000kHz
 *  @{
 */
#define RF_SR_4000K                     0x1
#define RF_SR_2000K                     0x2
#define RF_SR_1333K                     0x3
#define RF_SR_1000K                     0x4
#define RF_SR_800K                      0x5
#define RF_SR_666K                      0x6
#define RF_SR_500K                      0x8
#define RF_SR_400K                      0xA
/** @} */

/* The sub-register configures the relative cut-off frequency fCUT
    where 1.0 refers to half the sample frequency fS. */
/** Fcut = 0.25 * Fs/2 */
#define RF_RCUT_FS_BY_8                 (0x0 << RXDFE_RCUT_SHIFT)
/** Fcut = 0.375 * Fs/2 */
#define RF_RCUT_FS_BY_5P3               (0x1 << RXDFE_RCUT_SHIFT)
/** Fcut = 0.5 * Fs/2 */
#define RF_RCUT_FS_BY_4                 (0x2 << RXDFE_RCUT_SHIFT)
/** Fcut = 0.75 * Fs/2 */
#define RF_RCUT_FS_BY_2P6               (0x3 << RXDFE_RCUT_SHIFT)
/** Fcut = 1.0 * Fs/2 */
#define RF_RCUT_FS_BY_2                 (0x4 << RXDFE_RCUT_SHIFT)

/** The averaging time is calculated by T[μs]=DF*DTB.
 * @{
 */
#define RF_DTB_2_US                     0x0
#define RF_DTB_8_US                     0x1
#define RF_DTB_32_US                    0x2
#define RF_DTB_128_US                   0x3
/** @} */

/** BPSK, rate ½, 4 x frequency repetition */
#define BB_MCS_BPSK_REP4                0
/** BPSK, rate ½, 2 x frequency repetition */
#define BB_MCS_BPSK_REP2                1
/** QPSK, rate ½, 2 x frequency repetition */
#define BB_MCS_QPSK_REP2                2
/** QPSK, rate ½ */
#define BB_MCS_QPSK_1BY2                3
/** QPSK, rate ¾ */
#define BB_MCS_QPSK_3BY4                4
/** 16-QAM, rate ½ */
#define BB_MCS_16QAM_1BY2               5
/** 16-QAM, rate ¾ */
#define BB_MCS_16QAM_3BY4               6

/** receive only MR-O-QPSK */
#define RXM_MR_OQPSK                    0x0
/** receive only legacy O-QPSK */
#define RXM_LEGACY_OQPSK                0x1
/** receive both legacy & MR-O-QPSK */
#define RXM_BOTH_OQPSK                  0x2
/** receive nothing */
#define RXM_DISABLE                     0x3

/** Modulation Order 2-FSK */
#define FSK_MORD_2SFK                   (0 << FSKC0_MORD_SHIFT)
/** Modulation Order 4-FSK */
#define FSK_MORD_4SFK                   (1 << FSKC0_MORD_SHIFT)

/**
 * FSK modulation index
 * @{
 */
#define FSK_MIDX_3_BY_8                 (0 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_4_BY_8                 (1 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_6_BY_8                 (2 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_8_BY_8                 (3 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_10_BY_8                (4 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_12_BY_8                (5 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_14_BY_8                (6 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_16_BY_8                (7 << FSKC0_MIDX_SHIFT)
/** @} */

/**
 * FSK modulation index scale
 * @{
 */
#define FSK_MIDXS_SCALE_7_BY_8          (0 << FSKC0_MIDXS_SHIFT)
#define FSK_MIDXS_SCALE_8_BY_8          (1 << FSKC0_MIDXS_SHIFT)
#define FSK_MIDXS_SCALE_9_BY_8          (2 << FSKC0_MIDXS_SHIFT)
#define FSK_MIDXS_SCALE_10_BY_8         (3 << FSKC0_MIDXS_SHIFT)
/** @} */

/**
 * FSK bandwidth time product
 * @{
 */
#define FSK_BT_05                       (0 << FSKC0_BT_SHIFT)
#define FSK_BT_10                       (1 << FSKC0_BT_SHIFT)
#define FSK_BT_15                       (2 << FSKC0_BT_SHIFT)
#define FSK_BT_20                       (3 << FSKC0_BT_SHIFT)
/** @} */

/**
 * FSK symbol rate (kHz)
 * @{
 */
#define FSK_SRATE_50K                   0x0
#define FSK_SRATE_100K                  0x1
#define FSK_SRATE_150K                  0x2
#define FSK_SRATE_200K                  0x3
#define FSK_SRATE_300K                  0x4
#define FSK_SRATE_400K                  0x5
/** @} */

/**
 * FSK channel spacing (kHz)
 * @{
 */
#define FSK_CHANNEL_SPACING_200K        0x0
#define FSK_CHANNEL_SPACING_400K        0x1
/** @} */

/** Lower values increase the SFD detector sensitivity.
   Higher values increase the SFD selectivity.
   The default value 8 is recommended for simultaneous sensing
   of the SFD pairs according to IEEE 802.15.4g. */
#define FSKC3_SFDT(n) (((n) << FSKC3_SFDT_SHIFT) & FSKC3_SFDT_MASK)

/** Lower values increase the preamble detector sensitivity. */
#define FSKC3_PDT(n)  (((n) << FSKC3_PDT_SHIFT) & FSKC3_PDT_MASK)

#ifdef __cplusplus
}
#endif

/** @} */
