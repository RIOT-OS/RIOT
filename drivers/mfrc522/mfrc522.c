/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_mfrc522
 * @brief   Device driver for the MFRC522 controller
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#include <string.h>
#include <stdio.h>

#include "periph/gpio.h"
#include "ztimer.h"

#include "mfrc522.h"
#include "mfrc522_regs.h"

#define MFRC522_PICC_TYPE_NAMES_ARRAY_SIZE (MFRC522_PICC_TYPE_UNKNOWN + 1)

const char mfrc522_picc_type_names[MFRC522_PICC_TYPE_NAMES_ARRAY_SIZE][50] = {
    "PICC compliant with ISO/IEC 14443-4",
    "PICC compliant with ISO/IEC 18092 (NFC)",
    "MIFARE Mini, 320 bytes",
    "MIFARE 1KB",
    "MIFARE 4KB",
    "MIFARE Ultralight or Ultralight C",
    "MIFARE Plus",
    "MIFARE DESFire",
    "MIFARE TNP3XXX",
    "SAK indicates UID is not complete.",
    "Unknown type"
};

/**
 * @brief Write n bytes to a given register of the device.
 *
 * @param[in] dev       Device descriptor of the MFRC522
 * @param[in] reg       Register to write to
 * @param[in] count     Number of bytes to write
 * @param[in] values    Bytes to write
 */
static void _device_write_n(mfrc522_t *dev, mfrc522_pcd_register_t reg,
                            uint8_t count, const uint8_t *values)
{
    assert(dev);

    spi_acquire(dev->params.spi_dev, dev->params.cs_pin, SPI_MODE_0, dev->params.spi_clk);

    /* LSB always 0 and address needs to be shifted left by one. (Datasheet 8.1.2.3) */
    reg = reg << 1;

    /* MSB == 0 is for writing. LSB is not used in address. (Datasheet 8.1.2.3) */
    CLRBIT(reg, 0x80);

    /* Tell MFRC522 which address we want to write */
    spi_transfer_byte(dev->params.spi_dev, dev->params.cs_pin, true, reg);

    for (uint8_t index = 0; index < count; index++) {

        bool stop = (index == count - 1) ? false : true;

        spi_transfer_byte(dev->params.spi_dev, dev->params.cs_pin, stop, values[index]);
    }

    spi_release(dev->params.spi_dev);
}

/**
 * @brief Write single byte to a given register of the device.
 *
 * @param[in] dev       Device descriptor of the MFRC522
 * @param[in] reg       Register to write to
 * @param[in] value     Byte to write#
 */
static void _device_write(mfrc522_t *dev,
                          mfrc522_pcd_register_t reg, uint8_t value)
{
    assert(dev);

    _device_write_n(dev, reg, 1, &value);
}

/**
 * @brief Read n bytes from a given register of the device.
 *
 * @param[in]  dev       Device descriptor of the MFRC522
 * @param[in]  reg       Register to read from
 * @param[in]  count     Number of bytes to read
 * @param[out] values    Byte array to store the values in
 * @param[in]  rx_align  Only bit positions rxAlign..7 in values[0] are updated.
 */
static void _device_read_n(mfrc522_t *dev, mfrc522_pcd_register_t reg,
                           uint8_t count, uint8_t *values,
                           uint8_t rx_align)
{
    assert(dev);

    if (count == 0) {
        return;
    }

    /* last read operation is done outside the loop */
    count -= 1;

    spi_acquire(dev->params.spi_dev, dev->params.cs_pin, SPI_MODE_0, dev->params.spi_clk);

    /* LSB always 0 and address needs to be shifted left by one. (Datasheet 8.1.2.3) */
    reg = reg << 1;

    /* MSB == 1 is for reading. LSB is not used in address. (Datasheet 8.1.2.3) */
    SETBIT(reg, 0x80);

    /* Tell MFRC522 which address we want to read */
    spi_transfer_byte(dev->params.spi_dev, dev->params.cs_pin, true, reg);

    /* Index in values array */
    uint8_t index = 0;

    /* Only update bit positions rx_align..7 in values[0] */
    if (rx_align) {
        /* Create bit mask for bit positions rx_align..7 */
        uint8_t mask = (0xFF << rx_align) & 0xFF;

        /* Read value and tell that we want to read the same address again */
        uint8_t value = spi_transfer_byte(dev->params.spi_dev, dev->params.cs_pin, true, reg);

        /* Apply mask to both current value of values[0] and the new data in value */
        values[0] = (values[0] & ~mask) | (value & mask);
        index++;
    }

    while (index < count) {
        /* Read value and tell that we want to read the same address again */
        values[index] = spi_transfer_byte(dev->params.spi_dev, dev->params.cs_pin, true, reg);
        index++;
    }

    /* Read the final byte. Send 0 to stop reading */
    values[count] = spi_transfer_byte(dev->params.spi_dev, dev->params.cs_pin, false, 0);

    spi_release(dev->params.spi_dev);
}

/**
 * @brief Read single byte from a given register of the device.
 *
 * @param[in] dev     Device descriptor of the MFRC522
 * @param[in] reg     Register to read from
 * @param[in] byte    uint8_t pointer to store the value in
 */
static void _device_read(mfrc522_t *dev, mfrc522_pcd_register_t reg,
                         uint8_t *value)
{
    assert(dev);

    _device_read_n(dev, reg, 1, value, 0);
}

/**
 * @brief Helper function for the two-step MIFARE Classic protocol operations
 *        Decrement, Increment and Restore
 *
 * @param[in] dev         Device descriptor of the MFRC522
 * @param[in] command     Command to execute - MFRC522_PICC_CMD_MF_INCREMENT,
 *                        MFRC522_PICC_CMD_MF_DECREMENT
 *                        or MFRC522_PICC_CMD_MF_RESTORE
 * @param[in] block_addr  Block (0-0xff) number
 * @param[in] data        Data to transfer in step 2
 *
 * @retval 0        on success
 * @retval -EINVAL  if command was not out of [MFRC522_PICC_CMD_MF_INCREMENT,
 *                  MFRC522_PICC_CMD_MF_DECREMENT, MFRC522_PICC_CMD_MF_RESTORE]
 */
static int _mifare_two_step_helper(mfrc522_t *dev,
                                   mfrc522_picc_command_t command,
                                   uint8_t block_addr, int32_t data)
{
    assert(dev);

    if (   command != MFRC522_PICC_CMD_MF_INCREMENT
        && command != MFRC522_PICC_CMD_MF_DECREMENT
        && command != MFRC522_PICC_CMD_MF_RESTORE) {
        return -EINVAL;
    }

    int rc;

    /* We only need room for 2 bytes */
    uint8_t cmd_buffer[2];

    /* Step 1: Tell the PICC the command and block address */
    cmd_buffer[0] = command;
    cmd_buffer[1] = block_addr;

    /* Adds CRC_A and checks that the response is MFRC522_MF_ACK */
    rc = mfrc522_pcd_mifare_transceive(dev, cmd_buffer, 2, false);
    if (rc != 0) {
        return rc;
    }

    /* Step 2: Transfer the data */
    /* Adds CRC_A and accept timeout as success */
    rc = mfrc522_pcd_mifare_transceive(dev, (uint8_t *)&data, 4, true);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

void mfrc522_pcd_set_register_bitmask(mfrc522_t *dev,
                                      mfrc522_pcd_register_t reg, uint8_t mask)
{
    assert(dev);

    uint8_t tmp;
    _device_read(dev, reg, &tmp);

    /* set bit mask */
    _device_write(dev, reg, tmp | mask);
}

void mfrc522_pcd_clear_register_bitmask(mfrc522_t *dev, mfrc522_pcd_register_t reg, uint8_t mask)
{
    assert(dev);

    uint8_t tmp;
    _device_read(dev, reg, &tmp);

    /* clear bit mask */
    _device_write(dev, reg, tmp & (~mask));
}

int mfrc522_pcd_calculate_crc(mfrc522_t *dev, const uint8_t *data, uint8_t length, uint8_t *result)
{
    assert(dev);

    /* Stop any active command */
    _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);

    /* Clear the CRCIRq interrupt request bit */
    _device_write(dev, MFRC522_REG_DIV_IRQ, MFRC522_BIT_DIV_IRQ_CRC_IRQ);

    /* FlushBuffer = 1, FIFO initialization */
    _device_write(dev, MFRC522_REG_FIFO_LEVEL, 0x80);

    /* Write data to the FIFO */
    _device_write_n(dev, MFRC522_REG_FIFO_DATA, length, data);

    /* Start the calculation */
    _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_CALC_CRC);

    /* Wait for the CRC calculation to complete */
    for (uint16_t i = 5000; i > 0; i--) {

        /* 5000 * 18 us sums up to 90 ms */
        ztimer_sleep(ZTIMER_USEC, 18);

        uint8_t n;
        _device_read(dev, MFRC522_REG_DIV_IRQ, &n);

        /* CRCIRq bit set - calculation done */
        if (n & MFRC522_BIT_DIV_IRQ_CRC_IRQ) {
            /* Stop calculating CRC for new content in the FIFO */
            _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);
            /* Transfer the result from the registers to the result buffer */
            _device_read(dev, MFRC522_REG_CRC_RESULT_LSB, &result[0]);
            _device_read(dev, MFRC522_REG_CRC_RESULT_MSB, &result[1]);

            return 0;
        }
    }

    /* 90 ms passed and nothing happened. Communication with the MFRC522 might be down. */
    return -ETIMEDOUT;
}

