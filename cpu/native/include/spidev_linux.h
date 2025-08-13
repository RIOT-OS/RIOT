/*
 * Copyright (C) 2019 Frank Hessel <frank@fhessel.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup drivers_spidev_linux Linux User Mode SPI Driver
 * @ingroup  cpu_native
 * @brief    Implementation of SPI access from Linux User Space
 * @{
 *
 * This module allows to connect a RIOT application that runs on a Linux host to
 * the physical SPI bus(ses) of that host. To do so, the application has to be
 * compiled for the native board in a Linux environment.
 *
 * SPI support is automatically included if either a module requiring the
 * `PERIPH_SPI` feature is added to the application or if it is explicitly
 * listed as `FEATURES_REQUIRED` in the application's Makefile.
 *
 * At runtime, the process has to be connected to a specific bus on the host
 * machine. SPI buses are exposed as `/dev/spidevB.D` character files, where B
 * is the Bus ID (MISO, MOSI and SCLK lines) and D denotes the connected device
 * or hardware chip select line. Ideally, this structure should be reflected
 * when mapping the device files to RIOT SPI buses.
 *
 * Example:
 *
 * ```
 * $ ./riot_native_app --spi=0:0:/dev/spidev0.0 --spi=0:1:/dev/spidev0.1
 * ```
 *
 * This will add `/dev/spidev0.0` and `/dev/spidev0.1` as SPI_DEV(0) in RIOT.
 * The first device can be used with SPI_HWCS(0) as CS parameter, the second one
 * with SPI_HWCS(1) as CS parameter.
 *
 * Multiple SPI buses can be added by increasing SPI_NUMOF in the Makefile:
 * ```
 * CFLAGS += -DSPI_NUMOF=n
 * ```
 *
 * The sames goes for the SPI_MAXCS parameter that defines the maximum number of
 * SPI_HWCS values per bus.
 *
 * Buses that aren't assigned during startup will return either SPI_NODEV or
 * SPI_NOCS when accessed.
 *
 * If the SPI API is called with SPI_CS_UNDEF as CS parameter, the driver will
 * select the file descriptor with the lowest HWCS id for that bus, but the
 * actual CS line will not be pulled low (if the hardware supports this). This
 * would (in principle) allow to control CS manually.
 */

/**
 * @file
 * @brief  Implementation of SPI access from Linux User Space
 * @author Frank Hessel <frank@fhessel.de>
 */

#if defined(__linux__) || defined(DOXYGEN) /* Linux-only */

#include "periph/spi.h"

#include "mutex.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Status codes for SPI device setup
 */
enum {
    SPI_SETUP_OK          =  0,     /**< parameters are sound */
    SPI_SETUP_INVALID     = -1,     /**< invalid params or duplicate definition */
};

/**
 * @brief Static runtime configuration for SPI port + CS line
 *
 * Contains the information that is passed by command line on startup
 */
typedef struct spidev_linux_conf {
    /** Filename for a specific SPI device + CS line (like /dev/spidev0.0) */
    char *device_filename[SPI_MAXCS];
} spidev_linux_conf_t;

/**
 * @brief Dynamic runtime state for SPI port + CS line
 *
 * Contains state of the line (whether if it's opened, in use, available, ...)
 */
typedef struct spidev_linux_state {
    /** Mutex for the whole bus (all CS lines) */
    mutex_t lock;
    /** File descriptors for each CS line on the bus */
    int fd[SPI_MAXCS];
} spidev_linux_state_t;

/**
 * @brief     register `/dev/spidev*` device to be used for SPI
 *
 * @param[in] bus     SPI bus id of the device
 * @param[in] cs_id   ID of CS line to configure
 * @param[in] name    path name for `/dev/spidev*` device
 * @return    SPI_SETUP_OK       On success
 * @return    SPI_SETUP_INVALID  On invalid parameters
 */
int spidev_linux_setup(spi_t bus, unsigned cs_id, const char *name);

/**
 * @brief     Close open SPI file descriptors
 */
void spidev_linux_teardown(void);

#ifdef __cplusplus
}
#endif

#else
/* Create the error in the header file as spi.c will be compiled to late to show it */
#ifdef MODULE_PERIPH_SPIDEV_LINUX
#error "MODULE periph_spidev_linux is only available on Linux"
#endif
#endif /* defined(__linux__) || defined(DOXYGEN) */

/** @} */
