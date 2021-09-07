/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ieee802154_security  IEEE 802.15.4 security
 * @ingroup     net_ieee802154
 * @brief       IEEE 802.15.4 security header
 *
 * @warning
 * This module is exposing raw 802.15.4 encryption without an underlying key
 * management framework. This is intended for experimentation with the security
 * modes of 802.15.4, and not for use cases where its security is depended on.
 *
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 security interface
 *
 * Specification: IEEE 802.15.4 - 2015
 * https://www.silabs.com/content/usergenerated/asi/cloud/attachments/siliconlabs/en/community/wireless/proprietary/forum/jcr:content/content/primary/qna/802_15_4_promiscuous-tbzR/hivukadin_vukadi-iTXQ/802.15.4-2015.pdf
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef NET_IEEE802154_SECURITY_H
#define NET_IEEE802154_SECURITY_H

#include <stdint.h>
#include <limits.h>
#include "kernel_defines.h"
#include "ieee802154.h"
#include "crypto/ciphers.h"
#include "bitfield.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Forward declaration of an IEEE 802.15.4 abstract security device
 */
typedef struct ieee802154_sec_dev ieee802154_sec_dev_t;

/**
 * @brief   Struct of security operations
 *
 * @note    A device can indicate that the fallback implementations should be
 *          used by setting the corresponding member to `NULL`, or pointing to
 *          @ref ieee802154_radio_cipher_ops, which does the same. Note that
 *          @ref ieee802154_radio_cipher_ops is the default security operations
 *          driver assigned when @ref ieee802154_sec_init is called.
 */
typedef struct ieee802154_radio_cipher_ops {
    /**
     * @brief   Function to set the encryption key for the
     *          next cipher operation
     *
     * @param[in]       dev         Will be @ref ieee802154_sec_context_t::ieee802154_sec_dev_t
     * @param[in]       key         Key to be used for the next cipher operation
     * @param[in]       key_size    key size in bytes
     */
    void (*set_key)(ieee802154_sec_dev_t *dev,
                    const uint8_t *key,
                    uint8_t key_size);
    /**
     * @brief   Function type to compute CBC-MAC
     *
     * @param[in]       dev         Will be @ref ieee802154_sec_context_t::ieee802154_sec_dev_t
     * @param[in]       cipher      Output cipher blocks
     * @param[in, out]  iv          in: IV; out: computed MIC
     * @param[in]       plain       Input plain blocks
     * @param[in]       nblocks     Number of blocks
     */
    void (*cbc)(const ieee802154_sec_dev_t *dev,
                uint8_t *cipher,
                uint8_t *iv,
                const uint8_t *plain,
                uint8_t nblocks);
    /**
     * @brief   Function type to perform ECB encryption
     *
     * @param[in]       dev         Will be @ref ieee802154_sec_context_t::ieee802154_sec_dev_t
     * @param[out]      cipher      Output cipher blocks
     * @param[in]       plain       Input plain blocks
     * @param[in]       nblocks     Number of blocks
     */
    void (*ecb)(const ieee802154_sec_dev_t *dev,
                uint8_t *cipher,
                const uint8_t *plain,
                uint8_t nblocks);
} ieee802154_radio_cipher_ops_t;

/**
 * @brief IEEE 802.15.4 security device descriptor
 */
struct ieee802154_sec_dev {
    /**
     * @brief Pointer to the operations of the device
     */
    const struct ieee802154_radio_cipher_ops *cipher_ops;
    /**
     * @brief Pointer to the context of the device
     */
    void *ctx;
};

#if !defined(CONFIG_IEEE802154_SEC_DEFAULT_KEY) || defined(DOXYGEN)
/**
 * @brief   AES default key
 *
 * @note    Predefine it yourself,
 *          if you want another key to be set up on initialization
 */
#define CONFIG_IEEE802154_SEC_DEFAULT_KEY       "pizza_margherita"
#endif

