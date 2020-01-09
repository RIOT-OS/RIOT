/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_nrf24l01p
 * @{
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marc Poulhiès <dkm@kataplop.net>
 * @}
 */
#include "nrf24l01p.h"
#include "nrf24l01p_settings.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "thread.h"
#include "msg.h"


#define ENABLE_DEBUG (0)
#include "debug.h"

#define DELAY_CS_TOGGLE_TICKS       (xtimer_ticks_from_usec(DELAY_CS_TOGGLE_US))
#define DELAY_AFTER_FUNC_TICKS      (xtimer_ticks_from_usec(DELAY_AFTER_FUNC_US))
#define DELAY_CHANGE_TXRX_TICKS     (xtimer_ticks_from_usec(DELAY_CHANGE_TXRX_US))

#define SPI_MODE            SPI_MODE_0
#define SPI_CLK             SPI_CLK_400KHZ

int nrf24l01p_read_reg(const nrf24l01p_t *dev, char reg, char *answer)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    *answer = (char)spi_transfer_reg(dev->spi, dev->cs,
                                     (CMD_R_REGISTER | (REGISTER_MASK & reg)),
                                     CMD_NOOP);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);
    return 0;
}

int nrf24l01p_write_reg(const nrf24l01p_t *dev, char reg, char write)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_reg(dev->spi, dev->cs,
                     (CMD_W_REGISTER | (REGISTER_MASK & reg)), (uint8_t)write);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);
    return 0;
}


int nrf24l01p_init(nrf24l01p_t *dev, spi_t spi, gpio_t ce, gpio_t cs, gpio_t irq)
{
    int status;
    static const char INITIAL_TX_ADDRESS[] =  {0xe7, 0xe7, 0xe7, 0xe7, 0xe7,};
    static const char INITIAL_RX_ADDRESS[] =  {0xe7, 0xe7, 0xe7, 0xe7, 0xe7,};

    dev->spi = spi;
    dev->ce = ce;
    dev->cs = cs;
    dev->irq = irq;
    dev->listener = KERNEL_PID_UNDEF;

    /* Init CE pin */
    gpio_init(dev->ce, GPIO_OUT);

    /* Init CS pin */
    spi_init_cs(dev->spi, dev->cs);

    /* Init IRQ pin */
    gpio_init_int(dev->irq, GPIO_IN_PU, GPIO_FALLING, nrf24l01p_rx_cb, dev);

    /* Test the SPI connection */
    if (spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK) != SPI_OK) {
        DEBUG("error: unable to acquire SPI bus with given params\n");
        return -1;
    }
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    /* Flush TX FIFIO */
    status = nrf24l01p_flush_tx_fifo(dev);

    if (status < 0) {
        return status;
    }

    /* Flush RX FIFIO */
    status = nrf24l01p_flush_rx_fifo(dev);

    if (status < 0) {
        return status;
    }

    /* Setup address width */
    status = nrf24l01p_set_address_width(dev, NRF24L01P_AW_5BYTE);

    if (status < 0) {
        return status;
    }

    /* Setup payload width */
    status = nrf24l01p_set_payload_width(dev, NRF24L01P_PIPE0, NRF24L01P_MAX_DATA_LENGTH);

    if (status < 0) {
        return status;
    }

    /* Set RF channel */
    status = nrf24l01p_set_channel(dev, INITIAL_RF_CHANNEL);

    if (status < 0) {
        return status;
    }

    /* Set RF power */
    status = nrf24l01p_set_power(dev, INITIAL_RX_POWER_0dB);

    if (status < 0) {
        return status;
    }

    /* Set RF datarate */
    status = nrf24l01p_set_datarate(dev, NRF24L01P_DR_250KBS);

    if (status < 0) {
        return status;
    }

    /* Set TX Address */
    status = nrf24l01p_set_tx_address(dev, INITIAL_TX_ADDRESS, INITIAL_ADDRESS_WIDTH);

    if (status < 0) {
        return status;
    }

    /* Set RX Address */
    status = nrf24l01p_set_rx_address(dev, NRF24L01P_PIPE0, INITIAL_RX_ADDRESS, INITIAL_ADDRESS_WIDTH);

    if (status < 0) {
        return status;
    }

    /* Reset auto ack for all pipes */
    status = nrf24l01p_disable_all_auto_ack(dev);

    if (status < 0) {
        return status;
    }

    /* Setup Auto ACK and retransmission */
    status = nrf24l01p_setup_auto_ack(dev, NRF24L01P_PIPE0, NRF24L01P_RETR_750US, 15);

    if (status < 0) {
        return status;
    }

    /* Setup CRC */
    status = nrf24l01p_enable_crc(dev, NRF24L01P_CRC_2BYTE);

    if (status < 0) {
        return status;
    }

    /* Reset all interrupt flags */
    status = nrf24l01p_reset_all_interrupts(dev);

    if (status < 0) {
        return status;
    }

    return nrf24l01p_on(dev);
}