int mfrc522_pcd_init(mfrc522_t *dev, const mfrc522_params_t *params)
{
    assert(dev);
    assert(params);

    dev->params = *params;

    /* Initialize SPI bus */
    int rc = spi_init_cs(dev->params.spi_dev, dev->params.cs_pin);
    if (rc < 0) {
        puts("error: unable to initialize the given chip select line");
        return rc;
    }

    bool hard_reset = false;

    /* If a valid pin number has been set, pull device out of power down / reset state */
    if (dev->params.rst_pin != 0) {

        gpio_init(dev->params.rst_pin, GPIO_IN);

        if (gpio_read(dev->params.rst_pin) == 0) {
            /* The MFRC522 chip is in power down mode */

            gpio_init(dev->params.rst_pin, GPIO_OUT);
            gpio_write(dev->params.rst_pin, 0);
            /* 8.8.1 Reset timing requirements says about 100ns. Let us be generous: 2μs */
            ztimer_sleep(ZTIMER_USEC, 2);
            /* Exit power down mode. This triggers a hard reset. */
            gpio_write(dev->params.rst_pin, 1);
            /* Section 8.8.2 in the datasheet says the oscillator start-up time
             * is the start-up time of the crystal + 37,74μs. Let us be generous: 50ms */
            ztimer_sleep(ZTIMER_MSEC, 50);
            hard_reset = true;
        }
    }

    if (!hard_reset) {
        mfrc522_pcd_reset(dev);
    }

    /* Reset baud rates */
    _device_write(dev, MFRC522_REG_TX_MODE, 0x00);
    _device_write(dev, MFRC522_REG_RX_MODE, 0x00);

    /* Reset MFRC522_REG_MOD_WIDTH */
    _device_write(dev, MFRC522_REG_MOD_WIDTH, 0x26);

    /* When communicating with a PICC we need a timeout if something goes wrong.
     * f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
     * TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg. */

    /* TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds */
    _device_write(dev, MFRC522_REG_T_MODE, 0x80);

    /* TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer =
     * 40kHz, i.e. a timer period of 25μs. */
    _device_write(dev, MFRC522_REG_T_PRESCALER, 0xA9);

    /* Reload timer with 0x3E8 = 1000, i.e. 25ms before timeout */
    _device_write(dev, MFRC522_REG_T_RELOAD_MSB, 0x03);
    _device_write(dev, MFRC522_REG_T_RELOAD_LSB, 0xE8);

    /* Default 0x00. Force a 100 % ASK modulation independent of the
     * MFRC522_REG_MOD_GS_P register setting */
    _device_write(dev, MFRC522_REG_TX_ASK, 0x40);

    /* Default 0x3F. Set the preset value for the CRC coprocessor for the
     * MFRC522_CMD_CALC_CRC command to 0x6363 (ISO 14443-3 part 6.2.4) */
    _device_write(dev, MFRC522_REG_MODE, 0x3D);

    /* Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset) */
    mfrc522_pcd_antenna_on(dev);

    return 0;
}

void mfrc522_pcd_reset(mfrc522_t *dev)
{
    assert(dev);

    _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_SOFT_RESET);

    /* The datasheet does not mention how long the SoftRest command takes to
     * complete. But the MFRC522 might have been in soft power-down mode
     * (triggered by bit 4 of CommandReg) Section 8.8.2 in the datasheet says
     * the oscillator start-up time is the start-up time of the crystal + 37,74μs.
     * Let us be generous: 50ms. */

    uint8_t count = 0;
    uint8_t value;
    do {
        /* Wait for the PowerDown bit in CommandReg to be cleared (max 3x50ms) */
        ztimer_sleep(ZTIMER_MSEC, 50);
        _device_read(dev, MFRC522_REG_COMMAND, &value);
    } while ((value & MFRC522_BIT_COMMAND_POWER_DOWN) && (++count) < 3);
}

void mfrc522_pcd_antenna_on(mfrc522_t *dev)
{
    assert(dev);

    uint8_t value;
    _device_read(dev, MFRC522_REG_TX_CONTROL, &value);

    uint8_t tx_enabled_mask =
        MFRC522_BIT_TX_CONTROL_TX1_RF_EN | MFRC522_BIT_TX_CONTROL_TX2_RF_EN;

    if ((value & tx_enabled_mask) != tx_enabled_mask) {
        _device_write(dev, MFRC522_REG_TX_CONTROL, value | tx_enabled_mask);
    }
}

void mfrc522_pcd_antenna_off(mfrc522_t *dev)
{
    assert(dev);

    uint8_t tx_enabled_mask =
        MFRC522_BIT_TX_CONTROL_TX1_RF_EN | MFRC522_BIT_TX_CONTROL_TX2_RF_EN;

    mfrc522_pcd_clear_register_bitmask(dev, MFRC522_REG_TX_CONTROL, tx_enabled_mask);
}

mfrc522_pcd_rx_gain_t mfrc522_pcd_get_antenna_gain(mfrc522_t *dev)
{
    assert(dev);

    /* See 9.3.3.6 / table 98
     * Return value scrubbed with MFRC522_BITMASK_RF_CFG_RX_GAIN
     * MFRC522_REG_RF_CFG may use reserved bits. */

    uint8_t reg_content;
    _device_read(dev, MFRC522_REG_RF_CFG, &reg_content);
    mfrc522_pcd_rx_gain_t gain = (reg_content & MFRC522_BITMASK_RF_CFG_RX_GAIN) >> 4;

    return gain;
}

void mfrc522_pcd_set_antenna_gain(mfrc522_t *dev, mfrc522_pcd_rx_gain_t rx_gain)
{
    assert(dev);

    if (mfrc522_pcd_get_antenna_gain(dev) != rx_gain) {

        /* See 9.3.3.6 / table 98
         * Given rx_gain is scrubbed with MFRC522_BITMASK_RF_CFG_RX_GAIN
         * MFRC522_REG_RF_CFG may use reserved bits.*/

        mfrc522_pcd_clear_register_bitmask(dev, MFRC522_REG_RF_CFG, MFRC522_BITMASK_RF_CFG_RX_GAIN);
        mfrc522_pcd_set_register_bitmask(dev, MFRC522_REG_RF_CFG,
                                         (rx_gain << 4) & MFRC522_BITMASK_RF_CFG_RX_GAIN);
    }
}

void mfrc522_pcd_soft_power_down(mfrc522_t *dev)
{
    assert(dev);

    uint8_t value;
    _device_read(dev, MFRC522_REG_COMMAND, &value);

    /* set PowerDown bit to 1 */
    value |= MFRC522_BIT_COMMAND_POWER_DOWN;
    _device_write(dev, MFRC522_REG_COMMAND, value);
}

void mfrc522_pcd_soft_power_up(mfrc522_t *dev)
{
    assert(dev);

    uint8_t value;
    _device_read(dev, MFRC522_REG_COMMAND, &value);

    /* set PowerDown bit to 0 */
    value &= ~MFRC522_BIT_COMMAND_POWER_DOWN;
    _device_write(dev, MFRC522_REG_COMMAND, value);

    /* wait until PowerDown bit is cleared (this indicates end of wake-up procedure) */

    /* create timer for timeout within 500 ms (just in case) */
    const uint64_t timeout = ztimer_now(ZTIMER_MSEC) + 500;

    while (ztimer_now(ZTIMER_MSEC) <= timeout) {

        _device_read(dev, MFRC522_REG_COMMAND, &value);

        /* if powerdown bit is 0 */
        if (!(value & MFRC522_BIT_COMMAND_POWER_DOWN)) {
            /* wake up procedure is finished */
            break;
        }
    }
}

int mfrc522_pcd_transceive_data(mfrc522_t *dev,
                                const uint8_t *send_data, uint8_t send_len,
                                uint8_t *back_data, uint8_t *back_len,
                                uint8_t *valid_bits, uint8_t rx_align, bool check_crc)
{
    assert(dev);

    uint8_t wait_irq = MFRC522_BIT_COM_IRQ_RX_IRQ | MFRC522_BIT_COM_IRQ_IDLE_IRQ;

    return mfrc522_pcd_communicate_with_picc(dev, MFRC522_CMD_TRANSCEIVE, wait_irq,
                                             send_data, send_len,
                                             back_data, back_len,
                                             valid_bits, rx_align, check_crc);
}

