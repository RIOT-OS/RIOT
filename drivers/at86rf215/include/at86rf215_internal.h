/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Low-Level functions for the AT86RF215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef AT86RF215_INTERNAL_H
#define AT86RF215_INTERNAL_H

#include <stdint.h>
#include "at86rf215.h"
#include "at86rf215_registers.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Minimum reset pulse width (tRST) in µs
 */
#ifndef CONFIG_AT86RF215_RESET_PULSE_WIDTH_US
#define CONFIG_AT86RF215_RESET_PULSE_WIDTH_US  (16U)
#endif

/**
 * @brief The typical transition time to TRX_OFF after reset (tPOWERON) in µs
 */
#define AT86RF215_RESET_DELAY_US        (16U)

/** Default energy detect threshold for CSMA (reset value) */
#define AT86RF215_EDT_DEFAULT           (-84) /* dBm */

/**
 * Default Parameters for 802.15.4 retransmissions & CSMA
 * @{
 */
#define AT86RF215_RETRIES_MAX_DEFAULT       (3)
#define AT86RF215_CSMA_RETRIES_MAX_DEFAULT  (4)
#define AT86RF215_CSMA_MIN_BE_DEFAULT       (3)
#define AT86RF215_CSMA_MAX_BE_DEFAULT       (5)
/** @} */

/** An ACK consists of 5 payload bytes */
#define AT86RF215_ACK_PSDU_BYTES            (5)

#define AT86RF215_OQPSK_MODE_LEGACY           (0x1)                             /**< legacy mode, 250 kbit/s */
#define AT86RF215_OQPSK_MODE_LEGACY_HDR       (0x3)                             /**< legacy mode, high data rate */
#define AT86RF215_MR_OQPSK_MODE(n)            ((n) << OQPSKPHRTX_MOD_SHIFT)     /**< MR-QPSK */

/**
 * @brief   Perform a reset of the entire chip.
 *
 * @param   dev     device to reset, will also reset sibling device
 * @return           0 on success, error if device is not available
 */
int at86rf215_hardware_reset(at86rf215_t *dev);

/**
 * @brief Write to a register at address `addr` from device `dev`.
 *
 * @param[in] dev   device to write to
 * @param[in] reg   address of the register to write
 * @param[in] val   value to write to the given register
 */
void at86rf215_reg_write(const at86rf215_t *dev, uint16_t reg, uint8_t val);

/**
 * @brief   Write a chunk of data into the memory of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] reg       address in the device to write to
 * @param[in] data      data to copy into the device
 * @param[in] len       number of bytes to write into the device
 */
void at86rf215_reg_write_bytes(const at86rf215_t *dev, uint16_t reg, const void *data, size_t len);

/**
 * @brief Read from a register at address `addr` from device `dev`.
 *
 * @param[in] dev   device to read from
 * @param[in] reg   address of the register to read
 *
 * @return          the value of the specified register
 */
uint8_t at86rf215_reg_read(const at86rf215_t *dev, uint16_t reg);

/**
 * @brief   Read a chunk of data from the memory of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  reg      starting address to read from
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read
 */
void at86rf215_reg_read_bytes(const at86rf215_t *dev, uint16_t reg, void *data, size_t len);

/**
 * @brief Enable / Disable the ACK filter
 *
 * @param[in] dev   device to configure
 * @param[in] on    if true, only ACK frames are received
 *                  if false, only non-ACK frames are received
 */
void at86rf215_filter_ack(at86rf215_t *dev, bool on);

/**
 * @brief   Read random data from the RNG
 *
 * @pre     The device has to be in state RX with PLL locked.
 *
 * @param[in]  dev      device to configure
 * @param[out] data     buffer to copy the random data to
 * @param[in]  len      number of random bytes to store in data
 */
void at86rf215_get_random(at86rf215_t *dev, void *data, size_t len);

/**
 * @ingroup  drivers_at86rf215
 * @defgroup drivers_at86rf215_fsk  AT86RF215 MR-FSK PHY
 * @{
 */

/**
 * @brief   Symbol Rates for register values, in 10kHz
 */
extern const uint8_t _at86rf215_fsk_srate_10kHz[];

/**
 * @brief   Channel Spacing for register values, in 25kHz
 */
extern const uint8_t _at86rf215_fsk_channel_spacing_25kHz[];

