/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014-2017 Freie Universität Berlin
 *               2016-2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "board.h"
#include "compiler_hints.h"
#include "mutex.h"
#include "periph/cpu_gpio_ll.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "pm_layered.h"
#include "cpu_conf.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Number of bits to shift the BR value in the CR1 register
 */
#define BR_SHIFT (27U)
#define BR_MAX (7U)

#define SPI_CR2_SETTINGS 0

#define SPI_CFG2_SETTINGS (SPI_CFG2_AFCNTR)
#define SPI_CFG1_SETTINGS ((0x7 << SPI_CFG1_DSIZE_Pos))
/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   Clock configuration cache
 */
static uint32_t clocks[SPI_NUMOF];

/**
 * @brief   Clock prescaler cache
 */
static uint8_t prescalers[SPI_NUMOF];

static inline SPI_TypeDef *dev(spi_t bus) { return spi_config[bus].dev; }

static uint8_t _get_prescaler(const spi_conf_t *conf, uint32_t clock) {
  uint32_t bus_clock = periph_apb_clk(conf->apbbus);

  uint8_t prescaler = 0;
  uint32_t prescaled_clock = bus_clock >> 1;
  const uint8_t prescaler_max = SPI_CFG1_MBR_Msk >> SPI_CFG1_MBR_Pos;
  for (; (prescaled_clock > clock) && (prescaler < prescaler_max);
       prescaler++) {
    prescaled_clock >>= 1;
  }

  /* If the callers asks for an SPI frequency of at most x, bad things will
   * happen if this cannot be met. So let's have a blown assertion
   * rather than runtime failures that require a logic analyzer to
   * debug. */
  assume(prescaled_clock <= clock);

  return prescaler;
}

static inline void enable_spi(spi_t bus) {
  // stm32u5 errata: 2.22.5 SPE may not be cleared upon MODF event
  dev(bus)->IFCR = SPI_IFCR_MODFC;
  // enable
  dev(bus)->CR1 |= SPI_CR1_SPE;
}
static inline void disable_spi(spi_t bus) {
  // clear EOT, TXTF, TIFRE and MODF
  dev(bus)->IFCR = SPI_IFCR_EOTC;
  dev(bus)->IFCR = SPI_IFCR_TXTFC;
  dev(bus)->IFCR = SPI_IFCR_TIFREC;
  dev(bus)->IFCR = SPI_IFCR_MODFC;

  // disable
  dev(bus)->CR1 &= ~SPI_CR1_SPE;
}

void spi_init(spi_t bus) {
  assume(bus < SPI_NUMOF);

  /* initialize device lock (as locked, spi_init_pins() will unlock it */
  locks[bus] = (mutex_t)MUTEX_INIT_LOCKED;
  /* trigger pin initialization */
  spi_init_pins(bus);

  periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rccmask);
  /* reset configuration */
  dev(bus)->CR1 = 0;
  dev(bus)->CFG1 = SPI_CFG1_SETTINGS;
  dev(bus)->CR2 = SPI_CR2_SETTINGS;
  dev(bus)->CFG2 = SPI_CFG2_MASTER;
  periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rccmask);
}

void spi_init_pins(spi_t bus) {
  assume(bus < SPI_NUMOF);
  if (gpio_is_valid(spi_config[bus].sclk_pin)) {
    gpio_init_af(spi_config[bus].sclk_pin, spi_config[bus].sclk_af);
  }

  if (gpio_is_valid(spi_config[bus].mosi_pin)) {
    gpio_init_af(spi_config[bus].mosi_pin, spi_config[bus].mosi_af);
  }

  if (gpio_is_valid(spi_config[bus].miso_pin)) {
    gpio_init(spi_config[bus].miso_pin, GPIO_IN);
    gpio_init_af(spi_config[bus].miso_pin, spi_config[bus].miso_af);
  }

  if (gpio_is_valid(spi_config[bus].cs_pin)) {
    gpio_init_af(spi_config[bus].cs_pin, spi_config[bus].cs_af);
  }
  mutex_unlock(&locks[bus]);
}

void spi_deinit_pins(spi_t bus) {
  assume(bus < SPI_NUMOF);
  mutex_lock(&locks[bus]);

  if (gpio_is_valid(spi_config[bus].sclk_pin)) {
    gpio_init(spi_config[bus].sclk_pin, GPIO_IN);
  }

  if (gpio_is_valid(spi_config[bus].mosi_pin)) {
    gpio_init(spi_config[bus].mosi_pin, GPIO_IN);
  }

  if (gpio_is_valid(spi_config[bus].miso_pin)) {
    gpio_init(spi_config[bus].miso_pin, GPIO_IN);
  }
}

