/*
 * Copyright (C) 2019-2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @ingroup     usbus_msc
 *
 * @{
 *
 * @file
 * @brief       SCSI protocol definitions for USBUS
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */


#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USBUS_MSC_VENDOR_ID
#define USBUS_MSC_VENDOR_ID "RIOT-OS"
#endif /* USBUS_MSC_VENDOR_ID */

#ifndef USBUS_MSC_PRODUCT_ID
#define USBUS_MSC_PRODUCT_ID "RIOT_MSC_DISK  "
#endif /* USBUS_MSC_PRODUCT_ID */

#ifndef USBUS_MSC_PRODUCT_REV
#define USBUS_MSC_PRODUCT_REV " 1.0"
#endif /* USBUS_MSC_PRODUCT_REV */

/**
 * @name USB SCSI Commands
 *
 * @see Table 9 - Packet Commands Supported by ATAPI Block Devices
 * from INF-8070i draft published by SFF
 * @{
 */
#define SCSI_TEST_UNIT_READY            0x00    /**< SCSI Test Unit Ready */
#define SCSI_REQUEST_SENSE              0x03    /**< SCSI Request Sense */
#define SCSI_FORMAT_UNIT                0x04    /**< SCSI Format Unit */
#define SCSI_INQUIRY                    0x12    /**< SCSI Inquiry */
#define SCSI_MODE_SELECT6               0x15    /**< SCSI Mode Select6 */
#define SCSI_MODE_SENSE6                0x1A    /**< SCSI Mode Sense6 */
#define SCSI_START_STOP_UNIT            0x1B    /**< SCSI Start Stop Unit */
#define SCSI_MEDIA_REMOVAL              0x1E    /**< SCSI Media Removal */
#define SCSI_READ_FORMAT_CAPACITIES     0x23    /**< SCSI Read Format Capacities */
#define SCSI_READ_CAPACITY              0x25    /**< SCSI Read Capacity */
#define SCSI_READ10                     0x28    /**< SCSI Read10 */
#define SCCI_READ12                     0xA8    /**< SCSI Read12 */
#define SCSI_WRITE10                    0x2A    /**< SCSI Write10 */
#define SCSI_WRITE12                    0xAA    /**< SCSI Write12 */
#define SCSI_SEEK                       0x2B    /**< SCSI Seek */
#define SCSI_WRITE_AND_VERIFY           0x2E    /**< SCSI Write and Verify */
#define SCSI_VERIFY10                   0x2F    /**< SCSI Verify10 */
#define SCSI_MODE_SELECT10              0x55    /**< SCSI Mode Select10 */
#define SCSI_MODE_SENSE10               0x5A    /**< SCSI Mode Sense10 */
/** @} */

/**
 * @brief Command Block Wrapper signature
 */
#define SCSI_CBW_SIGNATURE              0x43425355

/**
 * @brief Command Status Wrapper signature
 */
#define SCSI_CSW_SIGNATURE              0x53425355

/**
 * @name USB SCSI Version list
 * @{
 */
#define SCSI_VERSION_NONE               0x0000
#define SCSI_VERSION_SCSI1              0x0001
#define SCSI_VERSION_SCSI2              0x0002
/** @} */

/**
 * @name USB SCSI Read format capacities descriptor type
 *
 * @see ( @ref msc_read_fmt_capa_pkt_t )
 * @{
 */
#define SCSI_READ_FMT_CAPA_TYPE_RESERVED    0x00
#define SCSI_READ_FMT_CAPA_TYPE_UNFORMATTED 0x01
#define SCSI_READ_FMT_CAPA_TYPE_FORMATTED   0x02
#define SCSI_READ_FMT_CAPA_TYPE_NO_MEDIA    0x03
/** @} */

/**
 * @brief SCSI Request Sense error type
 */
#define SCSI_REQUEST_SENSE_ERROR              0x70

/**
 * @name USB SCSI Request Sense Sense Key type
 *
 * @{
 */
