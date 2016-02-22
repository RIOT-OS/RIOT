/*
 * Copyright (C) 2016 FH Campus Wien
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Mathias Tausig <mathias.tausig@fh-campuswien.ac.at>
 *
 * @}
 */

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_internal.h"
#include "periph/hwrng.h"

static at86rf2xx_t dev;

void hwrng_init(void)
{
  const at86rf2xx_params_t *p = &at86rf2xx_params[0];
  at86rf2xx_init(&dev,
		 p->spi,
		 p->spi_speed,
		 p->cs_pin,
		 p->int_pin,
		 p->sleep_pin,
		 p->reset_pin);
  //at86rf2xx_init return a negative value in case of an error. We are ignoring it, since hwrng_init has no error handling
}

void hwrng_read(uint8_t *buf, unsigned int num)
{
  for(unsigned int byteCount=0; byteCount<num;++byteCount) {
    uint8_t rnd = 0;
    for(uint8_t i = 0; i<4;++i) {
      //bit 5 and 6 of the AT86RF2XX_REG__PHY_RSSI register contain the RND_VALUE
      uint8_t regVal = at86rf2xx_reg_read (&dev, AT86RF2XX_REG__PHY_RSSI)
	& 0x60; 
      //shift the two random bits first to the right and then to the correct position of the return byte
      regVal = regVal >> 5;
      regVal = regVal << 2*i;
      rnd |= regVal;
    }
    buf[byteCount] = rnd;
  }
}