int nrf24l01p_on(const nrf24l01p_t *dev)
{
    char read;
    int status;

    nrf24l01p_read_reg(dev, REG_CONFIG, &read);
    status = nrf24l01p_write_reg(dev, REG_CONFIG, (read | PWR_UP));

    xtimer_usleep(DELAY_CHANGE_PWR_MODE_US);

    return status;
}

int nrf24l01p_off(const nrf24l01p_t *dev)
{
    char read;
    int status;

    nrf24l01p_read_reg(dev, REG_CONFIG, &read);
    status = nrf24l01p_write_reg(dev, REG_CONFIG, (read & ~PWR_UP));

    xtimer_usleep(DELAY_CHANGE_PWR_MODE_US);

    return status;
}

void nrf24l01p_transmit(const nrf24l01p_t *dev)
{
    gpio_set(dev->ce);
    xtimer_usleep(DELAY_CE_HIGH_US); /* at least 10 us high */
    gpio_clear(dev->ce);

    xtimer_spin(DELAY_CHANGE_TXRX_TICKS);
}

int nrf24l01p_read_payload(const nrf24l01p_t *dev, char *answer, unsigned int size)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_regs(dev->spi, dev->cs, CMD_R_RX_PAYLOAD, NULL, answer, size);
    xtimer_spin(DELAY_AFTER_FUNC_TICKS);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    return 0;
}

void nrf24l01p_register(nrf24l01p_t *dev, unsigned int *pid)
{
    dev->listener = *pid;
}

int nrf24l01p_unregister(nrf24l01p_t *dev, unsigned int pid)
{
    if (dev != NULL && dev->listener == pid) {
        dev->listener = 0;
        return 0;
    }
    else {
        return -1;
    }
}

void nrf24l01p_get_id(const nrf24l01p_t *dev, unsigned int *pid)
{
    *((int *)pid) = dev->listener;
}

void nrf24l01p_start(const nrf24l01p_t *dev)
{
    gpio_set(dev->ce);
    xtimer_usleep(DELAY_CE_START_US);
}

void nrf24l01p_stop(const nrf24l01p_t *dev)
{
    xtimer_spin(DELAY_CS_TOGGLE_TICKS);
    gpio_clear(dev->ce);
}

int nrf24l01p_preload(const nrf24l01p_t *dev, char *data, unsigned int size)
{
    size = (size <= 32) ? size : 32;

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_regs(dev->spi, dev->cs, CMD_W_TX_PAYLOAD, data, NULL, size);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);
    return 0;
}


int nrf24l01p_set_channel(const nrf24l01p_t *dev, uint8_t chan)
{
    if (chan > 125) {
        chan = 125;
    }

    return nrf24l01p_write_reg(dev, REG_RF_CH, chan);
}

int nrf24l01p_set_address_width(const nrf24l01p_t *dev, nrf24l01p_aw_t aw)
{
    char aw_setup;
    nrf24l01p_read_reg(dev, REG_SETUP_AW, &aw_setup);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    switch (aw) {
        case NRF24L01P_AW_3BYTE:
            aw_setup &= ~(3);
            aw_setup |= 1;
            break;

        case NRF24L01P_AW_4BYTE:
            aw_setup &= ~(3);
            aw_setup |= 2;
            break;

        case NRF24L01P_AW_5BYTE:
            aw_setup &= ~(3);
            aw_setup |= 3;
            break;

        default:
            return -1;
    }

    return nrf24l01p_write_reg(dev, REG_SETUP_AW, aw_setup);
}

