/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     pkg_semtech-loramac
 * @file
 * @brief       Compatibility functions for controlling the radio driver
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Francisco Molina <francisco.molina@inria.cl>
 * @}
 */

#include <assert.h>

#include "ztimer.h"

#include "net/netopt.h"
#include "net/lora.h"
#include "net/netdev.h"
#include "net/loramac.h"

#include "LoRaMac.h"
#include "radio/radio.h"

#include "sx127x.h"

#define ENABLE_DEBUG 0
#include "debug.h"

extern netdev_t *loramac_netdev_ptr;

/*
 * Radio driver functions implementation wrappers, the netdev2 object
 * is known within the scope of the function
 */
void SX127XInit(RadioEvents_t *events)
{
    (void) events;
    assert(loramac_netdev_ptr);
    if (loramac_netdev_ptr->driver->init(loramac_netdev_ptr) < 0) {
        DEBUG("[semtech-loramac] radio: failed to initialize radio\n");
    }

    DEBUG("[semtech-loramac] radio: initialization successful\n");
}

RadioState_t SX127XGetStatus(void)
{
    netopt_state_t state;
    loramac_netdev_ptr->driver->get(loramac_netdev_ptr, NETOPT_STATE, &state, sizeof(netopt_state_t));
    switch (state) {
    case NETOPT_STATE_RX:
    case NETOPT_STATE_IDLE:
        return RF_RX_RUNNING;
    case NETOPT_STATE_TX:
        return RF_TX_RUNNING;
    default:
        return RF_IDLE;
    }
}

void SX127XSetModem(RadioModems_t modem)
{
    (void)modem;
    uint16_t modem_val = NETDEV_TYPE_LORA;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_DEVICE_TYPE, &modem_val, sizeof(uint16_t));
}

void SX127XSetChannel(uint32_t freq)
{
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_CHANNEL_FREQUENCY, &freq, sizeof(uint32_t));
}

bool SX127XIsChannelFree(RadioModems_t modem, uint32_t freq,
                         int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    (void) maxCarrierSenseTime;
    Radio.SetChannel(freq);
    netopt_state_t state = NETOPT_STATE_IDLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_STATE, &state, sizeof(netopt_state_t));
    ztimer_sleep(ZTIMER_MSEC, 1); /* wait 1 millisecond */
    int16_t rssi = Radio.Rssi(modem);
    Radio.Sleep();
    return (rssi <= rssiThresh);
}

uint32_t SX127XRandom(void)
{
    uint32_t random;
    loramac_netdev_ptr->driver->get(loramac_netdev_ptr, NETOPT_RANDOM, &random, sizeof(uint32_t));
    return random;
}

void SX127XSetRxConfig(RadioModems_t modem, uint32_t bandwidth,
                       uint32_t spreading_factor, uint8_t coding_rate,
                       uint32_t bandwidthAfc, uint16_t preambleLen,
                       uint16_t symbTimeout, bool fixLen,
                       uint8_t payloadLen,
                       bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                       bool iqInverted, bool rxContinuous)
{
    (void)bandwidthAfc;
    (void)fixLen;
    (void)modem;
    uint16_t modem_val = NETDEV_TYPE_LORA;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_DEVICE_TYPE, &modem_val, sizeof(uint16_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_BANDWIDTH, &bandwidth, sizeof(uint8_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_CODING_RATE, &coding_rate, sizeof(uint8_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_SPREADING_FACTOR, &spreading_factor, sizeof(uint8_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_PREAMBLE_LENGTH, &preambleLen, sizeof(uint16_t));
    netopt_enable_t fixed_header = NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_FIXED_HEADER, &fixed_header, sizeof(netopt_enable_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_PDU_SIZE, &payloadLen, sizeof(uint8_t));
    netopt_enable_t enable_crc = (crcOn) ? NETOPT_ENABLE : NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_INTEGRITY_CHECK, &enable_crc, sizeof(netopt_enable_t));
    netopt_enable_t enable_freq_hop = (freqHopOn) ? NETOPT_ENABLE : NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_CHANNEL_HOP, &enable_freq_hop, sizeof(netopt_enable_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_CHANNEL_HOP_PERIOD, &hopPeriod, sizeof(uint8_t));
    netopt_enable_t iq_inverted = (iqInverted) ? NETOPT_ENABLE : NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_IQ_INVERT, &iq_inverted, sizeof(netopt_enable_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_RX_SYMBOL_TIMEOUT, &symbTimeout, sizeof(uint16_t));
    netopt_enable_t single_rx = rxContinuous ? NETOPT_DISABLE : NETOPT_ENABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_SINGLE_RECEIVE, &single_rx, sizeof(netopt_enable_t));
}

void SX127XSetTxConfig(RadioModems_t modem, int8_t power, uint32_t fdev,
                       uint32_t bandwidth, uint32_t spreading_factor,
                       uint8_t coding_rate, uint16_t preambleLen,
                       bool fixLen, bool crcOn, bool freqHopOn,
                       uint8_t hopPeriod, bool iqInverted, uint32_t timeout)
{
    (void)fdev;
    (void)fixLen;
    (void)modem;
    uint16_t modem_val = NETDEV_TYPE_LORA;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_DEVICE_TYPE, &modem_val, sizeof(uint16_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_BANDWIDTH, &bandwidth, sizeof(uint8_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_CODING_RATE, &coding_rate, sizeof(uint8_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_SPREADING_FACTOR, &spreading_factor, sizeof(uint8_t));
    netopt_enable_t enable_crc = (crcOn) ? NETOPT_ENABLE : NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_INTEGRITY_CHECK, &enable_crc, sizeof(netopt_enable_t));
    netopt_enable_t enable_freq_hop = (freqHopOn) ? NETOPT_ENABLE : NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_CHANNEL_HOP, &enable_freq_hop, sizeof(netopt_enable_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_CHANNEL_HOP_PERIOD, &hopPeriod, sizeof(uint8_t));
    netopt_enable_t fixed_header = NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_FIXED_HEADER, &fixed_header, sizeof(netopt_enable_t));
    netopt_enable_t iq_inverted = (iqInverted) ? NETOPT_ENABLE : NETOPT_DISABLE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_IQ_INVERT, &iq_inverted, sizeof(netopt_enable_t));
    int16_t power_val = power;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_TX_POWER, &power_val, sizeof(int16_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_PREAMBLE_LENGTH, &preambleLen, sizeof(uint16_t));
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_TX_TIMEOUT, &timeout, sizeof(uint32_t)); /* base unit ms, LoRaMAC ms */
}

