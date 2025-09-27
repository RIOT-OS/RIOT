/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_sam0_common
 * @brief           Generic Timer MCLK masks.
 * @{
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Automatically generated helper defines
 * @{
 */
#ifdef MCLK_APBAMASK_TC0
#  define MCLK_TC0        (&MCLK->APBAMASK.reg)
#  define MCLK_TC0_MASK   (MCLK_APBAMASK_TC0)
#endif
#ifdef MCLK_APBBMASK_TC0
#  define MCLK_TC0        (&MCLK->APBBMASK.reg)
#  define MCLK_TC0_MASK   (MCLK_APBBMASK_TC0)
#endif
#ifdef MCLK_APBCMASK_TC0
#  define MCLK_TC0        (&MCLK->APBCMASK.reg)
#  define MCLK_TC0_MASK   (MCLK_APBCMASK_TC0)
#endif
#ifdef MCLK_APBDMASK_TC0
#  define MCLK_TC0        (&MCLK->APBDMASK.reg)
#  define MCLK_TC0_MASK   (MCLK_APBDMASK_TC0)
#endif

#ifdef MCLK_APBAMASK_TC1
#  define MCLK_TC1        (&MCLK->APBAMASK.reg)
#  define MCLK_TC1_MASK   (MCLK_APBAMASK_TC1)
#endif
#ifdef MCLK_APBBMASK_TC1
#  define MCLK_TC1        (&MCLK->APBBMASK.reg)
#  define MCLK_TC1_MASK   (MCLK_APBBMASK_TC1)
#endif
#ifdef MCLK_APBCMASK_TC1
#  define MCLK_TC1        (&MCLK->APBCMASK.reg)
#  define MCLK_TC1_MASK   (MCLK_APBCMASK_TC1)
#endif
#ifdef MCLK_APBDMASK_TC1
#  define MCLK_TC1        (&MCLK->APBDMASK.reg)
#  define MCLK_TC1_MASK   (MCLK_APBDMASK_TC1)
#endif

#ifdef MCLK_APBAMASK_TC2
#  define MCLK_TC2        (&MCLK->APBAMASK.reg)
#  define MCLK_TC2_MASK   (MCLK_APBAMASK_TC2)
#endif
#ifdef MCLK_APBBMASK_TC2
#  define MCLK_TC2        (&MCLK->APBBMASK.reg)
#  define MCLK_TC2_MASK   (MCLK_APBBMASK_TC2)
#endif
#ifdef MCLK_APBCMASK_TC2
#  define MCLK_TC2        (&MCLK->APBCMASK.reg)
#  define MCLK_TC2_MASK   (MCLK_APBCMASK_TC2)
#endif
#ifdef MCLK_APBDMASK_TC2
#  define MCLK_TC2        (&MCLK->APBDMASK.reg)
#  define MCLK_TC2_MASK   (MCLK_APBDMASK_TC2)
#endif

#ifdef MCLK_APBAMASK_TC3
#  define MCLK_TC3        (&MCLK->APBAMASK.reg)
#  define MCLK_TC3_MASK   (MCLK_APBAMASK_TC3)
#endif
#ifdef MCLK_APBBMASK_TC3
#  define MCLK_TC3        (&MCLK->APBBMASK.reg)
#  define MCLK_TC3_MASK   (MCLK_APBBMASK_TC3)
#endif
#ifdef MCLK_APBCMASK_TC3
#  define MCLK_TC3        (&MCLK->APBCMASK.reg)
#  define MCLK_TC3_MASK   (MCLK_APBCMASK_TC3)
#endif
#ifdef MCLK_APBDMASK_TC3
#  define MCLK_TC3        (&MCLK->APBDMASK.reg)
#  define MCLK_TC3_MASK   (MCLK_APBDMASK_TC3)
#endif

#ifdef MCLK_APBAMASK_TC4
#  define MCLK_TC4        (&MCLK->APBAMASK.reg)
#  define MCLK_TC4_MASK   (MCLK_APBAMASK_TC4)
#endif
#ifdef MCLK_APBBMASK_TC4
#  define MCLK_TC4        (&MCLK->APBBMASK.reg)
#  define MCLK_TC4_MASK   (MCLK_APBBMASK_TC4)
#endif
#ifdef MCLK_APBCMASK_TC4
#  define MCLK_TC4        (&MCLK->APBCMASK.reg)
#  define MCLK_TC4_MASK   (MCLK_APBCMASK_TC4)
#endif
#ifdef MCLK_APBDMASK_TC4
#  define MCLK_TC4        (&MCLK->APBDMASK.reg)
#  define MCLK_TC4_MASK   (MCLK_APBDMASK_TC4)
#endif

#ifdef MCLK_APBAMASK_TC5
#  define MCLK_TC5        (&MCLK->APBAMASK.reg)
#  define MCLK_TC5_MASK   (MCLK_APBAMASK_TC5)
#endif
#ifdef MCLK_APBBMASK_TC5
#  define MCLK_TC5        (&MCLK->APBBMASK.reg)
#  define MCLK_TC5_MASK   (MCLK_APBBMASK_TC5)
#endif
#ifdef MCLK_APBCMASK_TC5
#  define MCLK_TC5        (&MCLK->APBCMASK.reg)
#  define MCLK_TC5_MASK   (MCLK_APBCMASK_TC5)
#endif
#ifdef MCLK_APBDMASK_TC5
#  define MCLK_TC5        (&MCLK->APBDMASK.reg)
#  define MCLK_TC5_MASK   (MCLK_APBDMASK_TC5)
#endif

#ifdef MCLK_APBAMASK_TC6
#  define MCLK_TC6        (&MCLK->APBAMASK.reg)
#  define MCLK_TC6_MASK   (MCLK_APBAMASK_TC6)
#endif
#ifdef MCLK_APBBMASK_TC6
#  define MCLK_TC6        (&MCLK->APBBMASK.reg)
#  define MCLK_TC6_MASK   (MCLK_APBBMASK_TC6)
#endif
#ifdef MCLK_APBCMASK_TC6
#  define MCLK_TC6        (&MCLK->APBCMASK.reg)
#  define MCLK_TC6_MASK   (MCLK_APBCMASK_TC6)
#endif
#ifdef MCLK_APBDMASK_TC6
#  define MCLK_TC6        (&MCLK->APBDMASK.reg)
#  define MCLK_TC6_MASK   (MCLK_APBDMASK_TC6)
#endif

#ifdef MCLK_APBAMASK_TC7
#  define MCLK_TC7        (&MCLK->APBAMASK.reg)
#  define MCLK_TC7_MASK   (MCLK_APBAMASK_TC7)
#endif
#ifdef MCLK_APBBMASK_TC7
#  define MCLK_TC7        (&MCLK->APBBMASK.reg)
#  define MCLK_TC7_MASK   (MCLK_APBBMASK_TC7)
#endif
#ifdef MCLK_APBCMASK_TC7
#  define MCLK_TC7        (&MCLK->APBCMASK.reg)
#  define MCLK_TC7_MASK   (MCLK_APBCMASK_TC7)
#endif
#ifdef MCLK_APBDMASK_TC7
#  define MCLK_TC7        (&MCLK->APBDMASK.reg)
#  define MCLK_TC7_MASK   (MCLK_APBDMASK_TC7)
#endif

#ifdef MCLK_APBAMASK_TCC0
#  define MCLK_TCC0        (&MCLK->APBAMASK.reg)
#  define MCLK_TCC0_MASK   (MCLK_APBAMASK_TCC0)
#endif
#ifdef MCLK_APBBMASK_TCC0
#  define MCLK_TCC0        (&MCLK->APBBMASK.reg)
#  define MCLK_TCC0_MASK   (MCLK_APBBMASK_TCC0)
#endif
#ifdef MCLK_APBCMASK_TCC0
#  define MCLK_TCC0        (&MCLK->APBCMASK.reg)
#  define MCLK_TCC0_MASK   (MCLK_APBCMASK_TCC0)
#endif
#ifdef MCLK_APBDMASK_TCC0
#  define MCLK_TCC0        (&MCLK->APBDMASK.reg)
#  define MCLK_TCC0_MASK   (MCLK_APBDMASK_TCC0)
#endif

#ifdef MCLK_APBAMASK_TCC1
#  define MCLK_TCC1        (&MCLK->APBAMASK.reg)
#  define MCLK_TCC1_MASK   (MCLK_APBAMASK_TCC1)
#endif
#ifdef MCLK_APBBMASK_TCC1
#  define MCLK_TCC1        (&MCLK->APBBMASK.reg)
#  define MCLK_TCC1_MASK   (MCLK_APBBMASK_TCC1)
#endif
#ifdef MCLK_APBCMASK_TCC1
#  define MCLK_TCC1        (&MCLK->APBCMASK.reg)
#  define MCLK_TCC1_MASK   (MCLK_APBCMASK_TCC1)
#endif
#ifdef MCLK_APBDMASK_TCC1
#  define MCLK_TCC1        (&MCLK->APBDMASK.reg)
#  define MCLK_TCC1_MASK   (MCLK_APBDMASK_TCC1)
#endif

#ifdef MCLK_APBAMASK_TCC2
#  define MCLK_TCC2        (&MCLK->APBAMASK.reg)
#  define MCLK_TCC2_MASK   (MCLK_APBAMASK_TCC2)
#endif
#ifdef MCLK_APBBMASK_TCC2
#  define MCLK_TCC2        (&MCLK->APBBMASK.reg)
#  define MCLK_TCC2_MASK   (MCLK_APBBMASK_TCC2)
#endif
#ifdef MCLK_APBCMASK_TCC2
#  define MCLK_TCC2        (&MCLK->APBCMASK.reg)
#  define MCLK_TCC2_MASK   (MCLK_APBCMASK_TCC2)
#endif
#ifdef MCLK_APBDMASK_TCC2
#  define MCLK_TCC2        (&MCLK->APBDMASK.reg)
#  define MCLK_TCC2_MASK   (MCLK_APBDMASK_TCC2)
#endif

#ifdef MCLK_APBAMASK_TCC3
#  define MCLK_TCC3        (&MCLK->APBAMASK.reg)
#  define MCLK_TCC3_MASK   (MCLK_APBAMASK_TCC3)
#endif
#ifdef MCLK_APBBMASK_TCC3
#  define MCLK_TCC3        (&MCLK->APBBMASK.reg)
#  define MCLK_TCC3_MASK   (MCLK_APBBMASK_TCC3)
#endif
#ifdef MCLK_APBCMASK_TCC3
#  define MCLK_TCC3        (&MCLK->APBCMASK.reg)
#  define MCLK_TCC3_MASK   (MCLK_APBCMASK_TCC3)
#endif
#ifdef MCLK_APBDMASK_TCC3
#  define MCLK_TCC3        (&MCLK->APBDMASK.reg)
#  define MCLK_TCC3_MASK   (MCLK_APBDMASK_TCC3)
#endif

#ifdef MCLK_APBAMASK_TCC4
#  define MCLK_TCC4        (&MCLK->APBAMASK.reg)
#  define MCLK_TCC4_MASK   (MCLK_APBAMASK_TCC4)
#endif
#ifdef MCLK_APBBMASK_TCC4
#  define MCLK_TCC4        (&MCLK->APBBMASK.reg)
#  define MCLK_TCC4_MASK   (MCLK_APBBMASK_TCC4)
#endif
#ifdef MCLK_APBCMASK_TCC4
#  define MCLK_TCC4        (&MCLK->APBCMASK.reg)
#  define MCLK_TCC4_MASK   (MCLK_APBCMASK_TCC4)
#endif
#ifdef MCLK_APBDMASK_TCC4
#  define MCLK_TCC4        (&MCLK->APBDMASK.reg)
#  define MCLK_TCC4_MASK   (MCLK_APBDMASK_TCC4)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