gpio_t spi_pin_miso(spi_t bus) {
  assume(bus < SPI_NUMOF);
  return spi_config[bus].miso_pin;
}

gpio_t spi_pin_mosi(spi_t bus) {
  assume(bus < SPI_NUMOF);
  return spi_config[bus].mosi_pin;
}

gpio_t spi_pin_clk(spi_t bus) {
  assume(bus < SPI_NUMOF);
  return spi_config[bus].sclk_pin;
}

int spi_init_cs(spi_t bus, spi_cs_t cs) {
  if (bus >= SPI_NUMOF) {
    return SPI_NODEV;
  }
  if (!gpio_is_valid(cs) ||
      (((cs & SPI_HWCS_MASK) == SPI_HWCS_MASK) && (cs & ~(SPI_HWCS_MASK)))) {
    return SPI_NOCS;
  }

  if (cs == SPI_HWCS_MASK) {
    if (!gpio_is_valid(spi_config[bus].cs_pin)) {
      return SPI_NOCS;
    }
    gpio_init(spi_config[bus].cs_pin, GPIO_OUT);
    gpio_init_af(spi_config[bus].cs_pin, spi_config[bus].cs_af);
  } else {
    gpio_init((gpio_t)cs, GPIO_OUT);
    gpio_set((gpio_t)cs);
  }

  return SPI_OK;
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk) {
  assume((unsigned)bus < SPI_NUMOF);

  /* lock bus */
  mutex_lock(&locks[bus]);
#ifdef STM32_PM_STOP
  /* block STOP mode */
  pm_block(STM32_PM_STOP);
#endif
  /* enable SPI device clock */
  periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rccmask);
  /* enable device */
  if (clk != clocks[bus]) {
    prescalers[bus] = _get_prescaler(&spi_config[bus], clk);
    clocks[bus] = clk;
  }
  uint8_t br = prescalers[bus];

  DEBUG("[spi] acquire: requested clock: %" PRIu32
        " Hz, resulting clock: %" PRIu32 " Hz, BR prescaler: %u\n",
        clk, periph_apb_clk(spi_config[bus].apbbus) >> (br + 1), (unsigned)br);
  uint32_t cfg1 = SPI_CFG1_SETTINGS | (br << BR_SHIFT);
  uint32_t cr1 = 0;
  /* Settings to add to CR2 in addition to SPI_CR2_SETTINGS */
  uint32_t cfg2 = SPI_CFG2_SETTINGS;
  // if not hardware chip select
  if (cs != SPI_HWCS_MASK) {
    // SSI: internal SS signal input level
    /*This bit has an effect only when the SSM bit is set. The value of this bit
      is forced onto the peripheral SS input internally and the I/O value of the
      SS pin is ignored.*/
    cr1 |= SPI_CR1_SSI;
    // Software SS management (SSM = 1) software management of SS signal input
    /*0: SS input value is determined by the SS PAD
      1: SS input value is determined by the SSI bit
      When master uses hardware SS output (SSM = 0 and SSOE = 1) the SS signal
      input is forced to not active state internally to prevent master mode
      fault error.*/
    cfg2 |= SPI_CFG2_SSM;
  } else {
    // Hardware SS management (SSM = 0): SS output enable (SSOE = 1)
    cfg2 |= SPI_CFG2_SSOE;
  }
  cfg2 |= SPI_CFG2_MASTER | mode;

  dev(bus)->CFG1 = cfg1;
  dev(bus)->CFG2 = cfg2;
  dev(bus)->CR1 = cr1;
  /* When SPE = 1, SPI data transfer is enabled, SPI_CFG1 and SPI_CFG2
     configuration registers, CRCPOLY, UDRDR, part of SPI_AUTOCR register and
     IOLOCK bit in the
     SPI_CR1 register are write protected */
}

void spi_release(spi_t bus) {
  /* disable device and release lock */
  disable_spi(bus);
  dev(bus)->CR2 = SPI_CR2_SETTINGS; /* Clear the DMA and SSOE flags */
  dev(bus)->CFG2 = 0;
  periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rccmask);
#ifdef STM32_PM_STOP
  /* unblock STOP mode */
  pm_unblock(STM32_PM_STOP);
#endif
  mutex_unlock(&locks[bus]);
}

