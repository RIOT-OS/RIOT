/*
 * Copyright (C) 2019 Frank Hessel <frank@fhessel.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_native
 * @ingroup     drivers_spidev_linux
 * @{
 *
 * @file
 * @brief       Implementation of SPI access from Linux User Space
 *
 * @author      Frank Hessel <frank@fhessel.de>
 * @}
 */

#ifdef MODULE_PERIPH_SPIDEV_LINUX

#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include <linux/limits.h>
#include <linux/spi/spidev.h>
/* Linux' SPI_MODE_N collide with RIOT's spi_mode_t enum */
#undef SPI_MODE_0
#undef SPI_MODE_1
#undef SPI_MODE_2
#undef SPI_MODE_3

#include "assert.h"
#include "native_internal.h"
#include "spidev_linux.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Holds the configuration for each SPI device (pathnames)
 */
static spidev_linux_conf_t device_conf[SPI_NUMOF];

/**
 * @brief Holds the current state for each SPI device (file descriptors, mutex)
 */
static spidev_linux_state_t device_state[SPI_NUMOF];

/**
 * @brief Returns the fd of the first valid cs line
 */
static int spidev_get_first_fd(spidev_linux_state_t *state);

/**
 * @brief Initializes a spidev_linux_state_t structure
 */
static void spidev_init_device_state(spidev_linux_state_t *state);

/**
 * @brief Applies bus parameters
 *
 * @param[in] fd   File descriptor for the bus
 * @param[in] hwcs true if the hardware chip select line should be used
 * @param[in] mode SPI mode (0..3)
 * @param[in] clk  Clock rate in Hertz
 *
 * @return    SPI_OK     If everything went well
 * @return    SPI_NOMODE If setting the mode didn't work
 * @return    SPI_NOCLK  If setting the clock didn't work
 */
static int spi_set_params(int fd, bool hwcs, spi_mode_t mode, spi_clk_t clk);

static void spidev_init_device_state(spidev_linux_state_t *state)
{
    mutex_init(&(state->lock));
    for (spi_cs_t cs = 0; cs < SPI_MAXCS; cs++) {
        state->fd[cs] = -1;
    }
}

static int spidev_get_first_fd(spidev_linux_state_t *state)
{
    int fd = -1;

    for (spi_cs_t cs = 0; cs < SPI_MAXCS && fd < 0; cs++) {
        fd = state->fd[cs];
    }
    return fd;
}

int spidev_linux_setup(spi_t bus, spi_cs_t cs, const char *name)
{
    if (bus >= SPI_NUMOF || cs >= SPI_MAXCS) {
        return SPI_SETUP_INVALID;
    }
    spidev_linux_conf_t *conf = &(device_conf[bus]);
    if (conf->device_filename[cs] != NULL) {
        return SPI_SETUP_INVALID;
    }
    device_conf[bus].device_filename[cs] = strndup(name, PATH_MAX - 1);
    return SPI_SETUP_OK;
}