/**
 * @brief   Configure the radio to make use of FSK modulation
 *
 * @param[in] dev       device to configure
 * @param[in] srate     symbol rate, possible values: FSK_SRATE_50K … FSK_SRATE_400K
 * @param[in] mod_idx   modulation index, multiplied by 64 (½ -> 32; 1 -> 64)
 * @param[in] mod_order modulation order, may be FSK_MORD_2SFK or FSK_MORD_4SFK
 * @param[in] fec       forward error correction, may be @ref IEEE802154_FEC_NONE,
 *                      @ref IEEE802154_FEC_NRNSC or @ref IEEE802154_FEC_RSC
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_configure_FSK(at86rf215_t *dev, uint8_t srate, uint8_t mod_idx, uint8_t mod_order, uint8_t fec);

/**
 * @brief   Configure the symbol rate of the FSK modulation
 *
 * @param[in] dev       device to configure
 * @param[in] srate     symbol rate, possible values: FSK_SRATE_50K … FSK_SRATE_400K
 */
int at86rf215_FSK_set_srate(at86rf215_t *dev, uint8_t srate);

/**
 * @brief   Get the symbol rate of the FSK modulation
 *
 * @param[in] dev   device to read from
 *
 * @return          symbol rate, possible values: FSK_SRATE_50K … FSK_SRATE_400K
 */
uint8_t at86rf215_FSK_get_srate(at86rf215_t *dev);

/**
 * @brief   Configure the modulation index of the FSK modulation
 *          The modulation index is a fractional value, it is represented as
 *          fractions of 64.
 *          Not all possible fractional values are configurable by the hardware.
 *          If the fractional can not be mapped to the hardware, the nearest matching
 *          one is chosen.
 *
 * @param[in] dev       device to configure
 * @param[in] mod_idx   modulation index, multiplied by 64 (½ -> 32; 1 -> 64)
 *
 * @return              0 on success, failure otherwise
 */
int at86rf215_FSK_set_mod_idx(at86rf215_t *dev, uint8_t mod_idx);

/**
 * @brief   Get the current modulation index of the FSK modulation
 *          The modulation index is a fractional value, it is represented as
 *          fractions of 64.
 *
 * @param[in] dev       device to read from
 *
 * @return              the current modulation index, multiplied by 64
 */
uint8_t at86rf215_FSK_get_mod_idx(at86rf215_t *dev);

/**
 * @brief   Configure the Forward Error Correction (coding) scheme for FSK modulation.
 *          Supported values are:
 *              - no error correction
 *              - non-recursive and non-systematic code (NRNSC)
 *              - recursive and systematic code (RSC)
 *
 * @param[in] dev       device to configure
 * @param[in] mode      forward error correction, may be @ref IEEE802154_FEC_NONE,
 *                      @ref IEEE802154_FEC_NRNSC or @ref IEEE802154_FEC_RSC
 *
 * @return              0 on success, failure otherwise
 */
int at86rf215_FSK_set_fec(at86rf215_t *dev, uint8_t mode);

/**
 * @brief   Get the Forward Error Correction (coding) scheme for FSK modulation.
 *          Supported values are:
 *              - no error correction
 *              - non-recursive and non-systematic code (NRNSC)
 *              - recursive and systematic code (RSC)
 *
 * @param[in] dev       device to read from
 *
 * @return              current coding scheme, may be @ref IEEE802154_FEC_NONE,
 *                      @ref IEEE802154_FEC_NRNSC or @ref IEEE802154_FEC_RSC
 */
uint8_t at86rf215_FSK_get_fec(at86rf215_t *dev);

/**
 * @brief   Configure the channel spacing for the FSK modulation
 *
 * @param[in] dev       device to configure
 * @param[in] ch_space  channel spacing, possible values: FSK_CHANNEL_SPACING_200K … _400K
 *
 * @return              0 on success, failure otherwise
 */
int at86rf215_FSK_set_channel_spacing(at86rf215_t *dev, uint8_t ch_space);

/**
 * @brief   Get the configured channel spacing
 *
 * @param[in] dev   device to read from
 *
 * @return          channel spacing in kHz
 */
uint16_t at86rf215_get_channel_spacing(at86rf215_t *dev);

/**
 * @brief   Configure the FSK modulation order.
 *          You can choose between 2-level and 4-level FSK
 *
 * @param[in] dev       device to configure
 * @param[in] mod_order modulation order, may be FSK_MORD_2SFK or FSK_MORD_4SFK
 *
 * @return              0 on success, failure otherwise
 */
int at86rf215_FSK_set_mod_order(at86rf215_t *dev, uint8_t mod_order);

/**
 * @brief   Get the current FSK modulation order.
 *          You can choose between 2-level and 4-level FSK
 *
 * @param[in] dev       device to read from
 *
 * @return              modulation order, may be FSK_MORD_2SFK or FSK_MORD_4SFK
 */
