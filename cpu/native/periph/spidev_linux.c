/*
 * Copyright (C) 2019 Frank Hessel <frank@fhessel.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup cpu_native
 * @ingroup drivers_spidev_linux
 * @brief   Implementation of SPI access from Linux User Space
 * @author  Frank Hessel <frank@fhessel.de>
 */

#ifdef MODULE_PERIPH_SPIDEV_LINUX

#include <assert.h>
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

#include "native_internal.h"
#include "spidev_linux.h"
#ifdef MODULE_PERIPH_GPIO
#include "periph/gpio.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief true, if x is a hardware-based chip select line
 */
#define IS_HW_CS(x) (x < UINT_MAX && x >= UINT_MAX - SPI_MAXCS )

/**
 * @brief true, if x is a gpio-based chip select line
 */
#ifdef MODULE_PERIPH_GPIO
#define IS_GPIO_CS(x) (x < UINT_MAX - SPI_MAXCS)
#else
#define IS_GPIO_CS(x) (0)
#endif

/**
 * @brief true, if x is a valid chip select line (either GPIO or HW)
 */
#define IS_VALID_CS(x) (IS_HW_CS(x) || IS_GPIO_CS(x))

/**
 * @brief If IS_HW_CS(x), this converts x to the corresponding CS ID for file
 * descriptor selection, basically a reverse SPI_HWCS(x)
 */
#define CS_TO_CSID(x) (x - (UINT_MAX - SPI_MAXCS))

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

int spidev_linux_setup(spi_t bus, unsigned cs_id, const char *name)
{
    if (bus >= SPI_NUMOF || cs_id >= SPI_MAXCS) {
        return SPI_SETUP_INVALID;
    }
    spidev_linux_conf_t *conf = &(device_conf[bus]);
    if (conf->device_filename[cs_id] != NULL) {
        return SPI_SETUP_INVALID;
    }
    device_conf[bus].device_filename[cs_id] = strndup(name, PATH_MAX - 1);
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

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    DEBUG("spi_acquire(%u, %u, 0x%02x, %d)\n", bus, cs, mode, clk);
    assert((unsigned)bus < SPI_NUMOF);

    mutex_lock(&(device_state[bus].lock));

    /* If true, the spidev API controls the chip select line. We need this here
     * if an SPI_HWCS parameter is used. If cs==SPI_CS_UNDEF, the driver should
     * not care about CS at all (use_hwcs=true), if cs is a GPIO_PIN and the
     * GPIO module is enabled, the driver controls the pin through that module.
     */
    bool use_hwcs = false;
    int fd = -1;
    if (IS_HW_CS(cs)) {
        use_hwcs = true;
        unsigned csid = CS_TO_CSID(cs);
        if (device_state[bus].fd[csid] < 0) {
            DEBUG("spi_acquire: No fd for %u:%u\n", bus, csid);
            assert(0);
        }
        fd = device_state[bus].fd[csid];
        DEBUG("spi_acquire: Using %u:%u with HWCS (-> fd 0x%x)\n", bus, csid, fd);
    }
    else if (IS_GPIO_CS(cs) || cs == SPI_CS_UNDEF) {
        fd = spidev_get_first_fd(&(device_state[bus]));
        if (fd < 0) {
            DEBUG("spi_acquire: Invalid CS parameter\n");
            assert(0);
        }
        DEBUG("spi_acquire: Using SPI_CS_UNDEF (-> fd 0x%x)\n", fd);
    }
    else {
        DEBUG("spi_acquire: Invalid CS parameter\n");
        assert(0);
    }

    int res = spi_set_params(fd, use_hwcs, mode, clk);
    if (res < 0) {
        DEBUG("spi_acquire: set_params failed\n");
        assert(0);
    }

    DEBUG("spi_acquire: bus %u acquired\n", bus);
}

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    spidev_linux_state_t *state = &(device_state[bus]);
    spidev_linux_conf_t *conf = &(device_conf[bus]);

    spidev_init_device_state(state);
    DEBUG("spi_init: init bus %u\n", bus);
    for (spi_cs_t cs = 0; cs < SPI_MAXCS; cs++) {
        if (conf->device_filename[cs] != NULL) {
            int fd = real_open(conf->device_filename[cs], O_RDWR);
            if (fd < 0) {
                /* Add a printf instead of only asserting to show invalid bus */
                real_printf(
                    "Cannot acquire %s for spidev%u:%u\n",
                    conf->device_filename[cs],
                    bus,
                    cs
                    );
                assert(false);
            }
            DEBUG("spi_init: %u:%u %s (fd 0x%x)\n", bus, cs,
                  conf->device_filename[cs], fd);
            state->fd[cs] = fd;
        }
        else {
            DEBUG("spi_init: %u:%u Unused\n", bus, cs);
        }
    }
    DEBUG("spi_init: done\n");
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    else if (!IS_VALID_CS(cs) && cs != SPI_CS_UNDEF) {
        return SPI_NOCS;
    }
    else if (IS_HW_CS(cs) && device_state[bus].fd[CS_TO_CSID(cs)] < 0) {
        return SPI_NOCS;
    }
    else if (IS_GPIO_CS(cs)) {
#ifdef MODULE_PERIPH_GPIO
        if (gpio_init(cs, GPIO_OUT) < 0) {
            return SPI_NOCS;
        }
        gpio_set(cs);
#endif
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
    DEBUG("spi_release(%u)\n", bus);
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
    if (bus >= SPI_NUMOF || (!IS_VALID_CS(cs) && cs != SPI_CS_UNDEF)) {
        DEBUG("spi_transfer_bytes: invalid bus/cs. Skipping transfer.\n");
        return;
    }

    int fd = IS_HW_CS(cs) ?
                device_state[bus].fd[CS_TO_CSID(cs)] :
                spidev_get_first_fd(&(device_state[bus]));

    if (fd < 0) {
        DEBUG("spi_transfer_bytes: no suitable fd. Skipping transfer.\n");
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

#ifdef MODULE_PERIPH_GPIO
    if (IS_GPIO_CS(cs)) {
        DEBUG("spi_transfer_bytes: using GPIO-based CS\n");
        gpio_clear(cs);
    }
#endif

    if (real_ioctl(fd, SPI_IOC_MESSAGE(1), &spi_tf) < 0) {
        DEBUG("spi_transfer_bytes: ioctl failed\n");
    }
    else {
        DEBUG("spi_transfer_bytes: transferred %zu bytes\n", len);
    }

#ifdef MODULE_PERIPH_GPIO
    if (IS_GPIO_CS(cs) && !cont) {
        gpio_set(cs);
    }
#endif
}

#endif   /* MODULE_PERIPH_SPIDEV_LINUX */
