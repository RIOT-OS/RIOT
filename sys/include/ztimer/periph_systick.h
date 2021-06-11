/*
 * Copyright (C) 2021   Franz Freitag, Justus Krebs, Nick Weiler
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ztimer_periph_systick  ztimer periph/systick backend
 * @ingroup     sys_ztimer
 * @brief       ztimer periph/systick backend
 *
 * This ztimer module implements a ztimer virtual clock on top of periph/systick.
 *
 * @{
 *
 * @file
 * @brief       ztimer periph/systick backend API
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 */

#ifndef ZTIMER_PERIPH_SYSTICK_H
#define ZTIMER_PERIPH_SYSTICK_H

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ztimer periph/systick backend initialization function
 *
 * @param[in, out]  clock   ztimer_periph_systick_t object to initialize
 */
void ztimer_periph_systick_init(ztimer_clock_t *clock);

#ifdef __cplusplus
}
#endif

#endif /* ZTIMER_PERIPH_SYSTICK_H */
/** @} */