uint8_t at86rf215_FSK_get_mod_order(at86rf215_t *dev);

/**
 * @brief   The FSK premable length needs to be switched between RX and TX
 *          This function takes care of putting FSK into RX mode.
 *
 * @param[in] dev       device that is entering RX mode
 */
void at86rf215_FSK_prepare_rx(at86rf215_t *dev);

/**
 * @brief   The FSK premable length needs to be switched between RX and TX
 *          This function takes care of putting FSK into TX mode.
 *
 * @param[in] dev       device that is entering TX mode
 */
void at86rf215_FSK_prepare_tx(at86rf215_t *dev);
/** @} */

/**
 * @ingroup  drivers_at86rf215
 * @defgroup drivers_at86rf215_ofdm AT86RF215 MR-OFDM PHY
 * @{
 */

/**
 * @brief   Configure the radio to make use of OFDM modulation.
 *          There are 4 OFDM options, each with a different number of active tones.
 *          The device supports BPSK, QPSK and 16-QAM modulation and coding schemes (MCS)
 *
 * @param[in] dev       device to configure
 * @param[in] option    modulation option, each increment halves the data rate
 * @param[in] mcs       modulation scheme, `BB_MCS_BPSK_REP4` … `BB_MCS_16QAM_3BY4`
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_configure_OFDM(at86rf215_t *dev, uint8_t option, uint8_t mcs);

/**
 * @brief   Set the current modulation and coding scheme (MCS)
 *
 * @param[in] dev       device to configure
 * @param[in] mcs       modulation and coding scheme
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_OFDM_set_scheme(at86rf215_t *dev, uint8_t mcs);

/**
 * @brief   Get the current modulation and coding scheme (MCS)
 *
 * @param[in] dev       device to read from
 *
 * @return              the current modulation & coding scheme
 */
uint8_t at86rf215_OFDM_get_scheme(at86rf215_t *dev);

/**
 * @brief   Set the current OFDM option
 *
 * @param[in] dev       device to configure
 * @param[in] option    OFDM option
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_OFDM_set_option(at86rf215_t *dev, uint8_t option);

/**
 * @brief   Get the current OFDM option
 *
 * @param[in] dev       device to read from
 *
 * @return              the current OFDM option
 */
uint8_t at86rf215_OFDM_get_option(at86rf215_t *dev);

/** @} */

/**
 * @ingroup     drivers_at86rf215
 * @defgroup    drivers_at86rf215_oqpsk     AT86RF215 MR-O-QPSK PHY
 * @{
 */

/**
 * @brief   Configure the radio to make use of O-QPSK modulation.
 *          The rate mode may be
 *              - 0 for compatibility with first-gen 802.15.4 devices (250 kbit/s)
 *              - 1 for compatibility with the proprietary high-data rate modes of at86rf2xx
 *
 * @param[in] dev       device to configure
 * @param[in] high_rate use proprietary high data rate compatible with at86rf2xx
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_configure_legacy_OQPSK(at86rf215_t *dev, bool high_rate);

/**
 * @brief   Configure the radio to make use of O-QPSK modulation.
 *          The chip rate is the number of bits per second (chips per second) used in the spreading signal.
 *          The rate mode may be
 *              - @ref AT86RF215_OQPSK_MODE_LEGACY for compatibility with first-gen 802.15.4 devices (250 kbit/s)
 *              - @ref AT86RF215_OQPSK_MODE_LEGACY_HDR for compatibility with the proprietary high-data rate mode
 *                of the at86rf233 (1000 kbit/s, 2.4 GHz) and at86rf212b (500 kbit/s, sub-GHz)
 *              - @ref AT86RF215_MR_OQPSK_MODE for the rate modes specified in 802.15.4g-2012
 *
 * @param[in] dev       device to configure
 * @param[in] chips     chip rate, `BB_FCHIP100` … `BB_FCHIP2000`
 * @param[in] rate      rate mode, may be @ref AT86RF215_OQPSK_MODE_LEGACY or @ref AT86RF215_MR_OQPSK_MODE
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_configure_OQPSK(at86rf215_t *dev, uint8_t chips, uint8_t rate);

/**
 * @brief   Get the current O-QPSK chip rate
 *
 * @param[in] dev       device to read from
 *
 * @return              the current chip rate
 */
uint8_t at86rf215_OQPSK_get_chips(at86rf215_t *dev);

/**
 * @brief   Set the current O-QPSK chip rate
 *
 * @param[in] dev       device to configure
 * @param[in] chips     chip rate in chip/s
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_OQPSK_set_chips(at86rf215_t *dev, uint8_t chips);

/**
 * @brief   Get the current O-QPSK rate mode
 *
 * @param[in] dev       device to read from
 *
 * @return              the current rate mode
 */