#define SCSI_SENSE_KEY_NO_SENSE             0x00
#define SCSI_SENSE_KEY_RECOVERED_ERROR      0x01
#define SCSI_SENSE_KEY_NOT_READY            0x02
#define SCSI_SENSE_KEY_MEDIUM_ERROR         0x03
#define SCSI_SENSE_KEY_HARDWARE_ERROR       0x04
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST      0x05
#define SCSI_SENSE_KEY_UNIT_ATTENTION       0x06
#define SCSI_SENSE_KEY_DATA_PROTECT         0x07
#define SCSI_SENSE_KEY_BLANK_CHECK          0x08
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC      0x09
#define SCSI_SENSE_KEY_ABORTED_COMMAND      0x0B
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW      0x0D
#define SCSI_SENSE_KEY_MISCOMPARE           0x0E
/** @} */

/**
 * @brief Packet structure to answer (@ref SCSI_TEST_UNIT_READY) request
 *
 * @see Inquiry Command from SCSI Primary Command
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< type of device */
    uint8_t logical_unit;   /**< Selected LUN */
    uint8_t reserved[10];   /**< Reserved */
} msc_test_unit_pkt_t;

/**
 * @brief Packet structure to answer (@ref SCSI_MODE_SELECT6) and
 * (@ref SCSI_MODE_SENSE6) requests
 *
 * @see Inquiry Command from SCSI Primary Command
 */
typedef struct __attribute__((packed)) {
    uint8_t mode_data_len;      /**< Size of the whole packet */
    uint8_t medium_type;        /**< Type of medium */
    uint8_t flags;              /**< Miscellaneous flags */
    uint8_t block_desc_len;     /**< Length of block descriptor */
} msc_mode_parameter_pkt_t;

/**
 * @brief CBW Packet structure for (@ref SCSI_READ10) and
 * (@ref SCSI_WRITE10) requests
 *
 * @see Read10 or Write10 Command from SCSI Primary Command
 *
 * @warning uint16_t and uint32_t will be BIG ENDIAN
 */
typedef struct __attribute__((packed)) {
    uint8_t opcode;         /**< Operation code */
    uint8_t flags;          /**< Miscellaneous flags */
    be_uint32_t blk_addr;   /**< Block address */
    uint8_t group_number;   /**< Group number */
    be_uint16_t xfer_len;   /**< Transfer length in bytes */
} msc_cbw_rw10_pkt_t;

/**
 * @brief Packet structure to answer (@ref SCSI_INQUIRY) request
 *
 * @see Inquiry Command from SCSI Primary Command
 *
 * @note Vendor specific information (Byte 36 and above) and flags from bytes
 *       5 to 7 are currently unsupported
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                   /**< Byte 0 Peripheral type */
    uint8_t reserved1 : 7;          /**< Byte 1 [B6..B0] Reserved */
    uint8_t removable : 1;          /**< Byte 1 [B7] Removable device flag */
    uint8_t version;                /**< Byte 2 SCSI Version */
    uint8_t response_format : 4;    /**< Byte 3 [B3..B0] Response Data Format */
    uint8_t reserved3 : 4;          /**< Byte 3 [B7..B4] Reserved */
    uint8_t length;                 /**< Byte 4 Additional Length (n-4) */
    uint8_t flags[3];               /**< Byte 7..5 Miscellaneous flags UNUSED BY USBUS ONLY */
    uint8_t vendor_id[8];           /**< Byte 15..8 Vendor Identification */
    uint8_t product_id[16];         /**< Byte 31..16 Product Identification */
    uint8_t product_rev[4];         /**< Byte 35..32 Product Revision */
} msc_inquiry_pkt_t;

/**
 * @brief Packet structure to answer (@ref SCSI_READ_FORMAT_CAPACITIES) request
 *
 * @see   READ FORMAT CAPACITIES from SCSI Multimedia Commands – 2 (MMC-2)
 *
 */
typedef struct __attribute__((packed)) {
    uint8_t reserved1[3];       /**< Byte 2..0 reserved */
    uint8_t list_length;        /**< Byte 3 Capacity list length */
    be_uint32_t blk_nb;            /**< Byte 7..4 Number of blocks */
    uint8_t type;               /**< Byte 8 Descriptor type */
    uint8_t blk_len[3];         /**< Byte 11..9 Block length */
} msc_read_fmt_capa_pkt_t;

