/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup drivers_mfrc522 MFRC522 RFID controller
 * @ingroup  drivers_actuators
 * @brief    Device driver for the NXP MFRC522 RFID controller
 *
 * With this driver MIFARE and NTAG tags/keys can be read and written contactless.
 *
 * Supported features:
 *   - Communication (Crypto1) with MIFARE Classic (1k, 4k, Mini)
 *   - Communication (Crypto1) with MIFARE Classic compatible PICCs
 *   - Firmware self check of MFRC522
 *   - Set the UID, write to sector 0, and unbrick Chinese UID changeable MIFARE cards
 *
 * Partially supported features:
 *   - Communication with MIFARE Ultralight
 *   - Other PICCs (Ntag216)
 *
 * Data sheet available here: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf
 *
 * This driver is a port of a driver originally written for Arduino, you can find
 * the original here: https://github.com/miguelbalboa/rfid (currently based on
 * commit 0a568b45baf1852883630e90ea125786e88f5322)
 *
 * @{
 *
 * @author   Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 */

#ifndef MFRC522_H
#define MFRC522_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdbool.h"

#include "periph/spi.h"

#include "mfrc522_regs.h"

/**
 * @brief Maximum size for uid byte array
 */
#define MFRC522_UID_MAX_SIZE  10

/**
 * @brief The MIFARE Classic uses a 4 bit ACK/NAK. Any other value than 0xA is NAK
 */
#define MFRC522_MF_ACK  0xA

/**
 * @brief A Mifare Crypto1 key is 6 bytes
 */
#define MFRC522_MF_KEY_SIZE  6

/**
 * @brief MFRC522 device initialization parameters
 */
typedef struct {
    spi_t spi_dev;      /**< SPI bus the controller is connected to */
    spi_clk_t spi_clk;  /**< SPI clock speed */
    gpio_t sck_pin;     /**< SCK pin */
    gpio_t miso_pin;    /**< MISO pin */
    gpio_t mosi_pin;    /**< MOSI pin */
    gpio_t cs_pin;      /**< Slave select pin */
    gpio_t rst_pin;     /**< Reset pin */
} mfrc522_params_t;

/**
 * @brief MFRC522 device data structure type
 */
typedef struct {
    mfrc522_params_t params;  /**< Device initialization parameters */
} mfrc522_t;

/**
 * @brief A struct used for passing the UID of a PICC
 */
typedef struct {
    uint8_t size;                               /**< Number of bytes in the UID. 4, 7 or 10 */
    uint8_t uid_byte[MFRC522_UID_MAX_SIZE];     /**< UID */
    uint8_t sak;                                /**< The SAK (Select acknowledge) byte returned from
                                                     the PICC after successful selection */
} mfrc522_uid_t;

/**
 * @brief A struct used for passing a MIFARE Crypto1 key
 */
typedef struct {
    uint8_t key_byte[MFRC522_MF_KEY_SIZE];  /**< Crypto1 key bytes */
} mfrc522_mifare_key_t;

/**
 * @brief PICC types
 */
typedef enum {
    MFRC522_PICC_TYPE_ISO_14443_4,      /**< PICC compliant with ISO/IEC 14443-4 */
    MFRC522_PICC_TYPE_ISO_18092,        /**< PICC compliant with ISO/IEC 18092 (NFC) */
    MFRC522_PICC_TYPE_MIFARE_MINI,      /**< MIFARE Classic protocol, 320 bytes */
    MFRC522_PICC_TYPE_MIFARE_1K,        /**< MIFARE Classic protocol, 1KB */
    MFRC522_PICC_TYPE_MIFARE_4K,        /**< MIFARE Classic protocol, 4KB */
    MFRC522_PICC_TYPE_MIFARE_UL,        /**< MIFARE Ultralight or Ultralight C */
    MFRC522_PICC_TYPE_MIFARE_PLUS,      /**< MIFARE Plus */
    MFRC522_PICC_TYPE_MIFARE_DESFIRE,   /**< MIFARE DESFire */
    MFRC522_PICC_TYPE_TNP3XXX,          /**< Only mentioned in NXP AN 10833 MIFARE
                                             Type Identification Procedure */
    MFRC522_PICC_TYPE_NOT_COMPLETE,     /**< SAK indicates UID is not complete */
    MFRC522_PICC_TYPE_UNKNOWN           /**< unknown type */
} mfrc522_picc_type_t;

/**
 * @brief Sets the bits given in mask in register reg
 *
 * @param[in] dev   Device descriptor of the MFRC522
 * @param[in] reg   Register to write to
 * @param[in] mask  Bitmask with the bits to set
 */
void mfrc522_pcd_set_register_bitmask(mfrc522_t *dev, mfrc522_pcd_register_t reg, uint8_t mask);

/**
 * @brief Clears the bits given in mask from register reg
 *
 * @param[in] dev   Device descriptor of the MFRC522
 * @param[in] reg   Register to write to
 * @param[in] mask  Bitmask with the bits to clear
 */
void mfrc522_pcd_clear_register_bitmask(mfrc522_t *dev, mfrc522_pcd_register_t reg, uint8_t mask);

/**
 * @brief Use the CRC coprocessor in the MFRC522 to calculate a CRC_A
 *
 * @param[in]  dev     Device descriptor of the MFRC522
 * @param[in]  data    Data to transfer to the FIFO for CRC calculation
 * @param[in]  length  Number of bytes to transfer
 * @param[out] result  Result buffer. Result is written to result[0..1], low
 *                     byte first.
 *
 * @retval  0          on success
 * @retval -ETIMEDOUT  on timeout
 */
int mfrc522_pcd_calculate_crc(mfrc522_t *dev, const uint8_t *data, uint8_t length, uint8_t *result);

/**
 * @brief Initialization
 *
 * @param[out]  dev     Device descriptor of the MFRC522
 * @param[in]   params  Parameters for device initialization
 *
 * @retval   0       Success
 * @retval  -EINVAL  Invalid CS pin/line
 * @retval  -ENXIO   Invalid device
 */
int mfrc522_pcd_init(mfrc522_t *dev, const mfrc522_params_t *params);

/**
 * @brief Performs a soft reset on the MFRC522 chip and waits for it to be ready
 *        again
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_pcd_reset(mfrc522_t *dev);

/**
 * @brief Turns the antenna on by enabling pins TX1 and TX2.
 *        After a reset these pins are disabled.
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_pcd_antenna_on(mfrc522_t *dev);

/**
 * @brief Turns the antenna off by disabling pins TX1 and TX2
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_pcd_antenna_off(mfrc522_t *dev);

/**
 * @brief Get the current MFRC522 receiver gain
 *
 * @param[in] dev  Device descriptor of the MFRC522
 *
 * @return Receiver gain
 */
mfrc522_pcd_rx_gain_t mfrc522_pcd_get_antenna_gain(mfrc522_t *dev);

/**
 * @brief Set the MFRC522 receiver gain
 *
 * @param[in] dev      Device descriptor of the MFRC522
 * @param[in] rx_gain  Antenna gain
 */
void mfrc522_pcd_set_antenna_gain(mfrc522_t *dev, mfrc522_pcd_rx_gain_t rx_gain);

/**
 * @brief Set the MFRC522 to soft power-down mode
 *
 * @note  Only soft power down mode is available through software.
 *
 * @note  Calling any other function that uses MFRC522_REG_COMMAND will disable
 *        soft power down mode! For more details about power control, refer to
 *        the datasheet - page 33 (8.6)
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_pcd_soft_power_down(mfrc522_t *dev);

/**
 * @brief Set the MFRC522 to leave soft power-down mode
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_pcd_soft_power_up(mfrc522_t *dev);

/**
 * @brief Executes MFRC522_CMD_TRANSCEIVE.
 *
 * @note  CRC validation can only be done if back_data and back_len are specified
 *
 * @param[in]     dev         Device descriptor of the MFRC522
 * @param[in]     send_data   Data to transfer to the FIFO
 * @param[in]     send_len    Number of bytes to transfer to the FIFO
 * @param[out]    back_data   Buffer if data should be read back after executing
 *                            the command, otherwise NULL
 * @param[inout]  back_len    Max number of bytes to write to *back_data.
 *                            Returns number of bytes returned.
 * @param[inout]  valid_bits  Number of valid bits in the last byte. 0 for 8
 *                            valid bits
 * @param[in]     rx_align    Defines the bit position in back_data[0] for the
 *                            first bit received
 * @param[in]     check_crc   If true, the last two bytes of the response are
 *                            assumed to be a CRC_A that must be validated
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_pcd_transceive_data(mfrc522_t *dev,
                                const uint8_t *send_data, uint8_t send_len,
                                uint8_t *back_data, uint8_t *back_len,
                                uint8_t *valid_bits, uint8_t rx_align, bool check_crc);

/**
 * @brief Transfers data to MFRC522's FIFO, executes a command, waits for
 *        completion and transfers data back from the FIFO
 *
 * @note  CRC validation can only be done if back_data and back_len are specified
 *
 * @param[in]     dev         Device descriptor of the MFRC522
 * @param[in]     command     The command to execute
 * @param[in]     wait_irq    The bits in the ComIrqReg register that signals
 *                            successful completion of the command
 * @param[in]     send_data   Data to transfer to the FIFO
 * @param[in]     send_len    Number of bytes to transfer to the FIFO
 * @param[out]    back_data   Buffer if data should be read back after executing
 *                            the command, otherwise NULL
 * @param[inout]  back_len    Max number of bytes to write to *back_data.
 *                            Returns number of bytes returned.
 * @param[inout]  valid_bits  Number of valid bits in the last byte. 0 for 8
 *                            valid bits.
 * @param[in]     rx_align    Defines the bit position in back_data[0] for the
 *                            first bit received
 * @param[in]     check_crc   True => The last two bytes of the response is
 *                            assumed to be a CRC_A that must be validated
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_pcd_communicate_with_picc(mfrc522_t *dev, mfrc522_pcd_command_t command,
                                      uint8_t wait_irq,
                                      const uint8_t *send_data, uint8_t send_len,
                                      uint8_t *back_data, uint8_t *back_len,
                                      uint8_t *valid_bits, uint8_t rx_align, bool check_crc);

/**
 * @brief Transmits REQA, Type A. Invites PICCs in state IDLE to go to READY and
 *        prepare for anti-collision or selection. 7 bit frame.
 *
 * @note  When two PICCs are in the field at the same time we often get
 *        -ETIMEDOUT - probably due do bad antenna design.
 *
 * @param[in]      dev          Device descriptor of the MFRC522
 * @param[out]     buffer_atqa  Buffer to store the ATQA in
 * @param[inout]   buffer_size  Buffer size, at least two bytes. Also number of
 *                              bytes returned on success.
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_picc_request_a(mfrc522_t *dev, uint8_t *buffer_atqa, uint8_t *buffer_size);

/**
 * @brief Transmits WUPA, Type A. Invites PICCs in state IDLE and HALT to go to
 *        READY(*) and prepare for anti-collision or selection. 7 bit frame.
 *
 * @note  When two PICCs are in the field at the same time we often get
 *        -ETIMEDOUT - probably due do bad antenna design.
 *
 * @param[in]      dev          Device descriptor of the MFRC522
 * @param[out]     buffer_atqa  Buffer to store the ATQA in
 * @param[inout]   buffer_size  Buffer size, at least two bytes. Also number of
 *                              bytes returned on success.
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_picc_wakeup_a(mfrc522_t *dev, uint8_t *buffer_atqa, uint8_t *buffer_size);

/**
 * @brief Transmits REQA (Type A) or WUPA (Type A) commands
 *
 * @note  When two PICCs are in the field at the same time we often get
 *        -ETIMEDOUT - probably due do bad antenna design.
 *
 * @param[in]     dev          Device descriptor of the MFRC522
 * @param[in]     command      Command to send - MFRC522_PICC_CMD_ISO_14443_REQA
 *                             or MFRC522_PICC_CMD_ISO_14443_WUPA
 * @param[out]    buffer_atqa  Buffer to store the ATQA in
 * @param[inout]  buffer_size  Buffer size, at least two bytes. Also number of
 *                             bytes returned on success.
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        if command was neither MFRC522_PICC_CMD_ISO_14443_REQA nor
 *                        MFRC522_PICC_CMD_ISO_14443_WUPA
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_picc_reqa_or_wupa(mfrc522_t *dev, mfrc522_picc_command_t command,
                              uint8_t *buffer_atqa, uint8_t *buffer_size);

/**
 * @brief Transmits SELECT/ANTICOLLISION commands to select a single PICC
 *
 * @note  Before calling this function the PICCs must be placed in the READY(*)
 *        state by calling mfrc522_picc_request_a() or mfrc522_picc_wakeup_a().
 *        On success:
 *          - The chosen PICC is in state ACTIVE(*) and all other PICCs have
 *            returned to state IDLE/HALT. (Figure 7 of the ISO/IEC 14443-3 draft.)
 *          - The UID size and value of the chosen PICC is returned in *uid along
 *            with the SAK.
 *
 * A PICC UID consists of 4, 7 or 10 bytes.
 * Only 4 bytes can be specified in a SELECT command, so for the longer UIDs two
 * or three iterations are used:
 *
 * UID size  Number of UID bytes  Cascade levels  Example of PICC
 * ========  ===================  ==============  ===============
 * single             4                   1       MIFARE Classic
 * double             7                   2       MIFARE Ultralight
 * triple            10                   3       Not currently in use?
 *
 * @param[in]     dev         Device descriptor of the MFRC522
 * @param[inout]  uid         Normally output, but can also be used to supply a
 *                            known UID
 * @param[in]     valid_bits  Number of known UID bits supplied in *uid.
 *                            Normally 0. If set you must also supply uid->size.
 *
 * @retval  0             on success
 * @retval -ECANCELED     on internal error
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_picc_select(mfrc522_t *dev, mfrc522_uid_t *uid, uint8_t valid_bits);

/**
 * @brief Instructs a PICC in state ACTIVE(*) to go to state HALT
 *
 * @param[in] dev  Device descriptor of the MFRC522
 *
 * @retval  0          on success
 * @retval -EIO        on communication error
 * @retval -ETIMEDOUT  on timeout
 */
int mfrc522_picc_halt_a(mfrc522_t *dev);

/**
 * @brief Executes the MFRC522 MFAuthent command
 *
 * This command manages MIFARE authentication to enable a secure communication
 * to any MIFARE Mini, MIFARE 1K and MIFARE 4K card. The authentication is
 * described in the MFRC522 datasheet section 10.3.1.9 and
 * http://www.nxp.com/documents/data_sheet/MF1S503x.pdf section 10.1. For use
 * with MIFARE Classic PICCs. The PICC must be selected - i.e. in state
 * ACTIVE(*) - before calling this function. Remember to call
 * mfrc522_pcd_stop_crypto1() after communicating with the authenticated PICC -
 * otherwise no new communications can start.
 *
 * All keys are set to FFFFFFFFFFFFh at chip delivery.
 *
 * @param[in] dev         Device descriptor of the MFRC522
 * @param[in] command     MFRC522_PICC_CMD_MF_AUTH_KEY_A or PICC_CMD_MF_AUTH_KEY_B
 * @param[in] block_addr  Block number. See numbering in the comments in the .h file.
 * @param[in] key         Crypteo1 key to use (6 bytes)
 * @param[in] uid         PICC's UID
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_pcd_authenticate(mfrc522_t *dev, mfrc522_picc_command_t command,
                             uint8_t block_addr, const mfrc522_mifare_key_t *key,
                             const mfrc522_uid_t *uid);

/**
 * @brief Used to exit the PCD from its authenticated state
 *
 * @note  Remember to call this function after communicating with an
 *        authenticated PICC - otherwise no new communications can start.
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_pcd_stop_crypto1(mfrc522_t *dev);

/**
 * @brief Read 16 bytes (+ 2 bytes CRC_A) from the active PICC
 *
 * @note  For MIFARE Classic the sector containing the block must be
 *        authenticated before calling this function.
 *
 * For MIFARE Ultralight only addresses 00h to 0Fh are decoded. The MF0ICU1
 * returns a NAK for higher addresses. The MF0ICU1 responds to the READ command
 * by sending 16 bytes starting from the page address defined by the command
 * argument. For example; if block_addr is 03h then pages 03h, 04h, 05h, 06h are
 * returned. A roll-back is implemented: If block_addr is 0Eh, then the contents
 * of pages 0Eh, 0Fh, 00h and 01h are returned.
 *
 * The buffer must be at least 18 bytes because a CRC_A is also returned.
 * Checks the CRC_A before returning.
 *
 * @param[in]      dev          Device descriptor of the MFRC522
 * @param[in]      block_addr   MIFARE Classic: The block (0-0xff) number.
 *                              MIFARE Ultralight: The first page to return data from.
 * @param[out]     buffer       Buffer to store the data in
 * @param[inout]   buffer_size  Buffer size, at least 18 bytes. Also number of
 *                              bytes returned on success.
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_read(mfrc522_t *dev, uint8_t block_addr, uint8_t *buffer, uint8_t *buffer_size);

/**
 * @brief Write 16 bytes to the active PICC
 *
 * @note  For MIFARE Classic the sector containing the block must be
 *        authenticated before calling this function.
 *
 * For MIFARE Ultralight the operation is called "COMPATIBILITY WRITE". Even
 * though 16 bytes are transferred to the Ultralight PICC, only the least
 * significant 4 bytes (bytes 0 to 3) are written to the specified address. It
 * is recommended to set the remaining bytes 04h to 0Fh to all logic 0.
 *
 * @param[in] dev          Device descriptor of the MFRC522
 * @param[in] block_addr   MIFARE Classic: The block (0-0xff) number.
 *                         MIFARE Ultralight: The page (2-15) to write to.
 * @param[in] buffer       The 16 bytes to write to the PICC
 * @param[in] buffer_size  Buffer size, must be at least 16 bytes. Exactly 16
 *                         bytes are written.
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_write(mfrc522_t *dev, uint8_t block_addr,
                         const uint8_t *buffer, uint8_t buffer_size);

/**
 * @brief Write a 4 byte page to the active MIFARE Ultralight PICC
 *
 * @param[in] dev          Device descriptor of the MFRC522
 * @param[in] page         The page (2-15) to write to
 * @param[in] buffer       The 4 bytes to write to the PICC
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_ultralight_write(mfrc522_t *dev, uint8_t page, const uint8_t *buffer);

/**
 * @brief Subtract the operand from the value of the addressed block, and store
 *        the result in the Transfer Buffer
 *
 * MIFARE Decrement subtracts the delta from the value of the addressed block,
 * and stores the result in a volatile memory. For MIFARE Classic only. The
 * sector containing the block must be authenticated before calling this
 * function. Only for blocks in "value block" mode, ie with access bits
 * [C1 C2 C3] = [110] or [001]. Use MIFARE_Transfer() to store the result in a
 * block.
 *
 * @param[in] dev         Device descriptor of the MFRC522
 * @param[in] block_addr  Block (0-0xff) number
 * @param[in] delta       Number to be subtracted from the value of block block_addr
 *
 * @retval  0          on success
 * @retval -ECANCELED  on internal error
 */
int mfrc522_mifare_decrement(mfrc522_t *dev, uint8_t block_addr, int32_t delta);

/**
 * @brief Add the operand to the value of the addressed block, and store the
 *        result in the Transfer Buffer
 *
 * MIFARE Increment adds the delta to the value of the addressed block, and
 * stores the result in a volatile memory. For MIFARE Classic only. The sector
 * containing the block must be authenticated before calling this function. Only
 * for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * Use MIFARE_Transfer() to store the result in a block.
 *
 * @param[in] dev         Device descriptor of the MFRC522
 * @param[in] block_addr  Block (0-0xff) number
 * @param[in] delta       Number to be added to the value of block block_addr
 *
 * @retval  0          on success
 * @retval -ECANCELED  on internal error
 */
int mfrc522_mifare_increment(mfrc522_t *dev, uint8_t block_addr, int32_t delta);

/**
 * @brief Copies the value of the addressed block into the Transfer Buffer
 *
 * MIFARE Restore copies the value of the addressed block into a volatile memory.
 * For MIFARE Classic only. The sector containing the block must be
 * authenticated before calling this function. Only for blocks in "value block"
 * mode, i.e. with access bits [C1 C2 C3] = [110] or [001]. Use
 * mfrc522_mifare_transfer() to store the result in a block.
 *
 * @param[in] dev         Device descriptor of the MFRC522
 * @param[in] block_addr  Block (0-0xff) number
 *
 * @retval  0          on success
 * @retval -ECANCELED  on internal error
 */
int mfrc522_mifare_restore(mfrc522_t *dev, uint8_t block_addr);

/**
 * @brief Write the value from the Transfer Buffer into destination block
 *
 * MIFARE Transfer writes the value stored in the volatile memory into one
 * MIFARE Classic block. For MIFARE Classic only. The sector containing the
 * block must be authenticated before calling this function. Only for blocks in
 * "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 *
 * @param[in] dev         Device descriptor of the MFRC522
 * @param[in] block_addr  Block (0-0xff) number
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_transfer(mfrc522_t *dev, uint8_t block_addr);

/**
 * @brief Helper routine to read the current value from a Value Block
 *
 * @note  Only for MIFARE Classic and only for blocks in "value block" mode,
 *        that is with access bits [C1 C2 C3] = [110] or [001]. The sector
 *        containing the block must be authenticated before calling this function.
 *
 * @param[in]  dev         Device descriptor of the MFRC522
 * @param[in]  block_addr  Block (0x00-0xff) number
 * @param[out] value       Current value of the Value Block
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_get_value(mfrc522_t *dev, uint8_t block_addr, int32_t *value);

/**
 * @brief Helper routine to write a specific value into a Value Block
 *
 * @note  Only for MIFARE Classic and only for blocks in "value block" mode,
 *        that is with access bits [C1 C2 C3] = [110] or [001]. The sector
 *        containing the block must be authenticated before calling this function.
 *
 * @param[in] dev         Device descriptor of the MFRC522
 * @param[in] block_addr  Block (0x00-0xff) number
 * @param[in] value       New value of the Value Block
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_set_value(mfrc522_t *dev, uint8_t block_addr, int32_t value);

/**
 * @brief Authenticate with a NTAG216
 *
 * @note  Only for NTAG216
 *
 * @param[in]  dev       Device descriptor of the MFRC522
 * @param[in]  password  Password (must have a size of exactly 4 bytes)
 * @param[out] p_ack     result (must have a size of exactly 2 bytes)
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_pcd_ntag216_auth(mfrc522_t *dev, const uint8_t *password, uint8_t p_ack[]);

/**
 * @brief Wrapper for MIFARE protocol communication
 *
 * Adds CRC_A, executes the Transceive command and checks that the response is
 * MFRC522_MF_ACK or a timeout.
 *
 * @param[in] dev             Device descriptor of the MFRC522
 * @param[in] send_data       Data to transfer to the FIFO. Do NOT include the CRC_A.
 * @param[in] send_len        Number of bytes in send_data
 * @param[in] accept_timeout  If true, then a timeout is also a success
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_pcd_mifare_transceive(mfrc522_t *dev,
                                  const uint8_t *send_data, uint8_t send_len,
                                  bool accept_timeout);

/**
 * @brief Translates the SAK (Select Acknowledge) to a PICC type
 *
 * @param[in] sak  The SAK byte returned from mfrc522_picc_select()
 *
 * @return PICC type
 */
mfrc522_picc_type_t mfrc522_picc_get_type(uint8_t sak);

/**
 * @brief Returns the name for the PICC type.
 *
 * @param[in] picc_type  PICC type enum
 *
 * @return PICC type name
 */
const char *mfrc522_picc_get_type_string(mfrc522_picc_type_t picc_type);

/**
 * @brief Calculates the bit pattern needed for the specified access bits. In
 *        the [C1 C2 C3] tuples C1 is MSB (=4) and C3 is LSB (=1).
 *
 * @param[out] access_bit_buffer  Pointer to byte 6, 7 and 8 in the sector
 *                                trailer. Bytes [0..2] will be set.
 * @param[in]  g0                 Access bits [C1 C2 C3] for block 0 (for
 *                                sectors 0-31) or blocks 0-4 (for sectors 32-39)
 * @param[in]  g1                 Access bits [C1 C2 C3] for block 1 (for
 *                                sectors 0-31) or blocks 5-9 (for sectors 32-39)
 * @param[in]  g2                 Access bits [C1 C2 C3] for block 2 (for
 *                                sectors 0-31) or blocks 10-14 (for sectors 32-39)
 * @param[in]  g3                 Access bits [C1 C2 C3] for the sector trailer,
 *                                block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
 */
void mfrc522_mifare_set_access_bits(uint8_t *access_bit_buffer,
                                    uint8_t g0, uint8_t g1, uint8_t g2, uint8_t g3);

/**
 * @brief Performs the "magic sequence" needed to get Chinese UID changeable
 *        Mifare cards to allow writing to sector 0, where the card UID is stored.
 *
 * @note  Note that you do not need to have selected the card through REQA or WUPA,
 *        this sequence works immediately when the card is in the reader vicinity.
 *        This means you can use this method even on "bricked" cards that your
 *        reader does not recognise anymore (see mfrc522_mifare_unbrick_uid_sector).
 *        Of course with non-bricked devices, you are free to select them before
 *        calling this function.
 *
 * @param[in] dev  Device descriptor of the MFRC522
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_open_uid_backdoor(mfrc522_t *dev);

/**
 * @brief Read entire block 0, including all manufacturer data, and overwrites
 *        that block with the new UID, a freshly calculated BCC, and the original
 *        manufacturer data.
 *
 * @note  It assumes a default KEY A of 0xFFFFFFFFFFFF. Make
 *        sure to have selected the PICC before this function is called and that
 *        the UID of the PICC is changeable.
 *
 * @param[in] dev           Device descriptor of the MFRC522
 * @param[in] uid           PICC's UID
 * @param[in] new_uid       New UID to set on PICC
 * @param[in] new_uid_size  Size of new UID
 *
 * @retval  0             on success
 * @retval -ECANCELED     on internal error
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_set_uid(mfrc522_t *dev, mfrc522_uid_t *uid,
                           const uint8_t *new_uid, uint8_t new_uid_size);

/**
 * @brief Reset entire sector 0 to zeroes, so the card can be read again by readers
 *
 * @param[in] dev  Device descriptor of the MFRC522
 *
 * @retval  0             on success
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_mifare_unbrick_uid_sector(mfrc522_t *dev);

/**
 * @brief Checks whether a new card could be detected
 *
 * @note  Only "new" cards in state IDLE are invited. Sleeping cards in state
 *        HALT are ignored.
 *
 * @param[in] dev  Device descriptor of the MFRC522
 *
 * @return true if new card was detected, false otherwise
 */
bool mfrc522_picc_is_new_card_present(mfrc522_t *dev);

/**
 * @brief Simple wrapper around mfrc522_picc_select()
 *
 * @note  Remember to call mfrc522_picc_is_new_card_present(),
 *        mfrc522_picc_request_a() or mfrc522_picc_wakeup_a() first.
 *
 * @param[in] dev  Device descriptor of the MFRC522
 * @param[in] uid  PICC's UID
 *
 * @retval  0             on success
 * @retval -ECANCELED     on internal error
 * @retval -ECONNABORTED  when a collision occurred
 * @retval -EINVAL        on invalid argument
 * @retval -EIO           on communication error
 * @retval -ENOBUFS       when a buffer is too small to receive data
 * @retval -ETIMEDOUT     on timeout
 */
int mfrc522_picc_read_card_serial(mfrc522_t *dev, mfrc522_uid_t *uid);

/**
 * @brief Dump debug info about the connected PCD to stdout
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_pcd_dump_version_to_serial(mfrc522_t *dev);

/**
 * @brief Dump debug info about the selected PICC to stdout
 *
 * @note  On success the PICC is halted after dumping the data.
 *        For MIFARE Classic the factory default key of 0xFFFFFFFFFFFF is tried.
 *
 * @param[in] dev  Device descriptor of the MFRC522
 * @param[in] uid  PICC's UID
 */
void mfrc522_picc_dump_to_serial(mfrc522_t *dev, mfrc522_uid_t *uid);

/**
 * @brief Dump card info (UID, SAK, Type) about the selected PICC to stdout
 *
 * @param[in] uid  PICC's UID
 */
void mfrc522_picc_dump_details_to_serial(mfrc522_uid_t *uid);

/**
 * @brief Dump memory contents of a MIFARE Classic PICC to stdout
 *
 * @note  On success the PICC is halted after dumping the data.
 *
 * @param[in] dev        Device descriptor of the MFRC522
 * @param[in] uid        PICC's UID
 * @param[in] picc_type  PICC type enum
 * @param[in] key        Key A used for all sectors.
 */
void mfrc522_picc_dump_mifare_classic_to_serial(mfrc522_t *dev, mfrc522_uid_t *uid,
                                                mfrc522_picc_type_t picc_type,
                                                mfrc522_mifare_key_t *key);

/**
 * @brief Dump memory contents of a sector of a MIFARE Classic PICC to stdout
 *
 * Uses mfrc522_pcd_authenticate(), mfrc522_mifare_read() and
 * mfrc522_pcd_stop_crypto1(). Always uses MFRC522_PICC_CMD_MF_AUTH_KEY_A
 * because only Key A can always read the sector trailer access bits.
 *
 * @param[in] dev     Device descriptor of the MFRC522
 * @param[in] uid     PICC's UID
 * @param[in] key     Key A for the sector.
 * @param[in] sector  The sector to dump, 0..39
 */
void mfrc522_picc_dump_mifare_classic_sector_to_serial(mfrc522_t *dev, mfrc522_uid_t *uid,
                                                       mfrc522_mifare_key_t *key, uint8_t sector);

/**
 * @brief Dump memory contents of a MIFARE Ultralight PICC to stdout
 *
 * @param[in] dev  Device descriptor of the MFRC522
 */
void mfrc522_picc_dump_mifare_ultralight_to_serial(mfrc522_t *dev);

/**
 * @brief Perform a self-test of the MFRC522
 *
 * (See section 16.1.1)
 *
 * @param[in] dev   Device descriptor of the MFRC522
 *
 * @return true if the test passed, false otherwise and if no firmware reference is available.
 */
bool mfrc522_pcd_perform_self_test(mfrc522_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MFRC522_H */