static inline void _wait_for_end(spi_t bus) {
  /* make sure the transfer is completed before continuing, see reference
   * manual(s) -> section 'Disabling the SPI' */
  /* When TSIZE>0, EOT and TXC signals are equal so polling of EOT is reliable
  at whatever SPI communication mode to check end of the bus activity */
  while (dev(bus)->CR2 != 0 && dev(bus)->SR & SPI_SR_EOT) {
    dev(bus)->IFCR = SPI_IFCR_EOTC;
  }
  dev(bus)->IFCR = SPI_IFCR_TXTFC;
}

static void _transfer_no_dma(spi_t bus, const void *out, void *in, size_t len) {
  // data to be sent tx
  const uint8_t *outbuf = out;
  // dest for data to receive rx
  uint8_t *inbuf = in;

  // lower 16 bits = TSIZE
  // When these bits are changed by software, the SPI must be disabled. SPE = 0
  assume(len <= UINT16_MAX);
  dev(bus)->CR2 = SPI_CR2_SETTINGS;
  // Fully featured interfaces (SPI1,SPI2) exhbit errornous behaviour when TSIZE
  // is set
  if (dev(bus) == SPI3)
    dev(bus)->CR2 |= (uint16_t)len;

  bool check_eot = (dev(bus)->CR2 & SPI_CR2_TSIZE) != 0;
  /* we need to recast the data register to uint_8 to force 8-bit access */
  volatile uint8_t *TXDR = (volatile uint8_t *)&(dev(bus)->TXDR);
  volatile uint8_t *RXDR = (volatile uint8_t *)&(dev(bus)->RXDR);

  /*To restart the internal
    state machine properly, SPI is strongly suggested to be disabled and
    re-enabled before next transaction starts despite its setting is not
    changed.*/
  disable_spi(bus);
  enable_spi(bus);

  if (dev(bus)->CFG2 & SPI_CFG2_MASTER) {
    assume(dev(bus)->CR1 & SPI_CR1_SPE);
    // CSTART: master transfer start This bit can be set by software if SPI is
    // *enabled* only
    dev(bus)->CR1 |= SPI_CR1_CSTART;
    assume(dev(bus)->CR1 & SPI_CR1_CSTART);
  }

  /* transfer data, use shortpath if only sending data */
  if (!inbuf) {
    for (size_t i = 0; i < len; i++) {
      while (!(dev(bus)->SR & SPI_SR_TXP)) {
      }
      *TXDR = outbuf[i];
      while ((dev(bus)->SR & SPI_SR_RXP)) { /* busy wait */
                                            // discard incoming bytes
        (void)*RXDR;
      }
    }
  } else if (!outbuf) {
    for (size_t i = 0; i < len; i++) {
      while (!(dev(bus)->SR & SPI_SR_TXP)) { /* busy wait */
      }
      *TXDR = 0;
      // The RXP is cleared as soon as less data than complete single packet is
      // available in the RxFIFO
      while (!(dev(bus)->SR & SPI_SR_RXP)) { /* busy wait */
      }
      inbuf[i] = *RXDR;
    }
  } else {
    uint16_t tx_remainder = len, rx_remainder = len;

    while ((tx_remainder > 0 || rx_remainder > 0)) {
      if (tx_remainder > 0 && dev(bus)->SR & SPI_SR_TXP) {
        *TXDR = outbuf[len - tx_remainder];
        tx_remainder--;
      }
      while (rx_remainder > 0 && (dev(bus)->SR & SPI_SR_RXP)) {
        inbuf[len - rx_remainder] = *RXDR;
        rx_remainder--;
      }
      if (check_eot && dev(bus)->SR & SPI_SR_EOT) {
        break;
      }
    }
  }

  /* wait until everything is finished and empty the receive buffer */
  // The standard disable procedure is based on polling EOT and/or TXC status to
  // check if a transmission session is (fully) completed
  while (check_eot && !(dev(bus)->SR & SPI_SR_EOT)) {
  }

  while ((dev(bus)->SR & SPI_SR_RXWNE) ||
         ((dev(bus)->SR & SPI_SR_RXPLVL) != 0)) {
    (void)*RXDR;
  }

  _wait_for_end(bus);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont, const void *out,
                        void *in, size_t len) {
  /* make sure at least one input or one output buffer is given */
  assume(out || in);

  /* activate the given chip select line by pulling the line low */
  if ((cs != SPI_HWCS_MASK) && gpio_is_valid(cs)) {
    gpio_clear((gpio_t)cs);
  }

  _transfer_no_dma(bus, out, in, len);

  /* release the chip select if not specified differently */
  if ((!cont) && gpio_is_valid(cs)) {
    if (cs != SPI_HWCS_MASK) {
      gpio_set((gpio_t)cs);
    }
  }
  // disable_spi(bus);
}
