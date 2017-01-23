/*
 * Copyright (C) 2016 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_ppp Options for PPP packets
 * @ingroup     net_gnrc_ppp
 * @{
 *
 * @file
 * @brief  Helpers for PPP Control Protocol options.
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef GNRC_PPP_OPT_H
#define GNRC_PPP_OPT_H

#include <errno.h>
#include <string.h>
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define GNRC_PPP_OPT_HDR_SIZE (2) /**< size of OPT hdr */
typedef uint8_t gnrc_ppp_option_t;


/**
 * @brief get type of current option
 *
 * @param opt pointer to option
 *
 * @return value of type field of current option
 */
static inline uint8_t ppp_opt_get_type(gnrc_ppp_option_t *opt)
{
    return (uint8_t) *((uint8_t *) opt);
}

static inline void ppp_opt_set_type(gnrc_ppp_option_t *opt, uint8_t type)
{
    *((uint8_t *) opt) = type;
}

/**
 * @brief get length of current option
 *
 * @param opt pointer to option
 *
 * @return value of length field of current option
 */
static inline uint8_t ppp_opt_get_length(gnrc_ppp_option_t *opt)
{
    return (uint8_t) *(((uint8_t *) opt) + 1);
}

static inline void ppp_opt_set_length(gnrc_ppp_option_t *opt, uint8_t length)
{
    *(((uint8_t *) opt) + 1) = length;
}

/**
 * @brief get payload of current option
 *
 * @param[in] opt pointer to option
 * @param[out] payload pointer to payload buffer to write current payload
 *
 * @return size of current option
 * @return payload of current option
 */
static inline uint8_t ppp_opt_get_payload(gnrc_ppp_option_t *opt, void **payload)
{
    *payload = ((uint8_t *) opt) + 2;
    return (uint8_t) *(((uint8_t *) opt) + 1);
}

/**
 * @brief set payload of current option
 *
 * @param opt pointer to option
 * @param data pointer to payload
 * @param size size of payload
 */
static inline void ppp_opt_set_payload(gnrc_ppp_option_t *opt, void *data, size_t size)
{
    memcpy(((uint8_t *) opt) + 2, data, size);
}

/**
 * @brief get next ppp option
 *
 * @param curr_opt pointer to option
 * @param head pointer to head option in option buffer
 * @param opt_size size of received options
 *
 * @return pointer to next option
 */
static inline gnrc_ppp_option_t *ppp_opt_get_next(gnrc_ppp_option_t *curr_opt, gnrc_ppp_option_t *head, size_t opt_size)
{
    gnrc_ppp_option_t *ret = NULL;

    ret = (gnrc_ppp_option_t *)(((uint8_t *) curr_opt) + ppp_opt_get_length(curr_opt));
    if (ret - head >= opt_size) {
        return NULL;
    }
    return ret;
}

/**
 * @brief check if current stream of options is valid
 *
 * @param opts_snip snippet of ppp options
 * @param expected_length expected length of option snippet.
 *
 * @return true if options are valid
 * @return -EBADMSG if options are not valid
 */
static inline int ppp_conf_opts_valid(gnrc_pktsnip_t *opts_snip, uint8_t expected_length)
{
    uint8_t opts_length = expected_length;

    if (opts_length < 4) {
        return -EBADMSG;
    }

    uint16_t cursor = 1;
    uint8_t *p = ((uint8_t *) opts_snip->data) + 1;

    while (cursor < opts_length) {
        if (*((uint8_t *) p) < 2) {
            return EBADMSG;
        }
        cursor += *((uint8_t *)p);
        if (cursor - 1 > opts_length) {
            return -EBADMSG;
        }
        p += *p;
    }

    if (cursor - 1 != opts_length) {
        return false;
    }

    return true;
}

/**
 * @brief check if an option is a subset of an option stream
 *
 * @param opt pointer to option
 * @param optset pointer to head option
 * @param size size of option set
 *
 * @return true if the option is present in option set
 * @return false otherwise
 */
static inline int ppp_opt_is_subset(gnrc_ppp_option_t *opt, gnrc_ppp_option_t *optset, size_t size)
{
    uint8_t opt_type = (uint8_t) *((uint8_t *) opt);
    uint8_t *curr_opt = (uint8_t *) optset;
    uint8_t cursor = 0;

    while (cursor < size) {
        if (opt_type == *curr_opt) {
            return true;
        }
        cursor += ppp_opt_get_length(curr_opt);
        curr_opt += (int) ppp_opt_get_length(curr_opt);
    }
    return false;
}

/**
 * @brief shortcut to write options in a buffer
 *
 * @param opt_buf pointer to buffer where options should be written
 * @param type type of option
 * @param payload payload of option
 * @param pay_size size of payload
 *
 * @return total size of current option, including header and payload.
 */
static inline int ppp_opt_fill(void *opt_buf, uint8_t type, void *payload, size_t pay_size)
{
    gnrc_ppp_option_t *opt = opt_buf;

    ppp_opt_set_type(opt, type);
    ppp_opt_set_length(opt, 2 + pay_size);
    ppp_opt_set_payload(opt, payload, pay_size);
    return 2 + pay_size;
}

#ifdef __cplusplus
}
#endif
#endif /* GNRC_PPP_OPT_H */
/**
 * @}
 */
