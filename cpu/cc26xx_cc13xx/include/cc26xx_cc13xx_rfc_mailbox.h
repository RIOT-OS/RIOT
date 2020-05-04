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
 * @brief           CC26xx/CC13xx RF Core interface definitions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_RFC_MAILBOX_H
#define CC26XX_CC13XX_RFC_MAILBOX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Type definition for RAT.
 */
typedef uint32_t rfc_ratmr_t;

/**
 * @brief   CMDSTA values
 *
 * Values returned in result byte of CMDSTA
 */
enum {
    RFC_CMDSTA_PENDING              = 0x00, /**< The command has not yet been
                                                 parsed */
    RFC_CMDSTA_DONE                 = 0x01, /**< Command successfully parsed */
    RFC_CMDSTA_ILLEGALPOINTER       = 0x81, /**< The pointer signaled in CMDR is
                                                 not valid */
    RFC_CMDSTA_UNKNOWNCOMMAND       = 0x82, /**< The command number in the
                                                 command structure is unknown */
    RFC_CMDSTA_UNKNOWNDIRCOMMAND    = 0x83, /**< The command number for a direct
                                                 command is unknown, or the
                                                 command is not a direct
                                                 command */
    RFC_CMDSTA_CONTEXTERROR         = 0x85, /**< An immediate or direct command
                                                 was issued in a context where
                                                 it is not supported */
    RFC_CMDSTA_SCHEDULINGERROR      = 0x86, /**< A radio operation command was
                                                 attempted to be scheduled while
                                                 another operation was already
                                                 running in the RF core */
    RFC_CMDSTA_PARERROR             = 0x87, /**< There were errors in the
                                                 command parameters that are
                                                 parsed on submission. */
    RFC_CMDSTA_QUEUEERROR           = 0x88, /**< An operation on a data entry
                                                 queue was attempted that was
                                                 not supported by the queue in
                                                 its current state */
    RFC_CMDSTA_QUEUEBUSY            = 0x89, /**< An operation on a data entry
                                                 was attempted while that entry
                                                 was busy */
};

/**
 * @brief   Macros for sending direct commands
 * @{
 */
/**
 * @brief   Direct command with no parameter
 */
#define RFC_CMDR_DIR_CMD(cmdid) (((cmdid) << 16) | 1)

/**
 * @brief   Direct command with 1-byte parameter
 */
#define RFC_CMDR_DIR_CMD_1BYTE(cmdid, par) (((cmdid) << 16) | ((par) << 8) | 1)

/**
 * @brief   Direct command with 2-byte parameter
 */
#define RFC_CMDR_DIR_CMD_2BYTE(cmdid, par) \
    (((cmdid) << 16) | ((par) & 0xFFFC) | 1)
/** @} */

/**
 * @brief   Definitions for trigger types
 */
enum {
    RFC_TRIG_NOW            = 0,    /**< Triggers immediately */
    RFC_TRIG_NEVER          = 1,    /**< Never trigs */
    RFC_TRIG_ABSTIME        = 2,    /**< Trigs at an absolute time */
    RFC_TRIG_REL_SUBMIT     = 3,    /**< Trigs at a time relative to the command
                                         was submitted */
    RFC_TRIG_REL_START      = 4,    /**< Trigs at a time relative to the command
                                         started */
    RFC_TRIG_REL_PREVSTART  = 5,    /**< Trigs at a time relative to the previous
                                         command in the chain started */
    RFC_TRIG_REL_FIRSTSTART = 6,    /**< Trigs at a time relative to the first
                                         command in the chain started */
    RFC_TRIG_REL_PREVEND    = 7,    /**< Trigs at a time relative to the previous
                                         command in the chain ended */
    RFC_TRIG_REL_EVT1       = 8,    /**< Trigs at a time relative to the context
                                         defined "Event 1" */
    RFC_TRIG_REL_EVT2       = 9,    /**< Trigs at a time relative to the context
                                         defined "Event 2" */
    RFC_TRIG_EXTERNAL       = 10,   /**< Trigs at an external event to the radio
                                         timer */
    RFC_TRIG_PAST_BM        = 0x80, /**< Bitmask for setting past trig bit in
                                         order to trig immediately if trigger
                                         happened in the past */
};

/**
 * @brief   Definitions for conditional execution
 */
enum {
    RFC_COND_ALWAYS         = 0,    /**< Always run next command (except in case
                                         of Abort) */
    RFC_COND_NEVER          = 1,    /**< Never run next command */
    RFC_COND_STOP_ON_FALSE  = 2,    /**< Run next command if this command
                                         returned True, stop if it returned False */
    RFC_COND_STOP_ON_TRUE   = 3,    /**< Stop if this command returned True, run
                                         next command if it returned False */
    RFC_COND_SKIP_ON_FALSE  = 4,    /**< Run next command if this command
                                         returned True, skip a number of
                                         commands if it returned False */
    RFC_COND_SKIP_ON_TRUE   = 5,    /**< Skip a number of commands if this
                                         command returned True, run next command
                                         if it returned False */
};