#if !defined(CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE) || defined(DOXYGEN)
/**
 * @brief   Array size to store key lookup structs @ref ieee802154_sec_key_lookup_t
 */
#define CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE    1U
#endif

#if !defined(CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE) || defined(DOXYGEN)
/**
 * @brief   Array size to store device lookup structs @ref ieee802154_sec_dev_lookup_t
 */
#define CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE     1U
#endif

#if !defined(CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE) || defined(DOXYGEN)
/**
 * @brief   Array size to store key structs @ref ieee802154_sec_key_t
 */
#define CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE     1U
#endif

#if !defined(CONFIG_IEEE802154_SEC_DEFAULT_REPLAY_PROTECTION) || defined(DOXYGEN)
/**
 * @brief   Set to 1 if you want to enable replay protection
 */
#define CONFIG_IEEE802154_SEC_DEFAULT_REPLAY_PROTECTION 0
#endif

/**
 * @brief   Length of an AES key in bytes
 */
#define IEEE802154_SEC_KEY_LENGTH               (16U)

/**
 * @brief   Block size of an encryption block
 */
#define IEEE802154_SEC_BLOCK_SIZE               (16U)

/**
 * @brief   Maximum length of the security auxiliary header in bytes
 */
#define IEEE802154_SEC_MAX_AUX_HDR_LEN          (14U)

/**
 * @brief   Maximum Size of IEEE 802.15.4 MAC
 */
#define IEEE802154_SEC_MAX_MAC_SIZE             (16U)

/**
 * @brief   Mask to get security level bits
 */
#define IEEE802154_SEC_SCF_SECLEVEL_MASK        (0x07)

/**
 * @brief   Number of shifts to set/get security level bits
 */
#define IEEE802154_SEC_SCF_SECLEVEL_SHIFT       (0)

/**
 * @brief   Mask to get key mode bits
 */
#define IEEE802154_SEC_SCF_KEYMODE_MASK         (0x18)

/**
 * @brief   Number of shifts to set/get key mode bits
 */
#define IEEE802154_SEC_SCF_KEYMODE_SHIFT        (3)

/**
 * @brief    Security levels
 *
 * <em>IEEE802154_SEC_SCF_SECLEVEL_MIC*</em>:
 * A message integrity code (MIC), also known as MAC,
 * is used to prove authentication. The MIC covers the whole frame
 * i.e. header, auxiliary header, and frame payload.
 * The MIC is always encrypted, thus it must be decrypted by the receiver,
 * to be checked.
 *
 * <em>IEEE802154_SEC_SCF_SECLEVEL_ENC*</em>:
 * AES-128 in ECB mode is used to encrypt the payload of a frame to provide
 * confidentiality.
 *
 * <em>IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC*</em>:
 * A combination of the two modes above is used to ensure
 * authentication and confidentiality.
 */
typedef enum {
    IEEE802154_SEC_SCF_SECLEVEL_NONE            = 0x00, /**< no security */
    IEEE802154_SEC_SCF_SECLEVEL_MIC32           = 0x01, /**< 32 bit MIC */
    IEEE802154_SEC_SCF_SECLEVEL_MIC64           = 0x02, /**< 64 bit MIC */
    IEEE802154_SEC_SCF_SECLEVEL_MIC128          = 0x03, /**< 128 bit MIC */
    IEEE802154_SEC_SCF_SECLEVEL_ENC             = 0x04, /**< encryption */
    IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC32       = 0x05, /**< enc. + 32 bit MIC */
    IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC64       = 0x06, /**< enc. + 64 bit MIC (mandatory) */
    IEEE802154_SEC_SCF_SECLEVEL_ENC_MIC128      = 0x07  /**< enc. + 128 bit MIC */
} ieee802154_sec_scf_seclevel_t;