uint8_t at86rf215_OQPSK_get_mode(at86rf215_t *dev);

/**
 * @brief   Set the current O-QPSK rate mode
 *
 * @param[in] dev       device to configure
 * @param[in] mode      rate mode
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_OQPSK_set_mode(at86rf215_t *dev, uint8_t mode);

/**
 * @brief   Get the current legacy O-QPSK mode
 *
 * @param[in] dev       device to read from
 *
 * @return              0 for IEEE 802.15.4 mode, 1 for high data rate
 */
uint8_t at86rf215_OQPSK_get_mode_legacy(at86rf215_t *dev);

/**
 * @brief   Set the current legacy O-QPSK rate mode
 *
 * @param[in] dev       device to configure
 * @param[in] high_rate set to use proprietary high data rate
 *
 * @return              0 on success, error otherwise
 */
int at86rf215_OQPSK_set_mode_legacy(at86rf215_t *dev, bool high_rate);

/**
 * @brief   Test if O-QPSK PHY operates in legacy mode
 *
 * @param[in] dev       device to test
 *
 * @return              true if device operates in legacy mode
 */
static inline bool at86rf215_OQPSK_is_legacy(at86rf215_t *dev) {
    return at86rf215_reg_read(dev, dev->BBC->RG_OQPSKPHRTX) & AT86RF215_OQPSK_MODE_LEGACY;
}

/** @} */

/**
 * @brief   Get the current PHY modulation.
 *          May be @ref IEEE802154_PHY_MR_FSK, @ref IEEE802154_PHY_MR_OFDM,
 *          @ref IEEE802154_PHY_MR_OQPSK, @ref IEEE802154_PHY_OQPSK
 *          or @ref IEEE802154_PHY_DISABLED.
 *
 * @param[in] dev       device to read from
 *
 * @return              the current PHY mode the device is operating with
 */
uint8_t at86rf215_get_phy_mode(at86rf215_t *dev);

/**
 * @brief   Check if a channel number is valid.
 *          The function takes the current frequency band and modulation into
 *          account to determine if `chan` would be a legal channel number.
 *          If so, it is returned unmodified. Otherwise the next closest legal
 *          channel number is returned.
 *
 * @note    This function does not change the configuration.
 *
 * @param[in] dev       device to check against
 * @param[in] chan      the channel number to check
 *
 * @return              If the channel number is legal, `chan` is returned.
 *                      Otherwise the next closest legal channel number is
 *                      returned.
 */
uint16_t at86rf215_chan_valid(at86rf215_t *dev, uint16_t chan);

/**
 * @brief   Converts radio state into human readable string.
 *
 * @param[in] state     radio state
 *
 * @return              fixed string representation of the radio state
 */
const char* at86rf215_hw_state2a(uint8_t state);

/**
 * @brief   Converts state machine state into human readable string.
 *
 * @param[in] state     state of the driver's state machine
 *
 * @return              fixed string representation of the state machine state
 */
const char* at86rf215_sw_state2a(at86rf215_state_t state);

/**
 * @brief   Reads the contents of `reg`, apply `val` with a bitwise AND
 *          and then writes the result back to `reg`.
 *
 * @param[in] dev       device to write to
 * @param[in] reg       register to write to
 * @param[in] val       value to bitwise AND with the register content
 */
static inline void at86rf215_reg_and(const at86rf215_t *dev, uint16_t reg, uint8_t val)
{
    val &= at86rf215_reg_read(dev, reg);
    at86rf215_reg_write(dev, reg, val);
}

/**
 * @brief   Reads the contents of `reg`, apply `val` with a bitwise OR
 *          and then writes the result back to `reg`.
 *
 * @param[in] dev       device to write to
 * @param[in] reg       register to write to
 * @param[in] val       value to bitwise OR with the register content
 */
static inline void at86rf215_reg_or(const at86rf215_t *dev, uint16_t reg, uint8_t val)
{
    val |= at86rf215_reg_read(dev, reg);
    at86rf215_reg_write(dev, reg, val);
}

/**
 * @brief Write a 16-bit word to a register at address `addr` from device `dev`.
 *
 * @param[in] dev   device to write to
 * @param[in] reg   address of the register to write
 * @param[in] val   value to write to the given register
 */
static inline void at86rf215_reg_write16(const at86rf215_t *dev, uint16_t reg, uint16_t val)
{
    at86rf215_reg_write_bytes(dev, reg, &val, sizeof(val));
}