void spidev_linux_teardown(void)
{
    for (spi_t bus = 0; bus < SPI_NUMOF; bus++) {
        spidev_linux_state_t *state = &(device_state[bus]);
        for (spi_cs_t cs = 0; cs < SPI_MAXCS; cs++) {
            if (state->fd[cs] >= 0) {
                real_close(state->fd[cs]);
            }
        }
        spidev_init_device_state(state);
    }
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    DEBUG("spi_acquire(%d, %d, 0x%02x, %d)\n", bus, cs, mode, clk);
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }

    mutex_lock(&(device_state[bus].lock));

    bool use_hwcs = false;
    int fd = -1;
    if (cs != SPI_CS_UNDEF) {
        use_hwcs = true;
        if (cs > SPI_MAXCS || device_state[bus].fd[cs] < 0) {
            DEBUG("spi_acquire: No fd for %d:%d\n", bus, cs);
            return SPI_NOCS;
        }
        fd = device_state[bus].fd[cs];
        DEBUG("spi_acquire: Using %d:%d with HWCS (-> fd 0x%x)\n", bus, cs, fd);
    }
    else {
        fd = spidev_get_first_fd(&(device_state[bus]));
        if (fd < 0) {
            return SPI_NOCS;
        }
        DEBUG("spi_acquire: Using SPI_NO_CS (-> fd 0x%x)\n", fd);
    }

    int res = spi_set_params(fd, use_hwcs, mode, clk);
    if (res < 0) {
        DEBUG("spi_acquire: set_params failed for %d:%d\n", bus, cs);
        mutex_unlock(&(device_state[bus].lock));
    }

    DEBUG("spi_acquire: %d:%d acquired\n", bus, cs);
    return SPI_OK;
}

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    spidev_linux_state_t *state = &(device_state[bus]);
    spidev_linux_conf_t *conf = &(device_conf[bus]);

    spidev_init_device_state(state);
    DEBUG("spi_init: init bus %d\n", bus);
    for (spi_cs_t cs = 0; cs < SPI_MAXCS; cs++) {
        if (conf->device_filename[cs] != NULL) {
            int fd = real_open(conf->device_filename[cs], O_RDWR);
            if (fd < 0) {
                /* Add a printf instead of only asserting to show invalid bus */
                real_printf(
                    "Cannot acquire %s for spidev%d:%d\n",
                    conf->device_filename[cs],
                    bus,
                    cs
                    );
                assert(false);
            }
            DEBUG("spi_init: %d:%d %s (fd 0x%x)\n", bus, cs,
                  conf->device_filename[cs], fd);
            state->fd[cs] = fd;
        }
        else {
            DEBUG("spi_init: %d:%d Unused\n", bus, cs);
        }
    }
    DEBUG("spi_init: done\n");
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    else if (cs != SPI_CS_UNDEF && cs >= SPI_MAXCS) {
        return SPI_NOCS;
    }
    else if (device_state[bus].fd[cs] < 0) {
        return SPI_NOCS;
    }
    return SPI_OK;
}

void spi_init_pins(spi_t bus)
{
    (void)bus;
    /* Nothing to do here, as the kernel driver does the pin management */
}

void spi_release(spi_t bus)
{
    DEBUG("spi_release(%d)\n", bus);
    if (bus < SPI_NUMOF) {
        mutex_unlock(&(device_state[bus].lock));
    }
}

static int spi_set_params(int fd, bool hwcs, spi_mode_t mode, spi_clk_t clk)
{
    uint8_t spi_mode = mode | (hwcs ? 0 : SPI_NO_CS);
    uint32_t ioctl_clk = clk;

    if (real_ioctl(fd, SPI_IOC_WR_MODE, &spi_mode) < 0) {
        return SPI_NOMODE;
    }
    if (real_ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &ioctl_clk) < 0) {
        return SPI_NOCLK;
    }
    return SPI_OK;
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    if (bus >= SPI_NUMOF || (cs != SPI_CS_UNDEF && cs >= SPI_MAXCS)) {
        return;
    }

    int fd = (cs == SPI_CS_UNDEF) ?
                spidev_get_first_fd(&(device_state[bus])) :
                device_state[bus].fd[cs];

    if (fd < 0) {
        return;
    }

    intptr_t out_addr = (intptr_t)out;
    intptr_t in_addr = (intptr_t)in;

    struct spi_ioc_transfer spi_tf = {
        .bits_per_word = 8,
        /*
         * The kernel documentation is a bit ambiguous about how to use the
         * cs_change value ("True to deselect device"). It seems like
         * setting it to true leaves the CS line actually low (=selected)
         * after transmission.
         */
        .cs_change = cont,
        .len = len,
        .rx_buf = (uint64_t)in_addr,
        .tx_buf = (uint64_t)out_addr,
        /* Leaving speed_hz as zero uses the value from spi_acquire */
        .speed_hz = 0,
    };

    if (real_ioctl(fd, SPI_IOC_MESSAGE(1), &spi_tf) < 0) {
        DEBUG("spi_transfer_bytes: ioctl failed\n");
    }
    else {
        DEBUG("\nspi_transfer_bytes: transferred %d bytes\n", len);
    }
}

#endif   /* MODULE_PERIPH_SPIDEV_LINUX */