/**
 * @brief   Radio operation status
 * @{
 */
/**
 * @brief   Operation not finished.
 */
enum {
    RFC_IDLE    = 0x0000,   /**< Operation not started */
    RFC_PENDING = 0x0001,   /**< Start of command is pending */
    RFC_ACTIVE  = 0x0002,   /**< Running */
    RFC_SKIPPED = 0x0003,   /**< Operation skipped due to condition in another
                                 command */
};
/**
 * @brief   Operation finished normally
 */
enum {
    RFC_DONE_OK        = 0x0400,    /**< Operation ended normally */
    RFC_DONE_COUNTDOWN = 0x0401,    /**< Counter reached zero */
    RFC_DONE_RXERR     = 0x0402,    /**< Operation ended with CRC error */
    RFC_DONE_TIMEOUT   = 0x0403,    /**< Operation ended with timeout */
    RFC_DONE_STOPPED   = 0x0404,    /**< Operation stopped after CMD_STOP
                                         command */
    RFC_DONE_ABORT     = 0x0405,    /**< Operation aborted by CMD_ABORT
                                         command */
    RFC_DONE_FAILED    = 0x0406,    /**< Scheduled immediate command failed */
};
/**
 * @brief   Operation finished with error
 */
enum {
    RFC_ERROR_PAST_START    = 0x0800,   /**< The start trigger occurred in the
                                             past */
    RFC_ERROR_START_TRIG    = 0x0801,   /**< Illegal start trigger parameter */
    RFC_ERROR_CONDITION     = 0x0802,   /**< Illegal condition for next
                                             operation */
    RFC_ERROR_PAR           = 0x0803,   /**< Error in a command specific
                                             parameter */
    RFC_ERROR_POINTER       = 0x0804,   /**< Invalid pointer to next
                                             operation */
    RFC_ERROR_CMDID         = 0x0805,   /**< Next operation has a command ID
                                             that is undefined or not a radio
                                             operation command */
    RFC_ERROR_WRONG_BG      = 0x0806,   /**< FG level command not compatible
                                             with running BG level command */
    RFC_ERROR_NO_SETUP      = 0x0807,   /**< Operation using Rx or Tx attempted
                                             without CMD_RADIO_SETUP */
    RFC_ERROR_NO_FS         = 0x0808,   /**< Operation using Rx or Tx attempted
                                             without frequency synth
                                             configured */
    RFC_ERROR_SYNTH_PROG    = 0x0809,   /**< Synthesizer calibration failed */
    RFC_ERROR_TXUNF         = 0x080A,   /**< Tx underflow observed */
    RFC_ERROR_RXOVF         = 0x080B,   /**< Rx overflow observed */
    RFC_ERROR_NO_RX         = 0x080C,   /**< Attempted to access data from Rx
                                             when no such data was yet
                                             received */
    RFC_ERROR_PENDING       = 0x080D,   /**< Command submitted in the future
                                             with another command at different
                                             level pending */
};
/** @} */

/**
 * @brief   Macros for RF register override
 * @{
 */
/**
 * @brief   Macro for ADI half-size value-mask combination
 */
#define RFC_ADI_VAL_MASK(addr, mask, value)                       \
    (((addr) & 1) ? (((mask) & 0x0F) | (((value) & 0x0F) << 4)) : \
    ((((mask) & 0x0F) << 4) | ((value) & 0x0F)))
/**
 * @brief   32-bit write of 16-bit value
 */
#define RFC_HW_REG_OVERRIDE(addr, val) \
    ((((uintptr_t) (addr)) & 0xFFFC) | ((uint32_t)(val) << 16))
/**
 * @brief   ADI register, full-size write
 */
#define RFC_ADI_REG_OVERRIDE(adino, addr, val)               \
    (2 | ((uint32_t)(val) << 16) | (((addr) & 0x3F) << 24) | \
     (((adino) ? 1U : 0) << 31))
/**
 * @brief   ADI register, half-size read-modify-write
 */
#define RFC_ADI_HALFREG_OVERRIDE(adino, addr, mask, val)                   \
    (2 | (RFC_ADI_VAL_MASK(addr, mask, val) << 16) | (((addr) & 0x3F) << 24) | \
     (1U << 30) | (((adino) ? 1U : 0) << 31))
/**
 * @brief   2 ADI registers, half-size read-modify-write
 */
#define RFC_ADI_2HALFREG_OVERRIDE(adino, addr, mask, val, addr2, mask2, val2)     \
    (2 | (RFC_ADI_VAL_MASK(addr2, mask2, val2) << 2) | (((addr2) & 0x3F) << 10) | \
     (RFC_ADI_VAL_MASK(addr, mask, val) << 16) | (((addr) & 0x3F) << 24) |        \
     (1U << 30) | (((adino) ? 1U : 0) << 31))
/**
 * @brief   End of string for override register
 */
#define RFC_END_OVERRIDE ((uint32_t)0xFFFFFFFF)
/** @} */

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif /* CC26XX_CC13XX_RFC_MAILBOX_H */

/*@}*/
