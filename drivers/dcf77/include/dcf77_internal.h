/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_dcf77
 * @brief
 * @{
 *
 * @file
 * @brief   Bit definitions for DCF77 transmission
 *
 * @author  Michel Gerlach <michel.gerlach@haw-hamburg.de>
 */
#ifndef DCF77_INTERNAL_H
#define DCF77_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timeinformation bitfields for DCF77 devices
 */
typedef union {
   /**
   * @brief   Struct of bitfields
   */
    struct values {            /**< Struct of bitfields*/
        uint64_t start : 1,    /**< Number of Bits for start value*/
        wheater : 14,          /**< Number of Bits for weather value */
        calling : 1,           /**< Number of Bits for calling value */
        mez_mesz_shift : 1,    /**< Number of Bits for shift value */
        mesz : 2,              /**< Number of Bits for mesz value */
        shift_sec : 1,         /**< Number of Bits for leap-second value */
        start_time : 1,        /**< Number of Bits for start_Bit value */
        minute_l : 4,          /**< Number of Bits for lower minute value */
        minute_h : 3,          /**< Number of Bits for higher minute value */
        minute_par : 1,        /**< Number of Bits for minuteparity value */
        hour_l : 4,            /**< Number of Bits for lower hour value */
        hour_h : 2,            /**< Number of Bits for higher hour value */
        hour_par : 1,          /**< Number of Bits for hourparity value */
        day_l : 4,             /**< Number of Bits for lower calenderday value */
        day_h : 2,             /**< Number of Bits for higher calenderday value */
        wday : 3,              /**< Number of Bits for weekday value */
        month_l : 4,           /**< Number of Bits for lower month value */
        month_h : 1,           /**< Number of Bits for higher month value */
        year_l : 4,            /**< Number of Bits for lower year value */
        year_h : 4,            /**< Number of Bits for higher year value */
        date_par : 1,          /**< Number of Bits for dateparity value */
        buff : 5;              /**< Number of Bits for experimental buffer value */
    } val;                     /**< struct with Bitfields of timeinformation*/
    uint64_t bits;             /**< Value of Bits in a received cycle */
} dcf77_bits_t;                /**< Union which contains the Bitfields struct */

/**
 * @brief device internal states
 */
enum {
    DCF77_STATE_IDLE,            /**< Device is in idle state */
    DCF77_STATE_START,           /**< Device is searching the start for a new minute */
    DCF77_STATE_RX,              /**< Device is in RX mode */
};

#ifdef __cplusplus
}
#endif

#endif /* DCF77_INTERNAL_H */
/** @} */