int mfrc522_pcd_communicate_with_picc(mfrc522_t *dev, mfrc522_pcd_command_t command,
                                      uint8_t wait_irq,
                                      const uint8_t *send_data, uint8_t send_len,
                                      uint8_t *back_data, uint8_t *back_len,
                                      uint8_t *valid_bits, uint8_t rx_align, bool check_crc)
{
    assert(dev);

    /* prepare values for BitFramingReg */
    uint8_t tx_last_bits = valid_bits ? *valid_bits : 0;

    /* RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0] */
    uint8_t bit_framing = (rx_align << 4) + tx_last_bits;

    /* stop any active command */
    _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);

    /* clear all seven interrupt request bits */
    uint8_t irq_bits = MFRC522_BIT_COM_IRQ_TIMER_IRQ
                     | MFRC522_BIT_COM_IRQ_ERR_IRQ
                     | MFRC522_BIT_COM_IRQ_LO_ALERT_IRQ
                     | MFRC522_BIT_COM_IRQ_HI_ALERT_IRQ
                     | MFRC522_BIT_COM_IRQ_IDLE_IRQ
                     | MFRC522_BIT_COM_IRQ_RX_IRQ
                     | MFRC522_BIT_COM_IRQ_TX_IRQ;

    _device_write(dev, MFRC522_REG_COM_IRQ, irq_bits);

    /* FlushBuffer = 1, FIFO initialization */
    _device_write(dev, MFRC522_REG_FIFO_LEVEL, 0x80);

    /* write send_data to the FIFO */
    _device_write_n(dev, MFRC522_REG_FIFO_DATA, send_len, send_data);

    /* bit adjustments */
    _device_write(dev, MFRC522_REG_BIT_FRAMING, bit_framing);

    /* execute the command */
    _device_write(dev, MFRC522_REG_COMMAND, command);

    if (command == MFRC522_CMD_TRANSCEIVE) {
        /* StartSend=1, transmission of data starts */
        mfrc522_pcd_set_register_bitmask(dev, MFRC522_REG_BIT_FRAMING, 0x80);
    }

    /* Wait for the command to complete. In mfrc522_pcd_init() we set the TAuto
     * flag in TModeReg. This means the timer automatically starts when the PCD
     * stops transmitting. */
    uint16_t i;
    for (i = 2000; i > 0; i--) {

        /* 2000 * 18 us sums up to 36 ms */
        ztimer_sleep(ZTIMER_USEC, 18);

        uint8_t n;
        _device_read(dev, MFRC522_REG_COM_IRQ, &n);

        /* One of the interrupts that signals success has been set */
        if (n & wait_irq) {
            break;
        }

        /* Timer interrupt - nothing received in 25ms */
        if (n & MFRC522_BIT_COM_IRQ_TIMER_IRQ) {
            return -ETIMEDOUT;
        }
    }

    /* 36ms and nothing happened. Communication with the MFRC522 might be down. */
    if (i == 0) {
        return -ETIMEDOUT;
    }

    uint8_t error_reg_value;
    _device_read(dev, MFRC522_REG_ERROR, &error_reg_value);

    uint8_t error_mask = MFRC522_BIT_ERROR_BUFFER_OVFL
                       | MFRC522_BIT_ERROR_PARITY_ERR
                       | MFRC522_BIT_ERROR_PROTOCOL_ERR;

    /* Stop now if any errors except collisions were detected */
    if (error_reg_value & error_mask) {
        return -EIO;
    }

    uint8_t _valid_bits = 0;

    /* If the caller wants data back, get it from the MFRC522 */
    if (back_data && back_len) {
        /* Number of bytes in the FIFO */
        uint8_t n;
        _device_read(dev, MFRC522_REG_FIFO_LEVEL, &n);
        if (n > *back_len) {
            return -ENOBUFS;
        }
        /* Number of bytes returned */
        *back_len = n;

        /* Get received data from FIFO */
        _device_read_n(dev, MFRC522_REG_FIFO_DATA, n, back_data, rx_align);

        /* RxLastBits[2:0] indicates the number of valid bits in the last
         * received byte. If this value is 000b, the whole byte is valid. */
        uint8_t tmp;
        _device_read(dev, MFRC522_REG_CONTROL, &tmp);
        _valid_bits = tmp & MFRC522_BITMASK_CONTROL_RX_LAST_BITS;

        if (valid_bits) {
            *valid_bits = _valid_bits;
        }
    }

    if (error_reg_value & MFRC522_BIT_ERROR_COLL_ERR) {
        return -ECONNABORTED;
    }

    /* Perform CRC_A validation if requested */
    if (back_data && back_len && check_crc) {

        /* In this case a MIFARE Classic NAK is not OK */
        if (*back_len == 1 && _valid_bits == 4) {
            return -EIO;
        }

        /* We need at least the CRC_A value and all 8 bits of the last byte must
         * be received */
        if (*back_len < 2 || _valid_bits != 0) {
            return -EIO;
        }

        /* Verify CRC_A - do our own calculation and store the control in
         * control_buffer */
        uint8_t control_buffer[2];

        int rc = mfrc522_pcd_calculate_crc(dev, &back_data[0], *back_len - 2, &control_buffer[0]);
        if (rc != 0) {
            return rc;
        }

        if ((back_data[*back_len - 2] != control_buffer[0]) ||
            (back_data[*back_len - 1] != control_buffer[1])) {
            return -EIO;
        }
    }

    return 0;
}

int mfrc522_picc_request_a(mfrc522_t *dev, uint8_t *buffer_atqa, uint8_t *buffer_size)
{
    assert(dev);

    return mfrc522_picc_reqa_or_wupa(
        dev, MFRC522_PICC_CMD_ISO_14443_REQA, buffer_atqa, buffer_size);
}

int mfrc522_picc_wakeup_a(mfrc522_t *dev, uint8_t *buffer_atqa, uint8_t *buffer_size)
{
    assert(dev);

    return mfrc522_picc_reqa_or_wupa(
        dev, MFRC522_PICC_CMD_ISO_14443_WUPA, buffer_atqa, buffer_size);
}

int mfrc522_picc_reqa_or_wupa(mfrc522_t *dev, mfrc522_picc_command_t command,
                              uint8_t *buffer_atqa, uint8_t *buffer_size)
{
    assert(dev);

    uint8_t valid_bits;
    int rc;

    if (   command != MFRC522_PICC_CMD_ISO_14443_REQA
        && command != MFRC522_PICC_CMD_ISO_14443_WUPA) {
        return -EINVAL;
    }

    /* The ATQA response is 2 bytes long */
    if (buffer_atqa == NULL || *buffer_size < 2) {
        return -ENOBUFS;
    }

    /* Bits received after collision are cleared */
    mfrc522_pcd_clear_register_bitmask(
        dev, MFRC522_REG_COLL, MFRC522_BIT_COLL_VALUES_AFTER_COLL);

    /* For REQA and WUPA we need the short frame format - transmit only 7 bits
     * of the last (and only) byte. TxLastBits = BitFramingReg[2..0] */
    valid_bits = 7;

    rc = mfrc522_pcd_transceive_data(dev, (uint8_t *)&command, 1, buffer_atqa, buffer_size,
                                     &valid_bits, 0, false);
    if (rc != 0) {
        return rc;
    }

    /* ATQA must be exactly 16 bits */
    if (*buffer_size != 2 || valid_bits != 0) {
        return -EIO;
    }

    return 0;
}

