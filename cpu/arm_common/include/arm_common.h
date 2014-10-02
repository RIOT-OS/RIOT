/*
 * Copyright (C) 2008-2009, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef __ARM_COMMON_H
#define __ARM_COMMON_H

/**
 * @defgroup    arm_common ARM CPU common
 * @brief       Common implementations and headers for ARM based micro-controllers
 * @ingroup     cpu
 * @{
 */

/**
 * @file
 * @brief       ARM CPU common declarations
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 *
 */

#include "bitarithm.h"

/**
 * @name    PCONP Constants
 * @{
 */
#define PCTIM0      (BIT1)
#define PCTIM1      (BIT2)
#define PCUART0     (BIT3)
#define PCUART1     (BIT4)
#define PCPWM1      (BIT6)
#define PCI2C0      (BIT7)
#define PCSPI       (BIT8)
#define PCRTC       (BIT9)
#define PCSSP1      (BIT10)
#define PCEMC       (BIT11)
#define PCAD        (BIT12)
#define PCAN1       (BIT13)
#define PCAN2       (BIT14)
#define PCI2C1      (BIT19)
#define PCSSP0      (BIT21)
#define PCTIM2      (BIT22)
#define PCTIM3      (BIT23)
#define PCUART2     (BIT24)
#define PCUART3     (BIT25)
#define PCI2C2      (BIT26)
#define PCI2S       (BIT27)
#define PCSDC       (BIT28)
#define PCGPDMA     (BIT29)
#define PCENET      (BIT30)
#define PCUSB       (BIT31)
/** @} */

/**
 * @name    PCON Constants
 * @{
 */
#define PM0         (BIT0)
#define PM1         (BIT1)
#define BODPDM      (BIT2)
#define BOGD        (BIT3)
#define BORD        (BIT4)
#define PM2         (BIT7)

#define PM_IDLE         (PM0)
#define PM_SLEEP        (PM2|PM0)
#define PM_MT_TRANSCEIVER_POWERDOWN    (PM1)
/** @} */

/**
 * @name    Timer Symbols
 * @{
 */
#define MR0I (BIT0)
#define MR0R (BIT1)
#define MR0S (BIT2)
#define MR1I (BIT3)
#define MR1R (BIT4)
#define MR1S (BIT5)
#define MR2I (BIT6)
#define MR2R (BIT7)
#define MR2S (BIT8)
#define MR3I (BIT9)
#define MR3R (BIT10)
#define MR3S (BIT11)
/** @} */

/** @} */
#endif // __ARM_COMMON_H
