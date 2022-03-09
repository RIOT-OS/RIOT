/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bq2429x
 *
 * @{
 * @file
 * @brief       Default configuration for BQ2429x power ICs.
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef BQ2429X_PARAMS_H
#define BQ2429X_PARAMS_H

#include "board.h"
#include "bq2429x.h"

#ifdef __cplusplus
extern "C" {
#endif

/* I2C configuration */
#ifndef BQ2429X_PARAM_I2C
#define BQ2429X_PARAM_I2C       I2C_DEV(0)
#endif

/* Interrupt pin configuration */
#ifndef BQ2429X_PARAM_INT_PIN
#define BQ2429X_PARAM_INT_PIN   GPIO_UNDEF
#endif

/* Control pins */
#ifndef BQ2429X_PARAM_CE_PIN
#define BQ2429X_PARMA_CE_PIN    GPIO_UNDEF
#endif
#ifndef BQ2429X_PARAM_OTG_PIN
#define BQ2429X_PARMA_OTG_PIN   GPIO_UNDEF
#endif

/* Input current/voltage */
#ifndef BQ2429X_PARAM_VLIM
#define BQ2429X_PARAM_VLIM      BQ2429X_VLIM_4360
#endif
#ifndef BQ2429X_PARAM_ILIM
#define BQ2429X_PARAM_ILIM      BQ2429X_ILIM_500
#endif

/* Battery charge current/voltage */
#ifndef BQ2429X_PARAM_ICHG
#define BQ2429X_PARAM_ICHG      BQ2429X_ICHG_512
#endif
#ifndef BQ2429X_PARAM_VREG
#define BQ2429X_PARAM_VREG      BQ2429X_VREG_4208
#endif

#if !IS_USED(MODULE_BQ2429X_INT)

#ifndef BQ2429X_PARAMS
#define BQ2429X_PARAMS  { \
                            .i2c        = BQ2429X_PARAM_I2C,        \
                            .ce_pin     = BQ2429X_PARMA_CE_PIN,     \
                            .otg_pin    = BQ2429X_PARMA_OTG_PIN,    \
                            .vlim       = BQ2429X_PARAM_VLIM,       \
                            .ilim       = BQ2429X_PARAM_ILIM,       \
                            .ichg       = BQ2429X_PARAM_ICHG,       \
                            .vreg       = BQ2429X_PARAM_VREG,       \
                        }
#endif

#else /* !IS_USED(MODULE_BQ2429X_INT) */

#ifndef BQ2429X_PARAMS
#define BQ2429X_PARAMS  { \
                            .i2c        = BQ2429X_PARAM_I2C,        \
                            .int_pin    = BQ2429X_PARAM_INT_PIN,    \
                            .ce_pin     = BQ2429X_PARMA_CE_PIN,     \
                            .otg_pin    = BQ2429X_PARMA_OTG_PIN,    \
                            .vlim       = BQ2429X_PARAM_VLIM,       \
                            .ilim       = BQ2429X_PARAM_ILIM,       \
                            .ichg       = BQ2429X_PARAM_ICHG,       \
                            .vreg       = BQ2429X_PARAM_VREG,       \
                        }
#endif

#endif /* !IS_USED(MODULE_BQ2429X_INT) */

static bq2429x_params_t bq2429x_params[] = {
    BQ2429X_PARAMS,
};

#ifdef __cplusplus
}
#endif

#endif /* BQ2429X_PARAMS_H */
/** @} */
