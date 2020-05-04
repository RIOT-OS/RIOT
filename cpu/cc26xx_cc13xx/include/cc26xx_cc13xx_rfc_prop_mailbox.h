/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx RF Core proprietary PHY interface definitions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_RFC_PROP_MAILBOX_H
#define CC26XX_CC13XX_RFC_PROP_MAILBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Radio operation status
 * @{
 */
/**
 * @enum    Operation finished normally
 */
enum {
    RFC_PROP_DONE_OK            = 0x3400,   /**< Operation ended normally */
    RFC_PROP_DONE_RXTIMEOUT     = 0x3401,   /**< Operation stopped after end
                                                 trigger while waiting for
                                                 sync */
    RFC_PROP_DONE_BREAK         = 0x3402,   /**< Rx stopped due to timeout in
                                                 the middle of a packet */
    RFC_PROP_DONE_ENDED         = 0x3403,   /**< Operation stopped after end
                                                 trigger during reception */
    RFC_PROP_DONE_STOPPED       = 0x3404,   /**< Operation stopped after stop
                                                 command */
    RFC_PROP_DONE_ABORT         = 0x3405,   /**< Operation aborted by abort
                                                 command */
    RFC_PROP_DONE_RXERR         = 0x3406,   /**< Operation ended after receiving
                                                 packet with CRC error */
    RFC_PROP_DONE_IDLE          = 0x3407,   /**< Carrier sense operation ended
                                                 because of idle channel */
    RFC_PROP_DONE_BUSY          = 0x3408,   /**< Carrier sense operation ended
                                                 because of busy channel */
    RFC_PROP_DONE_IDLETIMEOUT   = 0x3409,   /**< Carrier sense operation ended
                                                 because of timeout with
                                                 cs_conf.timeout_res = 1 */
    RFC_PROP_DONE_BUSYTIMEOUT   = 0x340A,   /**< Carrier sense operation ended
                                                 because of timeout with
                                                 cs_conf.timeout_res = 0 */
};

/**
 * @brief   Operation finished with error
 */
enum {
    RFC_PROP_ERROR_PAR      = 0x3800,   /**< Illegal parameter */
    RFC_PROP_ERROR_RXBUF    = 0x3801,   /**< No available Rx buffer at the
                                             start of a packet */
    RFC_PROP_ERROR_RXFULL   = 0x3802,   /**< Out of Rx buffer during reception
                                             in a partial read buffer */
    RFC_PROP_ERROR_NO_SETUP = 0x3803,   /**< Radio was not set up in proprietary
                                             mode */
    RFC_PROP_ERROR_NO_FS    = 0x3804,   /**< Synth was not programmed when
                                             running Rx or Tx */
    RFC_PROP_ERROR_RXOVF    = 0x3805,   /**< Rx overflow observed during
                                             operation */
    RFC_PROP_ERROR_TXUNF    = 0x3806,   /**< Tx underflow observed during
                                             operation */
};
/** @} */

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif /* CC26XX_CC13XX_RFC_PROP_MAILBOX_H */

/*@}*/
