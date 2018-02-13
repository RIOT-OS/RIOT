/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Collection of pre-computed bus pre-scalers for SPI configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CFG_SPI_DIVTABLE_H
#define CFG_SPI_DIVTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SPI bus divider values for pre-defined peripheral bus clock speeds
 *
 * @note    These spi_divtables are generated using
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 * @{
 */
#define CFG_SPIDIV_20               \
    {       /* for 20000000Hz */    \
        7,  /* -> 78125Hz */        \
        5,  /* -> 312500Hz */       \
        3,  /* -> 1250000Hz */      \
        1,  /* -> 5000000Hz */      \
        0   /* -> 10000000Hz */     \
    },

#define CFG_SPIDIV_30               \
    {       /* for 30000000Hz */    \
        7,  /* -> 117187Hz */       \
        5,  /* -> 468750Hz */       \
        4,  /* -> 937500Hz */       \
        2,  /* -> 3750000Hz */      \
        1   /* -> 7500000Hz */      \
    },

#define CFG_SPIDIV_32               \
    {       /* for 32000000Hz */    \
        7,  /* -> 125000Hz */       \
        5,  /* -> 500000Hz */       \
        4,  /* -> 1000000Hz */      \
        2,  /* -> 4000000Hz */      \
        1   /* -> 8000000Hz */      \
    },

#define CFG_SPIDIV_36               \
    {       /* for 36000000Hz */    \
        7,  /* -> 140625Hz */       \
        6,  /* -> 281250Hz */       \
        4,  /* -> 1125000Hz */      \
        2,  /* -> 4500000Hz */      \
        1   /* -> 9000000Hz */      \
    },

#define CFG_SPIDIV_40               \
    {       /* for 40000000Hz */    \
        7,  /* -> 156250Hz */       \
        6,  /* -> 312500Hz */       \
        4,  /* -> 1250000Hz */      \
        2,  /* -> 5000000Hz */      \
        1   /* -> 10000000Hz */     \
    },

#define CFG_SPIDIV_42               \
    {       /* for 42000000Hz */    \
        7,  /* -> 164062Hz */       \
        6,  /* -> 328125Hz */       \
        4,  /* -> 1312500Hz */      \
        2,  /* -> 5250000Hz */      \
        1   /* -> 10500000Hz */     \
    },

#define CFG_SPIDIV_45               \
    {       /* for 45000000Hz */    \
        7,  /* -> 175781Hz */       \
        6,  /* -> 351562Hz */       \
        5,  /* -> 703125Hz */       \
        2,  /* -> 5625000Hz */      \
        1   /* -> 11250000Hz */     \
    },

#define CFG_SPIDIV_48               \
    {       /* for 48000000Hz */    \
        7,  /* -> 187500Hz */       \
        6,  /* -> 375000Hz */       \
        5,  /* -> 750000Hz */       \
        2,  /* -> 6000000Hz */      \
        1   /* -> 12000000Hz */     \
    },

#define CFG_SPIDIV_50               \
    {       /* for 50000000Hz */    \
        7,  /* -> 195312Hz */       \
        6,  /* -> 390625Hz */       \
        5,  /* -> 781250Hz */       \
        2,  /* -> 6250000Hz */      \
        1   /* -> 12500000Hz */     \
    },

#define CFG_SPIDIV_60               \
    {       /* for 60000000Hz */    \
        7,  /* -> 234375Hz */       \
        6,  /* -> 468750Hz */       \
        5,  /* -> 937500Hz */       \
        3,  /* -> 3750000Hz */      \
        2   /* -> 7500000Hz */      \
    },

#define CFG_SPIDIV_64               \
    {       /* for 64000000Hz */    \
        7,  /* -> 250000Hz */       \
        6,  /* -> 500000Hz */       \
        5,  /* -> 1000000Hz */      \
        3,  /* -> 4000000Hz */      \
        2   /* -> 8000000Hz */      \
    },

#define CFG_SPIDIV_72               \
    {       /* for 72000000Hz */    \
        7,  /* -> 281250Hz */       \
        7,  /* -> 281250Hz */       \
        5,  /* -> 1125000Hz */      \
        3,  /* -> 4500000Hz */      \
        2   /* -> 9000000Hz */      \
    },

#define CFG_SPIDIV_84               \
    {       /* for 84000000Hz */    \
        7,  /* -> 328125Hz */       \
        7,  /* -> 328125Hz */       \
        5,  /* -> 1312500Hz */      \
        3,  /* -> 5250000Hz */      \
        2   /* -> 10500000Hz */     \
    },

#define CFG_SPIDIV_90               \
    {       /* for 90000000Hz */    \
        7,  /* -> 351562Hz */       \
        7,  /* -> 351562Hz */       \
        6,  /* -> 703125Hz */       \
        3,  /* -> 5625000Hz */      \
        2   /* -> 11250000Hz */     \
    },

#define CFG_SPIDIV_96               \
    {       /* for 96000000Hz */    \
        7,  /* -> 375000Hz */       \
        7,  /* -> 375000Hz */       \
        6,  /* -> 750000Hz */       \
        3,  /* -> 6000000Hz */      \
        2   /* -> 12000000Hz */     \
    },

#define CFG_SPIDIV_100              \
    {       /* for 100000000Hz */   \
        7,  /* -> 390625Hz */       \
        7,  /* -> 390625Hz */       \
        6,  /* -> 781250Hz */       \
        3,  /* -> 6250000Hz */      \
        2   /* -> 12500000Hz */     \
    },
/** @} */


/**
 * @name    SPI clock divisors
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 *
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
#if (CLOCK_APB1 == 20000000)
    CFG_SPIDIV_20
#elif (CLOCK_APB1 == 30000000)
    CFG_SPIDIV_30
#elif (CLOCK_APB1 == 32000000)
    CFG_SPIDIV_32
#elif (CLOCK_APB1 == 36000000)
    CFG_SPIDIV_36
#elif (CLOCK_APB1 == 40000000)
    CFG_SPIDIV_40
#elif (CLOCK_APB1 == 42000000)
    CFG_SPIDIV_42
#elif (CLOCK_APB1 == 45000000)
    CFG_SPIDIV_45
#elif (CLOCK_APB1 == 48000000)
    CFG_SPIDIV_48
#elif (CLOCK_APB1 == 50000000)
    CFG_SPIDIV_50
#elif (CLOCK_APB1 == 72000000)
    CFG_SPIDIV_72
#elif (CLOCK_APB1 == 60000000)
    CFG_SPIDIV_60
#elif (CLOCK_APB1 == 64000000)
    CFG_SPIDIV_64
#elif (CLOCK_APB1 == 84000000)
    CFG_SPIDIV_84
#elif (CLOCK_APB1 == 90000000)
    CFG_SPIDIV_90
#elif (CLOCK_APB1 == 96000000)
    CFG_SPIDIV_96
#elif (CLOCK_APB1 == 100000000)
    CFG_SPIDIV_100
#else
#error "CFG_SPI_DIVTABLE: no prescalers for selected APB1 bus clock defined"
#endif

#if (CLOCK_APB2 == 20000000)
    CFG_SPIDIV_20
#elif (CLOCK_APB2 == 30000000)
    CFG_SPIDIV_30
#elif (CLOCK_APB2 == 32000000)
    CFG_SPIDIV_32
#elif (CLOCK_APB2 == 36000000)
    CFG_SPIDIV_36
#elif (CLOCK_APB2 == 40000000)
    CFG_SPIDIV_40
#elif (CLOCK_APB2 == 42000000)
    CFG_SPIDIV_42
#elif (CLOCK_APB2 == 45000000)
    CFG_SPIDIV_45
#elif (CLOCK_APB2 == 48000000)
    CFG_SPIDIV_48
#elif (CLOCK_APB2 == 50000000)
    CFG_SPIDIV_50
#elif (CLOCK_APB2 == 72000000)
    CFG_SPIDIV_72
#elif (CLOCK_APB2 == 60000000)
    CFG_SPIDIV_60
#elif (CLOCK_APB2 == 64000000)
    CFG_SPIDIV_64
#elif (CLOCK_APB2 == 84000000)
    CFG_SPIDIV_84
#elif (CLOCK_APB2 == 90000000)
    CFG_SPIDIV_90
#elif (CLOCK_APB2 == 96000000)
    CFG_SPIDIV_96
#elif (CLOCK_APB2 == 100000000)
    CFG_SPIDIV_100
#else
#error "CFG_SPI_DIVTABLE: no prescalers for selected APB2 bus clock defined"
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* CFG_SPI_DIVTABLE_H */
/** @} */
