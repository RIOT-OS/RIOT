/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @defgroup    sys_can_dll Data Link Layer
 * @ingroup     sys_can
 * @brief       CAN Data Link Layer
 *
 * The Data Link Layer is composed of the device, router, pkt and dll files.
 * It can be used to send and receive raw CAN frames through multiple CAN
 * controllers.
 * @{
 *
 * @file
 * @brief       Definitions high-level CAN interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(__linux__)

#include <linux/can.h>
#include <libsocketcan.h>

#else

/**
 * @brief Max data length for classic and FD CAN frames (compliant with
 * libsocketcan macros)
 * @{
 */
#define CAN_MAX_DLEN (8)    /**< Classic CAN maximum data length */
#define CANFD_MAX_DLEN (64) /**< CAN FD maximum data length */
/** @} */

/**
 * @name CAN_ID flags and masks
 * @{
 */
/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG (0x80000000U) /**< EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG (0x40000000U) /**< remote transmission request */
#define CAN_ERR_FLAG (0x20000000U) /**< error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK (0x000007FFU) /**< standard frame format (SFF) */
#define CAN_EFF_MASK (0x1FFFFFFFU) /**< extended frame format (EFF) */
#define CAN_ERR_MASK (0x1FFFFFFFU) /**< omit EFF, RTR, ERR flags */
/** @} */

/**
 * @name CAN FD flags extracted from libsocketcan
 * @{
 */
#define CANFD_BRS 0x01 /**< bit rate switch (second bitrate for payload data) */
#define CANFD_ESI 0x02 /**< error state indicator of the transmitting node */
#define CANFD_FDF 0x04 /**< mark CAN FD for dual use of struct canfd_frame */
/** @} */

/**
 * @brief CAN operational and error states
 */
 enum can_state {
     CAN_STATE_ERROR_ACTIVE = 0,     /**< RX/TX error count < 96 */
     CAN_STATE_ERROR_WARNING,        /**< RX/TX error count < 128 */
     CAN_STATE_ERROR_PASSIVE,        /**< RX/TX error count < 256 */
     CAN_STATE_BUS_OFF,              /**< RX/TX error count >= 256 */
     CAN_STATE_STOPPED,              /**< Device is stopped */
     CAN_STATE_SLEEPING,             /**< Device is sleeping */
     CAN_STATE_MAX
 };

/**
 * @brief Controller Area Network Identifier structure
 *
 * bit 0-28  : CAN identifier (11/29 bit) right aligned for 11 bit
 * bit 29    : error message frame flag (0 = data frame, 1 = error message)
 * bit 30    : remote transmission request flag (1 = rtr frame)
 * bit 31    : frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
typedef uint32_t canid_t;

/**
 * @brief Controller Area Network frame
 */
struct can_frame {
    canid_t can_id;     /**< 32 bit CAN_ID + EFF/RTR/ERR flags */
    union {
        uint8_t len;    /**< frame payload length in byte (0 .. CAN_MAX_DLEN) */
        uint8_t can_dlc;/**< deprecated - see SocketCAN documentation */
    };
    uint8_t __pad;      /**< padding */
    uint8_t __res0;     /**< reserved / padding */
    uint8_t __res1;     /**< reserved / padding */
    /** Frame data */
    uint8_t data[CAN_MAX_DLEN] __attribute__((aligned(8)));
};

#ifdef MODULE_FDCAN
struct canfd_frame {
    canid_t can_id;  /**< 32 bit CAN_ID + EFF/RTR/ERR flags */
    uint8_t len; /**< frame payload length in byte (0 .. CAN_MAX_DLEN) */
    uint8_t flags;   /**< additional flags for CAN FD */
    uint8_t __res0;  /**< reserved / padding */
    uint8_t __res1;  /**< reserved / padding */
    /** Frame data */
    uint8_t data[CANFD_MAX_DLEN] __attribute__((aligned(8)));
};
#endif

/**
 * @brief Controller Area Network filter
 */
struct can_filter {
    canid_t can_id;    /**< CAN ID */
    canid_t can_mask;  /**< Mask */
#if (MODULE_CAN_RX_MAILBOX)
    uint8_t target_mailbox; /**< The mailbox to apply the filter to */
#endif
};

/**
 * @brief CAN bit-timing parameters
 *
 * For further information, please read chapter "8 BIT TIMING
 * REQUIREMENTS" of the "Bosch CAN Specification version 2.0":
 * https://www.kvaser.com/software/7330130980914/V1/can2spec.pdf
 */
struct can_bittiming {
    uint32_t bitrate;      /**< Bit-rate in bits/second */
    uint32_t sample_point; /**< Sample point in one-tenth of a percent */
    uint32_t tq;           /**< Time quanta (TQ) in nanoseconds */
    uint32_t prop_seg;     /**< Propagation segment in TQs */
    uint32_t phase_seg1;   /**< Phase buffer segment 1 in TQs */
    uint32_t phase_seg2;   /**< Phase buffer segment 2 in TQs */
    uint32_t sjw;          /**< Synchronisation jump width in TQs */
    uint32_t brp;          /**< Bit-rate prescaler */
};

/**
 * @brief CAN hardware-dependent bit-timing constant
 *
 * Used for calculating and checking bit-timing parameters
 */
struct can_bittiming_const {
    uint32_t tseg1_min;  /**< Time segment 1 = prop_seg + phase_seg1, min value */
    uint32_t tseg1_max;  /**< Time segment 1, max value */
    uint32_t tseg2_min;  /**< Time segment 2 = phase_seg2, min value */
    uint32_t tseg2_max;  /**< Time segment 2, max value */
    uint32_t sjw_max;    /**< Synchronisation jump width */
    uint32_t brp_min;    /**< Bit-rate prescaler, min value */
    uint32_t brp_max;    /**< Bit-rate prescaler, max value */
    uint32_t brp_inc;    /**< Bit-rate prescaler, increment */
};

#endif /* defined(__linux__) */

/**
 * @brief CAN frame
 */
#ifdef MODULE_FDCAN
typedef struct canfd_frame can_frame_t;
#else
typedef struct can_frame can_frame_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