int mfrc522_picc_select(mfrc522_t *dev, mfrc522_uid_t *uid, uint8_t valid_bits)
{
    assert(dev);

    int rc;
    bool uid_complete;
    bool use_cascade_tag;
    uint8_t cascade_level = 1;
    uint8_t count;
    uint8_t check_bit;

    /* First index in uid->uid_byte[] that is used in the current Cascade Level */
    uint8_t uid_index;

    /* SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A */
    uint8_t buffer[9];

    /* Number of bytes used in the buffer, i.e. the number of bytes to transfer to the FIFO */
    uint8_t buffer_used;

    /* Used in BitFramingReg. Defines the bit position for the first bit received */
    uint8_t rx_align;

    /* Used in BitFramingReg. The number of valid bits in the last transmitted byte */
    uint8_t tx_last_bits;
    uint8_t *response_buffer;
    uint8_t response_length;

    /* Description of buffer structure:
     * Byte 0: SEL Indicates the Cascade Level:
     *         MFRC522_PICC_CMD_ISO_14443_SEL_CL1, MFRC522_PICC_CMD_ISO_14443_SEL_CL2
     *         or MFRC522_PICC_CMD_ISO_14443_SEL_CL3
     * Byte 1: NVBNumber of Valid Bits (in complete command, not just the UID):
     *         High nibble: complete bytes, Low nibble: Extra bits.
     * Byte 2: UID-data or CTSee explanation below. CT means Cascade Tag.
     * Byte 3: UID-data
     * Byte 4: UID-data
     * Byte 5: UID-data
     * Byte 6: BCCBlock Check Character - XOR of bytes 2-5
     * Byte 7: CRC_A
     * Byte 8: CRC_A
     *
     * The BCC and CRC_A are only transmitted if we know all the UID bits of the
     * current Cascade Level.
     *
     * Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft:
     *      UID contents and cascade levels)
     * Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft:
     *      UID contents and cascade levels)
     *
     * UID size Cascade level Byte2 Byte3 Byte4 Byte5
     * ======== ============= ===== ===== ===== =====
     * 4 bytes  1        uid0  uid1 uid2  uid3
     * 7 bytes  1        CT    uid0 uid1  uid2
     * 2        uid3     uid4  uid5 uid6
     * 10 bytes 1        CT    uid0 uid1  uid2
     * 2        CT       uid3  uid4 uid5
     * 3        uid6     uid7  uid8 uid9
     */

    /* Sanity check */
    if (valid_bits > 80) {
        return -EINVAL;
    }

    /* Bits received after collision are cleared */
    mfrc522_pcd_clear_register_bitmask(
        dev, MFRC522_REG_COLL, MFRC522_BIT_COLL_VALUES_AFTER_COLL);

    /* Repeat Cascade Level loop until we have a complete UID */
    uid_complete = false;
    while (!uid_complete) {
        /* Set the Cascade Level in the SEL byte, find out if we need to use the
         * Cascade Tag in byte 2 */
        switch (cascade_level) {

        case 1:
            buffer[0] = MFRC522_PICC_CMD_ISO_14443_SEL_CL1;
            uid_index = 0;

            /* When we know that the UID has more than 4 bytes */
            use_cascade_tag = valid_bits && uid->size > 4;
            break;

        case 2:
            buffer[0] = MFRC522_PICC_CMD_ISO_14443_SEL_CL2;
            uid_index = 3;

            /* When we know that the UID has more than 7 bytes */
            use_cascade_tag = valid_bits && uid->size > 7;
            break;

        case 3:
            buffer[0] = MFRC522_PICC_CMD_ISO_14443_SEL_CL3;
            uid_index = 6;

            /* Never used in CL3 */
            use_cascade_tag = false;
            break;

        default:
            return -ECANCELED;
            break;
        }

        /* Number of known UID bits in the current Cascade Level */
        int8_t current_level_known_bits;

        /* How many UID bits are known in this Cascade Level? */
        current_level_known_bits = valid_bits - (8 * uid_index);
        if (current_level_known_bits < 0) {
            current_level_known_bits = 0;
        }


        /* destination index in buffer[] */
        uint8_t index = 2;
        if (use_cascade_tag) {
            buffer[index++] = MFRC522_PICC_CASCADE_TAG;
        }

        /* Number of bytes needed to represent the known bits for this level */
        uint8_t bytes_to_copy =
            current_level_known_bits / 8 + ((current_level_known_bits % 8) ? 1 : 0);

        if (bytes_to_copy) {
            /* Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag */
            uint8_t max_bytes = use_cascade_tag ? 3 : 4;

            if (bytes_to_copy > max_bytes) {
                bytes_to_copy = max_bytes;
            }

            for (count = 0; count < bytes_to_copy; count++) {
                buffer[index++] = uid->uid_byte[uid_index + count];
            }
        }

        /* Now that the data has been copied we need to include the 8 bits in CT
         * in current_level_known_bits */
        if (use_cascade_tag) {
            current_level_known_bits += 8;
        }

        /* Repeat anti collision loop until we can transmit all UID bits + BCC
         * and receive a SAK - max 32 iterations */
        bool select_done = false;
        while (!select_done) {
            /* Find out how many bits and bytes to send and receive */
            if (current_level_known_bits >= 32) {
                /* All UID bits in this Cascade Level are known. This is a SELECT. */

                /* NVB - Number of Valid Bits: Seven whole bytes */
                buffer[1] = 0x70;

                /* Calculate BCC - Block Check Character */
                buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];

                /* Calculate CRC_A */
                rc = mfrc522_pcd_calculate_crc(dev, buffer, 7, &buffer[7]);
                if (rc != 0) {
                    return rc;
                }

                /* 0 => All 8 bits are valid */
                tx_last_bits = 0;
                buffer_used = 9;

                /* Store response in the last 3 bytes of buffer (BCC and CRC_A -
                 * not needed after tx) */
                response_buffer = &buffer[6];
                response_length = 3;
            }
            else {
                /* This is an ANTICOLLISION */

                tx_last_bits = current_level_known_bits % 8;

                /* Number of whole bytes in the UID part */
                count = current_level_known_bits / 8;

                /* Number of whole bytes: SEL + NVB + UIDs */
                index = 2 + count;

                /* NVB - Number of Valid Bits */
                buffer[1] = (index << 4) + tx_last_bits;
                buffer_used = index + (tx_last_bits ? 1 : 0);

                /* Store response in the unused part of buffer */
                response_buffer = &buffer[index];
                response_length = sizeof(buffer) - index;
            }

            rx_align = tx_last_bits;
            /* RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0] */
            _device_write(dev, MFRC522_REG_BIT_FRAMING, (rx_align << 4) + tx_last_bits);

            /* Transmit the buffer and receive the response */
            rc = mfrc522_pcd_transceive_data(dev, buffer, buffer_used,
                                             response_buffer, &response_length,
                                             &tx_last_bits, rx_align, false);

            if (rc == -ECONNABORTED) {
                /* More than one PICC in the field => collision */

                uint8_t value_of_coll_reg;
                _device_read(dev, MFRC522_REG_COLL, &value_of_coll_reg);

                if (value_of_coll_reg & MFRC522_BIT_COLL_COLL_POS_NOT_VALID) {
                    /* Without a valid collision position we cannot continue */
                    return -ECONNABORTED;
                }

                /* Values 0-31, 0 means bit 32 */
                uint8_t collision_pos = value_of_coll_reg & 0x1F;
                if (collision_pos == 0) {
                    collision_pos = 32;
                }

                /* No progress - should not happen */
                if (collision_pos <= current_level_known_bits) {
                    return -ECANCELED;
                }

                /* Choose the PICC with the bit set */
                current_level_known_bits = collision_pos;

                /* The bit to modify */
                count = current_level_known_bits % 8;
                check_bit = (current_level_known_bits - 1) % 8;

                /* First byte is index 0 */
                index = 1 + (current_level_known_bits / 8) + (count ? 1 : 0);
                buffer[index] |= (1 << check_bit);
            }
            else if (rc != 0) {
                return rc;
            }
            else {
                /* SUCCESS */

                if (current_level_known_bits >= 32) {
                    /* This was a SELECT */

                    /* No more anticollision */
                    select_done = true;
                    /* We continue below outside the while */
                }
                else {
                    /* This was an ANTICOLLISION */

                    /* We now have all 32 bits of the UID in this Cascade Level */
                    current_level_known_bits = 32;
                    /* Run loop again to do the SELECT */
                }
            }
        }

        /* We do not check the CBB - it was constructed by us above */

        index = (buffer[2] == MFRC522_PICC_CASCADE_TAG) ? 3 : 2;
        bytes_to_copy = (buffer[2] == MFRC522_PICC_CASCADE_TAG) ? 3 : 4;
        for (count = 0; count < bytes_to_copy; count++) {
            uid->uid_byte[uid_index + count] = buffer[index++];
        }

        /* SAK must be exactly 24 bits (1 byte + CRC_A) */
        if (response_length != 3 || tx_last_bits != 0) {
            return -EIO;
        }

        /* Verify CRC_A - do our own calculation and store the control in
         * buffer[2..3] - those bytes are not needed anymore */
        rc = mfrc522_pcd_calculate_crc(dev, response_buffer, 1, &buffer[2]);
        if (rc != 0) {
            return rc;
        }

        if ((buffer[2] != response_buffer[1]) || (buffer[3] != response_buffer[2])) {
            return -EIO;
        }

        /* Cascade bit set - UID not complete yes*/
        if (response_buffer[0] & 0x04) {
            cascade_level++;
        }
        else {
            uid_complete = true;
            uid->sak = response_buffer[0];
        }
    }

    /* Set correct uid->size */
    uid->size = 3 * cascade_level + 1;

    return 0;
}

int mfrc522_picc_halt_a(mfrc522_t *dev)
{
    assert(dev);

    int rc;
    uint8_t buffer[4];

    /* Build command buffer */
    buffer[0] = MFRC522_PICC_CMD_ISO_14443_HLTA;
    buffer[1] = 0;

    /* Calculate CRC_A */
    rc = mfrc522_pcd_calculate_crc(dev, buffer, 2, &buffer[2]);
    if (rc != 0) {
        return rc;
    }

    /* Send the command. The standard says: If the PICC responds with any
     * modulation during a period of 1 ms after the end of the frame containing
     * the HLTA command, this response shall be interpreted as 'not acknowledge'.
     * We interpret that this way: Only -ETIMEDOUT is a success. */
    rc = mfrc522_pcd_transceive_data(dev, buffer, sizeof(buffer), NULL, NULL, NULL, 0, false);
    if (rc == -ETIMEDOUT) {
        return 0;
    }

    /* That is ironically NOT ok in this case ;-) */
    if (rc == 0) {
        return -EIO;
    }

    return rc;
}

int mfrc522_pcd_authenticate(mfrc522_t *dev, mfrc522_picc_command_t command,
                             uint8_t block_addr, const mfrc522_mifare_key_t *key,
                             const mfrc522_uid_t *uid)
{
    assert(dev);

    uint8_t wait_irq = MFRC522_BIT_COM_IRQ_IDLE_IRQ;

    uint8_t send_data[12];
    send_data[0] = command;
    send_data[1] = block_addr;

    /* 6 key bytes */
    for (uint8_t i = 0; i < MFRC522_MF_KEY_SIZE; i++) {
        send_data[2 + i] = key->key_byte[i];
    }

    /* Use the last uid bytes as specified in
     * http://cache.nxp.com/documents/application_note/AN10927.pdf section 3.2.5
     * "MIFARE Classic Authentication". The only missed case is the MF1Sxxxx
     * shortcut activation, but it requires cascade tag (CT) byte, that is not
     * part of uid. */

    for (uint8_t i = 0; i < 4; i++) {
        /* The last 4 bytes of the UID */
        send_data[8 + i] = uid->uid_byte[i + uid->size - 4];
    }

    /* Start the authentication */
    return mfrc522_pcd_communicate_with_picc(
        dev, MFRC522_CMD_MF_AUTHENT, wait_irq,
        &send_data[0], sizeof(send_data), NULL, NULL, NULL, 0, false);
}

