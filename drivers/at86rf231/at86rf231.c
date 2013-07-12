/**
  * at86rf231.c - Implementation of at86rf231 functions.
  * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */
  
#include <at86rf231.h>

//#define ENABLE_DEBUG
#include <debug.h>

static uint16_t radio_pan;
static uint8_t  radio_channel;
static uint16_t radio_address;
static uint64_t radio_address_long;

int transceiver_pid;

void at86rf231_init(int tpid)
{
  transceiver_pid = tpid;

  at86rf231_gpio_spi_interrupts_init();

  at86rf231_reset();

  // TODO : Enable addr decode, auto ack, auto crc
  // and configure security, power, channel, pan

  at86rf231_switch_to_rx();
}

void at86rf231_switch_to_rx(void)
{
  at86rf231_disable_interrupts();
  // Send a FORCE TRX OFF command
  at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__FORCE_TRX_OFF);

  // Reset IRQ to TRX END only
  at86rf231_reg_write(AT86RF231_REG__IRQ_MASK, AT86RF231_IRQ_STATUS_MASK__TRX_END);

  // Read IRQ to clear it
  at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);

  // Enable IRQ interrupt
  at86rf231_enable_interrupts();

  // Start RX
  at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__RX_ON);

  // wait until it is on RX_ON state
  uint8_t status;
  uint8_t max_wait = 100;   // TODO : move elsewhere, this is in 10us

  do
  {
    status = at86rf231_get_status();

    vtimer_usleep(10);

    if (!--max_wait)
    {
      printf("at86rf231 : ERROR : could not enter RX_ON mode");
      break;
    }
  } 
  while ((status & AT86RF231_TRX_STATUS_MASK__TRX_STATUS) != AT86RF231_TRX_STATUS__RX_ON);
}

void at86rf231_rx_irq(void)
{
  at86rf231_rx_handler();
}

uint16_t at86rf231_set_address(uint16_t address)
{
  radio_address = address;

  at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_0, (uint8_t)(0x0F & radio_address));
  at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_1, (uint8_t)(radio_address >> 8));

  return radio_address;
}

uint16_t at86rf231_get_address(void)
{
  return radio_address;
}

uint64_t at86rf231_set_address_long(uint64_t address)
{
  radio_address_long = address;

  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_0, (uint8_t)(0x0F & radio_address));
  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_1, (uint8_t)(radio_address >> 8));
  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_2, (uint8_t)(radio_address >> 16));
  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_3, (uint8_t)(radio_address >> 24));
  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_4, (uint8_t)(radio_address >> 32));
  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_5, (uint8_t)(radio_address >> 40));
  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_6, (uint8_t)(radio_address >> 48));
  at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_7, (uint8_t)(radio_address >> 56));

  return radio_address_long;
}

uint64_t at86rf231_get_address_long(void)
{
  return radio_address_long;
}

uint16_t at86rf231_set_pan(uint16_t pan)
{
  radio_pan = pan;

  at86rf231_reg_write(AT86RF231_REG__PAN_ID_0, (uint8_t)(0x0F & radio_pan));
  at86rf231_reg_write(AT86RF231_REG__PAN_ID_1, (uint8_t)(radio_pan >> 8));

  return radio_pan;
}

uint16_t at86rf231_get_pan(void)
{
  return radio_pan;
}

uint8_t at86rf231_set_channel(uint8_t channel)
{
  radio_channel = channel;

  if (channel < RF86RF231_MIN_CHANNEL ||
      channel > RF86RF231_MAX_CHANNEL) {
      radio_channel = RF86RF231_MAX_CHANNEL;
  }
  
  at86rf231_reg_write(AT86RF231_REG__PHY_CC_CCA, AT86RF231_PHY_CC_CCA_DEFAULT__CCA_MODE | radio_channel);

  return radio_channel;
}

uint8_t at86rf231_get_channel(void)
{ 
  return radio_channel;
  //return at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA) & 0x0F;
}

void at86rf231_set_monitor(uint8_t mode)
{
  // TODO
}

void at86rf231_swap_fcf_bytes(uint8_t *buf)
{
  uint8_t tmp;
  tmp = buf[1];
  buf[1] = buf[2];
  buf[2] = tmp;
}