int nrf24l01p_set_payload_width(const nrf24l01p_t *dev,
                                nrf24l01p_rx_pipe_t pipe, uint8_t width)
{
    char pipe_pw_address;

    switch (pipe) {
        case NRF24L01P_PIPE0:
            pipe_pw_address = REG_RX_PW_P0;
            break;

        case NRF24L01P_PIPE1:
            pipe_pw_address = REG_RX_PW_P1;
            break;

        case NRF24L01P_PIPE2:
            pipe_pw_address = REG_RX_PW_P2;
            break;

        case NRF24L01P_PIPE3:
            pipe_pw_address = REG_RX_PW_P3;
            break;

        case NRF24L01P_PIPE4:
            pipe_pw_address = REG_RX_PW_P4;
            break;

        case NRF24L01P_PIPE5:
            pipe_pw_address = REG_RX_PW_P5;
            break;

        default:
            return -1;
    }

    if (width > 32) {
        width = 32;
    }

    return nrf24l01p_write_reg(dev, pipe_pw_address, width);
}



int nrf24l01p_set_tx_address(const nrf24l01p_t *dev, const char *saddr, unsigned int length)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_regs(dev->spi, dev->cs,
                      (CMD_W_REGISTER | (REGISTER_MASK & REG_TX_ADDR)),
                      saddr, NULL, length);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    return (int)length;
}

int nrf24l01p_set_tx_address_long(const nrf24l01p_t *dev, uint64_t saddr, unsigned int length)
{
    char buf[length];

    if (length <= INITIAL_ADDRESS_WIDTH) {
        for (unsigned int i = 0; i < length; i++) {

            buf[i] = (uint8_t)(saddr >> (((length - 1) - i) * sizeof(uint64_t)));
        }
    }
    else {
        return -1;
    }

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_regs(dev->spi, dev->cs,
                      (CMD_W_REGISTER | (REGISTER_MASK & REG_TX_ADDR)),
                      buf, NULL, length);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);
    return (int)length;
}

uint64_t nrf24l01p_get_tx_address_long(const nrf24l01p_t *dev)
{
    uint64_t saddr_64 = 0;
    char addr_array[INITIAL_ADDRESS_WIDTH];

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_regs(dev->spi, dev->cs,
                      (CMD_R_REGISTER | (REGISTER_MASK & REG_TX_ADDR)),
                      NULL, addr_array, INITIAL_ADDRESS_WIDTH);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    for (int i = 0; i < INITIAL_ADDRESS_WIDTH; i++) {
        saddr_64 |= (((uint64_t) addr_array[i]) << (8 * (INITIAL_ADDRESS_WIDTH - i - 1)));
    }

    return saddr_64;
}


int nrf24l01p_set_rx_address(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe, const char *saddr, unsigned int length)
{
    char pipe_addr;

    switch (pipe) {
        case NRF24L01P_PIPE0:
            pipe_addr = REG_RX_ADDR_P0;
            break;

        case NRF24L01P_PIPE1:
            pipe_addr = REG_RX_ADDR_P1;
            break;

        case NRF24L01P_PIPE2:
            pipe_addr = REG_RX_ADDR_P2;
            break;

        case NRF24L01P_PIPE3:
            pipe_addr = REG_RX_ADDR_P3;
            break;

        case NRF24L01P_PIPE4:
            pipe_addr = REG_RX_ADDR_P4;
            break;

        case NRF24L01P_PIPE5:
            pipe_addr = REG_RX_ADDR_P5;
            break;

        default:
            return -1;
    }

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_regs(dev->spi, dev->cs,
                      (CMD_W_REGISTER | (REGISTER_MASK & pipe_addr)),
                      saddr, NULL, length);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    /* Enable this pipe */
    nrf24l01p_enable_pipe(dev, pipe);
    return (int)length;
}

int nrf24l01p_set_rx_address_long(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe, uint64_t saddr, unsigned int length)
{
    char buf[length];

    if (length <= INITIAL_ADDRESS_WIDTH) {
        for (unsigned int i = 0; i < length; i++) {

            buf[i] = (uint8_t)(saddr >> (((length - 1) - i) * 8));
        }
    }
    else {
        return -1;
    }

    return nrf24l01p_set_rx_address(dev, pipe, buf, length);
}