void mfrc522_pcd_stop_crypto1(mfrc522_t *dev)
{
    assert(dev);

    mfrc522_pcd_clear_register_bitmask(
        dev, MFRC522_REG_STATUS_2, MFRC522_BIT_STATUS_2_MF_CRYPTO_1_ON);
}

int mfrc522_mifare_read(mfrc522_t *dev, uint8_t block_addr, uint8_t *buffer, uint8_t *buffer_size)
{
    assert(dev);

    int rc;

    /* Sanity check */
    if (buffer == NULL || *buffer_size < 18) {
        return -ENOBUFS;
    }

    /* Build command buffer */
    buffer[0] = MFRC522_PICC_CMD_MF_READ;
    buffer[1] = block_addr;

    /* Calculate CRC_A */
    rc = mfrc522_pcd_calculate_crc(dev, buffer, 2, &buffer[2]);
    if (rc != 0) {
        return rc;
    }

    /* Transmit the buffer and receive the response, validate CRC_A */
    return mfrc522_pcd_transceive_data(
        dev, buffer, 4, buffer, buffer_size, NULL, 0, true);
}

int mfrc522_mifare_write(mfrc522_t *dev, uint8_t block_addr,
                         const uint8_t *buffer, uint8_t buffer_size)
{
    assert(dev);

    int rc;

    /* Sanity check */
    if (buffer == NULL || buffer_size < 16) {
        return -EINVAL;
    }

    /* Mifare Classic protocol requires two communications to perform a write.
     * Step 1: Tell the PICC we want to write to block block_addr */
    uint8_t cmd_buffer[2];
    cmd_buffer[0] = MFRC522_PICC_CMD_MF_WRITE;
    cmd_buffer[1] = block_addr;

    /* Adds CRC_A and checks that the response is MFRC522_MF_ACK */
    rc = mfrc522_pcd_mifare_transceive(dev, cmd_buffer, 2, false);
    if (rc != 0) {
        return rc;
    }

    /* Step 2: Transfer the data */
    /* Adds CRC_A and checks that the response is MFRC522_MF_ACK */
    rc = mfrc522_pcd_mifare_transceive(dev, buffer, buffer_size, false);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

int mfrc522_mifare_ultralight_write(mfrc522_t *dev, uint8_t page, const uint8_t *buffer)
{
    assert(dev);

    int rc;

    /* Sanity check */
    if (buffer == NULL) {
        return -EINVAL;
    }

    /* Build command buffer */
    uint8_t cmd_buffer[6];
    cmd_buffer[0] = MFRC522_PICC_CMD_MF_UL_WRITE;
    cmd_buffer[1] = page;
    memcpy(&cmd_buffer[2], buffer, 4);

    /* Perform the write */
    /* Adds CRC_A and checks that the response is MFRC522_MF_ACK */
    rc = mfrc522_pcd_mifare_transceive(dev, cmd_buffer, 6, false);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

int mfrc522_mifare_decrement(mfrc522_t *dev, uint8_t block_addr, int32_t delta)
{
    assert(dev);

    int rc = _mifare_two_step_helper(dev, MFRC522_PICC_CMD_MF_DECREMENT, block_addr, delta);

    if (rc == -EINVAL) {
        return -ECANCELED;
    }

    return rc;
}

int mfrc522_mifare_increment(mfrc522_t *dev, uint8_t block_addr, int32_t delta)
{
    assert(dev);

    int rc = _mifare_two_step_helper(dev, MFRC522_PICC_CMD_MF_INCREMENT, block_addr, delta);

    if (rc == -EINVAL) {
        return -ECANCELED;
    }

    return rc;
}

int mfrc522_mifare_restore(mfrc522_t *dev, uint8_t block_addr)
{
    assert(dev);

    /* The datasheet describes Restore as a two step operation, but does not
     * explain what data to transfer in step 2. Doing only a single step does
     * not work, so transfer 0L in step two. */
    int rc = _mifare_two_step_helper(dev, MFRC522_PICC_CMD_MF_RESTORE, block_addr, 0L);

    if (rc == -EINVAL) {
        return -ECANCELED;
    }

    return rc;
}

int mfrc522_mifare_transfer(mfrc522_t *dev, uint8_t block_addr)
{
    assert(dev);

    int rc;

    /* We only need room for 2 bytes */
    uint8_t cmd_buffer[2];

    /* Tell the PICC we want to transfer the result into block block_addr */
    cmd_buffer[0] = MFRC522_PICC_CMD_MF_TRANSFER;
    cmd_buffer[1] = block_addr;

    /* Adds CRC_A and checks that the response is MFRC522_MF_ACK */
    rc = mfrc522_pcd_mifare_transceive(dev, cmd_buffer, 2, false);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

int mfrc522_mifare_get_value(mfrc522_t *dev, uint8_t block_addr, int32_t *value)
{
    assert(dev);

    int rc;
    uint8_t buffer[18];
    uint8_t size = sizeof(buffer);

    rc = mfrc522_mifare_read(dev, block_addr, buffer, &size);

    if (rc == 0) {
        /* Extract the value */
        *value = (((int32_t)buffer[3]) << 24)
               | (((int32_t)buffer[2]) << 16)
               | (((int32_t)buffer[1]) << 8)
               | ((int32_t)buffer[0]);
    }

    return rc;
}

int mfrc522_mifare_set_value(mfrc522_t *dev, uint8_t block_addr, int32_t value)
{
    assert(dev);

    uint8_t buffer[18];

    /* Translate the int32_t into 4 bytes; repeated 2x in value block */
    buffer[0] = buffer[ 8] = (value & 0xFF);
    buffer[1] = buffer[ 9] = (value & 0xFF00) >> 8;
    buffer[2] = buffer[10] = (value & 0xFF0000) >> 16;
    buffer[3] = buffer[11] = (value & 0xFF000000) >> 24;

    /* Inverse 4 bytes also found in value block */
    buffer[4] = ~buffer[0];
    buffer[5] = ~buffer[1];
    buffer[6] = ~buffer[2];
    buffer[7] = ~buffer[3];

    /* Address 2x with inverse address 2x */
    buffer[12] = buffer[14] = block_addr;
    buffer[13] = buffer[15] = ~block_addr;

    /* Write the whole data block */
    return mfrc522_mifare_write(dev, block_addr, buffer, 16);
}

int mfrc522_pcd_ntag216_auth(mfrc522_t *dev, const uint8_t *password, uint8_t p_ack[])
{
    assert(dev);

    /* TODO: Fix cmd_buffer length and rx_len. They really should match.
     * (Better still, rx_len should not even be necessary.) */

    int rc;

    /* We need room for 16 bytes data and 2 bytes CRC_A */
    uint8_t cmd_buffer[18];

    /* Command for authentication */
    cmd_buffer[0] = 0x1B;

    for (uint8_t i = 0; i < 4; i++) {
        cmd_buffer[i + 1] = password[i];
    }

    rc = mfrc522_pcd_calculate_crc(dev, cmd_buffer, 5, &cmd_buffer[5]);
    if (rc != 0) {
        return rc;
    }

    /* Transceive the data, store the reply in cmd_buffer[] */
    /* RxIRq and IdleIRq */
    uint8_t wait_irq = 0x30;
    /* uint8_t cmd_buffer_size = sizeof(cmd_buffer); */
    uint8_t valid_bits = 0;
    uint8_t rx_len = 5;

    rc = mfrc522_pcd_communicate_with_picc(dev, MFRC522_CMD_TRANSCEIVE, wait_irq, cmd_buffer, 7,
                                           cmd_buffer, &rx_len, &valid_bits, 0, false);

    p_ack[0] = cmd_buffer[0];
    p_ack[1] = cmd_buffer[1];

    if (rc != 0) {
        return rc;
    }

    return 0;
}

int mfrc522_pcd_mifare_transceive(mfrc522_t *dev,
                                  const uint8_t *send_data, uint8_t send_len,
                                  bool accept_timeout)
{
    assert(dev);

    int rc;

    /* We need room for 16 bytes data and 2 bytes CRC_A */
    uint8_t cmd_buffer[18];

    /* Sanity check */
    if (send_data == NULL || send_len > 16) {
        return -EINVAL;
    }

    /* Copy send_data[] to cmd_buffer[] and add CRC_A */
    memcpy(cmd_buffer, send_data, send_len);

    rc = mfrc522_pcd_calculate_crc(dev, cmd_buffer, send_len, &cmd_buffer[send_len]);
    if (rc != 0) {
        return rc;
    }

    send_len += 2;

    /* Transceive the data, store the reply in cmd_buffer[] */
    uint8_t wait_irq = MFRC522_BIT_COM_IRQ_RX_IRQ | MFRC522_BIT_COM_IRQ_IDLE_IRQ;
    uint8_t cmd_buffer_size = sizeof(cmd_buffer);
    uint8_t valid_bits = 0;

    rc = mfrc522_pcd_communicate_with_picc(
        dev, MFRC522_CMD_TRANSCEIVE, wait_irq, cmd_buffer, send_len,
        cmd_buffer, &cmd_buffer_size, &valid_bits, 0, false);

    if (accept_timeout && rc == -ETIMEDOUT) {
        return 0;
    }

    if (rc != 0) {
        return rc;
    }

    /* The PICC must reply with a 4 bit ACK */
    if (cmd_buffer_size != 1 || valid_bits != 4) {
        return -EIO;
    }

    if (cmd_buffer[0] != MFRC522_MF_ACK) {
        return -EIO;
    }

    return 0;
}

mfrc522_picc_type_t mfrc522_picc_get_type(uint8_t sak)
{
    /* http://www.nxp.com/documents/application_note/AN10833.pdf 3.2 Coding of
     * Select Acknowledge (SAK): ignore 8-bit (iso14443 starts with LSBit = bit 1)
     * fixes wrong type for manufacturer Infineon
     * (http://nfc-tools.org/index.php?title=ISO14443A) */

    sak &= 0x7F;
    switch (sak) {
    case 0x04:
        /* UID not complete */
        return MFRC522_PICC_TYPE_NOT_COMPLETE;

    case 0x09:
        return MFRC522_PICC_TYPE_MIFARE_MINI;

    case 0x08:
        return MFRC522_PICC_TYPE_MIFARE_1K;

    case 0x18:
        return MFRC522_PICC_TYPE_MIFARE_4K;

    case 0x00:
        return MFRC522_PICC_TYPE_MIFARE_UL;

    case 0x10:
    /* fall through */
    case 0x11:
        return MFRC522_PICC_TYPE_MIFARE_PLUS;

    case 0x01:
        return MFRC522_PICC_TYPE_TNP3XXX;

    case 0x20:
        return MFRC522_PICC_TYPE_ISO_14443_4;

    case 0x40:
        return MFRC522_PICC_TYPE_ISO_18092;

    default:
        return MFRC522_PICC_TYPE_UNKNOWN;
    }
}

const char *mfrc522_picc_get_type_string(mfrc522_picc_type_t picc_type)
{
    switch (picc_type) {
    case MFRC522_PICC_TYPE_ISO_14443_4:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_ISO_14443_4];

    case MFRC522_PICC_TYPE_ISO_18092:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_ISO_18092];

    case MFRC522_PICC_TYPE_MIFARE_MINI:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_MIFARE_MINI];

    case MFRC522_PICC_TYPE_MIFARE_1K:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_MIFARE_1K];

    case MFRC522_PICC_TYPE_MIFARE_4K:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_MIFARE_4K];

    case MFRC522_PICC_TYPE_MIFARE_UL:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_MIFARE_UL];

    case MFRC522_PICC_TYPE_MIFARE_PLUS:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_MIFARE_PLUS];

    case MFRC522_PICC_TYPE_MIFARE_DESFIRE:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_MIFARE_DESFIRE];

    case MFRC522_PICC_TYPE_TNP3XXX:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_TNP3XXX];

    case MFRC522_PICC_TYPE_NOT_COMPLETE:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_NOT_COMPLETE];

    case MFRC522_PICC_TYPE_UNKNOWN:
    /* fall through */
    default:
        return mfrc522_picc_type_names[MFRC522_PICC_TYPE_UNKNOWN];
    }
}