/**
 * @brief Read a 16-bit word from a register at address `addr` from device `dev`.
 *
 * @param[in] dev   device to read from
 * @param[in] reg   address of the register to read
 *
 * @return          the value of the specified register
 */
static inline uint16_t at86rf215_reg_read16(const at86rf215_t *dev, uint16_t reg)
{
    uint16_t value;
    at86rf215_reg_read_bytes(dev, reg, &value, sizeof(value));
    return value;
}

/**
 * @brief Issue a radio command to the device
 *
 * @param[in] dev    device to configure
 * @param[in] rf_cmd command to send
 */
static inline void at86rf215_rf_cmd(const at86rf215_t *dev, uint8_t rf_cmd)
{
    at86rf215_reg_write(dev, dev->RF->RG_CMD, rf_cmd);
}

/**
 * @brief Get the radio state of the device
 *
 * @param[in] dev   device to read from
 *
 * @return          the current radio state
 */
static inline uint8_t at86rf215_get_rf_state(const at86rf215_t *dev)
{
    return at86rf215_reg_read(dev, dev->RF->RG_STATE) & STATE_STATE_MASK;
}

/**
 * @brief Blocks until the device has reached the given state
 *
 * @param[in] dev    device to poll
 * @param[in] state  the expected state
 */
static inline void at86rf215_await_state(const at86rf215_t *dev, uint8_t state)
{
    while (at86rf215_get_rf_state(dev) != state) {}
}

/**
 * @brief Blocks until the device has reached the given state
 *
 * @param[in] dev    device to poll
 * @param[in] state  the expected state
 */
static inline void at86rf215_await_state_end(const at86rf215_t *dev, uint8_t state)
{
    while (at86rf215_get_rf_state(dev) == state) {}
}

/**
 * @brief Switch device back to IDLE-RX from non-RX idle
 *
 * @param[in] dev         device to update
 * @param[out] old_state  pointer to store the previous state, may be NULL
 *
 * @return  true if the operation was possible
 */
bool at86rf215_set_rx_from_idle(at86rf215_t *dev, uint8_t *old_state);

/**
 * @brief Switch device to non-RX idle state from RX
 *
 * @param[in] dev         device to update
 * @param[out] state      the new state (may be CMD_RF_TRXOFF or CMD_RF_SLEEP)
 *
 * @return  true if the operation was possible
 */
bool at86rf215_set_idle_from_rx(at86rf215_t *dev, uint8_t state);

/**
 * @brief Enable the baseband processor of the device
 *
 * @param[in] dev   device to enable the baseband on
 */
static inline void at86rf215_enable_baseband(const at86rf215_t *dev)
{
    at86rf215_reg_or(dev, dev->BBC->RG_PC, PC_BBEN_MASK);
}

/**
 * @brief Disable the baseband processor of the device
 *
 * @param[in] dev   device to disable the baseband on
 */
static inline void at86rf215_disable_baseband(const at86rf215_t *dev) {
    at86rf215_reg_and(dev, dev->BBC->RG_PC, ~PC_BBEN_MASK);
}

/**
 * @brief Enable the radio hardware with a given modulation.
 *
 * @param[in] dev           device to enable
 * @param[in] modulation    modulation to configure on the radio
 */
static inline void at86rf215_enable_radio(at86rf215_t *dev, uint8_t modulation)
{
    /* 16 bit frame-checksum, baseband enabled, checksum calculated by chip,
       frames with invalid cs are dropped */
    at86rf215_reg_write(dev, dev->BBC->RG_PC, modulation   | PC_BBEN_MASK
                                            | PC_FCST_MASK | PC_TXAFCS_MASK
                                            | PC_FCSFE_MASK);
}

/**
 * @brief Internal convenience function to disable reduced power
 *        consumption (RPC) for energy detection.
 *
 * @param[in] dev           device to configure
 */
void at86rf215_disable_rpc(at86rf215_t *dev);

/**
 * @brief Internal convenience function to re-enable reduced power
 *        consumption (RPC) after energy detection.
 *
 * @param[in] dev           device to configure
 */
void at86rf215_enable_rpc(at86rf215_t *dev);

/**
 * @brief Checks whether the device operates in the sub-GHz band.
 *
 * @param[in] dev   device to read from
 *
 * @return          true if the device operates in the sub-GHz band
 *                  false if the device operates in the 2.4-GHz band
 */
static inline bool is_subGHz(const at86rf215_t *dev)
{
    return dev->RF->RG_IRQS == RG_RF09_IRQS;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AT86RF215_INTERNAL_H */