/**
 * @brief   Key identifier modes
 *
 * The key identifier field in the auxiliary header
 * consists of the key source and the key index fields and is only present
 * if the key identifier mode is not IEEE802154_SEC_SCF_KEYMODE_IMPLICIT.
 * (see 9.4.3 in the spec.)
 *
 * +----------------+-------------+------------------+------------------------------------+
 * |     mode       | key source  | key index        | description                        |
 * +----------------+-------------+------------------+------------------------------------+
 * | IMPLICIT       | 0 bytes     | 0 bytes          | The key is implicitly              |
 * |                |             |                  | known to the receiver.             |
 * +----------------+-------------+------------------+------------------------------------+
 * | INDEX          | 0 bytes     | 1 byte           | The key can be determined          |
 * |                |             |                  | from the key index.                |
 * +----------------+-------------+------------------+------------------------------------+
 * | SHORT_INDEX    | 4 bytes     | 1 byte           | The key is a group key and can be  |
 * |                |             |                  | determined from the key index and  |
 * |                |             |                  | the source PAN ID and the          |
 * |                |             |                  | short source address               |
 * |                |             |                  | of the originator of the frame.    |
 * +----------------+-------------+------------------+------------------------------------+
 * | HW_INDEX       | 8 bytes     | 1 byte           | The key can be determined          |
 * |                |             |                  | from the key index and             |
 * |                |             |                  | the long address of the originator |
 * |                |             |                  | of the frame.                      |
 * +----------------+-------------+------------------+------------------------------------+
 */
typedef enum {
    IEEE802154_SEC_SCF_KEYMODE_IMPLICIT         = 0x00, /**< Key is determined implicitly */
    IEEE802154_SEC_SCF_KEYMODE_INDEX            = 0x01, /**< Key is determined from key index */
    IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX      = 0x02, /**< Key is determined from 4 byte key source and key index */
    IEEE802154_SEC_SCF_KEYMODE_HW_INDEX         = 0x03  /**< Key is determined from 8 byte key source and key index */
} ieee802154_sec_scf_keymode_t;

/**
 * @brief   Device addressing mode of device descriptor
 */
typedef enum {
    IEEE802154_SEC_DEV_ADDRMODE_NONE            = 0x00, /**< PAN coordinator */
    IEEE802154_SEC_DEV_ADDRMODE_SHORT           = 0x01, /**< Short address as device address */
    IEEE802154_SEC_DEV_ADDRMODE_LONG            = 0x02  /**< Long address as device address */
} ieee802154_sec_dev_addrmode_t;

/**
 * @brief   IEEE 802.15.4 security error codes
 */
typedef enum {
    IEEE802154_SEC_OK,                                  /**< Everything went fine */
    IEEE802154_SEC_FRAME_COUNTER_OVERFLOW,              /**< The requested operation would let the frame counter overflow */
    IEEE802154_SEC_FRAME_COUNTER_ERROR,                 /**< The received frame counter is less than expected */
    IEEE802154_SEC_NO_KEY,                              /**< Could not find the key to perform a requested cipher operation */
    IEEE802154_SEC_NO_DEV,                              /**< Could not find peer device to check the frame counter */
    IEEE802154_SEC_NO_PEER,                             /**< Device was found but pairing has not been done to use the key */
    IEEE802154_SEC_MAC_CHECK_FAILURE,                   /**< The computet MAC did not match */
    IEEE802154_SEC_UNSUPORTED,                          /**< Unsupported operation */
} ieee802154_sec_error_t;

/**
 *  @brief  Value to represent an invalid index
 *
 *  We shall support 255 valid key slots [0, 254].
 */
#define IEEE802154_SEC_NO_IDENT     (UINT8_MAX)

/**
 * @brief   Type to identify a key
 */
typedef uint8_t ieee802154_sec_key_descriptor_t;

/**
 * @brief   Type to identify a peer device
 */
typedef uint8_t ieee802154_sec_dev_descriptor_t;

/**
 * @brief   Type to identify a key lookup
 */
