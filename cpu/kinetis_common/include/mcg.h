/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_common_mcg Kinetis MCG
 * @ingroup     cpu_kinetis_common
 * @brief       Implementation of the Kinetis Multipurpose Clock Generator
 *              (MCG) driver.
 *              Please add mcg.h in cpu conf.h
 *              and MCG configuration to periph_conf.h
 *
 *              MCG neighbor modes matrix:
 *
 *                x  | FEI | FEE | FBI | FBE | BLPI | BLPE | PBE | PEE
 *              :---:|:---:|:---:|:---:|:---:|:----:|:----:|:---:|:---:
 *              PEE  |  0  |  0  |  0  |  0  |  0   |  0   |  1  |  0
 *              PBE  |  0  |  0  |  0  |  1  |  0   |  1   |  0  |  1
 *              BLPE |  0  |  0  |  0  |  1  |  0   |  0   |  1  |  0
 *              BLPI |  0  |  0  |  1  |  0  |  0   |  0   |  0  |  0
 *              FBE  |  1  |  1  |  1  |  0  |  0   |  1   |  1  |  0
 *              FBI  |  1  |  1  |  0  |  1  |  1   |  0   |  0  |  0
 *              FEE  |  1  |  0  |  1  |  1  |  0   |  0   |  0  |  0
 *              FEI  |  1  |  1  |  1  |  1  |  0   |  0   |  0  |  0
 *
 *              Each neighbor mode can be selected directly.
 *              Further, the paths between the following modes are defined:
 *              -  FEI -> PEE
 *              -  BLPE -> PEE
 *              -  PEE -> BLPE
 *              -  FEE -> BLPE
 *              -  FEE -> BLPI
 *              -  BLPE -> FEE
 *              -  BLPI -> FEE
 *
 *              ### MCG Configuration Examples (for periph_conf.h) ###
 *
 *              Example for FEI Mode (MCGOUTCLK = 20MHz ... 25MHz):
 *
 *                  #define KINETIS_MCG_USE_ERC               0
 *                  #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *
 *
 *              Example for FEE Mode, 32768Hz Crystal,
 *              (MCGOUTCLK = 24MHz):
 *
 *                  #define KINETIS_MCG_USE_ERC               1
 *                  #define KINETIS_MCG_USE_PLL               0
 *                  #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *                  #define KINETIS_MCG_ERC_OSCILLATOR        1
 *                  #define KINETIS_MCG_ERC_FRDIV             0
 *                  #define KINETIS_MCG_ERC_RANGE             0
 *                  #define KINETIS_MCG_ERC_FREQ              (32768U)
 *
 *
 *              Example for FEE Mode, external 4MHz reference
 *              (\f$MCGOUTCLK
 *              = \frac{ERC_{\mathrm{freq}}\cdot FFL_{\mathrm{Factor}}}{FRDIV}
 *              = \frac{4\,\mathrm{MHz}\cdot 732}{128} = 22.875\,\mathrm{MHz}\f$):
 *
 *                  #define KINETIS_MCG_USE_ERC               1
 *                  #define KINETIS_MCG_USE_PLL               0
 *                  #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *                  #define KINETIS_MCG_ERC_OSCILLATOR        0
 *                  #define KINETIS_MCG_ERC_FRDIV             2
 *                  #define KINETIS_MCG_ERC_RANGE             1
 *                  #define KINETIS_MCG_ERC_FREQ              (4000000U)
 *
 *              Example for PEE Mode, external 4MHz reference
 *              (\f$MCGOUTCLK
 *              = \frac{ERC_{\mathrm{freq}}\cdot VDIV0}{PRDIV}
 *              = \frac{4\,\mathrm{MHz}\cdot 24}{2} = 48\,\mathrm{MHz}\f$):
 *
 *                  #define KINETIS_MCG_USE_ERC               1
 *                  #define KINETIS_MCG_USE_PLL               1
 *                  #define KINETIS_MCG_DCO_RANGE             (24000000U)
 *                  #define KINETIS_MCG_ERC_OSCILLATOR        0
 *                  #define KINETIS_MCG_ERC_FRDIV             2
 *                  #define KINETIS_MCG_ERC_RANGE             1
 *                  #define KINETIS_MCG_ERC_FREQ              (4000000U)
 *                  #define KINETIS_MCG_PLL_PRDIV             1
 *                  #define KINETIS_MCG_PLL_VDIV0             0
 *                  #define KINETIS_MCG_PLL_FREQ              (48000000U)
 *
 *
 *              The desired mode can be selected in cpu.c:cpu_clock_init()
 *              with kinetis_mcg_set_mode(KINETIS_MCG_PEE);
 * @{
 *
 * @file
 * @brief       Interface definition for the Kinetis MCG driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __MCG_CPU_H
#define __MCG_CPU_H

#include "cpu_conf.h"
#include "periph_conf.h"

#if KINETIS_CPU_USE_MCG

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum kinetis_mcg_mode {
    KINETIS_MCG_PEE = 0, /**< PLL Engaged External Mode */
    KINETIS_MCG_PBE,     /**< PLL Bypassed External Mode */
    KINETIS_MCG_BLPE,    /**< FLL Bypassed Low Power External Mode */
    KINETIS_MCG_BLPI,    /**< FLL Bypassed Low Power Internal Mode */
    KINETIS_MCG_FBE,     /**< FLL Bypassed External Mode */
    KINETIS_MCG_FBI,     /**< FLL Bypassed Internal Mode */
    KINETIS_MCG_FEE,     /**< FLL Engaged External Mode */
    KINETIS_MCG_FEI,     /**< FLL Engaged Internal Mode */
} kinetis_mcg_mode_t;

/**
 * @brief Initialize the MCG
 *
 */
int kinetis_mcg_set_mode(kinetis_mcg_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __MCG_CPU_H */
/** @} */

#endif /* KINETIS_CPU_USE_MCG */
