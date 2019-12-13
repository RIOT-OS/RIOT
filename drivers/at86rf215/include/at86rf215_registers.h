/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *               2014 Atmel Corporation
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Register Definitions for the AT86RF215 chip
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#ifndef AT86RF215_REGISTERS_H
#define AT86RF215_REGISTERS_H

#include <stdint.h>

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
 * @name    Enumeration for RF IRQs used for IRQS and IRQM registers
 * @{
 */
typedef enum rf_irq_tag {
    RF_IRQ_IQIFSF   =       0x20,
    RF_IRQ_TRXERR   =       0x10,
    RF_IRQ_BATLOW   =       0x08,
    RF_IRQ_EDC      =       0x04,
    RF_IRQ_TRXRDY   =       0x02,
    RF_IRQ_WAKEUP   =       0x01,
    RF_IRQ_NO_IRQ   =       0x00,
    RF_IRQ_ALL_IRQ  =       0x3F
} rf_irq_t;
/** @} */

/**
 * @name    Enumeration for Baseband IRQs used for IRQS and IRQM registers
 * @{
 */
typedef enum bb_irq_tag {
    BB_IRQ_FBLI     =       0x80,
    BB_IRQ_AGCR     =       0x40,
    BB_IRQ_AGCH     =       0x20,
    BB_IRQ_TXFE     =       0x10,
    BB_IRQ_RXEM     =       0x08,
    BB_IRQ_RXAM     =       0x04,
    BB_IRQ_RXFE     =       0x02,
    BB_IRQ_RXFS     =       0x01,
    BB_IRQ_NO_IRQ   =       0x00,
    BB_IRQ_ALL_IRQ  =       0xFF
} bb_irq_t;
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

/**
 * @name    AT86RF215 registers
 * @{
 */

/** Bit Mask for Sub-Register IRQS.WAKEUP */
#define IRQS_WAKEUP_MASK                0x01
/** Bit Offset for Sub-Register IRQS.WAKEUP */
#define IRQS_WAKEUP_SHIFT               0
/** Bit Mask for Sub-Register IRQS.TRXRDY */
#define IRQS_TRXRDY_MASK                0x02
/** Bit Offset for Sub-Register IRQS.TRXRDY */
#define IRQS_TRXRDY_SHIFT               1
/** Bit Mask for Sub-Register IRQS.EDC */
#define IRQS_EDC_MASK                   0x04
/** Bit Offset for Sub-Register IRQS.EDC */
#define IRQS_EDC_SHIFT                  2
/** Bit Mask for Sub-Register IRQS.BATLOW */
#define IRQS_BATLOW_MASK                0x08
/** Bit Offset for Sub-Register IRQS.BATLOW */
#define IRQS_BATLOW_SHIFT               3
/** Bit Mask for Sub-Register IRQS.TRXERR */
#define IRQS_TRXERR_MASK                0x10
/** Bit Offset for Sub-Register IRQS.TRXERR */
#define IRQS_TRXERR_SHIFT               4
/** Bit Mask for Sub-Register IRQS.IQIFSF */
#define IRQS_IQIFSF_MASK                0x20
/** Bit Offset for Sub-Register IRQS.IQIFSF */
#define IRQS_IQIFSF_SHIFT               5
/** Bit Mask for Sub-Register IRQS.RXFS */
#define IRQS_RXFS_MASK                  0x01
/** Bit Offset for Sub-Register IRQS.RXFS */
#define IRQS_RXFS_SHIFT                 0
/** Bit Mask for Sub-Register IRQS.RXFE */
#define IRQS_RXFE_MASK                  0x02
/** Bit Offset for Sub-Register IRQS.RXFE */
#define IRQS_RXFE_SHIFT                 1
/** Bit Mask for Sub-Register IRQS.RXAM */
#define IRQS_RXAM_MASK                  0x04
/** Bit Offset for Sub-Register IRQS.RXAM */
#define IRQS_RXAM_SHIFT                 2
/** Bit Mask for Sub-Register IRQS.RXEM */
#define IRQS_RXEM_MASK                  0x08
/** Bit Offset for Sub-Register IRQS.RXEM */
#define IRQS_RXEM_SHIFT                 3
/** Bit Mask for Sub-Register IRQS.TXFE */
#define IRQS_TXFE_MASK                  0x10
/** Bit Offset for Sub-Register IRQS.TXFE */
#define IRQS_TXFE_SHIFT                 4
/** Bit Mask for Sub-Register IRQS.AGCH */
#define IRQS_AGCH_MASK                  0x20
/** Bit Offset for Sub-Register IRQS.AGCH */
#define IRQS_AGCH_SHIFT                 5
/** Bit Mask for Sub-Register IRQS.AGCR */
#define IRQS_AGCR_MASK                  0x40
/** Bit Offset for Sub-Register IRQS.AGCR */
#define IRQS_AGCR_SHIFT                 6
/** Bit Mask for Sub-Register IRQS.FBLI */
#define IRQS_FBLI_MASK                  0x80
/** Bit Offset for Sub-Register IRQS.FBLI */
#define IRQS_FBLI_SHIFT                 7
/** Bit Mask for Sub-Register RST.CMD */
#define RST_CMD_MASK                    0x07
/** Bit Offset for Sub-Register RST.CMD */
#define RST_CMD_SHIFT                   0
/** Bit Mask for Sub-Register CFG.DRV */
#define CFG_DRV_MASK                    0x03
/** Bit Offset for Sub-Register CFG.DRV */
#define CFG_DRV_SHIFT                   0
/** Bit Mask for Sub-Register CFG.IRQP */
#define CFG_IRQP_MASK                   0x04
/** Bit Offset for Sub-Register CFG.IRQP */
#define CFG_IRQP_SHIFT                  2
/** Bit Mask for Sub-Register CFG.IRQMM */
#define CFG_IRQMM_MASK                  0x08
/** Bit Offset for Sub-Register CFG.IRQMM */
#define CFG_IRQMM_SHIFT                 3
/** Bit Mask for Sub-Register CLKO.OS */
#define CLKO_OS_MASK                    0x07
/** Bit Offset for Sub-Register CLKO.OS */
#define CLKO_OS_SHIFT                   0
/** Bit Mask for Sub-Register CLKO.DRV */
#define CLKO_DRV_MASK                   0x18
/** Bit Offset for Sub-Register CLKO.DRV */
#define CLKO_DRV_SHIFT                  3
/** Bit Mask for Sub-Register BMDVC.BMVTH */
#define BMDVC_BMVTH_MASK                0x0F
/** Bit Offset for Sub-Register BMDVC.BMVTH */
#define BMDVC_BMVTH_SHIFT               0
/** Bit Mask for Sub-Register BMDVC.BMHR */
#define BMDVC_BMHR_MASK                 0x10
/** Bit Offset for Sub-Register BMDVC.BMHR */
#define BMDVC_BMHR_SHIFT                4
/** Bit Mask for Sub-Register BMDVC.BMS */
#define BMDVC_BMS_MASK                  0x20
/** Bit Offset for Sub-Register BMDVC.BMS */
#define BMDVC_BMS_SHIFT                 5
/** Bit Mask for Sub-Register XOC.TRIM */
#define XOC_TRIM_MASK                   0x0F
/** Bit Offset for Sub-Register XOC.TRIM */
#define XOC_TRIM_SHIFT                  0
/** Bit Mask for Sub-Register XOC.FS */
#define XOC_FS_MASK                     0x10
/** Bit Offset for Sub-Register XOC.FS */
#define XOC_FS_SHIFT                    4
/** Bit Mask for Sub-Register IQIFC0.EEC */
#define IQIFC0_EEC_MASK                 0x01
/** Bit Offset for Sub-Register IQIFC0.EEC */
#define IQIFC0_EEC_SHIFT                0
/** Bit Mask for Sub-Register IQIFC0.CMV1V2 */
#define IQIFC0_CMV1V2_MASK              0x02
/** Bit Offset for Sub-Register IQIFC0.CMV1V2 */
#define IQIFC0_CMV1V2_SHIFT             1
/** Bit Mask for Sub-Register IQIFC0.CMV */
#define IQIFC0_CMV_MASK                 0x0C
/** Bit Offset for Sub-Register IQIFC0.CMV */
#define IQIFC0_CMV_SHIFT                2
/** Bit Mask for Sub-Register IQIFC0.DRV */
#define IQIFC0_DRV_MASK                 0x30
/** Bit Offset for Sub-Register IQIFC0.DRV */
#define IQIFC0_DRV_SHIFT                4
/** Bit Mask for Sub-Register IQIFC0.SF */
#define IQIFC0_SF_MASK                  0x40
/** Bit Offset for Sub-Register IQIFC0.SF */
#define IQIFC0_SF_SHIFT                 6
/** Bit Mask for Sub-Register IQIFC0.EXTLB */
#define IQIFC0_EXTLB_MASK               0x80
/** Bit Offset for Sub-Register IQIFC0.EXTLB */
#define IQIFC0_EXTLB_SHIFT              7
/** Bit Mask for Sub-Register IQIFC1.SKEWDRV */
#define IQIFC1_SKEWDRV_MASK             0x03
/** Bit Offset for Sub-Register IQIFC1.SKEWDRV */
#define IQIFC1_SKEWDRV_SHIFT            0
/** Bit Mask for Sub-Register IQIFC1.CHPM */
#define IQIFC1_CHPM_MASK                0x70
/** Bit Offset for Sub-Register IQIFC1.CHPM */
#define IQIFC1_CHPM_SHIFT               4
/** Bit Mask for Sub-Register IQIFC1.FAILSF */
#define IQIFC1_FAILSF_MASK              0x80
/** Bit Offset for Sub-Register IQIFC1.FAILSF */
#define IQIFC1_FAILSF_SHIFT             7
/** Bit Mask for Sub-Register IQIFC2.SYNC */
#define IQIFC2_SYNC_MASK                0x80
/** Bit Offset for Sub-Register IQIFC2.SYNC */
#define IQIFC2_SYNC_SHIFT               7
/** Bit Mask for Sub-Register PN.PN */
#define PN_PN_MASK                      0xFF
/** Bit Offset for Sub-Register PN.PN */
#define PN_PN_SHIFT                     0
/** Bit Mask for Sub-Register VN.VN */
#define VN_VN_MASK                      0xFF
/** Bit Offset for Sub-Register VN.VN */
#define VN_VN_SHIFT                     0
/** Bit Mask for Sub-Register IRQM.WAKEUP */
#define IRQM_WAKEUP_MASK                0x01
/** Bit Offset for Sub-Register IRQM.WAKEUP */
#define IRQM_WAKEUP_SHIFT               0
/** Bit Mask for Sub-Register IRQM.TRXRDY */
#define IRQM_TRXRDY_MASK                0x02
/** Bit Offset for Sub-Register IRQM.TRXRDY */
#define IRQM_TRXRDY_SHIFT               1
/** Bit Mask for Sub-Register IRQM.EDC */
#define IRQM_EDC_MASK                   0x04
/** Bit Offset for Sub-Register IRQM.EDC */
#define IRQM_EDC_SHIFT                  2
/** Bit Mask for Sub-Register IRQM.BATLOW */
#define IRQM_BATLOW_MASK                0x08
/** Bit Offset for Sub-Register IRQM.BATLOW */
#define IRQM_BATLOW_SHIFT               3
/** Bit Mask for Sub-Register IRQM.TRXERR */
#define IRQM_TRXERR_MASK                0x10
/** Bit Offset for Sub-Register IRQM.TRXERR */
#define IRQM_TRXERR_SHIFT               4
/** Bit Mask for Sub-Register IRQM.IQIFSF */
#define IRQM_IQIFSF_MASK                0x20
/** Bit Offset for Sub-Register IRQM.IQIFSF */
#define IRQM_IQIFSF_SHIFT               5
/** Bit Mask for Sub-Register AUXS.PAVC */
#define AUXS_PAVC_MASK                  0x03
/** Bit Offset for Sub-Register AUXS.PAVC */
#define AUXS_PAVC_SHIFT                 0
/** Bit Mask for Sub-Register AUXS.AVS */
#define AUXS_AVS_MASK                   0x04
/** Bit Offset for Sub-Register AUXS.AVS */
#define AUXS_AVS_SHIFT                  2
/** Bit Mask for Sub-Register AUXS.AVEN */
#define AUXS_AVEN_MASK                  0x08
/** Bit Offset for Sub-Register AUXS.AVEN */
#define AUXS_AVEN_SHIFT                 3
/** Bit Mask for Sub-Register AUXS.AVEXT */
#define AUXS_AVEXT_MASK                 0x10
/** Bit Offset for Sub-Register AUXS.AVEXT */
#define AUXS_AVEXT_SHIFT                4
/** Bit Mask for Sub-Register AUXS.AGCMAP */
#define AUXS_AGCMAP_MASK                0x60
/** Bit Offset for Sub-Register AUXS.AGCMAP */
#define AUXS_AGCMAP_SHIFT               5
/** Bit Mask for Sub-Register AUXS.EXTLNABYP */
#define AUXS_EXTLNABYP_MASK             0x80
/** Bit Offset for Sub-Register AUXS.EXTLNABYP */
#define AUXS_EXTLNABYP_SHIFT            7
/** Bit Mask for Sub-Register STATE.STATE */
#define STATE_STATE_MASK                0x07
/** Bit Offset for Sub-Register STATE.STATE */
#define STATE_STATE_SHIFT               0
/** Bit Mask for Sub-Register CMD.CMD */
#define CMD_CMD_MASK                    0x07
/** Bit Offset for Sub-Register CMD.CMD */
#define CMD_CMD_SHIFT                   0
/** Bit Mask for Sub-Register CS.CS */
#define CS_CS_MASK                      0xFF
/** Bit Offset for Sub-Register CS.CS */
#define CS_CS_SHIFT                     0
/** Bit Mask for Sub-Register CCF0L.CCF0L */
#define CCF0L_CCF0L_MASK                0xFF
/** Bit Offset for Sub-Register CCF0L.CCF0L */
#define CCF0L_CCF0L_SHIFT               0
/** Bit Mask for Sub-Register CCF0H.CCF0H */
#define CCF0H_CCF0H_MASK                0xFF
/** Bit Offset for Sub-Register CCF0H.CCF0H */
#define CCF0H_CCF0H_SHIFT               0

/* offset (in Hz) for CCF0 in 2.4 GHz mode */
#define CCF0_24G_OFFSET          1500000U

/** Bit Mask for Sub-Register CNL.CNL */
#define CNL_CNL_MASK                    0xFF
/** Bit Offset for Sub-Register CNL.CNL */
#define CNL_CNL_SHIFT                   0
/** Bit Mask for Sub-Register CNM.CNH */
#define CNM_CNH_MASK                    0x01
/** Bit Offset for Sub-Register CNM.CNH */
#define CNM_CNH_SHIFT                   0
/** Bit Mask for Sub-Register CNM.CM */
#define CNM_CM_MASK                     0xC0
/** Bit Offset for Sub-Register CNM.CM */
#define CNM_CM_SHIFT                    6
/** Bit Mask for Sub-Register RXBWC.BW */
#define RXBWC_BW_MASK                   0x0F
/** Bit Offset for Sub-Register RXBWC.BW */
#define RXBWC_BW_SHIFT                  0
/** Bit Mask for Sub-Register RXBWC.IFS */
#define RXBWC_IFS_MASK                  0x10
/** Bit Offset for Sub-Register RXBWC.IFS */
#define RXBWC_IFS_SHIFT                 4
/** Bit Mask for Sub-Register RXBWC.IFI */
#define RXBWC_IFI_MASK                  0x20
/** Bit Offset for Sub-Register RXBWC.IFI */
#define RXBWC_IFI_SHIFT                 5
/** Bit Mask for Sub-Register RXDFE.SR */
#define RXDFE_SR_MASK                   0x0F
/** Bit Offset for Sub-Register RXDFE.SR */
#define RXDFE_SR_SHIFT                  0

/* The sub-register configures the sampling frequency of the received signal.
   Undefined values are mapped to default setting fS=4000kHz */
#define RF_SR_4000K                     0x1
#define RF_SR_2000K                     0x2
#define RF_SR_1333K                     0x3
#define RF_SR_1000K                     0x4
#define RF_SR_800K                      0x5
#define RF_SR_666K                      0x6
#define RF_SR_500K                      0x8
#define RF_SR_400K                      0xA

/** Bit Mask for Sub-Register RXDFE.RCUT */
#define RXDFE_RCUT_MASK                 0xE0
/** Bit Offset for Sub-Register RXDFE.RCUT */
#define RXDFE_RCUT_SHIFT                5

/* The sub-register configures the relative cut-off frequency fCUT
    where 1.0 refers to half the sample frequency fS. */
/* Fcut = 0.25 * Fs/2 */
#define RF_RCUT_FS_BY_8                 (0x0 << RXDFE_RCUT_SHIFT)
/* Fcut = 0.375 * Fs/2 */
#define RF_RCUT_FS_BY_5P3               (0x1 << RXDFE_RCUT_SHIFT)
/* Fcut = 0.5 * Fs/2 */
#define RF_RCUT_FS_BY_4                 (0x2 << RXDFE_RCUT_SHIFT)
/* Fcut = 0.75 * Fs/2 */
#define RF_RCUT_FS_BY_2P6               (0x3 << RXDFE_RCUT_SHIFT)
/* Fcut = 1.0 * Fs/2 */
#define RF_RCUT_FS_BY_2                 (0x4 << RXDFE_RCUT_SHIFT)

/** Bit Mask for Sub-Register AGCC.EN */
#define AGCC_EN_MASK                    0x01
/** Bit Offset for Sub-Register AGCC.EN */
#define AGCC_EN_SHIFT                   0
/** Bit Mask for Sub-Register AGCC.FRZC */
#define AGCC_FRZC_MASK                  0x02
/** Bit Offset for Sub-Register AGCC.FRZC */
#define AGCC_FRZC_SHIFT                 1
/** Bit Mask for Sub-Register AGCC.FRZS */
#define AGCC_FRZS_MASK                  0x04
/** Bit Offset for Sub-Register AGCC.FRZS */
#define AGCC_FRZS_SHIFT                 2
/** Bit Mask for Sub-Register AGCC.RST */
#define AGCC_RST_MASK                   0x08
/** Bit Offset for Sub-Register AGCC.RST */
#define AGCC_RST_SHIFT                  3
/** Bit Mask for Sub-Register AGCC.AVGS */
#define AGCC_AVGS_MASK                  0x30
/** Bit Offset for Sub-Register AGCC.AVGS */
#define AGCC_AVGS_SHIFT                 4
/** Bit Mask for Sub-Register AGCC.AGCI */
#define AGCC_AGCI_MASK                  0x40
/** Bit Offset for Sub-Register AGCC.AGCI */
#define AGCC_AGCI_SHIFT                 6
/** Bit Mask for Sub-Register AGCS.GCW */
#define AGCS_GCW_MASK                   0x1F
/** Bit Offset for Sub-Register AGCS.GCW */
#define AGCS_GCW_SHIFT                  0
/** Bit Mask for Sub-Register AGCS.TGT */
#define AGCS_TGT_MASK                   0xE0
/** Bit Offset for Sub-Register AGCS.TGT */
#define AGCS_TGT_SHIFT                  5
/** Bit Mask for Sub-Register RSSI.RSSI */
#define RSSI_RSSI_MASK                  0xFF
/** Bit Offset for Sub-Register RSSI.RSSI */
#define RSSI_RSSI_SHIFT                 0
/** Bit Mask for Sub-Register EDC.EDM */
#define EDC_EDM_MASK                    0x03
/** Bit Offset for Sub-Register EDC.EDM */
#define EDC_EDM_SHIFT                   0
/** Bit Mask for Sub-Register EDD.DTB */
#define EDD_DTB_MASK                    0x03
/** Bit Offset for Sub-Register EDD.DTB */
#define EDD_DTB_SHIFT                   0

/* The averaging time is calculated by T[μs]=DF*DTB. */
#define RF_DTB_2_US                     0x0
#define RF_DTB_8_US                     0x1
#define RF_DTB_32_US                    0x2
#define RF_DTB_128_US                   0x3

/** Bit Mask for Sub-Register EDD.DF */
#define EDD_DF_MASK                     0xFC
/** Bit Offset for Sub-Register EDD.DF */
#define EDD_DF_SHIFT                    2
/** Bit Mask for Sub-Register EDV.EDV */
#define EDV_EDV_MASK                    0xFF
/** Bit Offset for Sub-Register EDV.EDV */
#define EDV_EDV_SHIFT                   0
/** Bit Mask for Sub-Register RNDV.RNDV */
#define RNDV_RNDV_MASK                  0xFF
/** Bit Offset for Sub-Register RNDV.RNDV */
#define RNDV_RNDV_SHIFT                 0
/** Bit Mask for Sub-Register TXCUTC.LPFCUT */
#define TXCUTC_LPFCUT_MASK              0x0F
/** Bit Offset for Sub-Register TXCUTC.LPFCUT */
#define TXCUTC_LPFCUT_SHIFT             0
/** Bit Mask for Sub-Register TXCUTC.PARAMP */
#define TXCUTC_PARAMP_MASK              0xC0
/** Bit Offset for Sub-Register TXCUTC.PARAMP */
#define TXCUTC_PARAMP_SHIFT             6
/** Bit Mask for Sub-Register TXDFE.SR */
#define TXDFE_SR_MASK                   0x0F
/** Bit Offset for Sub-Register TXDFE.SR */
#define TXDFE_SR_SHIFT                  0
/** Bit Mask for Sub-Register TXDFE.DM */
#define TXDFE_DM_MASK                   0x10
/** Bit Offset for Sub-Register TXDFE.DM */
#define TXDFE_DM_SHIFT                  4
/** Bit Mask for Sub-Register TXDFE.RCUT */
#define TXDFE_RCUT_MASK                 0xE0
/** Bit Offset for Sub-Register TXDFE.RCUT */
#define TXDFE_RCUT_SHIFT                5
/** Bit Mask for Sub-Register PAC.TXPWR */
#define PAC_TXPWR_MASK                  0x1F
/** Bit Offset for Sub-Register PAC.TXPWR */
#define PAC_TXPWR_SHIFT                 0
/** Bit Mask for Sub-Register PAC.PACUR */
#define PAC_PACUR_MASK                  0x60
/** Bit Offset for Sub-Register PAC.PACUR */
#define PAC_PACUR_SHIFT                 5
/** Bit Mask for Sub-Register PADFE.PADFE */
#define PADFE_PADFE_MASK                0xC0
/** Bit Offset for Sub-Register PADFE.PADFE */
#define PADFE_PADFE_SHIFT               6
/** Bit Mask for Sub-Register PLL.LS */
#define PLL_LS_MASK                     0x02
/** Bit Offset for Sub-Register PLL.LS */
#define PLL_LS_SHIFT                    1
/** Bit Mask for Sub-Register PLL.LBW */
#define PLL_LBW_MASK                    0x30
/** Bit Offset for Sub-Register PLL.LBW */
#define PLL_LBW_SHIFT                   4
/** Bit Mask for Sub-Register PLLCF.CF */
#define PLLCF_CF_MASK                   0x3F
/** Bit Offset for Sub-Register PLLCF.CF */
#define PLLCF_CF_SHIFT                  0
/** Bit Mask for Sub-Register TXCI.DCOI */
#define TXCI_DCOI_MASK                  0x3F
/** Bit Offset for Sub-Register TXCI.DCOI */
#define TXCI_DCOI_SHIFT                 0
/** Bit Mask for Sub-Register TXCQ.DCOQ */
#define TXCQ_DCOQ_MASK                  0x3F
/** Bit Offset for Sub-Register TXCQ.DCOQ */
#define TXCQ_DCOQ_SHIFT                 0
/** Bit Mask for Sub-Register TXDACI.TXDACID */
#define TXDACI_TXDACID_MASK             0x7F
/** Bit Offset for Sub-Register TXDACI.TXDACID */
#define TXDACI_TXDACID_SHIFT            0
/** Bit Mask for Sub-Register TXDACI.ENTXDACID */
#define TXDACI_ENTXDACID_MASK           0x80
/** Bit Offset for Sub-Register TXDACI.ENTXDACID */
#define TXDACI_ENTXDACID_SHIFT          7
/** Bit Mask for Sub-Register TXDACQ.TXDACQD */
#define TXDACQ_TXDACQD_MASK             0x7F
/** Bit Offset for Sub-Register TXDACQ.TXDACQD */
#define TXDACQ_TXDACQD_SHIFT            0
/** Bit Mask for Sub-Register TXDACQ.ENTXDACQD */
#define TXDACQ_ENTXDACQD_MASK           0x80
/** Bit Offset for Sub-Register TXDACQ.ENTXDACQD */
#define TXDACQ_ENTXDACQD_SHIFT          7
/** Bit Mask for Sub-Register IRQM.RXFS */
#define IRQM_RXFS_MASK                  0x01
/** Bit Offset for Sub-Register IRQM.RXFS */
#define IRQM_RXFS_SHIFT                 0
/** Bit Mask for Sub-Register IRQM.RXFE */
#define IRQM_RXFE_MASK                  0x02
/** Bit Offset for Sub-Register IRQM.RXFE */
#define IRQM_RXFE_SHIFT                 1
/** Bit Mask for Sub-Register IRQM.RXAM */
#define IRQM_RXAM_MASK                  0x04
/** Bit Offset for Sub-Register IRQM.RXAM */
#define IRQM_RXAM_SHIFT                 2
/** Bit Mask for Sub-Register IRQM.RXEM */
#define IRQM_RXEM_MASK                  0x08
/** Bit Offset for Sub-Register IRQM.RXEM */
#define IRQM_RXEM_SHIFT                 3
/** Bit Mask for Sub-Register IRQM.TXFE */
#define IRQM_TXFE_MASK                  0x10
/** Bit Offset for Sub-Register IRQM.TXFE */
#define IRQM_TXFE_SHIFT                 4
/** Bit Mask for Sub-Register IRQM.AGCH */
#define IRQM_AGCH_MASK                  0x20
/** Bit Offset for Sub-Register IRQM.AGCH */
#define IRQM_AGCH_SHIFT                 5
/** Bit Mask for Sub-Register IRQM.AGCR */
#define IRQM_AGCR_MASK                  0x40
/** Bit Offset for Sub-Register IRQM.AGCR */
#define IRQM_AGCR_SHIFT                 6
/** Bit Mask for Sub-Register IRQM.FBLI */
#define IRQM_FBLI_MASK                  0x80
/** Bit Offset for Sub-Register IRQM.FBLI */
#define IRQM_FBLI_SHIFT                 7
/** Bit Mask for Sub-Register PC.PT */
#define PC_PT_MASK                      0x03
/** Bit Offset for Sub-Register PC.PT */
#define PC_PT_SHIFT                     0
/** Bit Mask for Sub-Register PC.BBEN */
#define PC_BBEN_MASK                    0x04
/** Bit Offset for Sub-Register PC.BBEN */
#define PC_BBEN_SHIFT                   2
/** Bit Mask for Sub-Register PC.FCST */
#define PC_FCST_MASK                    0x08
/** Bit Offset for Sub-Register PC.FCST */
#define PC_FCST_SHIFT                   3
/** Bit Mask for Sub-Register PC.TXAFCS */
#define PC_TXAFCS_MASK                  0x10
/** Bit Offset for Sub-Register PC.TXAFCS */
#define PC_TXAFCS_SHIFT                 4
/** Bit Mask for Sub-Register PC.FCSOK */
#define PC_FCSOK_MASK                   0x20
/** Bit Offset for Sub-Register PC.FCSOK */
#define PC_FCSOK_SHIFT                  5
/** Bit Mask for Sub-Register PC.FCSFE */
#define PC_FCSFE_MASK                   0x40
/** Bit Offset for Sub-Register PC.FCSFE */
#define PC_FCSFE_SHIFT                  6
/** Bit Mask for Sub-Register PC.CTX */
#define PC_CTX_MASK                     0x80
/** Bit Offset for Sub-Register PC.CTX */
#define PC_CTX_SHIFT                    7
/** Bit Mask for Sub-Register PS.TXUR */
#define PS_TXUR_MASK                    0x01
/** Bit Offset for Sub-Register PS.TXUR */
#define PS_TXUR_SHIFT                   0
/** Bit Mask for Sub-Register RXFLL.RXFLL */
#define RXFLL_RXFLL_MASK                0xFF
/** Bit Offset for Sub-Register RXFLL.RXFLL */
#define RXFLL_RXFLL_SHIFT               0
/** Bit Mask for Sub-Register RXFLH.RXFLH */
#define RXFLH_RXFLH_MASK                0x07
/** Bit Offset for Sub-Register RXFLH.RXFLH */
#define RXFLH_RXFLH_SHIFT               0
/** Bit Mask for Sub-Register TXFLL.TXFLL */
#define TXFLL_TXFLL_MASK                0xFF
/** Bit Offset for Sub-Register TXFLL.TXFLL */
#define TXFLL_TXFLL_SHIFT               0
/** Bit Mask for Sub-Register TXFLH.TXFLH */
#define TXFLH_TXFLH_MASK                0x07
/** Bit Offset for Sub-Register TXFLH.TXFLH */
#define TXFLH_TXFLH_SHIFT               0
/** Bit Mask for Sub-Register FBLL.FBLL */
#define FBLL_FBLL_MASK                  0xFF
/** Bit Offset for Sub-Register FBLL.FBLL */
#define FBLL_FBLL_SHIFT                 0
/** Bit Mask for Sub-Register FBLH.FBLH */
#define FBLH_FBLH_MASK                  0x07
/** Bit Offset for Sub-Register FBLH.FBLH */
#define FBLH_FBLH_SHIFT                 0
/** Bit Mask for Sub-Register FBLIL.FBLIL */
#define FBLIL_FBLIL_MASK                0xFF
/** Bit Offset for Sub-Register FBLIL.FBLIL */
#define FBLIL_FBLIL_SHIFT               0
/** Bit Mask for Sub-Register FBLIH.FBLIH */
#define FBLIH_FBLIH_MASK                0x07
/** Bit Offset for Sub-Register FBLIH.FBLIH */
#define FBLIH_FBLIH_SHIFT               0
/** Bit Mask for Sub-Register OFDMPHRTX.MCS */
#define OFDMPHRTX_MCS_MASK              0x07
/** Bit Offset for Sub-Register OFDMPHRTX.MCS */
#define OFDMPHRTX_MCS_SHIFT             0

/* BPSK, rate ½, 4 x frequency repetition */
#define BB_MCS_BPSK_REP4                0
/* BPSK, rate ½, 2 x frequency repetition */
#define BB_MCS_BPSK_REP2                1
/* QPSK, rate ½, 2 x frequency repetition */
#define BB_MCS_QPSK_REP2                2
/* QPSK, rate ½ */
#define BB_MCS_QPSK_1BY2                3
/* QPSK, rate ¾ */
#define BB_MCS_QPSK_3BY4                4
/* 16-QAM, rate ½ */
#define BB_MCS_16QAM_1BY2               5
/* 16-QAM, rate ¾ */
#define BB_MCS_16QAM_3BY4               6

/** Bit Mask for Sub-Register OFDMPHRTX.RB5 */
#define OFDMPHRTX_RB5_MASK              0x10
/** Bit Offset for Sub-Register OFDMPHRTX.RB5 */
#define OFDMPHRTX_RB5_SHIFT             4
/** Bit Mask for Sub-Register OFDMPHRTX.RB17 */
#define OFDMPHRTX_RB17_MASK             0x20
/** Bit Offset for Sub-Register OFDMPHRTX.RB17 */
#define OFDMPHRTX_RB17_SHIFT            5
/** Bit Mask for Sub-Register OFDMPHRTX.RB18 */
#define OFDMPHRTX_RB18_MASK             0x40
/** Bit Offset for Sub-Register OFDMPHRTX.RB18 */
#define OFDMPHRTX_RB18_SHIFT            6
/** Bit Mask for Sub-Register OFDMPHRTX.RB21 */
#define OFDMPHRTX_RB21_MASK             0x80
/** Bit Offset for Sub-Register OFDMPHRTX.RB21 */
#define OFDMPHRTX_RB21_SHIFT            7
/** Bit Mask for Sub-Register OFDMPHRRX.MCS */
#define OFDMPHRRX_MCS_MASK              0x07
/** Bit Offset for Sub-Register OFDMPHRRX.MCS */
#define OFDMPHRRX_MCS_SHIFT             0
/** Bit Mask for Sub-Register OFDMPHRRX.SPC */
#define OFDMPHRRX_SPC_MASK              0x08
/** Bit Offset for Sub-Register OFDMPHRRX.SPC */
#define OFDMPHRRX_SPC_SHIFT             3
/** Bit Mask for Sub-Register OFDMPHRRX.RB5 */
#define OFDMPHRRX_RB5_MASK              0x10
/** Bit Offset for Sub-Register OFDMPHRRX.RB5 */
#define OFDMPHRRX_RB5_SHIFT             4
/** Bit Mask for Sub-Register OFDMPHRRX.RB17 */
#define OFDMPHRRX_RB17_MASK             0x20
/** Bit Offset for Sub-Register OFDMPHRRX.RB17 */
#define OFDMPHRRX_RB17_SHIFT            5
/** Bit Mask for Sub-Register OFDMPHRRX.RB18 */
#define OFDMPHRRX_RB18_MASK             0x40
/** Bit Offset for Sub-Register OFDMPHRRX.RB18 */
#define OFDMPHRRX_RB18_SHIFT            6
/** Bit Mask for Sub-Register OFDMPHRRX.RB21 */
#define OFDMPHRRX_RB21_MASK             0x80
/** Bit Offset for Sub-Register OFDMPHRRX.RB21 */
#define OFDMPHRRX_RB21_SHIFT            7
/** Bit Mask for Sub-Register OFDMC.OPT */
#define OFDMC_OPT_MASK                  0x03
/** Bit Offset for Sub-Register OFDMC.OPT */
#define OFDMC_OPT_SHIFT                 0
/** Bit Mask for Sub-Register OFDMC.POI */
#define OFDMC_POI_MASK                  0x04
/** Bit Offset for Sub-Register OFDMC.POI */
#define OFDMC_POI_SHIFT                 2
/** Bit Mask for Sub-Register OFDMC.LFO */
#define OFDMC_LFO_MASK                  0x08
/** Bit Offset for Sub-Register OFDMC.LFO */
#define OFDMC_LFO_SHIFT                 3
/** Bit Mask for Sub-Register OFDMC.SSTX */
#define OFDMC_SSTX_MASK                 0x30
/** Bit Offset for Sub-Register OFDMC.SSTX */
#define OFDMC_SSTX_SHIFT                4
/** Bit Mask for Sub-Register OFDMC.SSRX */
#define OFDMC_SSRX_MASK                 0xC0
/** Bit Offset for Sub-Register OFDMC.SSRX */
#define OFDMC_SSRX_SHIFT                6
/** Bit Mask for Sub-Register OFDMSW.RXO */
#define OFDMSW_RXO_MASK                 0x10
/** Bit Offset for Sub-Register OFDMSW.RXO */
#define OFDMSW_RXO_SHIFT                4
/** Bit Mask for Sub-Register OFDMSW.PDT */
#define OFDMSW_PDT_MASK                 0xE0
/** Bit Offset for Sub-Register OFDMSW.PDT */
#define OFDMSW_PDT_SHIFT                5
/** Bit Mask for Sub-Register OQPSKC0.FCHIP */
#define OQPSKC0_FCHIP_MASK              0x03
/** Bit Offset for Sub-Register OQPSKC0.FCHIP */
#define OQPSKC0_FCHIP_SHIFT             0
/** Bit Mask for Sub-Register OQPSKC0.MOD */
#define OQPSKC0_MOD_MASK                0x08
/** Bit Offset for Sub-Register OQPSKC0.MOD */
#define OQPSKC0_MOD_SHIFT               3
/** Bit Mask for Sub-Register OQPSKC0.DM */
#define OQPSKC0_DM_MASK                 0x10
/** Bit Offset for Sub-Register OQPSKC0.DM */
#define OQPSKC0_DM_SHIFT                4
/** Bit Mask for Sub-Register OQPSKC1.PDT0 */
#define OQPSKC1_PDT0_MASK               0x07
/** Bit Offset for Sub-Register OQPSKC1.PDT0 */
#define OQPSKC1_PDT0_SHIFT              0
/** Bit Mask for Sub-Register OQPSKC1.PDT1 */
#define OQPSKC1_PDT1_MASK               0x38
/** Bit Offset for Sub-Register OQPSKC1.PDT1 */
#define OQPSKC1_PDT1_SHIFT              3
/** Bit Mask for Sub-Register OQPSKC1.RXOLEG */
#define OQPSKC1_RXOLEG_MASK             0x40
/** Bit Offset for Sub-Register OQPSKC1.RXOLEG */
#define OQPSKC1_RXOLEG_SHIFT            6
/** Bit Mask for Sub-Register OQPSKC1.RXO */
#define OQPSKC1_RXO_MASK                0x80
/** Bit Offset for Sub-Register OQPSKC1.RXO */
#define OQPSKC1_RXO_SHIFT               7
/** Bit Mask for Sub-Register OQPSKC2.RXM */
#define OQPSKC2_RXM_MASK                0x03
/** Bit Offset for Sub-Register OQPSKC2.RXM */
#define OQPSKC2_RXM_SHIFT               0

#define RXM_MR_OQPSK                    0x0
#define RXM_LEGACY_OQPSK                0x1
#define RXM_BOTH_OQPSK                  0x2
#define RXM_DISABLE                     0x3

/** Bit Mask for Sub-Register OQPSKC2.FCSTLEG */
#define OQPSKC2_FCSTLEG_MASK            0x04
/** Bit Offset for Sub-Register OQPSKC2.FCSTLEG */
#define OQPSKC2_FCSTLEG_SHIFT           2
/** Bit Mask for Sub-Register OQPSKC2.ENPROP */
#define OQPSKC2_ENPROP_MASK             0x08
/** Bit Offset for Sub-Register OQPSKC2.ENPROP */
#define OQPSKC2_ENPROP_SHIFT            3
/** Bit Mask for Sub-Register OQPSKC2.RPC */
#define OQPSKC2_RPC_MASK                0x10
/** Bit Offset for Sub-Register OQPSKC2.RPC */
#define OQPSKC2_RPC_SHIFT               4
/** Bit Mask for Sub-Register OQPSKC2.SPC */
#define OQPSKC2_SPC_MASK                0x20
/** Bit Offset for Sub-Register OQPSKC2.SPC */
#define OQPSKC2_SPC_SHIFT               5
/** Bit Mask for Sub-Register OQPSKC3.NSFD */
#define OQPSKC3_NSFD_MASK               0x0C
/** Bit Offset for Sub-Register OQPSKC3.NSFD */
#define OQPSKC3_NSFD_SHIFT              2
/** Bit Mask for Sub-Register OQPSKC3.HRLEG */
#define OQPSKC3_HRLEG_MASK              0x20
/** Bit Offset for Sub-Register OQPSKC3.HRLEG */
#define OQPSKC3_HRLEG_SHIFT             5
/** Bit Mask for Sub-Register OQPSKPHRTX.LEG */
#define OQPSKPHRTX_LEG_MASK             0x01
/** Bit Offset for Sub-Register OQPSKPHRTX.LEG */
#define OQPSKPHRTX_LEG_SHIFT            0
/** Bit Mask for Sub-Register OQPSKPHRTX.MOD */
#define OQPSKPHRTX_MOD_MASK             0x0E
/** Bit Offset for Sub-Register OQPSKPHRTX.MOD */
#define OQPSKPHRTX_MOD_SHIFT            1
/** Bit Mask for Sub-Register OQPSKPHRTX.RB0 */
#define OQPSKPHRTX_RB0_MASK             0x10
/** Bit Offset for Sub-Register OQPSKPHRTX.RB0 */
#define OQPSKPHRTX_RB0_SHIFT            4
/** Bit Mask for Sub-Register OQPSKPHRTX.PPDUT */
#define OQPSKPHRTX_PPDUT_MASK           0x20
/** Bit Offset for Sub-Register OQPSKPHRTX.PPDUT */
#define OQPSKPHRTX_PPDUT_SHIFT          5
/** Bit Mask for Sub-Register OQPSKPHRRX.LEG */
#define OQPSKPHRRX_LEG_MASK             0x01
/** Bit Offset for Sub-Register OQPSKPHRRX.LEG */
#define OQPSKPHRRX_LEG_SHIFT            0
/** Bit Mask for Sub-Register OQPSKPHRRX.MOD */
#define OQPSKPHRRX_MOD_MASK             0x0E
/** Bit Offset for Sub-Register OQPSKPHRRX.MOD */
#define OQPSKPHRRX_MOD_SHIFT            1
/** Bit Mask for Sub-Register OQPSKPHRRX.RB0 */
#define OQPSKPHRRX_RB0_MASK             0x10
/** Bit Offset for Sub-Register OQPSKPHRRX.RB0 */
#define OQPSKPHRRX_RB0_SHIFT            4
/** Bit Mask for Sub-Register OQPSKPHRRX.PPDUT */
#define OQPSKPHRRX_PPDUT_MASK           0x20
/** Bit Offset for Sub-Register OQPSKPHRRX.PPDUT */
#define OQPSKPHRRX_PPDUT_SHIFT          5
/** Bit Mask for Sub-Register AFC0.AFEN0 */
#define AFC0_AFEN0_MASK                 0x01
/** Bit Offset for Sub-Register AFC0.AFEN0 */
#define AFC0_AFEN0_SHIFT                0
/** Bit Mask for Sub-Register AFC0.AFEN1 */
#define AFC0_AFEN1_MASK                 0x02
/** Bit Offset for Sub-Register AFC0.AFEN1 */
#define AFC0_AFEN1_SHIFT                1
/** Bit Mask for Sub-Register AFC0.AFEN2 */
#define AFC0_AFEN2_MASK                 0x04
/** Bit Offset for Sub-Register AFC0.AFEN2 */
#define AFC0_AFEN2_SHIFT                2
/** Bit Mask for Sub-Register AFC0.AFEN3 */
#define AFC0_AFEN3_MASK                 0x08
/** Bit Offset for Sub-Register AFC0.AFEN3 */
#define AFC0_AFEN3_SHIFT                3
/** Bit Mask for Sub-Register AFC0.PM */
#define AFC0_PM_MASK                    0x10
/** Bit Offset for Sub-Register AFC0.PM */
#define AFC0_PM_SHIFT                   4
/** Bit Mask for Sub-Register AFC1.PANC0 */
#define AFC1_PANC0_MASK                 0x01
/** Bit Offset for Sub-Register AFC1.PANC0 */
#define AFC1_PANC0_SHIFT                0
/** Bit Mask for Sub-Register AFC1.PANC1 */
#define AFC1_PANC1_MASK                 0x02
/** Bit Offset for Sub-Register AFC1.PANC1 */
#define AFC1_PANC1_SHIFT                1
/** Bit Mask for Sub-Register AFC1.PANC2 */
#define AFC1_PANC2_MASK                 0x04
/** Bit Offset for Sub-Register AFC1.PANC2 */
#define AFC1_PANC2_SHIFT                2
/** Bit Mask for Sub-Register AFC1.PANC3 */
#define AFC1_PANC3_MASK                 0x08
/** Bit Offset for Sub-Register AFC1.PANC3 */
#define AFC1_PANC3_SHIFT                3
/** Bit Mask for Sub-Register AFC1.MRFT0 */
#define AFC1_MRFT0_MASK                 0x10
/** Bit Offset for Sub-Register AFC1.MRFT0 */
#define AFC1_MRFT0_SHIFT                4
/** Bit Mask for Sub-Register AFC1.MRFT1 */
#define AFC1_MRFT1_MASK                 0x20
/** Bit Offset for Sub-Register AFC1.MRFT1 */
#define AFC1_MRFT1_SHIFT                5
/** Bit Mask for Sub-Register AFC1.MRFT2 */
#define AFC1_MRFT2_MASK                 0x40
/** Bit Offset for Sub-Register AFC1.MRFT2 */
#define AFC1_MRFT2_SHIFT                6
/** Bit Mask for Sub-Register AFC1.MRFT3 */
#define AFC1_MRFT3_MASK                 0x80
/** Bit Offset for Sub-Register AFC1.MRFT3 */
#define AFC1_MRFT3_SHIFT                7
/** Bit Mask for Sub-Register AFFTM.AFFTM */
#define AFFTM_AFFTM_MASK                0xFF
/** Bit Offset for Sub-Register AFFTM.AFFTM */
#define AFFTM_AFFTM_SHIFT               0
/** Bit Mask for Sub-Register AFFVM.AFFVM */
#define AFFVM_AFFVM_MASK                0x0F
/** Bit Offset for Sub-Register AFFVM.AFFVM */
#define AFFVM_AFFVM_SHIFT               0
/** Bit Mask for Sub-Register AFS.AM0 */
#define AFS_AM0_MASK                    0x01
/** Bit Offset for Sub-Register AFS.AM0 */
#define AFS_AM0_SHIFT                   0
/** Bit Mask for Sub-Register AFS.AM1 */
#define AFS_AM1_MASK                    0x02
/** Bit Offset for Sub-Register AFS.AM1 */
#define AFS_AM1_SHIFT                   1
/** Bit Mask for Sub-Register AFS.AM2 */
#define AFS_AM2_MASK                    0x04
/** Bit Offset for Sub-Register AFS.AM2 */
#define AFS_AM2_SHIFT                   2
/** Bit Mask for Sub-Register AFS.AM3 */
#define AFS_AM3_MASK                    0x08
/** Bit Offset for Sub-Register AFS.AM3 */
#define AFS_AM3_SHIFT                   3
/** Bit Mask for Sub-Register AFS.EM */
#define AFS_EM_MASK                     0x10
/** Bit Offset for Sub-Register AFS.EM */
#define AFS_EM_SHIFT                    4
/** Bit Mask for Sub-Register MACEA0.MACEA0 */
#define MACEA0_MACEA0_MASK              0xFF
/** Bit Offset for Sub-Register MACEA0.MACEA0 */
#define MACEA0_MACEA0_SHIFT             0
/** Bit Mask for Sub-Register MACEA1.MACEA1 */
#define MACEA1_MACEA1_MASK              0xFF
/** Bit Offset for Sub-Register MACEA1.MACEA1 */
#define MACEA1_MACEA1_SHIFT             0
/** Bit Mask for Sub-Register MACEA2.MACEA2 */
#define MACEA2_MACEA2_MASK              0xFF
/** Bit Offset for Sub-Register MACEA2.MACEA2 */
#define MACEA2_MACEA2_SHIFT             0
/** Bit Mask for Sub-Register MACEA3.MACEA3 */
#define MACEA3_MACEA3_MASK              0xFF
/** Bit Offset for Sub-Register MACEA3.MACEA3 */
#define MACEA3_MACEA3_SHIFT             0
/** Bit Mask for Sub-Register MACEA4.MACEA4 */
#define MACEA4_MACEA4_MASK              0xFF
/** Bit Offset for Sub-Register MACEA4.MACEA4 */
#define MACEA4_MACEA4_SHIFT             0
/** Bit Mask for Sub-Register MACEA5.MACEA5 */
#define MACEA5_MACEA5_MASK              0xFF
/** Bit Offset for Sub-Register MACEA5.MACEA5 */
#define MACEA5_MACEA5_SHIFT             0
/** Bit Mask for Sub-Register MACEA6.MACEA6 */
#define MACEA6_MACEA6_MASK              0xFF
/** Bit Offset for Sub-Register MACEA6.MACEA6 */
#define MACEA6_MACEA6_SHIFT             0
/** Bit Mask for Sub-Register MACEA7.MACEA7 */
#define MACEA7_MACEA7_MASK              0xFF
/** Bit Offset for Sub-Register MACEA7.MACEA7 */
#define MACEA7_MACEA7_SHIFT             0
/** Bit Mask for Sub-Register MACPID0F0.MACPID0F0 */
#define MACPID0F0_MACPID0F0_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F0.MACPID0F0 */
#define MACPID0F0_MACPID0F0_SHIFT       0
/** Bit Mask for Sub-Register MACPID1F0.MACPID1F0 */
#define MACPID1F0_MACPID1F0_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F0.MACPID1F0 */
#define MACPID1F0_MACPID1F0_SHIFT       0
/** Bit Mask for Sub-Register MACSHA0F0.MACSHA0F0 */
#define MACSHA0F0_MACSHA0F0_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F0.MACSHA0F0 */
#define MACSHA0F0_MACSHA0F0_SHIFT       0
/** Bit Mask for Sub-Register MACSHA1F0.MACSHA1F0 */
#define MACSHA1F0_MACSHA1F0_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F0.MACSHA1F0 */
#define MACSHA1F0_MACSHA1F0_SHIFT       0
/** Bit Mask for Sub-Register MACPID0F1.MACPID0F1 */
#define MACPID0F1_MACPID0F1_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F1.MACPID0F1 */
#define MACPID0F1_MACPID0F1_SHIFT       0
/** Bit Mask for Sub-Register MACPID1F1.MACPID1F1 */
#define MACPID1F1_MACPID1F1_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F1.MACPID1F1 */
#define MACPID1F1_MACPID1F1_SHIFT       0
/** Bit Mask for Sub-Register MACSHA0F1.MACSHA0F1 */
#define MACSHA0F1_MACSHA0F1_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F1.MACSHA0F1 */
#define MACSHA0F1_MACSHA0F1_SHIFT       0
/** Bit Mask for Sub-Register MACSHA1F1.MACSHA1F1 */
#define MACSHA1F1_MACSHA1F1_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F1.MACSHA1F1 */
#define MACSHA1F1_MACSHA1F1_SHIFT       0
/** Bit Mask for Sub-Register MACPID0F2.MACPID0F2 */
#define MACPID0F2_MACPID0F2_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F2.MACPID0F2 */
#define MACPID0F2_MACPID0F2_SHIFT       0
/** Bit Mask for Sub-Register MACPID1F2.MACPID1F2 */
#define MACPID1F2_MACPID1F2_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F2.MACPID1F2 */
#define MACPID1F2_MACPID1F2_SHIFT       0
/** Bit Mask for Sub-Register MACSHA0F2.MACSHA0F2 */
#define MACSHA0F2_MACSHA0F2_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F2.MACSHA0F2 */
#define MACSHA0F2_MACSHA0F2_SHIFT       0
/** Bit Mask for Sub-Register MACSHA1F2.MACSHA1F2 */
#define MACSHA1F2_MACSHA1F2_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F2.MACSHA1F2 */
#define MACSHA1F2_MACSHA1F2_SHIFT       0
/** Bit Mask for Sub-Register MACPID0F3.MACPID0F3 */
#define MACPID0F3_MACPID0F3_MASK        0xFF
/** Bit Offset for Sub-Register MACPID0F3.MACPID0F3 */
#define MACPID0F3_MACPID0F3_SHIFT       0
/** Bit Mask for Sub-Register MACPID1F3.MACPID1F3 */
#define MACPID1F3_MACPID1F3_MASK        0xFF
/** Bit Offset for Sub-Register MACPID1F3.MACPID1F3 */
#define MACPID1F3_MACPID1F3_SHIFT       0
/** Bit Mask for Sub-Register MACSHA0F3.MACSHA0F3 */
#define MACSHA0F3_MACSHA0F3_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA0F3.MACSHA0F3 */
#define MACSHA0F3_MACSHA0F3_SHIFT       0
/** Bit Mask for Sub-Register MACSHA1F3.MACSHA1F3 */
#define MACSHA1F3_MACSHA1F3_MASK        0xFF
/** Bit Offset for Sub-Register MACSHA1F3.MACSHA1F3 */
#define MACSHA1F3_MACSHA1F3_SHIFT       0
/** Bit Mask for Sub-Register AMCS.TX2RX */
#define AMCS_TX2RX_MASK                 0x01
/** Bit Offset for Sub-Register AMCS.TX2RX */
#define AMCS_TX2RX_SHIFT                0
/** Bit Mask for Sub-Register AMCS.CCATX */
#define AMCS_CCATX_MASK                 0x02
/** Bit Offset for Sub-Register AMCS.CCATX */
#define AMCS_CCATX_SHIFT                1
/** Bit Mask for Sub-Register AMCS.CCAED */
#define AMCS_CCAED_MASK                 0x04
/** Bit Offset for Sub-Register AMCS.CCAED */
#define AMCS_CCAED_SHIFT                2
/** Bit Mask for Sub-Register AMCS.AACK */
#define AMCS_AACK_MASK                  0x08
/** Bit Offset for Sub-Register AMCS.AACK */
#define AMCS_AACK_SHIFT                 3
/** Bit Mask for Sub-Register AMCS.AACKS */
#define AMCS_AACKS_MASK                 0x10
/** Bit Offset for Sub-Register AMCS.AACKS */
#define AMCS_AACKS_SHIFT                4
/** Bit Mask for Sub-Register AMCS.AACKDR */
#define AMCS_AACKDR_MASK                0x20
/** Bit Offset for Sub-Register AMCS.AACKDR */
#define AMCS_AACKDR_SHIFT               5
/** Bit Mask for Sub-Register AMCS.AACKFA */
#define AMCS_AACKFA_MASK                0x40
/** Bit Offset for Sub-Register AMCS.AACKFA */
#define AMCS_AACKFA_SHIFT               6
/** Bit Mask for Sub-Register AMCS.AACKFT */
#define AMCS_AACKFT_MASK                0x80
/** Bit Offset for Sub-Register AMCS.AACKFT */
#define AMCS_AACKFT_SHIFT               7
/** Bit Mask for Sub-Register AMEDT.AMEDT */
#define AMEDT_AMEDT_MASK                0xFF
/** Bit Offset for Sub-Register AMEDT.AMEDT */
#define AMEDT_AMEDT_SHIFT               0
/** Bit Mask for Sub-Register AMAACKPD.PD0 */
#define AMAACKPD_PD0_MASK               0x01
/** Bit Offset for Sub-Register AMAACKPD.PD0 */
#define AMAACKPD_PD0_SHIFT              0
/** Bit Mask for Sub-Register AMAACKPD.PD1 */
#define AMAACKPD_PD1_MASK               0x02
/** Bit Offset for Sub-Register AMAACKPD.PD1 */
#define AMAACKPD_PD1_SHIFT              1
/** Bit Mask for Sub-Register AMAACKPD.PD2 */
#define AMAACKPD_PD2_MASK               0x04
/** Bit Offset for Sub-Register AMAACKPD.PD2 */
#define AMAACKPD_PD2_SHIFT              2
/** Bit Mask for Sub-Register AMAACKPD.PD3 */
#define AMAACKPD_PD3_MASK               0x08
/** Bit Offset for Sub-Register AMAACKPD.PD3 */
#define AMAACKPD_PD3_SHIFT              3
/** Bit Mask for Sub-Register AMAACKTL.AMAACKTL */
#define AMAACKTL_AMAACKTL_MASK          0xFF
/** Bit Offset for Sub-Register AMAACKTL.AMAACKTL */
#define AMAACKTL_AMAACKTL_SHIFT         0
/** Bit Mask for Sub-Register AMAACKTH.AMAACKTH */
#define AMAACKTH_AMAACKTH_MASK          0x07
/** Bit Offset for Sub-Register AMAACKTH.AMAACKTH */
#define AMAACKTH_AMAACKTH_SHIFT         0
/** Bit Mask for Sub-Register FSKC0.MORD */
#define FSKC0_MORD_MASK                 0x01
/** Bit Offset for Sub-Register FSKC0.MORD */
#define FSKC0_MORD_SHIFT                0

#define FSK_MORD_2SFK                   (0 << FSKC0_MORD_SHIFT)
#define FSK_MORD_4SFK                   (1 << FSKC0_MORD_SHIFT)

/** Bit Mask for Sub-Register FSKC0.MIDX */
#define FSKC0_MIDX_MASK                 0x0E
/** Bit Offset for Sub-Register FSKC0.MIDX */
#define FSKC0_MIDX_SHIFT                1

#define FSK_MIDX_3_BY_8                 (0 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_4_BY_8                 (1 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_6_BY_8                 (2 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_8_BY_8                 (3 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_10_BY_8                (4 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_12_BY_8                (5 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_14_BY_8                (6 << FSKC0_MIDX_SHIFT)
#define FSK_MIDX_16_BY_8                (7 << FSKC0_MIDX_SHIFT)

/** Bit Mask for Sub-Register FSKC0.MIDXS */
#define FSKC0_MIDXS_MASK                0x30
/** Bit Offset for Sub-Register FSKC0.MIDXS */
#define FSKC0_MIDXS_SHIFT               4

#define FSK_MIDXS_SCALE_7_BY_8          (0 << FSKC0_MIDXS_SHIFT)
#define FSK_MIDXS_SCALE_8_BY_8          (1 << FSKC0_MIDXS_SHIFT)
#define FSK_MIDXS_SCALE_9_BY_8          (2 << FSKC0_MIDXS_SHIFT)
#define FSK_MIDXS_SCALE_10_BY_8         (3 << FSKC0_MIDXS_SHIFT)

/** Bit Mask for Sub-Register FSKC0.BT */
#define FSKC0_BT_MASK                   0xC0
/** Bit Offset for Sub-Register FSKC0.BT */
#define FSKC0_BT_SHIFT                  6

#define FSK_BT_05    (0 << FSKC0_BT_SHIFT)
#define FSK_BT_10    (1 << FSKC0_BT_SHIFT)
#define FSK_BT_15    (2 << FSKC0_BT_SHIFT)
#define FSK_BT_20    (3 << FSKC0_BT_SHIFT)

/** Bit Mask for Sub-Register FSKC1.SRATE */
#define FSKC1_SRATE_MASK                0x0F
/** Bit Offset for Sub-Register FSKC1.SRATE */
#define FSKC1_SRATE_SHIFT               0

#define FSK_SRATE_50K                   0x0
#define FSK_SRATE_100K                  0x1
#define FSK_SRATE_150K                  0x2
#define FSK_SRATE_200K                  0x3
#define FSK_SRATE_300K                  0x4
#define FSK_SRATE_400K                  0x5

#define FSK_CHANNEL_SPACING_200K        0x0
#define FSK_CHANNEL_SPACING_400K        0x1

/** Bit Mask for Sub-Register FSKC1.FI */
#define FSKC1_FI_MASK                   0x20
/** Bit Offset for Sub-Register FSKC1.FI */
#define FSKC1_FI_SHIFT                  5
/** Bit Mask for Sub-Register FSKC1.FSKPLH */
#define FSKC1_FSKPLH_MASK               0xC0
/** Bit Offset for Sub-Register FSKC1.FSKPLH */
#define FSKC1_FSKPLH_SHIFT              6
/** Bit Mask for Sub-Register FSKC2.FECIE */
#define FSKC2_FECIE_MASK                0x01
/** Bit Offset for Sub-Register FSKC2.FECIE */
#define FSKC2_FECIE_SHIFT               0
/** Bit Mask for Sub-Register FSKC2.FECS */
#define FSKC2_FECS_MASK                 0x02
/** Bit Offset for Sub-Register FSKC2.FECS */
#define FSKC2_FECS_SHIFT                1
/** Bit Mask for Sub-Register FSKC2.PRI */
#define FSKC2_PRI_MASK                  0x04
/** Bit Offset for Sub-Register FSKC2.PRI */
#define FSKC2_PRI_SHIFT                 2
/** Bit Mask for Sub-Register FSKC2.MSE */
#define FSKC2_MSE_MASK                  0x08
/** Bit Offset for Sub-Register FSKC2.MSE */
#define FSKC2_MSE_SHIFT                 3
/** Bit Mask for Sub-Register FSKC2.RXPTO */
#define FSKC2_RXPTO_MASK                0x10
/** Bit Offset for Sub-Register FSKC2.RXPTO */
#define FSKC2_RXPTO_SHIFT               4
/** Bit Mask for Sub-Register FSKC2.RXO */
#define FSKC2_RXO_MASK                  0x60
/** Bit Offset for Sub-Register FSKC2.RXO */
#define FSKC2_RXO_SHIFT                 5
/** Bit Mask for Sub-Register FSKC2.PDTM */
#define FSKC2_PDTM_MASK                 0x80
/** Bit Offset for Sub-Register FSKC2.PDTM */
#define FSKC2_PDTM_SHIFT                7
/** Bit Mask for Sub-Register FSKC3.PDT */
#define FSKC3_PDT_MASK                  0x0F
/** Bit Offset for Sub-Register FSKC3.PDT */
#define FSKC3_PDT_SHIFT                 0
/** Bit Mask for Sub-Register FSKC3.SFDT */
#define FSKC3_SFDT_MASK                 0xF0
/** Bit Offset for Sub-Register FSKC3.SFDT */
#define FSKC3_SFDT_SHIFT                4

/* Lower values increase the SFD detector sensitivity.
   Higher values increase the SFD selectivity.
   The default value 8 is recommended for simultaneous sensing
   of the SFD pairs according to IEEE 802.15.4g. */
#define FSKC3_SFDT(n) (((n) << FSKC3_SFDT_SHIFT) & FSKC3_SFDT_MASK)

/* Lower values increase the preamble detector sensitivity. */
#define FSKC3_PDT(n)  (((n) << FSKC3_PDT_SHIFT) & FSKC3_PDT_MASK)

/** Bit Mask for Sub-Register FSKC4.CSFD0 */
#define FSKC4_CSFD0_MASK                0x03
/** Bit Offset for Sub-Register FSKC4.CSFD0 */
#define FSKC4_CSFD0_SHIFT               0
/** Bit Mask for Sub-Register FSKC4.CSFD1 */
#define FSKC4_CSFD1_MASK                0x0C
/** Bit Offset for Sub-Register FSKC4.CSFD1 */
#define FSKC4_CSFD1_SHIFT               2
/** Bit Mask for Sub-Register FSKC4.RAWRBIT */
#define FSKC4_RAWRBIT_MASK              0x10
/** Bit Offset for Sub-Register FSKC4.RAWRBIT */
#define FSKC4_RAWRBIT_SHIFT             4
/** Bit Mask for Sub-Register FSKC4.SFD32 */
#define FSKC4_SFD32_MASK                0x20
/** Bit Offset for Sub-Register FSKC4.SFD32 */
#define FSKC4_SFD32_SHIFT               5
/** Bit Mask for Sub-Register FSKC4.SFDQ */
#define FSKC4_SFDQ_MASK                 0x40
/** Bit Offset for Sub-Register FSKC4.SFDQ */
#define FSKC4_SFDQ_SHIFT                6
/** Bit Mask for Sub-Register FSKPLL.FSKPLL */
#define FSKPLL_FSKPLL_MASK              0xFF
/** Bit Offset for Sub-Register FSKPLL.FSKPLL */
#define FSKPLL_FSKPLL_SHIFT             0
/** Bit Mask for Sub-Register FSKSFD0L.FSKSFD0L */
#define FSKSFD0L_FSKSFD0L_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD0L.FSKSFD0L */
#define FSKSFD0L_FSKSFD0L_SHIFT         0
/** Bit Mask for Sub-Register FSKSFD0H.FSKSFD0H */
#define FSKSFD0H_FSKSFD0H_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD0H.FSKSFD0H */
#define FSKSFD0H_FSKSFD0H_SHIFT         0
/** Bit Mask for Sub-Register FSKSFD1L.FSKSFD1L */
#define FSKSFD1L_FSKSFD1L_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD1L.FSKSFD1L */
#define FSKSFD1L_FSKSFD1L_SHIFT         0
/** Bit Mask for Sub-Register FSKSFD1H.FSKSFD1H */
#define FSKSFD1H_FSKSFD1H_MASK          0xFF
/** Bit Offset for Sub-Register FSKSFD1H.FSKSFD1H */
#define FSKSFD1H_FSKSFD1H_SHIFT         0
/** Bit Mask for Sub-Register FSKPHRTX.RB1 */
#define FSKPHRTX_RB1_MASK               0x01
/** Bit Offset for Sub-Register FSKPHRTX.RB1 */
#define FSKPHRTX_RB1_SHIFT              0
/** Bit Mask for Sub-Register FSKPHRTX.RB2 */
#define FSKPHRTX_RB2_MASK               0x02
/** Bit Offset for Sub-Register FSKPHRTX.RB2 */
#define FSKPHRTX_RB2_SHIFT              1
/** Bit Mask for Sub-Register FSKPHRTX.DW */
#define FSKPHRTX_DW_MASK                0x04
/** Bit Offset for Sub-Register FSKPHRTX.DW */
#define FSKPHRTX_DW_SHIFT               2
/** Bit Mask for Sub-Register FSKPHRTX.SFD */
#define FSKPHRTX_SFD_MASK               0x08
/** Bit Offset for Sub-Register FSKPHRTX.SFD */
#define FSKPHRTX_SFD_SHIFT              3
/** Bit Mask for Sub-Register FSKPHRRX.RB1 */
#define FSKPHRRX_RB1_MASK               0x01
/** Bit Offset for Sub-Register FSKPHRRX.RB1 */
#define FSKPHRRX_RB1_SHIFT              0
/** Bit Mask for Sub-Register FSKPHRRX.RB2 */
#define FSKPHRRX_RB2_MASK               0x02
/** Bit Offset for Sub-Register FSKPHRRX.RB2 */
#define FSKPHRRX_RB2_SHIFT              1
/** Bit Mask for Sub-Register FSKPHRRX.DW */
#define FSKPHRRX_DW_MASK                0x04
/** Bit Offset for Sub-Register FSKPHRRX.DW */
#define FSKPHRRX_DW_SHIFT               2
/** Bit Mask for Sub-Register FSKPHRRX.SFD */
#define FSKPHRRX_SFD_MASK               0x08
/** Bit Offset for Sub-Register FSKPHRRX.SFD */
#define FSKPHRRX_SFD_SHIFT              3
/** Bit Mask for Sub-Register FSKPHRRX.MS */
#define FSKPHRRX_MS_MASK                0x40
/** Bit Offset for Sub-Register FSKPHRRX.MS */
#define FSKPHRRX_MS_SHIFT               6
/** Bit Mask for Sub-Register FSKPHRRX.FCST */
#define FSKPHRRX_FCST_MASK              0x80
/** Bit Offset for Sub-Register FSKPHRRX.FCST */
#define FSKPHRRX_FCST_SHIFT             7
/** Bit Mask for Sub-Register FSKRPC.BASET */
#define FSKRPC_BASET_MASK               0x07
/** Bit Offset for Sub-Register FSKRPC.BASET */
#define FSKRPC_BASET_SHIFT              0
/** Bit Mask for Sub-Register FSKRPC.EN */
#define FSKRPC_EN_MASK                  0x08
/** Bit Offset for Sub-Register FSKRPC.EN */
#define FSKRPC_EN_SHIFT                 3
/** Bit Mask for Sub-Register FSKRPCONT.FSKRPCONT */
#define FSKRPCONT_FSKRPCONT_MASK        0xFF
/** Bit Offset for Sub-Register FSKRPCONT.FSKRPCONT */
#define FSKRPCONT_FSKRPCONT_SHIFT       0
/** Bit Mask for Sub-Register FSKRPCOFFT.FSKRPCOFFT */
#define FSKRPCOFFT_FSKRPCOFFT_MASK      0xFF
/** Bit Offset for Sub-Register FSKRPCOFFT.FSKRPCOFFT */
#define FSKRPCOFFT_FSKRPCOFFT_SHIFT     0
/** Bit Mask for Sub-Register FSKRRXFLL.FSKRRXFLL */
#define FSKRRXFLL_FSKRRXFLL_MASK        0xFF
/** Bit Offset for Sub-Register FSKRRXFLL.FSKRRXFLL */
#define FSKRRXFLL_FSKRRXFLL_SHIFT       0
/** Bit Mask for Sub-Register FSKRRXFLH.FSKRRXFLH */
#define FSKRRXFLH_FSKRRXFLH_MASK        0x07
/** Bit Offset for Sub-Register FSKRRXFLH.FSKRRXFLH */
#define FSKRRXFLH_FSKRRXFLH_SHIFT       0
/** Bit Mask for Sub-Register FSKDM.EN */
#define FSKDM_EN_MASK                   0x01
/** Bit Offset for Sub-Register FSKDM.EN */
#define FSKDM_EN_SHIFT                  0
/** Bit Mask for Sub-Register FSKDM.PE */
#define FSKDM_PE_MASK                   0x02
/** Bit Offset for Sub-Register FSKDM.PE */
#define FSKDM_PE_SHIFT                  1
/** Bit Mask for Sub-Register FSKPE0.FSKPE0 */
#define FSKPE0_FSKPE0_MASK              0xFF
/** Bit Offset for Sub-Register FSKPE0.FSKPE0 */
#define FSKPE0_FSKPE0_SHIFT             0
/** Bit Mask for Sub-Register FSKPE1.FSKPE1 */
#define FSKPE1_FSKPE1_MASK              0xFF
/** Bit Offset for Sub-Register FSKPE1.FSKPE1 */
#define FSKPE1_FSKPE1_SHIFT             0
/** Bit Mask for Sub-Register FSKPE2.FSKPE2 */
#define FSKPE2_FSKPE2_MASK              0xFF
/** Bit Offset for Sub-Register FSKPE2.FSKPE2 */
#define FSKPE2_FSKPE2_SHIFT             0
/** Bit Mask for Sub-Register PMUC.EN */
#define PMUC_EN_MASK                    0x01
/** Bit Offset for Sub-Register PMUC.EN */
#define PMUC_EN_SHIFT                   0
/** Bit Mask for Sub-Register PMUC.AVG */
#define PMUC_AVG_MASK                   0x02
/** Bit Offset for Sub-Register PMUC.AVG */
#define PMUC_AVG_SHIFT                  1
/** Bit Mask for Sub-Register PMUC.SYNC */
#define PMUC_SYNC_MASK                  0x1C
/** Bit Offset for Sub-Register PMUC.SYNC */
#define PMUC_SYNC_SHIFT                 2
/** Bit Mask for Sub-Register PMUC.FED */
#define PMUC_FED_MASK                   0x20
/** Bit Offset for Sub-Register PMUC.FED */
#define PMUC_FED_SHIFT                  5
/** Bit Mask for Sub-Register PMUC.IQSEL */
#define PMUC_IQSEL_MASK                 0x40
/** Bit Offset for Sub-Register PMUC.IQSEL */
#define PMUC_IQSEL_SHIFT                6
/** Bit Mask for Sub-Register PMUC.CCFTS */
#define PMUC_CCFTS_MASK                 0x80
/** Bit Offset for Sub-Register PMUC.CCFTS */
#define PMUC_CCFTS_SHIFT                7
/** Bit Mask for Sub-Register PMUVAL.PMUVAL */
#define PMUVAL_PMUVAL_MASK              0xFF
/** Bit Offset for Sub-Register PMUVAL.PMUVAL */
#define PMUVAL_PMUVAL_SHIFT             0
/** Bit Mask for Sub-Register PMUQF.PMUQF */
#define PMUQF_PMUQF_MASK                0xFF
/** Bit Offset for Sub-Register PMUQF.PMUQF */
#define PMUQF_PMUQF_SHIFT               0
/** Bit Mask for Sub-Register PMUI.PMUI */
#define PMUI_PMUI_MASK                  0xFF
/** Bit Offset for Sub-Register PMUI.PMUI */
#define PMUI_PMUI_SHIFT                 0
/** Bit Mask for Sub-Register PMUQ.PMUQ */
#define PMUQ_PMUQ_MASK                  0xFF
/** Bit Offset for Sub-Register PMUQ.PMUQ */
#define PMUQ_PMUQ_SHIFT                 0
/** Bit Mask for Sub-Register CNTC.EN */
#define CNTC_EN_MASK                    0x01
/** Bit Offset for Sub-Register CNTC.EN */
#define CNTC_EN_SHIFT                   0
/** Bit Mask for Sub-Register CNTC.RSTRXS */
#define CNTC_RSTRXS_MASK                0x02
/** Bit Offset for Sub-Register CNTC.RSTRXS */
#define CNTC_RSTRXS_SHIFT               1
/** Bit Mask for Sub-Register CNTC.RSTTXS */
#define CNTC_RSTTXS_MASK                0x04
/** Bit Offset for Sub-Register CNTC.RSTTXS */
#define CNTC_RSTTXS_SHIFT               2
/** Bit Mask for Sub-Register CNTC.CAPRXS */
#define CNTC_CAPRXS_MASK                0x08
/** Bit Offset for Sub-Register CNTC.CAPRXS */
#define CNTC_CAPRXS_SHIFT               3
/** Bit Mask for Sub-Register CNTC.CAPTXS */
#define CNTC_CAPTXS_MASK                0x10
/** Bit Offset for Sub-Register CNTC.CAPTXS */
#define CNTC_CAPTXS_SHIFT               4
/** Bit Mask for Sub-Register CNT0.CNT0 */
#define CNT0_CNT0_MASK                  0xFF
/** Bit Offset for Sub-Register CNT0.CNT0 */
#define CNT0_CNT0_SHIFT                 0
/** Bit Mask for Sub-Register CNT1.CNT1 */
#define CNT1_CNT1_MASK                  0xFF
/** Bit Offset for Sub-Register CNT1.CNT1 */
#define CNT1_CNT1_SHIFT                 0
/** Bit Mask for Sub-Register CNT2.CNT2 */
#define CNT2_CNT2_MASK                  0xFF
/** Bit Offset for Sub-Register CNT2.CNT2 */
#define CNT2_CNT2_SHIFT                 0
/** Bit Mask for Sub-Register CNT3.CNT3 */
#define CNT3_CNT3_MASK                  0xFF
/** Bit Offset for Sub-Register CNT3.CNT3 */
#define CNT3_CNT3_SHIFT                 0
/** Bit Mask for Sub-Register FBRXS.FBRXS */
#define FBRXS_FBRXS_MASK                0xFF
/** Bit Offset for Sub-Register FBRXS.FBRXS */
#define FBRXS_FBRXS_SHIFT               0
/** Bit Mask for Sub-Register FBRXE.FBRXE */
#define FBRXE_FBRXE_MASK                0xFF
/** Bit Offset for Sub-Register FBRXE.FBRXE */
#define FBRXE_FBRXE_SHIFT               0
/** Bit Mask for Sub-Register FBTXS.FBTXS */
#define FBTXS_FBTXS_MASK                0xFF
/** Bit Offset for Sub-Register FBTXS.FBTXS */
#define FBTXS_FBTXS_SHIFT               0
/** Bit Mask for Sub-Register FBTXE.FBTXE */
#define FBTXE_FBTXE_MASK                0xFF
/** Bit Offset for Sub-Register FBTXE.FBTXE */
#define FBTXE_FBTXE_SHIFT               0
/** Address for register RF09_IRQS */
#define RG_RF09_IRQS                    (0x00)
/** Address for register RF24_IRQS */
#define RG_RF24_IRQS                    (0x01)
/** Address for register BBC0_IRQS */
#define RG_BBC0_IRQS                    (0x02)
/** Address for register BBC1_IRQS */
#define RG_BBC1_IRQS                    (0x03)
/** Address for register RF_RST */
#define RG_RF_RST                       (0x05)
/** Address for register RF_CFG */
#define RG_RF_CFG                       (0x06)
/** Constant DRV2 for sub-register SR_DRV in register RF_CFG */
#define RF_DRV2                         (0x0)
/** Constant DRV4 for sub-register SR_DRV in register RF_CFG */
#define RF_DRV4                         (0x1)
/** Constant DRV6 for sub-register SR_DRV in register RF_CFG */
#define RF_DRV6                         (0x2)
/** Constant DRV8 for sub-register SR_DRV in register RF_CFG */
#define RF_DRV8                         (0x3)
/** Address for register RF_CLKO */
#define RG_RF_CLKO                      (0x07)
/** Constant DRVCLKO2 for sub-register SR_DRV in register RF_CLKO */
#define RF_DRVCLKO2                     (0x0)
/** Constant DRVCLKO4 for sub-register SR_DRV in register RF_CLKO */
#define RF_DRVCLKO4                     (0x1)
/** Constant DRVCLKO6 for sub-register SR_DRV in register RF_CLKO */
#define RF_DRVCLKO6                     (0x2)
/** Constant DRVCLKO8 for sub-register SR_DRV in register RF_CLKO */
#define RF_DRVCLKO8                     (0x3)
/** Address for register RF_BMDVC */
#define RG_RF_BMDVC                     (0x08)
/** Address for register RF_XOC */
#define RG_RF_XOC                       (0x09)
/** Address for register RF_IQIFC0 */
#define RG_RF_IQIFC0                    (0x0A)
/** Address for register RF_IQIFC1 */
#define RG_RF_IQIFC1                    (0x0B)
/** Constant MODE_BBRF for sub-register SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_BBRF                    (0x0)
/** Constant MODE_BBRF09 for sub-register SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_BBRF09                  (0x4)
/** Constant MODE_BBRF24 for sub-register SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_BBRF24                  (0x5)
/** Constant MODE_RF for sub-register SR_CHPM in register RF_IQIFC1 */
#define RF_MODE_RF                      (0x1)
/** Address for register RF_IQIFC2 */
#define RG_RF_IQIFC2                    (0x0C)
/** Address for register RF_PN */
#define RG_RF_PN                        (0x0D)
/** Address for register RF_VN */
#define RG_RF_VN                        (0x0E)
/** Address for register RF09_IRQM */
#define RG_RF09_IRQM                    (0x100)
/** Address for register RF09_AUXS */
#define RG_RF09_AUXS                    (0x101)
/** Address for register RF09_STATE */
#define RG_RF09_STATE                   (0x102)
/** Constant TRANSITION for sub-register SR_STATE in register RF09_STATE */
#define RF_TRANSITION                   (0x6)
/** Address for register RF09_CMD */
#define RG_RF09_CMD                     (0x103)
/** Address for register RF09_CS */
#define RG_RF09_CS                      (0x104)
/** Address for register RF09_CCF0L */
#define RG_RF09_CCF0L                   (0x105)
/** Address for register RF09_CCF0H */
#define RG_RF09_CCF0H                   (0x106)
/** Address for register RF09_CNL */
#define RG_RF09_CNL                     (0x107)
/** Address for register RF09_CNM */
#define RG_RF09_CNM                     (0x108)
/** Address for register RF09_RXBWC */
#define RG_RF09_RXBWC                   (0x109)

/** Constant BW160KHZ_IF250KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW160KHZ_IF250KHZ            (0x0)
/** Constant BW200KHZ_IF250KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW200KHZ_IF250KHZ            (0x1)
/** Constant BW250KHZ_IF250KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW250KHZ_IF250KHZ            (0x2)
/** Constant BW320KHZ_IF500KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW320KHZ_IF500KHZ            (0x3)
/** Constant BW400KHZ_IF500KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW400KHZ_IF500KHZ            (0x4)
/** Constant BW500KHZ_IF500KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW500KHZ_IF500KHZ            (0x5)
/** Constant BW630KHZ_IF1000KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW630KHZ_IF1000KHZ           (0x6)
/** Constant BW800KHZ_IF1000KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW800KHZ_IF1000KHZ           (0x7)
/** Constant BW1000KHZ_IF1000KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW1000KHZ_IF1000KHZ          (0x8)
/** Constant BW1250KHZ_IF2000KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW1250KHZ_IF2000KHZ          (0x9)
/** Constant BW1600KHZ_IF2000KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW1600KHZ_IF2000KHZ          (0xA)
/** Constant BW2000KHZ_IF2000KHZ for sub-register SR_BW in register RF09_RXBWC */
#define RF_BW2000KHZ_IF2000KHZ          (0xB)

/** Address for register RF09_RXDFE */
#define RG_RF09_RXDFE                   (0x10A)
/** Address for register RF09_AGCC */
#define RG_RF09_AGCC                    (0x10B)
/** Address for register RF09_AGCS */
#define RG_RF09_AGCS                    (0x10C)
/** Address for register RF09_RSSI */
#define RG_RF09_RSSI                    (0x10D)
/** Address for register RF09_EDC */
#define RG_RF09_EDC                     (0x10E)
/** Constant EDAUTO for sub-register SR_EDM in register RF09_EDC */
#define RF_EDAUTO                       (0x0)
/** Constant EDCONT for sub-register SR_EDM in register RF09_EDC */
#define RF_EDCONT                       (0x2)
/** Constant EDOFF for sub-register SR_EDM in register RF09_EDC */
#define RF_EDOFF                        (0x3)
/** Constant EDSINGLE for sub-register SR_EDM in register RF09_EDC */
#define RF_EDSINGLE                     (0x1)
/** Address for register RF09_EDD */
#define RG_RF09_EDD                     (0x10F)
/** Address for register RF09_EDV */
#define RG_RF09_EDV                     (0x110)
/** Address for register RF09_RNDV */
#define RG_RF09_RNDV                    (0x111)
/** Address for register RF09_TXCUTC */
#define RG_RF09_TXCUTC                  (0x112)
/** Constant FLC80KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC80KHZ                     (0x0)
/** Constant FLC100KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC100KHZ                    (0x1)
/** Constant FLC125KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC125KHZ                    (0x2)
/** Constant FLC160KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC160KHZ                    (0x3)
/** Constant FLC200KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC200KHZ                    (0x4)
/** Constant FLC250KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC250KHZ                    (0x5)
/** Constant FLC315KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC315KHZ                    (0x6)
/** Constant FLC400KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC400KHZ                    (0x7)
/** Constant FLC500KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC500KHZ                    (0x8)
/** Constant FLC625KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC625KHZ                    (0x9)
/** Constant FLC800KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC800KHZ                    (0xA)
/** Constant FLC1000KHZ for sub-register SR_LPFCUT in register RF09_TXCUTC */
#define RF_FLC1000KHZ                   (0xB)
/** Constant PARAMP4U for sub-register SR_PARAMP in register RF09_TXCUTC */
#define RF_PARAMP4U                     (0x0 << TXCUTC_PARAMP_SHIFT)
/** Constant PARAMP8U for sub-register SR_PARAMP in register RF09_TXCUTC */
#define RF_PARAMP8U                     (0x1 << TXCUTC_PARAMP_SHIFT)
/** Constant PARAMP16U for sub-register SR_PARAMP in register RF09_TXCUTC */
#define RF_PARAMP16U                    (0x2 << TXCUTC_PARAMP_SHIFT)
/** Constant PARAMP32U for sub-register SR_PARAMP in register RF09_TXCUTC */
#define RF_PARAMP32U                    (0x3 << TXCUTC_PARAMP_SHIFT)
/** Address for register RF09_TXDFE */
#define RG_RF09_TXDFE                   (0x113)
/** Address for register RF09_PAC */
#define RG_RF09_PAC                     (0x114)
/** Address for register RF09_PADFE */
#define RG_RF09_PADFE                   (0x116)
/** Constant FEMODE0 for sub-register SR_PADFE in register RF09_PADFE */
#define RF_FEMODE0                      (0x0)
/** Constant FEMODE1 for sub-register SR_PADFE in register RF09_PADFE */
#define RF_FEMODE1                      (0x1)
/** Constant FEMODE2 for sub-register SR_PADFE in register RF09_PADFE */
#define RF_FEMODE2                      (0x2)
/** Constant FEMODE3 for sub-register SR_PADFE in register RF09_PADFE */
#define RF_FEMODE3                      (0x3)
/** Address for register RF09_PLL */
#define RG_RF09_PLL                     (0x121)
/** Address for register RF09_PLLCF */
#define RG_RF09_PLLCF                   (0x122)
/** Address for register RF09_TXCI */
#define RG_RF09_TXCI                    (0x125)
/** Address for register RF09_TXCQ */
#define RG_RF09_TXCQ                    (0x126)
/** Address for register RF09_TXDACI */
#define RG_RF09_TXDACI                  (0x127)
/** Address for register RF09_TXDACQ */
#define RG_RF09_TXDACQ                  (0x128)
/** Address for register RF24_IRQM */
#define RG_RF24_IRQM                    (0x200)
/** Address for register BBC0_FBRXS */
#define RG_BBC0_FBRXS                   (0x2000)
/** Address for register RF24_AUXS */
#define RG_RF24_AUXS                    (0x201)
/** Address for register RF24_STATE */
#define RG_RF24_STATE                   (0x202)
/** Address for register RF24_CMD */
#define RG_RF24_CMD                     (0x203)
/** Address for register RF24_CS */
#define RG_RF24_CS                      (0x204)
/** Address for register RF24_CCF0L */
#define RG_RF24_CCF0L                   (0x205)
/** Address for register RF24_CCF0H */
#define RG_RF24_CCF0H                   (0x206)
/** Address for register RF24_CNL */
#define RG_RF24_CNL                     (0x207)
/** Address for register RF24_CNM */
#define RG_RF24_CNM                     (0x208)
/** Address for register RF24_RXBWC */
#define RG_RF24_RXBWC                   (0x209)
/** Address for register RF24_RXDFE */
#define RG_RF24_RXDFE                   (0x20A)
/** Address for register RF24_AGCC */
#define RG_RF24_AGCC                    (0x20B)
/** Address for register RF24_AGCS */
#define RG_RF24_AGCS                    (0x20C)
/** Address for register RF24_RSSI */
#define RG_RF24_RSSI                    (0x20D)
/** Address for register RF24_EDC */
#define RG_RF24_EDC                     (0x20E)
/** Address for register RF24_EDD */
#define RG_RF24_EDD                     (0x20F)
/** Address for register RF24_EDV */
#define RG_RF24_EDV                     (0x210)
/** Address for register RF24_RNDV */
#define RG_RF24_RNDV                    (0x211)
/** Address for register RF24_TXCUTC */
#define RG_RF24_TXCUTC                  (0x212)
/** Address for register RF24_TXDFE */
#define RG_RF24_TXDFE                   (0x213)
/** Address for register RF24_PAC */
#define RG_RF24_PAC                     (0x214)
/** Address for register RF24_PADFE */
#define RG_RF24_PADFE                   (0x216)
/** Address for register RF24_PLL */
#define RG_RF24_PLL                     (0x221)
/** Address for register RF24_PLLCF */
#define RG_RF24_PLLCF                   (0x222)
/** Address for register RF24_TXCI */
#define RG_RF24_TXCI                    (0x225)
/** Address for register RF24_TXCQ */
#define RG_RF24_TXCQ                    (0x226)
/** Address for register RF24_TXDACI */
#define RG_RF24_TXDACI                  (0x227)
/** Address for register RF24_TXDACQ */
#define RG_RF24_TXDACQ                  (0x228)
/** Address for register BBC0_FBRXE */
#define RG_BBC0_FBRXE                   (0x27FE)
/** Address for register BBC0_FBTXS */
#define RG_BBC0_FBTXS                   (0x2800)
/** Address for register BBC0_FBTXE */
#define RG_BBC0_FBTXE                   (0x2FFE)
/** Address for register BBC0_IRQM */
#define RG_BBC0_IRQM                    (0x300)
/** Address for register BBC1_FBRXS */
#define RG_BBC1_FBRXS                   (0x3000)
/** Address for register BBC0_PC */
#define RG_BBC0_PC                      (0x301)
/** Constant MRFSK for sub-register SR_PT in register BBC0_PC */
#define BB_MRFSK                        (0x1)
/** Constant MROFDM for sub-register SR_PT in register BBC0_PC */
#define BB_MROFDM                       (0x2)
/** Constant MROQPSK for sub-register SR_PT in register BBC0_PC */
#define BB_MROQPSK                      (0x3)
/** Constant PHYOFF for sub-register SR_PT in register BBC0_PC */
#define BB_PHYOFF                       (0x0)
/** Address for register BBC0_PS */
#define RG_BBC0_PS                      (0x302)
/** Address for register BBC0_RXFLL */
#define RG_BBC0_RXFLL                   (0x304)
/** Address for register BBC0_RXFLH */
#define RG_BBC0_RXFLH                   (0x305)
/** Address for register BBC0_TXFLL */
#define RG_BBC0_TXFLL                   (0x306)
/** Address for register BBC0_TXFLH */
#define RG_BBC0_TXFLH                   (0x307)
/** Address for register BBC0_FBLL */
#define RG_BBC0_FBLL                    (0x308)
/** Address for register BBC0_FBLH */
#define RG_BBC0_FBLH                    (0x309)
/** Address for register BBC0_FBLIL */
#define RG_BBC0_FBLIL                   (0x30A)
/** Address for register BBC0_FBLIH */
#define RG_BBC0_FBLIH                   (0x30B)
/** Address for register BBC0_OFDMPHRTX */
#define RG_BBC0_OFDMPHRTX               (0x30C)
/** Address for register BBC0_OFDMPHRRX */
#define RG_BBC0_OFDMPHRRX               (0x30D)
/** Address for register BBC0_OFDMC */
#define RG_BBC0_OFDMC                   (0x30E)
/** Address for register BBC0_OFDMSW */
#define RG_BBC0_OFDMSW                  (0x30F)
/** Address for register BBC0_OQPSKC0 */
#define RG_BBC0_OQPSKC0                 (0x310)
/** Constant FCHIP100 for sub-register SR_FCHIP in register BBC0_OQPSKC0 */
#define BB_FCHIP100                     (0x0)
/** Constant FCHIP1000 for sub-register SR_FCHIP in register BBC0_OQPSKC0 */
#define BB_FCHIP1000                    (0x2)
/** Constant FCHIP200 for sub-register SR_FCHIP in register BBC0_OQPSKC0 */
#define BB_FCHIP200                     (0x1)
/** Constant FCHIP2000 for sub-register SR_FCHIP in register BBC0_OQPSKC0 */
#define BB_FCHIP2000                    (0x3)
/** Constant RC08 for sub-register SR_MOD in register BBC0_OQPSKC0 */
#define BB_RC08                         (0x0)
/** Constant RRC08 for sub-register SR_MOD in register BBC0_OQPSKC0 */
#define BB_RRC08                        (0x1)
/** Address for register BBC0_OQPSKC1 */
#define RG_BBC0_OQPSKC1                 (0x311)
/** Address for register BBC0_OQPSKC2 */
#define RG_BBC0_OQPSKC2                 (0x312)
/** Address for register BBC0_OQPSKC3 */
#define RG_BBC0_OQPSKC3                 (0x313)
/** Address for register BBC0_OQPSKPHRTX */
#define RG_BBC0_OQPSKPHRTX              (0x314)
/** Address for register BBC0_OQPSKPHRRX */
#define RG_BBC0_OQPSKPHRRX              (0x315)
/** Address for register BBC0_AFC0 */
#define RG_BBC0_AFC0                    (0x320)
/** Address for register BBC0_AFC1 */
#define RG_BBC0_AFC1                    (0x321)
/** Address for register BBC0_AFFTM */
#define RG_BBC0_AFFTM                   (0x322)
/** Address for register BBC0_AFFVM */
#define RG_BBC0_AFFVM                   (0x323)
/** Address for register BBC0_AFS */
#define RG_BBC0_AFS                     (0x324)
/** Address for register BBC0_MACEA0 */
#define RG_BBC0_MACEA0                  (0x325)
/** Address for register BBC0_MACEA1 */
#define RG_BBC0_MACEA1                  (0x326)
/** Address for register BBC0_MACEA2 */
#define RG_BBC0_MACEA2                  (0x327)
/** Address for register BBC0_MACEA3 */
#define RG_BBC0_MACEA3                  (0x328)
/** Address for register BBC0_MACEA4 */
#define RG_BBC0_MACEA4                  (0x329)
/** Address for register BBC0_MACEA5 */
#define RG_BBC0_MACEA5                  (0x32A)
/** Address for register BBC0_MACEA6 */
#define RG_BBC0_MACEA6                  (0x32B)
/** Address for register BBC0_MACEA7 */
#define RG_BBC0_MACEA7                  (0x32C)
/** Address for register BBC0_MACPID0F0 */
#define RG_BBC0_MACPID0F0               (0x32D)
/** Address for register BBC0_MACPID1F0 */
#define RG_BBC0_MACPID1F0               (0x32E)
/** Address for register BBC0_MACSHA0F0 */
#define RG_BBC0_MACSHA0F0               (0x32F)
/** Address for register BBC0_MACSHA1F0 */
#define RG_BBC0_MACSHA1F0               (0x330)
/** Address for register BBC0_MACPID0F1 */
#define RG_BBC0_MACPID0F1               (0x331)
/** Address for register BBC0_MACPID1F1 */
#define RG_BBC0_MACPID1F1               (0x332)
/** Address for register BBC0_MACSHA0F1 */
#define RG_BBC0_MACSHA0F1               (0x333)
/** Address for register BBC0_MACSHA1F1 */
#define RG_BBC0_MACSHA1F1               (0x334)
/** Address for register BBC0_MACPID0F2 */
#define RG_BBC0_MACPID0F2               (0x335)
/** Address for register BBC0_MACPID1F2 */
#define RG_BBC0_MACPID1F2               (0x336)
/** Address for register BBC0_MACSHA0F2 */
#define RG_BBC0_MACSHA0F2               (0x337)
/** Address for register BBC0_MACSHA1F2 */
#define RG_BBC0_MACSHA1F2               (0x338)
/** Address for register BBC0_MACPID0F3 */
#define RG_BBC0_MACPID0F3               (0x339)
/** Address for register BBC0_MACPID1F3 */
#define RG_BBC0_MACPID1F3               (0x33A)
/** Address for register BBC0_MACSHA0F3 */
#define RG_BBC0_MACSHA0F3               (0x33B)
/** Address for register BBC0_MACSHA1F3 */
#define RG_BBC0_MACSHA1F3               (0x33C)
/** Address for register BBC0_AMCS */
#define RG_BBC0_AMCS                    (0x340)
/** Constant CH_BUSY for sub-register SR_CCAED in register BBC0_AMCS */
#define BB_CH_BUSY                      (0x1)
/** Constant CH_CLEAR for sub-register SR_CCAED in register BBC0_AMCS */
#define BB_CH_CLEAR                     (0x0)
/** Address for register BBC0_AMEDT */
#define RG_BBC0_AMEDT                   (0x341)
/** Address for register BBC0_AMAACKPD */
#define RG_BBC0_AMAACKPD                (0x342)
/** Address for register BBC0_AMAACKTL */
#define RG_BBC0_AMAACKTL                (0x343)
/** Address for register BBC0_AMAACKTH */
#define RG_BBC0_AMAACKTH                (0x344)
/** Address for register BBC0_FSKC0 */
#define RG_BBC0_FSKC0                   (0x360)
/** Address for register BBC0_FSKC1 */
#define RG_BBC0_FSKC1                   (0x361)
/** Address for register BBC0_FSKC2 */
#define RG_BBC0_FSKC2                   (0x362)
/** Address for register BBC0_FSKC3 */
#define RG_BBC0_FSKC3                   (0x363)
/** Address for register BBC0_FSKC4 */
#define RG_BBC0_FSKC4                   (0x364)
/** Address for register BBC0_FSKPLL */
#define RG_BBC0_FSKPLL                  (0x365)
/** Address for register BBC0_FSKSFD0L */
#define RG_BBC0_FSKSFD0L                (0x366)
/** Address for register BBC0_FSKSFD0H */
#define RG_BBC0_FSKSFD0H                (0x367)
/** Address for register BBC0_FSKSFD1L */
#define RG_BBC0_FSKSFD1L                (0x368)
/** Address for register BBC0_FSKSFD1H */
#define RG_BBC0_FSKSFD1H                (0x369)
/** Address for register BBC0_FSKPHRTX */
#define RG_BBC0_FSKPHRTX                (0x36A)
/** Address for register BBC0_FSKPHRRX */
#define RG_BBC0_FSKPHRRX                (0x36B)
/** Address for register BBC0_FSKRPC */
#define RG_BBC0_FSKRPC                  (0x36C)
/** Address for register BBC0_FSKRPCONT */
#define RG_BBC0_FSKRPCONT               (0x36D)
/** Address for register BBC0_FSKRPCOFFT */
#define RG_BBC0_FSKRPCOFFT              (0x36E)
/** Address for register BBC0_FSKRRXFLL */
#define RG_BBC0_FSKRRXFLL               (0x370)
/** Address for register BBC0_FSKRRXFLH */
#define RG_BBC0_FSKRRXFLH               (0x371)
/** Address for register BBC0_FSKDM */
#define RG_BBC0_FSKDM                   (0x372)
/** Address for register BBC0_FSKPE0 */
#define RG_BBC0_FSKPE0                  (0x373)
/** Address for register BBC0_FSKPE1 */
#define RG_BBC0_FSKPE1                  (0x374)
/** Address for register BBC0_FSKPE2 */
#define RG_BBC0_FSKPE2                  (0x375)
/** Address for register BBC1_FBRXE */
#define RG_BBC1_FBRXE                   (0x37FE)
/** Address for register BBC0_PMUC */
#define RG_BBC0_PMUC                    (0x380)
/** Address for register BBC1_FBTXS */
#define RG_BBC1_FBTXS                   (0x3800)
/** Address for register BBC0_PMUVAL */
#define RG_BBC0_PMUVAL                  (0x381)
/** Address for register BBC0_PMUQF */
#define RG_BBC0_PMUQF                   (0x382)
/** Address for register BBC0_PMUI */
#define RG_BBC0_PMUI                    (0x383)
/** Address for register BBC0_PMUQ */
#define RG_BBC0_PMUQ                    (0x384)
/** Address for register BBC0_CNTC */
#define RG_BBC0_CNTC                    (0x390)
/** Address for register BBC0_CNT0 */
#define RG_BBC0_CNT0                    (0x391)
/** Address for register BBC0_CNT1 */
#define RG_BBC0_CNT1                    (0x392)
/** Address for register BBC0_CNT2 */
#define RG_BBC0_CNT2                    (0x393)
/** Address for register BBC0_CNT3 */
#define RG_BBC0_CNT3                    (0x394)
/** Address for register BBC1_FBTXE */
#define RG_BBC1_FBTXE                   (0x3FFE)
/** Address for register BBC1_IRQM */
#define RG_BBC1_IRQM                    (0x400)
/** Address for register BBC1_PC */
#define RG_BBC1_PC                      (0x401)
/** Address for register BBC1_PS */
#define RG_BBC1_PS                      (0x402)
/** Address for register BBC1_RXFLL */
#define RG_BBC1_RXFLL                   (0x404)
/** Address for register BBC1_RXFLH */
#define RG_BBC1_RXFLH                   (0x405)
/** Address for register BBC1_TXFLL */
#define RG_BBC1_TXFLL                   (0x406)
/** Address for register BBC1_TXFLH */
#define RG_BBC1_TXFLH                   (0x407)
/** Address for register BBC1_FBLL */
#define RG_BBC1_FBLL                    (0x408)
/** Address for register BBC1_FBLH */
#define RG_BBC1_FBLH                    (0x409)
/** Address for register BBC1_FBLIL */
#define RG_BBC1_FBLIL                   (0x40A)
/** Address for register BBC1_FBLIH */
#define RG_BBC1_FBLIH                   (0x40B)
/** Address for register BBC1_OFDMPHRTX */
#define RG_BBC1_OFDMPHRTX               (0x40C)
/** Address for register BBC1_OFDMPHRRX */
#define RG_BBC1_OFDMPHRRX               (0x40D)
/** Address for register BBC1_OFDMC */
#define RG_BBC1_OFDMC                   (0x40E)
/** Address for register BBC1_OFDMSW */
#define RG_BBC1_OFDMSW                  (0x40F)
/** Address for register BBC1_OQPSKC0 */
#define RG_BBC1_OQPSKC0                 (0x410)
/** Address for register BBC1_OQPSKC1 */
#define RG_BBC1_OQPSKC1                 (0x411)
/** Address for register BBC1_OQPSKC2 */
#define RG_BBC1_OQPSKC2                 (0x412)
/** Address for register BBC1_OQPSKC3 */
#define RG_BBC1_OQPSKC3                 (0x413)
/** Address for register BBC1_OQPSKPHRTX */
#define RG_BBC1_OQPSKPHRTX              (0x414)
/** Address for register BBC1_OQPSKPHRRX */
#define RG_BBC1_OQPSKPHRRX              (0x415)
/** Address for register BBC1_AFC0 */
#define RG_BBC1_AFC0                    (0x420)
/** Address for register BBC1_AFC1 */
#define RG_BBC1_AFC1                    (0x421)
/** Address for register BBC1_AFFTM */
#define RG_BBC1_AFFTM                   (0x422)
/** Address for register BBC1_AFFVM */
#define RG_BBC1_AFFVM                   (0x423)
/** Address for register BBC1_AFS */
#define RG_BBC1_AFS                     (0x424)
/** Address for register BBC1_MACEA0 */
#define RG_BBC1_MACEA0                  (0x425)
/** Address for register BBC1_MACEA1 */
#define RG_BBC1_MACEA1                  (0x426)
/** Address for register BBC1_MACEA2 */
#define RG_BBC1_MACEA2                  (0x427)
/** Address for register BBC1_MACEA3 */
#define RG_BBC1_MACEA3                  (0x428)
/** Address for register BBC1_MACEA4 */
#define RG_BBC1_MACEA4                  (0x429)
/** Address for register BBC1_MACEA5 */
#define RG_BBC1_MACEA5                  (0x42A)
/** Address for register BBC1_MACEA6 */
#define RG_BBC1_MACEA6                  (0x42B)
/** Address for register BBC1_MACEA7 */
#define RG_BBC1_MACEA7                  (0x42C)
/** Address for register BBC1_MACPID0F0 */
#define RG_BBC1_MACPID0F0               (0x42D)
/** Address for register BBC1_MACPID1F0 */
#define RG_BBC1_MACPID1F0               (0x42E)
/** Address for register BBC1_MACSHA0F0 */
#define RG_BBC1_MACSHA0F0               (0x42F)
/** Address for register BBC1_MACSHA1F0 */
#define RG_BBC1_MACSHA1F0               (0x430)
/** Address for register BBC1_MACPID0F1 */
#define RG_BBC1_MACPID0F1               (0x431)
/** Address for register BBC1_MACPID1F1 */
#define RG_BBC1_MACPID1F1               (0x432)
/** Address for register BBC1_MACSHA0F1 */
#define RG_BBC1_MACSHA0F1               (0x433)
/** Address for register BBC1_MACSHA1F1 */
#define RG_BBC1_MACSHA1F1               (0x434)
/** Address for register BBC1_MACPID0F2 */
#define RG_BBC1_MACPID0F2               (0x435)
/** Address for register BBC1_MACPID1F2 */
#define RG_BBC1_MACPID1F2               (0x436)
/** Address for register BBC1_MACSHA0F2 */
#define RG_BBC1_MACSHA0F2               (0x437)
/** Address for register BBC1_MACSHA1F2 */
#define RG_BBC1_MACSHA1F2               (0x438)
/** Address for register BBC1_MACPID0F3 */
#define RG_BBC1_MACPID0F3               (0x439)
/** Address for register BBC1_MACPID1F3 */
#define RG_BBC1_MACPID1F3               (0x43A)
/** Address for register BBC1_MACSHA0F3 */
#define RG_BBC1_MACSHA0F3               (0x43B)
/** Address for register BBC1_MACSHA1F3 */
#define RG_BBC1_MACSHA1F3               (0x43C)
/** Address for register BBC1_AMCS */
#define RG_BBC1_AMCS                    (0x440)
/** Address for register BBC1_AMEDT */
#define RG_BBC1_AMEDT                   (0x441)
/** Address for register BBC1_AMAACKPD */
#define RG_BBC1_AMAACKPD                (0x442)
/** Address for register BBC1_AMAACKTL */
#define RG_BBC1_AMAACKTL                (0x443)
/** Address for register BBC1_AMAACKTH */
#define RG_BBC1_AMAACKTH                (0x444)
/** Address for register BBC1_FSKC0 */
#define RG_BBC1_FSKC0                   (0x460)
/** Address for register BBC1_FSKC1 */
#define RG_BBC1_FSKC1                   (0x461)
/** Address for register BBC1_FSKC2 */
#define RG_BBC1_FSKC2                   (0x462)
/** Address for register BBC1_FSKC3 */
#define RG_BBC1_FSKC3                   (0x463)
/** Address for register BBC1_FSKC4 */
#define RG_BBC1_FSKC4                   (0x464)
/** Address for register BBC1_FSKPLL */
#define RG_BBC1_FSKPLL                  (0x465)
/** Address for register BBC1_FSKSFD0L */
#define RG_BBC1_FSKSFD0L                (0x466)
/** Address for register BBC1_FSKSFD0H */
#define RG_BBC1_FSKSFD0H                (0x467)
/** Address for register BBC1_FSKSFD1L */
#define RG_BBC1_FSKSFD1L                (0x468)
/** Address for register BBC1_FSKSFD1H */
#define RG_BBC1_FSKSFD1H                (0x469)
/** Address for register BBC1_FSKPHRTX */
#define RG_BBC1_FSKPHRTX                (0x46A)
/** Address for register BBC1_FSKPHRRX */
#define RG_BBC1_FSKPHRRX                (0x46B)
/** Address for register BBC1_FSKRPC */
#define RG_BBC1_FSKRPC                  (0x46C)
/** Address for register BBC1_FSKRPCONT */
#define RG_BBC1_FSKRPCONT               (0x46D)
/** Address for register BBC1_FSKRPCOFFT */
#define RG_BBC1_FSKRPCOFFT              (0x46E)
/** Address for register BBC1_FSKRRXFLL */
#define RG_BBC1_FSKRRXFLL               (0x470)
/** Address for register BBC1_FSKRRXFLH */
#define RG_BBC1_FSKRRXFLH               (0x471)
/** Address for register BBC1_FSKDM */
#define RG_BBC1_FSKDM                   (0x472)
/** Address for register BBC1_FSKPE0 */
#define RG_BBC1_FSKPE0                  (0x473)
/** Address for register BBC1_FSKPE1 */
#define RG_BBC1_FSKPE1                  (0x474)
/** Address for register BBC1_FSKPE2 */
#define RG_BBC1_FSKPE2                  (0x475)
/** Address for register BBC1_PMUC */
#define RG_BBC1_PMUC                    (0x480)
/** Address for register BBC1_PMUVAL */
#define RG_BBC1_PMUVAL                  (0x481)
/** Address for register BBC1_PMUQF */
#define RG_BBC1_PMUQF                   (0x482)
/** Address for register BBC1_PMUI */
#define RG_BBC1_PMUI                    (0x483)
/** Address for register BBC1_PMUQ */
#define RG_BBC1_PMUQ                    (0x484)
/** Address for register BBC1_CNTC */
#define RG_BBC1_CNTC                    (0x490)
/** Address for register BBC1_CNT0 */
#define RG_BBC1_CNT0                    (0x491)
/** Address for register BBC1_CNT1 */
#define RG_BBC1_CNT1                    (0x492)
/** Address for register BBC1_CNT2 */
#define RG_BBC1_CNT2                    (0x493)
/** Address for register BBC1_CNT3 */
#define RG_BBC1_CNT3                    (0x494)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AT86RF215_REGISTERS_H */