typedef uint8_t ieee802154_sec_key_lookup_descriptor_t;

/**
 * @brief   Stores peer device information
 */
typedef struct ieee802154_sec_dev_lookup {
    /**
     * @brief   PAN ID
     */
    uint8_t pan_id[2];
    /**
     * @brief   Short address
     */
    uint8_t short_addr[IEEE802154_SHORT_ADDRESS_LEN];
    /**
     * @brief   Extended address
     */
    uint8_t long_addr[IEEE802154_LONG_ADDRESS_LEN];
    /**
     * @brief   Incoming frame counter for replay protection
     */
    uint32_t fc;
} ieee802154_sec_dev_lookup_t;

/**
 * @brief   Lookup table for peer devices to securely communicate with
 */
typedef struct ieee802154_sec_dev_lookup_table {
    /**
     * @brief   Bitmask that shows which slots are occupied
     */
    BITFIELD(mask, CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE);

    /**
     * @brief   Array of peer devices
     */
    ieee802154_sec_dev_lookup_t dev_lookup[CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE];
} ieee802154_sec_dev_lookup_table_t;

/**
 * @brief   Key structure
 *
 * Table 9.10 secKeyDescriptor
 */
typedef struct ieee802154_sec_key {
    /**
     * @brief   Key
     */
    uint8_t key[IEEE802154_SEC_KEY_LENGTH];

    /**
     *  @brief  Frame counter for the key
     */
    uint32_t fc;

    /**
     * @brief   List of devices which are using the key
     *
     * This is a subset of our stored peers.
     * The device reference is used to check the incoming frame counter.
     */
    BITFIELD(dev_fc_list, CONFIG_IEEE802154_SEC_DEFAULT_DEVSTORE_SIZE);
} ieee802154_sec_key_t;

/**
 * @brief   Keystore
 */
typedef struct ieee802154_sec_key_table {
    /**
     *  @brief  Bitmask which shows which key slots are occupied
     */
    BITFIELD(mask, CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE);

    /**
     *  @brief  Stored keys
     */
    ieee802154_sec_key_t keys[CONFIG_IEEE802154_SEC_DEFAULT_KEYSTORE_SIZE];
} ieee802154_sec_key_table_t;

/**
 * @brief   Struct to store key properties to find the key to be used for
 *          encryption or decryption
 *
 *          Table 9.9 secKeyIdLookupDescriptor
 */
typedef struct ieee802154_sec_key_lookup {
    /**
     * @brief   Key mode @ref ieee802154_sec_scf_keymode_t
     */
    uint8_t key_mode;
    /**
     * @brief   Either an implicit or explicit key lookup
     */
    union {
        /**
         * @brief Explicit key lookup
         */
        struct {
            /**
             * @brief Key index
             */
            uint8_t key_index;
            /**
             * @brief Key sourcce
             */
            uint8_t key_source[IEEE802154_LONG_ADDRESS_LEN];
        } explicit;
        /**
         * @brief   Implicit key lookup
         */
        struct {
            /**
             * @brief   Indicates how to match the device address
             *          @ref ieee802154_sec_dev_addrmode_t
             */
            uint8_t dev_mode;
            /**
             * @brief   PAN id
             */
            uint8_t dev_pan_id[2];
            /**
             * @brief   Device address, either short or extended
             */
            uint8_t dev_addr[IEEE802154_LONG_ADDRESS_LEN];
        } implicit;
    } key_lookup;
    /**
     * @brief   Key descriptor of the key which is associated with this key lookup
     */
    ieee802154_sec_key_descriptor_t key;
} ieee802154_sec_key_lookup_t;

/**
 * @brief   Lookup table for key modes
 */