uint64_t nrf24l01p_get_rx_address_long(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe)
{
    char pipe_addr;
    uint64_t saddr_64 = 0;

    char addr_array[INITIAL_ADDRESS_WIDTH];

    switch (pipe) {
        case NRF24L01P_PIPE0:
            pipe_addr = REG_RX_ADDR_P0;
            break;

        case NRF24L01P_PIPE1:
            pipe_addr = REG_RX_ADDR_P1;
            break;

        case NRF24L01P_PIPE2:
            pipe_addr = REG_RX_ADDR_P2;
            break;

        case NRF24L01P_PIPE3:
            pipe_addr = REG_RX_ADDR_P3;
            break;

        case NRF24L01P_PIPE4:
            pipe_addr = REG_RX_ADDR_P4;
            break;

        case NRF24L01P_PIPE5:
            pipe_addr = REG_RX_ADDR_P5;
            break;

        default:
            return -1;
    }

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_regs(dev->spi, dev->cs,
                      (CMD_R_REGISTER | (REGISTER_MASK & pipe_addr)),
                      NULL, addr_array, INITIAL_ADDRESS_WIDTH);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    for (int i = 0; i < INITIAL_ADDRESS_WIDTH; i++) {
        saddr_64 |= (((uint64_t) addr_array[i]) << (8 * (INITIAL_ADDRESS_WIDTH - i - 1)));
    }

    return saddr_64;
}


int nrf24l01p_set_datarate(const nrf24l01p_t *dev, nrf24l01p_dr_t dr)
{
    char rf_setup;

    nrf24l01p_read_reg(dev, REG_RF_SETUP, &rf_setup);

    switch (dr) {
        case NRF24L01P_DR_250KBS:
            rf_setup |= RF_SETUP_RF_DR_LOW;
            rf_setup &= ~(RF_SETUP_RF_DR_HIGH);
            break;

        case NRF24L01P_DR_1MBS:
            rf_setup &= ~(RF_SETUP_RF_DR_LOW | RF_SETUP_RF_DR_HIGH);
            break;

        case NRF24L01P_DR_2MBS:
            rf_setup &= ~RF_SETUP_RF_DR_LOW;
            rf_setup |= RF_SETUP_RF_DR_HIGH;
            break;

        default:
            return -1;
    }

    return nrf24l01p_write_reg(dev, REG_RF_SETUP, rf_setup);
}

int nrf24l01p_get_status(const nrf24l01p_t *dev)
{
    uint8_t status;

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    status = spi_transfer_byte(dev->spi, dev->cs, false, CMD_NOOP);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    return (int)status;
}

int nrf24l01p_set_power(const nrf24l01p_t *dev, int pwr)
{
    char rf_setup;

    nrf24l01p_read_reg(dev, REG_RF_SETUP, &rf_setup);

    if (pwr >= -3) {
        rf_setup &= ~(3 << 1);
        rf_setup |= (NRF24L01P_PWR_0DBM << 1);
    }

    if (pwr < -3) {
        rf_setup &= ~(3 << 1);
        rf_setup |= (NRF24L01P_PWR_N6DBM << 1);
    }

    if (pwr < -9) {
        rf_setup &= ~(3 << 1);
        rf_setup |= (NRF24L01P_PWR_N12DBM << 1);
    }

    if (pwr < -15) {
        rf_setup &= ~(3 << 1);
    }

    return nrf24l01p_write_reg(dev, REG_RF_SETUP, rf_setup);
}

static const int8_t _nrf24l01p_power_map[4] = { -18, -12, -6, 0 };

int nrf24l01p_get_power(const nrf24l01p_t *dev)
{
    char rf_setup;
    nrf24l01p_read_reg(dev, REG_RF_SETUP, &rf_setup);
    return _nrf24l01p_power_map[(rf_setup & 0x6) >> 1];
}


int nrf24l01p_set_txmode(const nrf24l01p_t *dev)
{
    char conf;
    int status;

    nrf24l01p_stop(dev);

    nrf24l01p_mask_interrupt(dev, (MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT));

    nrf24l01p_flush_tx_fifo(dev);

    nrf24l01p_read_reg(dev, REG_CONFIG, &conf);
    conf &= ~(PRIM_RX);
    status = nrf24l01p_write_reg(dev, REG_CONFIG, conf);

    xtimer_usleep(DELAY_CHANGE_TXRX_US);

    return status;
}