void mfrc522_mifare_set_access_bits(uint8_t *access_bit_buffer,
                                    uint8_t g0, uint8_t g1, uint8_t g2, uint8_t g3)
{
    uint8_t c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
    uint8_t c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
    uint8_t c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);

    access_bit_buffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
    access_bit_buffer[1] =          c1 << 4 | (~c3 & 0xF);
    access_bit_buffer[2] =          c3 << 4 | c2;
}

int mfrc522_mifare_open_uid_backdoor(mfrc522_t *dev)
{
    assert(dev);

    /* Magic sequence:
     * > 50 00 57 CD (HALT + CRC)
     * > 40 (7 bits only)
     * < A (4 bits only)
     * > 43
     * < A (4 bits only)
     * Then you can write to sector 0 without authenticating */

    /* 50 00 57 CD */
    mfrc522_picc_halt_a(dev);

    uint8_t cmd = 0x40;

    /* Our command is only 7 bits. After receiving card response,
     * this will contain amount of valid response bits. */
    uint8_t valid_bits = 7;

    /* Card's response is written here */
    uint8_t response[32];
    uint8_t received = sizeof(response);

    /* 40 */
    int rc = mfrc522_pcd_transceive_data(dev, &cmd, 1, response, &received, &valid_bits, 0, false);
    if (rc != 0) {
        return rc;
    }

    if (received != 1 || response[0] != 0x0A) {
        return -EIO;
    }

    cmd = 0x43;
    valid_bits = 8;

    /* 43 */
    rc = mfrc522_pcd_transceive_data(dev, &cmd, 1, response, &received, &valid_bits, 0, false);
    if (rc != 0) {
        return rc;
    }

    if (received != 1 || response[0] != 0x0A) {
        return -EIO;
    }

    /* You can now write to sector 0 without authenticating! */
    return 0;
}

int mfrc522_mifare_set_uid(mfrc522_t *dev, mfrc522_uid_t *uid,
                           const uint8_t *new_uid, uint8_t new_uid_size)
{
    assert(dev);

    /* UID + BCC byte can not be larger than 15 together */
    if (!new_uid || !new_uid_size || new_uid_size > 15) {
        return -EINVAL;
    }

    /* Authenticate for reading */
    mfrc522_mifare_key_t key = { .key_byte = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };

    int rc = mfrc522_pcd_authenticate(dev, MFRC522_PICC_CMD_MF_AUTH_KEY_A, 1, &key, uid);

    if (rc != 0) {

        if (rc == -ETIMEDOUT) {
            /* We get a read timeout if no card is selected yet, so let's select one */

            /* Wake the card up again if sleeping
             * uint8_t atqa_answer[2];
             * uint8_t atqa_size = 2;
             * mfrc522_picc_wakeup_a(atqa_answer, &atqa_size); */

            if (!mfrc522_picc_is_new_card_present(dev)) {
                return -EIO;
            }

            rc = mfrc522_picc_read_card_serial(dev, uid);
            if (rc != 0) {
                return rc;
            }

            rc = mfrc522_pcd_authenticate(dev, MFRC522_PICC_CMD_MF_AUTH_KEY_A, 1, &key, uid);

            if (rc != 0) {
                /* We tried, time to give up */
                return rc;
            }
        }
        else {
            return rc;
        }
    }

    /* Read block 0 */
    uint8_t block0_buffer[18];
    uint8_t byte_count = sizeof(block0_buffer);

    rc = mfrc522_mifare_read(dev, 0, block0_buffer, &byte_count);
    if (rc != 0) {
        return rc;
    }

    /* Write new UID to the data we just read, and calculate BCC byte */
    uint8_t bcc = 0;
    for (uint8_t i = 0; i < new_uid_size; i++) {
        block0_buffer[i] = new_uid[i];
        bcc ^= new_uid[i];
    }

    block0_buffer[new_uid_size] = bcc;

    /* Stop encrypted traffic so we can send raw bytes */
    mfrc522_pcd_stop_crypto1(dev);

    /* Activate UID backdoor */
    rc = mfrc522_mifare_open_uid_backdoor(dev);
    if (rc != 0) {
        return rc;
    }

    /* Write modified block 0 back to card */
    rc = mfrc522_mifare_write(dev, 0, block0_buffer, 16);
    if (rc != 0) {
        return rc;
    }

    /* Wake the card up again */
    uint8_t atqa_answer[2];
    uint8_t atqa_size = 2;
    mfrc522_picc_wakeup_a(dev, atqa_answer, &atqa_size);

    return 0;
}

int mfrc522_mifare_unbrick_uid_sector(mfrc522_t *dev)
{
    assert(dev);

    int rc = mfrc522_mifare_open_uid_backdoor(dev);
    if (rc != 0) {
        return rc;
    }

    uint8_t block0_buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x04, 0x08, 0x04, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    /* Write modified block 0 back to card */
    return mfrc522_mifare_write(dev, (uint8_t)0, block0_buffer, (uint8_t)16);
}

bool mfrc522_picc_is_new_card_present(mfrc522_t *dev)
{
    assert(dev);

    uint8_t buffer_ATQA[2];
    uint8_t buffer_size = sizeof(buffer_ATQA);

    /* Reset baud rates */
    _device_write(dev, MFRC522_REG_TX_MODE, 0x00);
    _device_write(dev, MFRC522_REG_RX_MODE, 0x00);

    /* Reset ModWidthReg */
    _device_write(dev, MFRC522_REG_MOD_WIDTH, 0x26);

    int rc = mfrc522_picc_request_a(dev, buffer_ATQA, &buffer_size);

    return (rc == 0 || rc == -ECONNABORTED);
}

int mfrc522_picc_read_card_serial(mfrc522_t *dev, mfrc522_uid_t *uid)
{
    assert(dev);

    return mfrc522_picc_select(dev, uid, 0);
}

void mfrc522_pcd_dump_version_to_serial(mfrc522_t *dev)
{
    assert(dev);

    /* Get the MFRC522 firmware version */
    uint8_t version;
    _device_read(dev, MFRC522_REG_VERSION, &version);
    printf("0x%x", version);

    /* Lookup which version */
    switch (version) {
    case 0x88:
        printf(" = (clone)");
        break;
    case 0x90:
        printf(" = v0.0");
        break;
    case 0x91:
        printf(" = v1.0");
        break;
    case 0x92:
        printf(" = v2.0");
        break;
    case 0x12:
        printf(" = counterfeit chip");
        break;
    default:
        printf(" = (unknown)");
        break;
    }

    /* When 0x00 or 0xFF is returned, communication probably failed */
    if ((version == 0x00) || (version == 0xFF)) {
        printf("WARNING: Communication failure, is the MFRC522 properly connected?");
    }

    printf("\n");
}