typedef struct ieee802154_sec_key_lookup_table {
    /**
     * @brief   Bitmask that shows which slots are occupied
     */
    BITFIELD(mask, CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE);

    /**
     * @brief   Array of key lookup structs
     */
    ieee802154_sec_key_lookup_t key_lookup[CONFIG_IEEE802154_SEC_DEFAULT_KEYLOOKUP_SIZE];
} ieee802154_sec_key_lookup_table_t;

/**
 * @brief   Struct to hold IEEE 802.15.4 security information
 */
typedef struct ieee802154_sec_context {
    /**
     * @brief   802.15.4 security dev
     */
    ieee802154_sec_dev_t dev;
    /**
     * @brief Cipher context with AES128 interface and key storage
     */
    cipher_t cipher;
    /**
     * @brief   Security level IEEE802154_SEC_SCF_SECLEVEL_*
     */
    uint8_t security_level;
    /**
     * @brief   Key mode IEEE802154_SEC_SCF_KEYMODE_*
     */
    uint8_t key_id_mode;
    /**
     * @brief   Key index
     */
    uint8_t key_index;
    /**
     * @brief   Key source
     *
     * Content depends on key_id_mode
     */
    uint8_t key_source[IEEE802154_LONG_ADDRESS_LEN];
    /**
     * @brief   Key lookup table
     *
     * @warning This datastructure should be stored in persistent storage.
     *          Access to this datastructure is likely to be changed in the future.
     *          Only use dedicated functions and do not assume anything about this type.
     */
    ieee802154_sec_key_lookup_table_t key_lookup_table;
    /**
     * @brief   Keystore for this netdev
     *
     * @warning This datastructure should be stored in persistent storage.
     *          Access to this datastructure is likely to be changed in the future.
     *          Only use dedicated functions and do not assume anything about this type.
     */
    ieee802154_sec_key_table_t keystore;
    /**
     * @brief   Peer device lookup table
     *
     * @warning This datastructure should be stored in persistent storage.
     *          Access to this datastructure is likely to be changed in the future.
     *          Only use dedicated functions and do not assume anything about this type.
     */
    ieee802154_sec_dev_lookup_table_t dev_lookup_table;
    /**
     * @brief   Internal lock to protect concurrent operations on datastructures
     */
    mutex_t lock;
} ieee802154_sec_context_t;

/**
 * @brief   IEEE 802.15.4 auxiliary security header
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   Security Control field (SCF)
     *
     *    Bit 7    Bit 6    Bit 5    Bit 4    Bit 3    Bit 2    Bit 1    Bit 0
     *  +--------+--------+--------+--------+--------+--------+--------+--------+
     *  | security level  | key id. mode    | fc sup.| ASN    |   r    |   r    |
     *  +--------+--------+--------+--------+--------+--------+--------+--------+
     *
     * security level:
     * one of IEEE802154_SEC_SCF_SECLEVEL_*
     * key identifier mode:
     * one of IEEE802154_SEC_SCF_KEY_*
     * frame counter suppression:
     * basically always zero because we do not support TSCH right now
     * ASN:
     * basically always zero because we do not support TSCG right now
     */
    uint8_t scf;
    /**
     * @brief   frame counter
     */
    uint32_t fc;
    /**
     * @brief   key identifier (0 - 9 bytes) according to key id. mode
     */
    uint8_t key_id[];
} ieee802154_sec_aux_t;

/**
 * @brief   Content of key_source if key mode is IEEE802154_SEC_SCF_KEYMODE_INDEX
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   Key index of key from originator, defined by key source
     */
    uint8_t key_index;
} ieee802154_sec_aux_key_identifier_1_t;

/**
 * @brief   Content of key_source if key mode is IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   macPANId concatenated with macShortAddress
     */
    uint8_t key_source[4];
    /**
     * @brief   Key index of key from originator, defined by key source
     */
    uint8_t key_index;
} ieee802154_sec_aux_key_identifier_5_t;

