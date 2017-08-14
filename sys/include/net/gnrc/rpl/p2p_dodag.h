/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_rpl_p2p
 * @{
 *
 * @file
 * @brief       DODAG-related functions for P2P-RPL
 *
 * Header file, which defines all public known DODAG-related functions for P2P-RPL.
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 */

#ifndef NET_GNRC_RPL_P2P_DODAG_H
#define NET_GNRC_RPL_P2P_DODAG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/gnrc/rpl/p2p_structs.h"

/**
 * @brief   Number of P2P RPL DODAG extensions
 */
#ifndef GNRC_RPL_P2P_EXTS_NUMOF
#define GNRC_RPL_P2P_EXTS_NUMOF (1)
#endif

/**
 * @brief   P2P-RPL DODAG extensions table
 */
extern gnrc_rpl_p2p_ext_t gnrc_rpl_p2p_exts[GNRC_RPL_P2P_EXTS_NUMOF];

/**
 * @brief   Allocate a free P2P-RPL DODAG extension
 *
 * @return  Pointer to a free P2P-RPL DODAG extension
 * @return  NULL, if no free P2P-RPL DODAG extension is available
 */
gnrc_rpl_p2p_ext_t *gnrc_rpl_p2p_ext_new(gnrc_rpl_dodag_t *dodag);

/**
 * @brief   Free the P2P-RPL DODAG extension of @p dodag
 *
 * @param[in] dodag     Pointer to a P2P-RPL DODAG
 */
void gnrc_rpl_p2p_ext_remove(gnrc_rpl_dodag_t *dodag);

/**
 * @brief   Get the appropriate P2P-RPL DODAG extension of the @p dodag
 *
 * @param[in] dodag     Pointer to a P2P-RPL DODAG
 *
 * @return  Pointer to a P2P-RPL DODAG extension
 * @return  NULL, if no P2P-RPL DODAG extension is available for @p dodag
 */
gnrc_rpl_p2p_ext_t *gnrc_rpl_p2p_ext_get(gnrc_rpl_dodag_t *dodag);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_RPL_P2P_DODAG_H */
/**
 * @}
 */