/**
 * @brief Packet structure to answer (@ref SCSI_REQUEST_SENSE) request
 *
 */
typedef struct __attribute__((packed)) {
    uint8_t error_code;         /**< Byte 0 Error code */
    uint8_t reserved1;          /**< Reserved */
    uint8_t sense_key;          /**< Byte 2 Sense key */
    uint8_t reserved2[4];       /**< Byte 6..3 Information */
    uint8_t add_len;            /**< Byte 7 Additional sense length */
    uint8_t reserved3[4];       /**< Byte 11..8 Vendor specific */
    uint8_t asc;                /**< Byte 12 Additional Sense Code */
    uint8_t ascq;               /**< Byte 13 Additional Sense Code Qualifier */
    uint8_t fruc;               /**< Byte 14 Field Replaceable Unit Code */
    uint16_t sk_spec;           /**< Byte 16..15 Sense Key Specific */
} msc_request_sense_pkt_t;

/**
 * @brief Packet structure to answer (@ref SCSI_READ_CAPACITY) request
 *
 * @note Multiply the two values from this struct between them
 * indicates the total size of your MTD device
 *
 * @see Read Capacities from SCSI Primary Command
 */
typedef struct __attribute__((packed)) {
    be_uint32_t last_blk;  /**< Indicate last block number */
    be_uint32_t blk_len;   /**< Total size of a block in bytes */
} msc_read_capa_pkt_t;

/**
 * @brief Command Block Wrapper packet structure
 *
 * @see Table 5.1 Command Block Wrapper (CBW)
 * from Universal Serial Bus Mass Storage Class Bulk-Only Transport
 */
typedef struct __attribute__((packed)) {
    uint32_t signature; /**< CBW signature (@ref SCSI_CBW_SIGNATURE) */
    uint32_t tag;       /**< ID for the current command */
    uint32_t data_len;  /**< Number of bytes host expects to transfer from/to */
    uint8_t flags;      /**< Command block flags */
    uint8_t lun;        /**< Target Logical Unit Number */
    uint8_t cb_len;     /**< Length of the block in bytes (max: 16 bytes) */
    uint8_t cb[16];     /**< Command block buffer */
} msc_cbw_buf_t;

/**
 * @brief Command Status Wrapper packet structure
 *
 * @see Table 5.2 - Command Status Wrapper
 * from Universal Serial Bus Mass Storage Class Bulk-Only Transport
 */
typedef struct __attribute__((packed)) {
    uint32_t signature; /**< CSW signature (@ref SCSI_CSW_SIGNATURE) */
    uint32_t tag;       /**< ID for the answered CBW */
    uint32_t data_left; /**< Indicate how many bytes from the CBW were not processed */
    uint8_t status;     /**< Status of the command */
} msc_csw_buf_t;

/**
 * @brief USBUS Command Block Wrapper information
 *
 */
typedef struct {
    uint32_t tag;   /**< Store tag information from CBW */
    size_t len;     /**< Remaining bytes to handle */
    uint8_t status; /**< Status of the current operation */
    uint8_t lun;    /**< Store LUN from CBW */
} cbw_info_t;

/**
 * @brief Process incoming Command Block Wrapper buffer
 *
 * @param   usbus   USBUS thread to use
 * @param   handler MSC device struct
 * @param   ep      Endpoint pointer to read CBW from
 * @param   len     Size of the received CBW buffer
 */
void usbus_msc_scsi_process_cmd(usbus_t *usbus, usbus_handler_t *handler, usbdev_ep_t *ep,
                                size_t len);

/**
 * @brief Generate Command Status Wrapper and send it to the host
 *
 * @param   handler   MSC device struct
 * @param   cmd       struct containing needed information to generate CBW response
 */
void scsi_gen_csw(usbus_handler_t *handler, cbw_info_t *cmd);

#ifdef __cplusplus
}
#endif

/** @} */