/**
 * @brief   Content of key_source if key mode is IEEE802154_SEC_SCF_KEYMODE_HW_INDEX
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   macExtendedAddress
     */
    uint8_t key_source[IEEE802154_LONG_ADDRESS_LEN];
    /**
     * @brief   Key index of key from originator, defined by key source
     */
    uint8_t key_index;
} ieee802154_sec_aux_key_identifier_9_t;

/**
 * @brief   Format of 13 byte nonce
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   Source long address
     */
    uint8_t src_addr[IEEE802154_LONG_ADDRESS_LEN];
    /**
     * @brief   Frame counter
     */
    uint32_t frame_counter;
    /**
     * @brief   One of IEEE802154_SEC_SCF_SECLEVEL_*
     */
    uint8_t security_level;
} ieee802154_sec_ccm_nonce_t;

/**
 * @brief   Format of 16 byte input block of CCM
 */
typedef struct __attribute__((packed)) {
    /**
     * @brief   Flags field
     */
    uint8_t flags;
    /**
     * @brief   Nonce (Number that is only used once)
     */
    ieee802154_sec_ccm_nonce_t nonce;
    /**
     * @brief   Either the length of the actual message (for CBC-MAC) or
     *          a block counter (for CTR)
     */
    uint16_t counter;
} ieee802154_sec_ccm_block_t;

/**
 * @brief   Initialize IEEE 802.15.4 security context with default values
 *
 * @param[out]      ctx                     IEEE 802.15.4 security context
 * @param[in]       sec_level               Default security level of outgoing frames
 * @param[in]       sec_key_mode            Default key mode of outgoing frames
 * @param[in]       sec_key_index           Default key index of outgoing frames
 * @param[in]       sec_key_source          Default key source of outgoing frames
 */
void ieee802154_sec_init(ieee802154_sec_context_t *ctx,
                         ieee802154_sec_scf_seclevel_t sec_level,
                         ieee802154_sec_scf_keymode_t sec_key_mode,
                         uint8_t sec_key_index,
                         const void *sec_key_source);

/**
 * @brief   Encrypt IEEE 802.15.4 frame according to @p ctx
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       header                  Pointer to frame header
 * @param[in, out]  header_size             in: Header size; out: Size of header and auxiliary header
 * @param[in,out]   payload                 in: Plain payload; out: Encrypted payload
 * @param[in]       payload_size            Size of payload
 * @param[out]      mic                     Buffer to store computed MIC
 * @param[out]      mic_size                Size of MIC
 * @param[in]       src_address             Source address
 *
 * @pre     @p header should be large enough to also store the auxiliary header
 *
 * @return          0 Success
 * @return          negative integer on error
 */
int ieee802154_sec_encrypt_frame(ieee802154_sec_context_t *ctx,
                                 const uint8_t *header, uint8_t *header_size,
                                 uint8_t *payload, uint16_t payload_size,
                                 uint8_t *mic, uint8_t *mic_size,
                                 const uint8_t *src_address);

/**
 * @brief   Decrypt IEEE 802.15.4 frame according to @p ctx
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       frame_size              Size of received frame
 * @param[in]       header                  Pointer to header, which is also the frame
 * @param[in, out]  header_size             in: Header size; out: Size of header and auxiliary header
 * @param[out]      payload                 Will point to the beginning of the payload
 * @param[out]      payload_size            Pointer to store the payload size
 * @param[out]      mic                     Will point to the beginning of the MIC
 * @param[out]      mic_size                Pointer to store the size of the MIC
 *
 * @pre     After @p header follows the auxiliary header
 *
 * @return          0 Success
 * @return          negative integer on error
 */
int ieee802154_sec_decrypt_frame(ieee802154_sec_context_t *ctx,
                                 uint16_t frame_size,
                                 uint8_t *header, uint8_t *header_size,
                                 uint8_t **payload, uint16_t *payload_size,
                                 uint8_t **mic, uint8_t *mic_size);

