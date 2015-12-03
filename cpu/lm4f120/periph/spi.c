/*
 * Copyright (C) 2015 Marc Poulhiès
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */
#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#if SPI_0_EN  

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1] = MUTEX_INIT,
#endif
#if SPI_2_EN
    [SPI_2] = MUTEX_INIT,
#endif
#if SPI_3_EN
    [SPI_3] = MUTEX_INIT,
#endif
};

static const struct spi_conf_s {
  unsigned long ssi_sysctl;
  unsigned long ssi_base;
  unsigned long gpio_sysctl;
  unsigned long gpio_port;
  struct spi_pin_conf_s {
    unsigned long clk;
    unsigned long fss;
    unsigned long rx;
    unsigned long tx;
    unsigned long mask;
  } pins;
} spi_confs[] = {
#if SPI_0_EN
  [SPI_0] = {
    .ssi_sysctl = SYSCTL_PERIPH_SSI0,
    .ssi_base = SSI0_BASE,
    .gpio_sysctl = SYSCTL_PERIPH_GPIOA,
    .gpio_port = GPIO_PORTA_BASE,
    .pins={
      .clk = GPIO_PA2_SSI0CLK,
      .fss = GPIO_PA3_SSI0FSS,
      .rx = GPIO_PA4_SSI0RX,
      .tx = GPIO_PA5_SSI0TX,
      .mask = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5
    }
  },
#endif
};

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
  unsigned long initialData = 0;
  unsigned long mode = 0;
  unsigned long bitrate;

  if (dev >= SPI_NUMOF) {
    return -1;
  }

  spi_poweron(dev);
  ROM_SSIDisable(spi_confs[dev].ssi_base);
  ROM_SysCtlPeripheralEnable(spi_confs[dev].gpio_sysctl);

  ROM_GPIOPinConfigure(spi_confs[dev].pins.clk);
  ROM_GPIOPinConfigure(spi_confs[dev].pins.fss);
  ROM_GPIOPinConfigure(spi_confs[dev].pins.rx);
  ROM_GPIOPinConfigure(spi_confs[dev].pins.tx);
  
  ROM_GPIOPinTypeSSI(spi_confs[dev].gpio_port, spi_confs[dev].pins.mask);

  switch(speed)
    {
    case SPI_SPEED_100KHZ:
      bitrate = 100000;
      break;
    case SPI_SPEED_1MHZ:
      bitrate = 1000000;
      break;
    case SPI_SPEED_5MHZ:
      bitrate = 5000000;
      break;
    case SPI_SPEED_10MHZ:
      bitrate = 10000000;
      break;
    default:
    case SPI_SPEED_400KHZ:
      bitrate = 400000;
      break;

    }

        /*
          Polarity Phase        Mode
             0     0   SSI_FRF_MOTO_MODE_0
             0     1   SSI_FRF_MOTO_MODE_1
             1     0   SSI_FRF_MOTO_MODE_2
             1     1   SSI_FRF_MOTO_MODE_3
        */
  switch(conf)
    {
    case SPI_CONF_FIRST_RISING: /**< first data bit is transacted on the first rising SCK edge */
      mode = SSI_FRF_MOTO_MODE_0;
      break;
    case SPI_CONF_SECOND_RISING:/**< first data bit is transacted on the second rising SCK edge */
      mode = SSI_FRF_MOTO_MODE_1;
      break;
    case SPI_CONF_FIRST_FALLING:/**< first data bit is transacted on the first falling SCK edge */
      mode = SSI_FRF_MOTO_MODE_2;
      break;
    case SPI_CONF_SECOND_FALLING:/**< first data bit is transacted on the second falling SCK edge */
      mode = SSI_FRF_MOTO_MODE_3;
      break;
    }	    

  ROM_SSIClockSourceSet(spi_confs[dev].ssi_base, SSI_CLOCK_SYSTEM);
  ROM_SSIConfigSetExpClk(spi_confs[dev].ssi_base, ROM_SysCtlClockGet(),
			 mode, /* protocol */
			 SSI_MODE_MASTER,     /* mode/operation */
			 bitrate,	      /* bitrate */
			 8);		      /* data width */

  ROM_SSIEnable(spi_confs[dev].ssi_base);

  while(ROM_SSIDataGetNonBlocking(spi_confs[dev].ssi_base, &initialData));
  return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    /* TODO */
    return -1;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* TODO*/
}

int spi_acquire(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}


int spi_transfer_byte(spi_t dev, char out, char *in)
{
  unsigned long lin;
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    DEBUG("SPI byte transfer (before in: %d, out: %d) ", *in, out);
    ROM_SSIDataPut(spi_confs[dev].ssi_base, out);

    // wait until tx over
    while(ROM_SSIBusy(spi_confs[dev].ssi_base));

    ROM_SSIDataGet(spi_confs[dev].ssi_base, &lin);

    // wait until rx over
    while(ROM_SSIBusy(spi_confs[dev].ssi_base));

    *in = (char)lin;
    DEBUG("in %d, out %d\n", *in, out);

    return 0;
}

void spi_poweron(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return;
    }
    ROM_SysCtlPeripheralEnable(spi_confs[dev].ssi_sysctl);
}

void spi_poweroff(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return;
    }
    ROM_SSIDisable(spi_confs[dev].ssi_base);
    ROM_SysCtlPeripheralDisable(spi_confs[dev].ssi_sysctl);
}

#endif /* SPI_0_EN */