void mfrc522_picc_dump_to_serial(mfrc522_t *dev, mfrc522_uid_t *uid)
{
    assert(dev);

    mfrc522_mifare_key_t key;

    /* Dump UID, SAK and Type */
    mfrc522_picc_dump_details_to_serial(uid);

    /* Dump contents */
    mfrc522_picc_type_t picc_type = mfrc522_picc_get_type(uid->sak);

    switch (picc_type) {
    case MFRC522_PICC_TYPE_MIFARE_MINI:
    /* fall through */
    case MFRC522_PICC_TYPE_MIFARE_1K:
    /* fall through */
    case MFRC522_PICC_TYPE_MIFARE_4K:
        /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
        for (uint8_t i = 0; i < 6; i++) {
            key.key_byte[i] = 0xFF;
        }
        mfrc522_picc_dump_mifare_classic_to_serial(dev, uid, picc_type, &key);
        break;

    case MFRC522_PICC_TYPE_MIFARE_UL:
        mfrc522_picc_dump_mifare_ultralight_to_serial(dev);
        break;

    case MFRC522_PICC_TYPE_ISO_14443_4:
    /* fall through */
    case MFRC522_PICC_TYPE_MIFARE_DESFIRE:
    /* fall through */
    case MFRC522_PICC_TYPE_ISO_18092:
    /* fall through */
    case MFRC522_PICC_TYPE_MIFARE_PLUS:
    /* fall through */
    case MFRC522_PICC_TYPE_TNP3XXX:
        printf("Dumping memory contents not implemented for that PICC type.");
        break;

    case MFRC522_PICC_TYPE_UNKNOWN:
    /* fall through */
    case MFRC522_PICC_TYPE_NOT_COMPLETE:
    /* fall through */
    default:
        /* No memory dump here */
        break;
    }

    printf("\n");

    /* Already done if it was a MIFARE Classic PICC */
    mfrc522_picc_halt_a(dev);
}

void mfrc522_picc_dump_details_to_serial(mfrc522_uid_t *uid)
{
    printf("Card UID:");
    for (uint8_t i = 0; i < uid->size; i++) {
        if (uid->uid_byte[i] < 0x10) {
            printf(" 0");
        }
        else {
            printf(" ");
        }
        printf("%x", uid->uid_byte[i]);
    }
    printf("\n");

    printf("Card SAK: ");
    if (uid->sak < 0x10) {
        printf("0");
    }
    printf("%x\n", uid->sak);

    /* (suggested) PICC type */
    mfrc522_picc_type_t picc_type = mfrc522_picc_get_type(uid->sak);
    printf("PICC type: %s\n", mfrc522_picc_get_type_string(picc_type));
}

void mfrc522_picc_dump_mifare_classic_to_serial(mfrc522_t *dev, mfrc522_uid_t *uid,
                                                mfrc522_picc_type_t picc_type,
                                                mfrc522_mifare_key_t *key)
{
    assert(dev);

    uint8_t no_of_sectors = 0;

    switch (picc_type) {
    case MFRC522_PICC_TYPE_MIFARE_MINI:
        /* Has 5 sectors * 4 blocks/sector * 16 bytes/block = 320 bytes */
        no_of_sectors = 5;
        break;

    case MFRC522_PICC_TYPE_MIFARE_1K:
        /* Has 16 sectors * 4 blocks/sector * 16 bytes/block = 1024 bytes */
        no_of_sectors = 16;
        break;

    case MFRC522_PICC_TYPE_MIFARE_4K:
        /* Has (32 sectors * 4 blocks/sector + 8 sectors * 16 blocks/sector) * 16 bytes/block = 4096 bytes */
        no_of_sectors = 40;
        break;

    default:
        /* Should not happen. Ignore */
        break;
    }

    /* Dump sectors, highest address first */
    if (no_of_sectors) {
        printf("Sector Block   0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15  AccessBits\n");
        for (int8_t i = no_of_sectors - 1; i >= 0; i--) {
            mfrc522_picc_dump_mifare_classic_sector_to_serial(dev, uid, key, i);
        }
    }

    /* Halt the PICC before stopping the encrypted session */
    mfrc522_picc_halt_a(dev);
    mfrc522_pcd_stop_crypto1(dev);
}

void mfrc522_picc_dump_mifare_classic_sector_to_serial(mfrc522_t *dev, mfrc522_uid_t *uid,
                                                       mfrc522_mifare_key_t *key, uint8_t sector)
{
    assert(dev);

    int rc;

    /* Address of lowest address to dump actually last block dumped */
    uint8_t first_block;
    /* Number of blocks in sector */
    uint8_t no_of_blocks;
    /* Set to true while handling the "last" (i.e. highest address) in the sector */
    bool is_sector_trailer;

    /* The access bits are stored in a peculiar fashion.
     * There are four groups:
     * g[3] Access bits for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
     * g[2] Access bits for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
     * g[1] Access bits for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
     * g[0] Access bits for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
     * Each group has access bits [C1 C2 C3]. In this code C1 is MSB and C3 is LSB.
     * The four CX bits are stored together in a nible cx and an inverted nible cx_ */

    /* Nibbles */
    uint8_t c1, c2, c3;

    /* Inverted nibbles */
    uint8_t c1_, c2_, c3_;

    /* True if one of the inverted nibbles did not match */
    bool inverted_error = false;

    /* Access bits for each of the four groups */
    uint8_t g[4];

    /* 0-3 - active group for access bits */
    uint8_t group;

    /* True for the first block dumped in the group */
    bool first_in_group;

    /* Determine position and size of sector */
    if (sector < 32) {
        /* Sectors 0..31 has 4 blocks each */
        no_of_blocks = 4;
        first_block = sector * no_of_blocks;
    }
    else if (sector < 40) {
        /* Sectors 32-39 has 16 blocks each */
        no_of_blocks = 16;
        first_block = 128 + (sector - 32) * no_of_blocks;
    }
    else {
        /* Illegal input, no MIFARE Classic PICC has more than 40 sectors */
        return;
    }

    /* Dump blocks, highest address first */
    uint8_t byte_count;
    uint8_t buffer[18];
    is_sector_trailer = true;

    for (int8_t blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--) {
        uint8_t block_addr = first_block + blockOffset;

        /* Sector number - only on first line */
        if (is_sector_trailer) {
            if (sector < 10) {
                /* Pad with spaces */
                printf("   ");
            }
            else {
                /* Pad with spaces */
                printf("  ");
            }
            printf("%d", sector);
            /* Pad with spaces */
            printf("   ");
        }
        else {
            /* Pad with spaces */
            printf("       ");
        }

        /* Block number */
        if (block_addr < 10) {
            /* Pad with spaces */
            printf("   ");
        }
        else {
            if (block_addr < 100) {
                /* Pad with spaces */
                printf("  ");
            }
            else {
                /* Pad with spaces */
                printf(" ");
            }
        }
        printf("%d", block_addr);
        printf("  ");

        /* Establish encrypted communications before reading the first block */
        if (is_sector_trailer) {
            rc = mfrc522_pcd_authenticate(dev, MFRC522_PICC_CMD_MF_AUTH_KEY_A, first_block, key, uid);
            if (rc != 0) {
                printf("mfrc522_pcd_authenticate() failed: %d\n", rc);
                return;
            }
        }

        /* Read block */
        byte_count = sizeof(buffer);

        rc = mfrc522_mifare_read(dev, block_addr, buffer, &byte_count);
        if (rc != 0) {
            printf("mfrc522_mifare_read() failed: %d\n", rc);
            continue;
        }

        /* Dump data */
        for (uint8_t index = 0; index < 16; index++) {

            if (buffer[index] < 0x10) {
                printf(" 0");
            }
            else {
                printf(" ");
            }

            printf("%x", buffer[index]);
            if ((index % 4) == 3) {
                printf(" ");
            }
        }

        /* Parse sector trailer data */
        if (is_sector_trailer) {
            c1  = buffer[7] >> 4;
            c2  = buffer[8] & 0xF;
            c3  = buffer[8] >> 4;
            c1_ = buffer[6] & 0xF;
            c2_ = buffer[6] >> 4;
            c3_ = buffer[7] & 0xF;
            inverted_error = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
            g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
            g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
            g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
            g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
            is_sector_trailer = false;
        }

        /* Which access group is this block in? */
        if (no_of_blocks == 4) {
            group = blockOffset;
            first_in_group = true;
        }
        else {
            group = blockOffset / 5;
            first_in_group = (group == 3) || (group != (blockOffset + 1) / 5);
        }

        if (first_in_group) {
            /* Print access bits */
            printf(" [ ");
            printf("%d", (g[group] >> 2) & 1);
            printf(" ");
            printf("%d", (g[group] >> 1) & 1);
            printf(" ");
            printf("%d", (g[group] >> 0) & 1);
            printf(" ] ");
            if (inverted_error) {
                printf(" Inverted access bits did not match! ");
            }
        }

        /* Not a sector trailer, a value block */
        if (group != 3 && (g[group] == 1 || g[group] == 6)) {
            uint32_t value = (((uint32_t)buffer[3]) << 24)
                           | (((uint32_t)buffer[2]) << 16)
                           | (((uint32_t)buffer[1]) << 8)
                           | ((uint32_t)buffer[0]);
            printf(" Value=0x");
            printf("%lx", ((unsigned long)value));
            printf(" Addr=0x");
            printf("%x", buffer[12]);
        }
        printf("\n");
    }

    return;
}