/**
 * @brief   Add a peer device to the device lookup table
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       pan_id                  Peer device PAN ID (host byte order)
 * @param[in]       short_addr              Peer device short address
 * @param[in]       long_addr               Peer device long address
 *
 * @return          Device descriptor on success
 * @return          IEEE802154_SEC_NO_IDENT on error
 */
ieee802154_sec_dev_descriptor_t ieee802154_sec_add_dev(ieee802154_sec_context_t *ctx,
                                                       uint16_t pan_id,
                                                       const uint8_t *short_addr,
                                                       const uint8_t *long_addr);

/**
 * @brief   Add new key material
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       key                     16 bytes key material
 *
 * @return          Key descriptor on success
 * @return          IEEE802154_SEC_NO_IDENT on error
 */
ieee802154_sec_key_descriptor_t ieee802154_sec_add_key(ieee802154_sec_context_t *ctx,
                                                       const uint8_t *key);

/**
 * @brief   Perform a key update
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       k                       Key descriptor
 * @param[in]       new                     The new key material
 *
 * @return          -IEEE802154_SEC_NO_KEY if @p k is invalid
 * @return          IEEE802154_SEC_OK on success
 */
int ieee802154_sec_update_key(ieee802154_sec_context_t *ctx,
                               ieee802154_sec_key_descriptor_t k,
                               const uint8_t *new);

/**
 * @brief   Pair with a device
 *
 * @experimental
 * @warning The current implementation always assumes that the pairing succeeds,
 *          and does not send an actual pairing request.
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       l                       Key lookup descriptor
 * @param[in]       pan_id                  Device PAN ID
 * @param[in]       short_addr              Device short address
 * @param[in]       long_addr               Device long address
 *
 * @return          -IEEE802154_SEC_NO_KEY if @p l is invalid
 * @return          -IEEE802154_SEC_NO_DEV if no device with matching parameters was found
 * @return          IEEE802154_SEC_OK on success
 */
int ieee802154_sec_peer_dev(ieee802154_sec_context_t *ctx,
                            ieee802154_sec_key_lookup_descriptor_t l,
                            uint16_t pan_id,
                            const uint8_t *short_addr, const uint8_t *long_addr);

/**
 * @brief   Add an implicit key lookup descriptor to the key lookup table
 *
 * A descriptor of this kind allows the recipient to identify the key to be used,
 * implicitly by the address of the sender.
 *
 * @pre A device descriptor corresponding to @p dev_pan_id and @p dev_addr
 *      must have been added before, using @ref ieee802154_sec_add_dev().
 *
 * @pre A key corresponding to @p k must have been added before,
        using @ref ieee802154_sec_add_key().
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       dev_mode                Device addressing mode
 * @param[in]       dev_pan_id              Device PAN ID (host byte order)
 * @param[in]       dev_addr                Device address
 * @param[in]       k                       Key descriptor
 *
 * @return          Key lookup descriptor on success
 * @return          IEEE802154_SEC_NO_IDENT on error
 */
ieee802154_sec_key_lookup_descriptor_t
ieee802154_sec_add_key_lookup_implicit(ieee802154_sec_context_t *ctx,
                                       ieee802154_sec_dev_addrmode_t dev_mode,
                                       uint16_t dev_pan_id,
                                       const uint8_t *dev_addr,
                                       ieee802154_sec_key_descriptor_t k);

/**
 * @brief   Add an explicit key lookup descriptor to the key lookup table
 *
 * An explicit key is a group key.
 * A descriptor of this kind allows the recipient to identify the key to be used,
 * explicitly by the sent key parameters @p key_mode, @p key_index , and @p key_source.
 * If @p key_mode is IEEE802154_SEC_SCF_KEYMODE_INDEX, @p key_source is not evaluated.
 * If @p key_mode is IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX, @p key_source is the key
 * originator´s PAN ID and short address.
 * If @p key_mode is IEEE802154_SEC_SCF_KEYMODE_HW_INDEX, @p key_source is the key
 * originator´s long address.
 *
 * @pre A key descriptor corresponding to @p k must have been added before,
 *      using @ref ieee802154_sec_add_key().
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       key_mode                Key mode
 * @param[in]       key_index               Key index
 * @param[in]       key_source              Key source, content depends of @p key_mode
 * @param[in]       k                       Key descriptor
 *
 * @return          Key lookup descriptor on success
 * @return          IEEE802154_SEC_NO_IDENT on error
 */