int nrf24l01p_set_rxmode(const nrf24l01p_t *dev)
{
    char conf;
    int status;

    nrf24l01p_unmask_interrupt(dev, MASK_RX_DR);
    nrf24l01p_mask_interrupt(dev, (MASK_TX_DS | MASK_MAX_RT));

    nrf24l01p_flush_rx_fifo(dev);

    nrf24l01p_read_reg(dev, REG_CONFIG, &conf);
    conf |= PRIM_RX;
    status = nrf24l01p_write_reg(dev, REG_CONFIG, conf);

    nrf24l01p_start(dev);

    xtimer_usleep(DELAY_CHANGE_TXRX_US);

    return status;
}


int nrf24l01p_reset_interrupts(const nrf24l01p_t *dev, char intrs)
{
    return nrf24l01p_write_reg(dev, REG_STATUS, intrs);
}

int nrf24l01p_reset_all_interrupts(const nrf24l01p_t *dev)
{
    return nrf24l01p_write_reg(dev, REG_STATUS, ALL_INT_MASK);
}

int nrf24l01p_mask_interrupt(const nrf24l01p_t *dev, char intr)
{
    char conf;

    nrf24l01p_read_reg(dev, REG_CONFIG, &conf);
    conf |= intr;

    return nrf24l01p_write_reg(dev, REG_CONFIG, conf);
}

int nrf24l01p_unmask_interrupt(const nrf24l01p_t *dev, char intr)
{
    char conf;

    nrf24l01p_read_reg(dev, REG_CONFIG, &conf);
    conf &= ~intr;

    return nrf24l01p_write_reg(dev, REG_CONFIG, conf);
}

int nrf24l01p_enable_dynamic_payload(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe)
{
    char feature_val;
    char en_aa_val;
    char dynpd_val;
    int pipe_mask = 0;
    int dpl_mask = 0;

    if (nrf24l01p_read_reg(dev, REG_FEATURE, &feature_val) < 0) {
        DEBUG("Can't read REG_FEATURE\n");
        return -1;
    }
    if (!(feature_val & FEATURE_EN_DPL)){
        feature_val |= FEATURE_EN_DPL;
        if (nrf24l01p_write_reg(dev, REG_FEATURE, feature_val) < 0){
            DEBUG("Can't write REG_FEATURE\n");
            return -1;
        }
    }

    if (nrf24l01p_read_reg(dev, REG_EN_AA, &en_aa_val) < 0){
        DEBUG("Can't read REG_EN_AA\n");
        return -1;
    }
    switch (pipe){
    case NRF24L01P_PIPE0:
        pipe_mask = ENAA_P0;
        dpl_mask = DYNPD_DPL_P0;
        break;
    case NRF24L01P_PIPE1:
        pipe_mask = ENAA_P1;
        dpl_mask = DYNPD_DPL_P1;
        break;
    case NRF24L01P_PIPE2:
        pipe_mask = ENAA_P2;
        dpl_mask = DYNPD_DPL_P2;
        break;
    case NRF24L01P_PIPE3:
        pipe_mask = ENAA_P3;
        dpl_mask = DYNPD_DPL_P3;
        break;
    case NRF24L01P_PIPE4:
        pipe_mask = ENAA_P4;
        dpl_mask = DYNPD_DPL_P4;
        break;
    case NRF24L01P_PIPE5:
        pipe_mask = ENAA_P5;
        dpl_mask = DYNPD_DPL_P5;
        break;
    }

    if (!(en_aa_val & pipe_mask)){
        en_aa_val |= pipe_mask;
        if (nrf24l01p_write_reg(dev, REG_EN_AA, en_aa_val) < 0){
            DEBUG("Can't write REG_EN_AA\n");
            return -1;
        }
    }

    if (nrf24l01p_read_reg(dev, REG_DYNPD, &dynpd_val) < 0){
        DEBUG("Can't read REG_DYNPD\n");
        return -1;
    }

    if (!(dynpd_val & dpl_mask)){
        dynpd_val |= dpl_mask;
        if (nrf24l01p_write_reg(dev, REG_DYNPD, dynpd_val) < 0){
            DEBUG("Can't write REG_DYNPD\n");
            return -1;
        }
    }

    return 0;
}

int nrf24l01p_enable_pipe(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe)
{
    char pipe_conf;

    nrf24l01p_read_reg(dev, REG_EN_RXADDR, &pipe_conf);
    pipe_conf |= (1 << pipe);

    return nrf24l01p_write_reg(dev, REG_EN_RXADDR, pipe_conf);
}

