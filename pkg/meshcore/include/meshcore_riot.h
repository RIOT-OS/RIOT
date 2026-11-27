/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_meshcore
 * @{
 *
 * @file
 * @brief       RIOT implementation of the abstractions of MeshCore
 *
 * The radio is a LoRa netdev device, so MeshCore works with the sub-GHz LoRa
 * transceivers supported by RIOT.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mutex.h"
#include "net/lora.h"
#include "net/netdev.h"
#include "sched.h"

#ifdef __cplusplus
}
#endif

/* The CMSIS headers of some MCUs, as the STM32, define RNG as the RNG
 * peripheral, which conflicts with the RNG class of MeshCore: the CPU header is
 * included first, then the macro is undefined */
#include "cpu.h"
#undef RNG

#include "Dispatcher.h"
#include "Mesh.h"
#include "MeshCore.h"
#include "Utils.h"

/**
 * @brief   Period of the processing of the mesh, in milliseconds, when there
 *          is no radio event
 */
#ifndef CONFIG_MESHCORE_LOOP_PERIOD_MS
#  define CONFIG_MESHCORE_LOOP_PERIOD_MS    (10U)
#endif

/**
 * @brief   Default frequency, in Hz
 */
#ifndef CONFIG_MESHCORE_FREQUENCY
#  define CONFIG_MESHCORE_FREQUENCY         (869618000UL)
#endif

/**
 * @brief   Default bandwidth, one of LORA_BW_*
 */
#ifndef CONFIG_MESHCORE_BANDWIDTH
#  define CONFIG_MESHCORE_BANDWIDTH         (LORA_BW_62_5_KHZ)
#endif

/**
 * @brief   Default spreading factor, one of LORA_SF*
 */
#ifndef CONFIG_MESHCORE_SPREADING_FACTOR
#  define CONFIG_MESHCORE_SPREADING_FACTOR  (LORA_SF8)
#endif

/**
 * @brief   Default coding rate, one of LORA_CR_*
 */
#ifndef CONFIG_MESHCORE_CODING_RATE
#  define CONFIG_MESHCORE_CODING_RATE       (LORA_CR_4_5)
#endif

/**
 * @brief   Default transmission power, in dBm
 */
#ifndef CONFIG_MESHCORE_TX_POWER
#  define CONFIG_MESHCORE_TX_POWER          (14)
#endif

/**
 * @brief   LoRa parameters of the mesh
 */
typedef struct {
    uint32_t frequency;         /**< frequency, in Hz */
    uint8_t bandwidth;          /**< bandwidth, LORA_BW_* */
    uint8_t spreading_factor;   /**< spreading factor, LORA_SF* */
    uint8_t coding_rate;        /**< coding rate, LORA_CR_* */
    int16_t tx_power;           /**< transmission power, in dBm */
    uint8_t sync_word;          /**< sync word */
    uint16_t preamble_len;      /**< preamble length, in symbols */
} meshcore_radio_params_t;

namespace meshcore_riot {

/**
 * @brief   Default LoRa parameters, set by the CONFIG_MESHCORE_* macros
 *
 * The defaults of MeshCore are 869.618 MHz, 62.5 kHz, SF8 and coding rate
 * 4/5. The sync word is private and the preamble lasts 16 symbols.
 */
constexpr meshcore_radio_params_t default_radio_params = {
    CONFIG_MESHCORE_FREQUENCY,
    CONFIG_MESHCORE_BANDWIDTH,
    CONFIG_MESHCORE_SPREADING_FACTOR,
    CONFIG_MESHCORE_CODING_RATE,
    CONFIG_MESHCORE_TX_POWER,
    0x12,
    16,
};

/**
 * @brief   MeshCore radio based on a LoRa netdev device
 */
class Radio : public mesh::Radio {
public:
    /**
     * @brief   Constructor
     *
     * @param[in] dev   LoRa netdev device, set up but not initialized
     */
    explicit Radio(netdev_t *dev) : _dev(dev) { }

    /**
     * @brief   Initializes and configures the device, then starts the
     *          reception
     *
     * Must be called from the thread processing the mesh, which receives the
     * interrupts of the device.
     *
     * @param[in] params    LoRa parameters
     *
     * @return  0 on success
     * @return  a negative errno on failure
     */
    int init(const meshcore_radio_params_t &params);

    /**
     * @brief   Handles the pending interrupt of the device
     */
    void handleIsr();

    /**
     * @brief   Reads random bytes from the noise of the radio
     *
     * The device must be initialized. It can't receive meanwhile, the
     * reception is restarted afterwards.
     *
     * @param[out] dest     buffer for the random bytes
     * @param[in] len       number of random bytes
     *
     * @return  0 on success
     * @return  a negative errno if the device can't generate random numbers
     */
    int readNoise(uint8_t *dest, size_t len);