ieee802154_sec_key_lookup_descriptor_t
ieee802154_sec_add_key_lookup_explicit(ieee802154_sec_context_t *ctx,
                                       ieee802154_sec_scf_keymode_t key_mode,
                                       uint8_t key_index,
                                       const void *key_source,
                                       ieee802154_sec_key_descriptor_t k);

/**
 * @brief   Remove a peer device descriptor
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       d                       Descriptor of the device to be removed
 */
void ieee802154_sec_remove_dev(ieee802154_sec_context_t *ctx,
                               ieee802154_sec_dev_descriptor_t d);

/**
 * @brief   Remove a key descriptor
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       k                       Descriptor of the key to be removed
 */
void ieee802154_sec_remove_key(ieee802154_sec_context_t *ctx,
                               ieee802154_sec_key_descriptor_t k);

/**
 * @brief   Remove a key lookup descriptor
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       l                       Descriptor of the key lookup to be removed
 */
void ieee802154_sec_remove_key_lookup(ieee802154_sec_context_t *ctx,
                                      ieee802154_sec_key_lookup_descriptor_t l);

/**
 * @brief   Acquire the lock of @p ctx
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 */
void ieee802154_sec_iterator_acquire(ieee802154_sec_context_t *ctx);

/**
 * @brief   Release the lock of @p ctx
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 */
void ieee802154_sec_iterator_release(ieee802154_sec_context_t *ctx);

/**
 * @brief   Iterate over all stored keys of @p ctx
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       prev                    Previously obtained descriptor or
 *                                          @ref IEEE802154_SEC_NO_IDENT to begin
 * @param[out]      key                     Output key object
 *
 * @return          Key descriptor
 */
ieee802154_sec_key_descriptor_t
ieee802154_sec_key_iterator(ieee802154_sec_context_t *ctx,
                            ieee802154_sec_key_descriptor_t prev,
                            ieee802154_sec_key_t *key);

/**
 * @brief   Iterate over all stored devices of @p ctx
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       prev                    Previously obtained descriptor or
 *                                          @ref IEEE802154_SEC_NO_IDENT to begin
 * @param[out]      dev                     Output device object
 *
 * @return          device descriptor
 */
ieee802154_sec_dev_descriptor_t
ieee802154_sec_dev_iterator(ieee802154_sec_context_t *ctx,
                            ieee802154_sec_dev_descriptor_t prev,
                            ieee802154_sec_dev_lookup_t *dev);

/**
 * @brief   Iterate over all stored key lookup descriptors of @p ctx
 *
 * @param[in]       ctx                     IEEE 802.15.4 security context
 * @param[in]       prev                    Previously obtained descriptor or
 *                                          @ref IEEE802154_SEC_NO_IDENT to begin
 * @param[out]      lookup                  Output key lookup object
 *
 * @return          Key lokkup descriptor
 */
ieee802154_sec_key_lookup_descriptor_t
ieee802154_sec_key_lookup_iterator(ieee802154_sec_context_t *ctx,
                                   ieee802154_sec_key_lookup_descriptor_t prev,
                                   ieee802154_sec_key_lookup_t *lookup);

/**
 * @brief Default descriptor that will fallback to default implementations
 */
extern const ieee802154_radio_cipher_ops_t ieee802154_radio_cipher_ops;

#ifdef __cplusplus
}
#endif

#endif /* NET_IEEE802154_SECURITY_H */
/** @} */