void mfrc522_picc_dump_mifare_ultralight_to_serial(mfrc522_t *dev)
{
    assert(dev);

    int rc;
    uint8_t byte_count;
    uint8_t buffer[18];
    uint8_t i;

    printf("Page  0  1  2  3\n");
    /* Try the mpages of the original Ultralight. Ultralight C has more pages. */
    /* Read returns data for 4 pages at a time */
    for (uint8_t page = 0; page < 16; page += 4) {
        /* Read pages */
        byte_count = sizeof(buffer);

        rc = mfrc522_mifare_read(dev, page, buffer, &byte_count);
        if (rc != 0) {
            printf("mfrc522_mifare_read() failed: %d\n", rc);
            break;
        }

        /* Dump data */
        for (uint8_t offset = 0; offset < 4; offset++) {
            i = page + offset;
            if (i < 10) {
                /* Pad with spaces */
                printf("  ");
            }
            else {
                /* Pad with spaces */
                printf(" ");
            }

            printf("%d", i);
            printf("  ");
            for (uint8_t index = 0; index < 4; index++) {
                i = 4 * offset + index;

                if (buffer[i] < 0x10) {
                    printf(" 0");
                }
                else {
                    printf(" ");
                }

                printf("%x", buffer[i]);
            }
            printf("\n");
        }
    }
}

/* Firmware data for self-test
 * Reference values based on firmware version */

/* Version 0.0 (0x90)
 * Philips Semiconductors; Preliminary Specification Revision 2.0 - 01 August 2005; 16.1 self-test
 */
const uint8_t MFRC522_FIRMWARE_REFERENCEV0_0[] = {
    0x00, 0x87, 0x98, 0x0f, 0x49, 0xFF, 0x07, 0x19,
    0xBF, 0x22, 0x30, 0x49, 0x59, 0x63, 0xAD, 0xCA,
    0x7F, 0xE3, 0x4E, 0x03, 0x5C, 0x4E, 0x49, 0x50,
    0x47, 0x9A, 0x37, 0x61, 0xE7, 0xE2, 0xC6, 0x2E,
    0x75, 0x5A, 0xED, 0x04, 0x3D, 0x02, 0x4B, 0x78,
    0x32, 0xFF, 0x58, 0x3B, 0x7C, 0xE9, 0x00, 0x94,
    0xB4, 0x4A, 0x59, 0x5B, 0xFD, 0xC9, 0x29, 0xDF,
    0x35, 0x96, 0x98, 0x9E, 0x4F, 0x30, 0x32, 0x8D
};

/* Version 1.0 (0x91)
 * NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
 */
const uint8_t MFRC522_FIRMWARE_REFERENCEV1_0[] = {
    0x00, 0xC6, 0x37, 0xD5, 0x32, 0xB7, 0x57, 0x5C,
    0xC2, 0xD8, 0x7C, 0x4D, 0xD9, 0x70, 0xC7, 0x73,
    0x10, 0xE6, 0xD2, 0xAA, 0x5E, 0xA1, 0x3E, 0x5A,
    0x14, 0xAF, 0x30, 0x61, 0xC9, 0x70, 0xDB, 0x2E,
    0x64, 0x22, 0x72, 0xB5, 0xBD, 0x65, 0xF4, 0xEC,
    0x22, 0xBC, 0xD3, 0x72, 0x35, 0xCD, 0xAA, 0x41,
    0x1F, 0xA7, 0xF3, 0x53, 0x14, 0xDE, 0x7E, 0x02,
    0xD9, 0x0F, 0xB5, 0x5E, 0x25, 0x1D, 0x29, 0x79
};

/* Version 2.0 (0x92)
 * NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
 */
const uint8_t MFRC522_FIRMWARE_REFERENCEV2_0[] = {
    0x00, 0xEB, 0x66, 0xBA, 0x57, 0xBF, 0x23, 0x95,
    0xD0, 0xE3, 0x0D, 0x3D, 0x27, 0x89, 0x5C, 0xDE,
    0x9D, 0x3B, 0xA7, 0x00, 0x21, 0x5B, 0x89, 0x82,
    0x51, 0x3A, 0xEB, 0x02, 0x0C, 0xA5, 0x00, 0x49,
    0x7C, 0x84, 0x4D, 0xB3, 0xCC, 0xD2, 0x1B, 0x81,
    0x5D, 0x48, 0x76, 0xD5, 0x71, 0x61, 0x21, 0xA9,
    0x86, 0x96, 0x83, 0x38, 0xCF, 0x9D, 0x5B, 0x6D,
    0xDC, 0x15, 0xBA, 0x3E, 0x7D, 0x95, 0x3B, 0x2F
};

/* Clone
 * Fudan Semiconductor FM17522 (0x88)
 */
const uint8_t MFRC522_FM17522_FIRMWARE_REFERENCE[] = {
    0x00, 0xD6, 0x78, 0x8C, 0xE2, 0xAA, 0x0C, 0x18,
    0x2A, 0xB8, 0x7A, 0x7F, 0xD3, 0x6A, 0xCF, 0x0B,
    0xB1, 0x37, 0x63, 0x4B, 0x69, 0xAE, 0x91, 0xC7,
    0xC3, 0x97, 0xAE, 0x77, 0xF4, 0x37, 0xD7, 0x9B,
    0x7C, 0xF5, 0x3C, 0x11, 0x8F, 0x15, 0xC3, 0xD7,
    0xC1, 0x5B, 0x00, 0x2A, 0xD0, 0x75, 0xDE, 0x9E,
    0x51, 0x64, 0xAB, 0x3E, 0xE9, 0x15, 0xB5, 0xAB,
    0x56, 0x9A, 0x98, 0x82, 0x26, 0xEA, 0x2A, 0x62
};

bool mfrc522_pcd_perform_self_test(mfrc522_t *dev)
{
    assert(dev);

    /* This follows directly the steps outlined in 16.1.1 */
    /* 1. Perform a soft reset */
    mfrc522_pcd_reset(dev);

    /* 2. Clear the internal buffer by writing 25 bytes of 00h */
    uint8_t ZEROES[25] = { 0 };

    /* flush the FIFO buffer */
    _device_write(dev, MFRC522_REG_FIFO_LEVEL, 0x80);

    /* write 25 bytes of 00h to FIFO */
    _device_write_n(dev, MFRC522_REG_FIFO_DATA, 25, ZEROES);

    /* transfer to internal buffer */
    _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_MEM);

    /*  3. Enable self-test */
    _device_write(dev, MFRC522_REG_AUTO_TEST, 0x09);

    /* 4. Write 00h to FIFO buffer */
    _device_write(dev, MFRC522_REG_FIFO_DATA, 0x00);

    /* 5. Start self-test by issuing the CalcCRC command */
    _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_CALC_CRC);

    /* 6. Wait for self-test to complete */
    for (uint8_t i = 0; i < 0xFF; i++) {
        /* The datasheet does not specify exact completion condition except
         * that FIFO buffer should contain 64 bytes. While selftest is initiated
         * by CalcCRC command it behaves differently from normal CRC computation,
         * so one can't reliably use DivIrqReg to check for completion. It is
         * reported that some devices does not trigger CRCIRq flag during selftest. */

        uint8_t n;
        _device_read(dev, MFRC522_REG_FIFO_LEVEL, &n);
        if (n >= 64) {
            break;
        }
    }

    /* Stop calculating CRC for new content in the FIFO */
    _device_write(dev, MFRC522_REG_COMMAND, MFRC522_CMD_IDLE);

    /* 7. Read out resulting 64 bytes from the FIFO buffer */
    uint8_t result[64];
    _device_read_n(dev, MFRC522_REG_FIFO_DATA, 64, result, 0);

    /* Auto self-test done */
    /* Reset AutoTestReg register to be 0 again. Required for normal operation. */
    _device_write(dev, MFRC522_REG_AUTO_TEST, 0x00);

    /* Determine firmware version (see section 9.3.4.8 in spec) */
    uint8_t version;
    _device_read(dev, MFRC522_REG_VERSION, &version);

    /* Pick the appropriate reference values */
    const uint8_t *reference;
    switch (version) {
    /* Fudan Semiconductor FM17522 clone */
    case 0x88:
        reference = MFRC522_FM17522_FIRMWARE_REFERENCE;
        break;
    /* Version 0.0 */
    case 0x90:
        reference = MFRC522_FIRMWARE_REFERENCEV0_0;
        break;
    /* Version 1.0 */
    case 0x91:
        reference = MFRC522_FIRMWARE_REFERENCEV1_0;
        break;
    /* Version 2.0 */
    case 0x92:
        reference = MFRC522_FIRMWARE_REFERENCEV2_0;
        break;
    /* Unknown version */
    default:
        /* abort test */
        return false;
    }

    /* Verify that the results match up to our expectations */
    for (uint8_t i = 0; i < 64; i++) {
        if (result[i] != reference[i]) {
            return false;
        }
    }

    /* Test passed; all is good */
    return true;
}
