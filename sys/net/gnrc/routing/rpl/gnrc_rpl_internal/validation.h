/*
 * Copyright (C) 2016 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       RPL control message validation functions
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/icmpv6.h"

/**
 * @brief   Checks validity of control message options
 *
 * @param[in]   msg_type    Type of the control message
 * @param[in]   inst        The RPL instance
 * @param[in]   opt         Options of the control message
 * @param[in]   len         Length of the options
 *
 * @return  true, if @p opt is valid
 * @return  false, otherwise
 */
bool gnrc_rpl_validation_options(int msg_type, gnrc_rpl_instance_t *inst,
                                 gnrc_rpl_opt_t *opt, uint16_t len);

#ifdef __cplusplus
}
#endif

/** @} */