int nrf24l01p_disable_pipe(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe)
{
    char pipe_conf;

    nrf24l01p_read_reg(dev, REG_EN_RXADDR, &pipe_conf);
    pipe_conf &= ~(1 << pipe);

    return nrf24l01p_write_reg(dev, REG_EN_RXADDR, pipe_conf);
}

int nrf24l01p_disable_crc(const nrf24l01p_t *dev)
{
    char conf;

    nrf24l01p_read_reg(dev, REG_CONFIG, &conf);
    return nrf24l01p_write_reg(dev, REG_CONFIG, (conf & ~(EN_CRC)));
}

int nrf24l01p_enable_crc(const nrf24l01p_t *dev, nrf24l01p_crc_t crc)
{
    char conf;

    nrf24l01p_read_reg(dev, REG_CONFIG, &conf);

    switch (crc) {
        case NRF24L01P_CRC_1BYTE:
            conf &= ~(CRCO);
            break;

        case NRF24L01P_CRC_2BYTE:
            conf |= CRCO;
            break;

        default:
            return -1;
    }

    return nrf24l01p_write_reg(dev, REG_CONFIG, (conf | EN_CRC));
}

int nrf24l01p_setup_auto_ack(const nrf24l01p_t *dev, nrf24l01p_rx_pipe_t pipe, nrf24l01p_retransmit_delay_t delay_retrans, char count_retrans)
{
    char en_aa;
    int status;
    nrf24l01p_read_reg(dev, REG_EN_AA, &en_aa);

    switch (pipe) {
        case NRF24L01P_PIPE0:
            en_aa |= (1 << 0);
            break;

        case NRF24L01P_PIPE1:
            en_aa |= (1 << 1);
            break;

        case NRF24L01P_PIPE2:
            en_aa |= (1 << 2);
            break;

        case NRF24L01P_PIPE3:
            en_aa |= (1 << 3);
            break;

        case NRF24L01P_PIPE4:
            en_aa |= (1 << 4);
            break;

        case NRF24L01P_PIPE5:
            en_aa |= (1 << 5);
            break;

        default:
            return -1;
    }

    /* Enable Auto Ack */
    status = nrf24l01p_write_reg(dev, REG_EN_AA, en_aa);

    if (status < 0) {
        return status;
    }

    count_retrans = (count_retrans < 16) ? count_retrans : 15;

    /* setup auto retransmit delay and count */
    return nrf24l01p_write_reg(dev, REG_SETUP_RETR, ((delay_retrans << 4) | count_retrans));
}

int nrf24l01p_enable_dynamic_ack(const nrf24l01p_t *dev)
{
    char feature;

    if (nrf24l01p_read_reg(dev, REG_FEATURE, &feature) < 0){
        DEBUG("Can't read FEATURE reg\n");
       return -1;
    }
    if (!(feature & FEATURE_EN_DYN_ACK)){
        feature |= FEATURE_EN_DYN_ACK;
        if (nrf24l01p_write_reg(dev, REG_FEATURE, feature) < 0){
            DEBUG("Can't write FEATURE reg\n");
            return -1;
        }
    }
    return 0;
}

int nrf24l01p_disable_all_auto_ack(const nrf24l01p_t *dev)
{
    return nrf24l01p_write_reg(dev, REG_EN_AA, 0x00);
}


int nrf24l01p_flush_tx_fifo(const nrf24l01p_t *dev)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_byte(dev->spi, dev->cs, false, CMD_FLUSH_TX);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    return 0;
}

int nrf24l01p_flush_rx_fifo(const nrf24l01p_t *dev)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi, dev->cs, SPI_MODE, SPI_CLK);
    spi_transfer_byte(dev->spi, dev->cs, false, CMD_FLUSH_RX);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    xtimer_spin(DELAY_AFTER_FUNC_TICKS);

    return 0;
}

void nrf24l01p_rx_cb(void *arg)
{
    DEBUG("In HW cb\n");

    nrf24l01p_t *dev = (nrf24l01p_t *)arg;

    /* clear interrupt */
    nrf24l01p_reset_all_interrupts(dev);

    /* informs thread about available rx data*/
    if (dev->listener != KERNEL_PID_UNDEF) {
        msg_t m;
        m.type = RCV_PKT_NRF24L01P;
        m.content.ptr = dev;
        /* transmit more things here ? */
        msg_send_int(&m, dev->listener);
    }
}
