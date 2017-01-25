/*
 * Copyright (C) 2016 Leon George, Florent-Valéry Coen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_rfc_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 RFC IEEE definitions
 *
 * @author          Leon George <leon@georgemail.eu>
 * @author          Florent-Valéry Coen <florent.coen@gmail.com>
 */

#ifndef CC26x0_RFC_IEEE_H
#define CC26x0_RFC_IEEE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup cc2x0_rop_command_ids
 * @{
 */
/* background */
#define CMDR_CMDID_IEEE_RX              (0x2801)
#define CMDR_CMDID_IEEE_ED_SCAN         (0x2802)
/* foreground */
#define CMDR_CMDID_IEEE_TX              (0x2C01)
#define CMDR_CMDID_IEEE_CSMA            (0x2C02)
#define CMDR_CMDID_IEEE_RX_ACK          (0x2C03)
#define CMDR_CMDID_IEEE_ABORT_BG        (0x2C04)
/* immediate */
#define CMDR_CMDID_IEEE_MOD_CCA         (0x2001)
#define CMDR_CMDID_IEEE_MOD_FILT        (0x2002)
#define CMDR_CMDID_IEEE_MOD_SRC_MATCH   (0x2003)
#define CMDR_CMDID_IEEE_ABORT_FG        (0x2401)
#define CMDR_CMDID_IEEE_STOP_FG         (0x2402)
#define CMDR_CMDID_IEEE_CCA_REQ         (0x2403)
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* CC26x0_RFC_IEEE_H */

/*@}*/