uint32_t SX127XTimeOnAir(RadioModems_t modem, uint8_t pktLen)
{
    (void) modem;
    uint8_t cr;
    loramac_netdev_ptr->driver->get(loramac_netdev_ptr, NETOPT_CODING_RATE, &cr, sizeof(uint8_t));
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DATARATE;
    LoRaMacMibGetRequestConfirm(&mibReq);
    uint8_t dr = (uint8_t)mibReq.Param.ChannelsDatarate;
    return lora_time_on_air(pktLen, dr, cr) >> 10;  /* divide by 1024: return value in ms */
}

void SX127XSend(uint8_t *buffer, uint8_t size)
{
    iolist_t iol = { .iol_base = buffer, .iol_len = size };
    loramac_netdev_ptr->driver->send(loramac_netdev_ptr, &iol);
}

void SX127XSleep(void)
{
    netopt_state_t state = NETOPT_STATE_SLEEP;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_STATE, &state, sizeof(netopt_state_t));
}

void SX127XStandby(void)
{
    netopt_state_t state = NETOPT_STATE_STANDBY;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_STATE, &state, sizeof(netopt_state_t));
}

void SX127XRx(uint32_t timeout)
{
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_RX_TIMEOUT, &timeout, sizeof(uint32_t));
    netopt_state_t state = NETOPT_STATE_RX;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_STATE, &state, sizeof(netopt_state_t));
}

void SX127XStartCad(void)
{
}

int16_t SX127XRssi(RadioModems_t modem)
{
    (void)modem;
    int8_t rssi;
    loramac_netdev_ptr->driver->get(loramac_netdev_ptr, NETOPT_RSSI, &rssi, sizeof(int8_t));
    return rssi;
}

void SX127XWrite(uint16_t addr, uint8_t data)
{
    (void)addr;
    (void)data;
}

uint8_t SX127XRead(uint16_t addr)
{
    (void)addr;
    return 0;
}

void SX127XWriteBuffer(uint16_t addr, uint8_t *buffer, uint8_t size)
{
    (void)addr;
    (void)buffer;
    (void)size;
}

void SX127XReadBuffer(uint16_t addr, uint8_t *buffer, uint8_t size)
{
    (void)addr;
    (void)buffer;
    (void)size;
}

void SX127XSetMaxPayloadLength(RadioModems_t modem, uint8_t max)
{
    (void) modem;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_MAX_PDU_SIZE, &max, sizeof(uint8_t));
}

bool SX127XCheckRfFrequency(uint32_t frequency)
{
    (void) frequency;
    /* Implement check. Currently all frequencies are supported */
    return true;
}

void SX127XSetTxContinuousWave(uint32_t freq, int8_t power, uint16_t time)
{
    (void) freq;
    (void) power;
    (void) time;
    /* TODO */
}

void SX127XSetPublicNetwork(bool enable)
{
    uint8_t syncword = (enable) ? LORA_SYNCWORD_PUBLIC : LORA_SYNCWORD_PRIVATE;
    loramac_netdev_ptr->driver->set(loramac_netdev_ptr, NETOPT_SYNCWORD, &syncword, sizeof(uint8_t));
}

uint32_t SX127XGetWakeupTime(void)
{
    return 0;
}

void SX127XIrqProcess(void)
{
    return;
}

void SX127XRxBoosted(uint32_t timeout)
{
    (void) timeout;
    return;
}

void SX127XSetRxDutyCycle(uint32_t rx_time, uint32_t sleep_time)
{
    (void) rx_time;
    (void) sleep_time;
    return;
}

/**
 * LoRa function callbacks
 */
const struct Radio_s Radio =
{
    SX127XInit,
    SX127XGetStatus,
    SX127XSetModem,
    SX127XSetChannel,
    SX127XIsChannelFree,
    SX127XRandom,
    SX127XSetRxConfig,
    SX127XSetTxConfig,
    SX127XCheckRfFrequency,
    SX127XTimeOnAir,
    SX127XSend,
    SX127XSleep,
    SX127XStandby,
    SX127XRx,
    SX127XStartCad,
    SX127XSetTxContinuousWave,
    SX127XRssi,
    SX127XWrite,
    SX127XRead,
    SX127XWriteBuffer,
    SX127XReadBuffer,
    SX127XSetMaxPayloadLength,
    SX127XSetPublicNetwork,
    SX127XGetWakeupTime,
    SX127XIrqProcess,
    SX127XRxBoosted,
    SX127XSetRxDutyCycle,
};