    /**
     * @brief   Returns the LoRa parameters
     *
     * @return  the parameters given to init()
     */
    const meshcore_radio_params_t &getParams() const
    {
        return _params;
    }

    /** @brief Copies the last received frame, returns its length or 0 */
    int recvRaw(uint8_t *bytes, int sz) override;
    /** @brief Returns the time on air of a frame, in milliseconds */
    uint32_t getEstAirtimeFor(int len_bytes) override;
    /** @brief Returns the score of a received frame, from its SNR and length */
    float packetScore(float snr, int packet_len) override;
    /** @brief Starts sending a frame, returns false on failure */
    bool startSendRaw(const uint8_t *bytes, int len) override;
    /** @brief Tells whether the frame being sent is sent */
    bool isSendComplete() override;
    /** @brief Ends the transmission and restarts the reception */
    void onSendFinished() override;
    /** @brief Tells whether the radio is receiving, not sending */
    bool isInRecvMode() const override;
    /** @brief Tells whether a frame is being received */
    bool isReceiving() override;
    /** @brief Returns the RSSI of the last received frame, in dBm */
    float getLastRSSI() const override;
    /** @brief Returns the SNR of the last received frame, in dB */
    float getLastSNR() const override;

private:
    static void _event_cb(netdev_t *dev, netdev_event_t event);
    void _onEvent(netdev_event_t event);
    void _startRx();

    netdev_t *_dev;
    kernel_pid_t _pid = KERNEL_PID_UNDEF;
    meshcore_radio_params_t _params = default_radio_params;
    uint8_t _rx_buf[MAX_TRANS_UNIT];
    int _rx_len = 0;
    float _rssi = 0;
    float _snr = 0;
    uint32_t _rx_start = 0;
    bool _receiving = false;
    bool _tx_busy = false;
    bool _tx_done = false;
};

/**
 * @brief   Millisecond clock, based on ZTIMER_MSEC
 */
class MillisClock : public mesh::MillisecondClock {
public:
    /** @brief Returns the milliseconds since boot */
    unsigned long getMillis() override;
};

/**
 * @brief   Real-time clock, based on ZTIMER_SEC, which starts at a fixed time
 *          after a reboot
 */
class RTCClock : public mesh::RTCClock {
public:
    /** @brief Returns the current time, in seconds since the epoch */
    uint32_t getCurrentTime() override;
    /** @brief Sets the current time, in seconds since the epoch */
    void setCurrentTime(uint32_t time) override;

private:
    /* 2026-01-01 00:00:00 UTC */
    uint32_t _base = 1767225600;
    uint32_t _start = 0;
};

/**
 * @brief   Random number generator, based on periph_hwrng when available,
 *          else on the random module
 *
 * The random module is seeded with the unique ID of the CPU when there is no
 * periph_hwrng, so this generator is not suited to generate keys: use
 * @ref RadioRNG for the identity.
 */
class RNG : public mesh::RNG {
public:
    /** @brief Fills @p dest with @p sz random bytes */
    void random(uint8_t *dest, size_t sz) override;
};

/**
 * @brief   Random number generator combining the noise of the radio with
 *          @ref RNG, to generate the identity, as MeshCore does
 *
 * The radio must be initialized, and the mesh must not be processed while the
 * random numbers are generated.
 */
class RadioRNG : public mesh::RNG {
public:
    /**
     * @brief   Constructor
     *
     * @param[in] radio     the radio, initialized
     */
    explicit RadioRNG(Radio &radio) : _radio(radio) { }

    /** @brief Fills @p dest with @p sz random bytes */
    void random(uint8_t *dest, size_t sz) override;

private:
    Radio &_radio;
};

/**
 * @brief   Runs the processing of a mesh
 */
class Runner {
public:
    /**
     * @brief   Constructor
     *
     * @param[in] mesh      the mesh
     * @param[in] radio     the radio of the mesh
     */
    Runner(mesh::Mesh &mesh, Radio &radio) : _mesh(mesh), _radio(radio) { }

    /**
     * @brief   Initializes the radio
     *
     * Must be called from the thread processing the mesh, before run(). The
     * identity can then be generated with @ref RadioRNG.
     *
     * @param[in] params    LoRa parameters
     *
     * @return  0 on success
     * @return  a negative errno if the radio can't be initialized
     */
    int init(const meshcore_radio_params_t &params);

    /**
     * @brief   Starts the mesh, then processes it forever
     */
    void run();

    /**
     * @brief   Locks the mesh, to use it from another thread
     */
    void lock()
    {
        mutex_lock(&_lock);
    }

    /**
     * @brief   Unlocks the mesh
     */
    void unlock()
    {
        mutex_unlock(&_lock);
    }

private:
    mesh::Mesh &_mesh;
    Radio &_radio;
    mutex_t _lock = MUTEX_INIT;
};

} /* namespace meshcore_riot */

/** @} */
