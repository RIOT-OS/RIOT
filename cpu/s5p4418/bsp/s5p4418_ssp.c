//------------------------------------------------------------------------------
//
//  Copyright (C) 2017
//
//  Module      : TIMER
//  File        : s5p4418_ssp.c
//  Description : S5P4418 SPI/SSP FUNCTIONS
//  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)
//  History     :
//------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <io.h>
#include "s5p4418_ssp.h"
#include "s5p4418_clk.h"
#include "s5p4418_gpio.h"
#include "s5p4418_chip.h"
#include "s5p4418_rstcon.h"

// SSP Status Register - SSP_SR
#define SSP_SR_MASK_TFE		(0x1UL << 0) /* Transmit FIFO empty */
#define SSP_SR_MASK_TNF		(0x1UL << 1) /* Transmit FIFO not full */
#define SSP_SR_MASK_RNE		(0x1UL << 2) /* Receive FIFO not empty */
#define SSP_SR_MASK_RFF		(0x1UL << 3) /* Receive FIFO full */
#define SSP_SR_MASK_BSY		(0x1UL << 4) /* Busy Flag */

// GPIO base address table
const ssp_typedef *ssp_base_addr_table[3] = {SSP0, SSP1, SSP2};

/**
 *  @brief      Set value on NCS pin
 *  @param[in]  ssp_channel_t SSP channel
 *  @param[in]  bool Value to set
 *  @return     None
 */
void s5p4418_ssp_ncs_out(ssp_channel_t ch, bool val)
{
    switch(ch)
    {
        case SSP_CH0:
            s5p4418_gpio_set(TO_GPIO(PORTC, 30), val);
        break;

        case SSP_CH1:
            s5p4418_gpio_set(TO_GPIO(PORTE, 15), val);
        break;

        case SSP_CH2:
            s5p4418_gpio_set(TO_GPIO(PORTC, 10), val);
        break;

        default:
        break;
    }
}

/**
 *  @brief      Configure SSP CS pins function
 *  @param[in]  ssp_channel_t SSP channel
 *              uint8_t       GPIO number 
 *  @return     None
 */
void s5p4418_ssp_cs_config(uint8_t gpio)
{
    s5p4418_gpio_set_alt(gpio, GPIO_ALT0);
    s5p4418_gpio_set_dir(gpio, GPIO_DIR_OUT);
    s5p4418_gpio_set_drv(gpio, GPIO_DRV_MEDIUM);
    s5p4418_gpio_set_rate(gpio, GPIO_RATE_NORM);
}

/**
 *  @brief      Configure SSP pins function
 *  @param[in]  ssp_channel_t SSP channel
 *  @return     None
 */
void s5p4418_ssp_gpio_config(ssp_channel_t ch)
{
    switch(ch)
    {
        case SSP_CH0:
            s5p4418_gpio_set_alt(TO_GPIO(PORTC, 29), GPIO_ALT1);    // SSP0_CLK
            s5p4418_gpio_set_alt(TO_GPIO(PORTC, 31), GPIO_ALT1);    // SSP0_MOSI
            s5p4418_gpio_set_alt(TO_GPIO(PORTD,  0), GPIO_ALT1);    // SSP0_MISO
            //s5p4418_gpio_set_alt(TO_GPIO(PORTC, 30), GPIO_ALT0);    // SSP0_NCS
            //s5p4418_gpio_set_dir(TO_GPIO(PORTC, 30), GPIO_DIR_OUT); // SSP0_NCS as output
        break;

        case SSP_CH1:
            s5p4418_gpio_set_alt(TO_GPIO(PORTE, 14), GPIO_ALT2);    // SSP1_CLK
            s5p4418_gpio_set_alt(TO_GPIO(PORTE, 18), GPIO_ALT2);    // SSP1_MOSI
            s5p4418_gpio_set_alt(TO_GPIO(PORTE, 19), GPIO_ALT2);    // SSP1_MISO
            //s5p4418_gpio_set_alt(TO_GPIO(PORTE, 15), GPIO_ALT0);    // SSP1_NCS
            //s5p4418_gpio_set_dir(TO_GPIO(PORTE, 15), GPIO_DIR_OUT); // SSP1_NCS as output
        break;

        case SSP_CH2:
            s5p4418_gpio_set_alt(TO_GPIO(PORTC,  9), GPIO_ALT2);    // SSP2_CLK
            s5p4418_gpio_set_alt(TO_GPIO(PORTC, 10), GPIO_ALT2);    // SSP2_MOSI
            s5p4418_gpio_set_alt(TO_GPIO(PORTC, 12), GPIO_ALT2);    // SSP2_MISO
            //s5p4418_gpio_set_alt(TO_GPIO(PORTC, 10), GPIO_ALT1);    // SSP2_NCS
            //s5p4418_gpio_set_dir(TO_GPIO(PORTC, 10), GPIO_DIR_OUT); // SSP2_NCS as output
        break;

        default:
        break;
    }
}

/**
 *  @brief      Configure SSP output clock rate
 *  @param[in]  ssp_channel_t SSP channel
 *  @param[in]  uint32_t Clock rate (Hz) (Clock rate <= 25000000 Hz)
 *  @return     bool true  : Configure successfully
 *                   false : Configure failed
 */
bool s5p4418_ssp_set_lock_rate(ssp_channel_t ch, uint32_t clock_freq)
{
    ssp_typedef *SSPx;
    char name[10] = {0};
    struct clk *clk = NULL;
    uint32_t CPSDVR, n;

    if((ch > SSP_CH2) || (clock_freq == 0) || (clock_freq > 25000000))
    {
        return false;
    }

    /* Get SSP and CLK gen base address */
    SSPx = (ssp_typedef *) ssp_base_addr_table[ch];

    if(SSPx == NULL)
    {
       return false;
    }

    /* Fsspclk(min) = 2 x Fsspclkout (max) [Master mode]
     * Fsspclk(min) = 12 x Fsspclkin (max) [Slave mode]
     * Fsspclk(max) = 254 x 256 x Fsspclkout (min) [Master mode]
     * Fsspclk(max) = 254 x 256 x Fsspclkin (min) [Slave mode]
     * Fsspclkout = Fsspclk / (CPSDVR x (1 + SCR))
     */

    /* Chose Fsspclk = 50,000,000 Hz
     * CLKDIV0 + 1 = 16 => CLKDIV0 = 15
     *
     * SSP clock prescaler register setting
     * Fsspclkout = Fsspclk / (CPSDVR x (1 + SCR))
     * Chose SCR = 0
     * Fsspclkout = 50,000,000 / CPSDVR
     * Where CPSDVR = 2*n
     */

    sprintf(name, "s5p4418-spi%d", ch);
    clk = clk_get(NULL, name);
    clk_set_rate(clk, 50000000);
    clk_enable(clk);

    /* Reset SSP0 IP */
    s5p4418_ip_reset(RESET_ID_SSP0_P);
    s5p4418_ip_reset(RESET_ID_SSP0);

    /* Disable SSP */
    SSPx->SSPCR1 &= ~(1 << 1);

    /* Configure prescaler */
    n = 25000000/clock_freq;
    CPSDVR = (uint8_t)(2*n);

    /* Setup SRC & CPSDVR */
    SSPx->SSPCR0 &= (uint32_t)(~(0xFF<<8));
    SSPx->SSPCR0 |= (uint32_t)(1<<8);

    SSPx->SSPCPSR = CPSDVR;

    return true;
}

/**
 *  @brief      Initialize SSP
 *  @param[in]  ssp_info_t SSP information
 *  @return     bool true  : Initialize successfully
 *                   false : Initialize failed
 */
bool s5p4418_ssp_init(ssp_info_t ssp_info)
{
    ssp_typedef *SSPx;
    ssp_channel_t ch;
    uint32_t clock_freq;

    /* Get SSP channel and clock rate */
    ch = ssp_info.channel;
    clock_freq = ssp_info.clock_freq;

    /* Configure SSP pad function */
    // s5p4418_ssp_gpio_config(ch);

    /* Configure SSP clock rate */
    if (s5p4418_ssp_set_lock_rate(ch, clock_freq) == false)
    {
        return false;
    }

    /* Get SSP base address */
    SSPx = (ssp_typedef *) ssp_base_addr_table[ch];

    if(SSPx == NULL)
    {
        return false;
    }

    /* Disable SPI */
    SSPx->SSPCR1 &= ~(1 << 1);
    SSPx->SSPCR1 |=  (1 << 3); // SOD = 1

    /* Set SPI mode */
    SSPx->SSPCR0 &= ~(0x03 << 6);
    SSPx->SSPCR0 |= (ssp_info.mode << 6);

    /* Protocol setup */
    SSPx->SSPCR0 &= ~(0x03 << 4);
    SSPx->SSPCR0 |= (ssp_info.protocol << 4);

    /* Data size setup */
    SSPx->SSPCR0 &= ~(0x0F);
    SSPx->SSPCR0 |= (ssp_info.data_size);

    /* Configure device mode */
    SSPx->SSPCR1 &= ~(0x03 << 2);
    SSPx->SSPCR1 |= (ssp_info.device_mode << 2);

    /* Disable DMA */
    SSPx->SSPDMACR = 0;

    /* Disable interrupt */
    SSPx->SSPIMSC = 0;

    return true;
}

/**
 *  @brief      SSP write/read byte
 *  @param[in]  ssp_channel_t SSP channel number
 *  @param[in]  uint8_t* transmit data
 *  @param[out] uint8_t* receive data
 *  @return     None
 */
void s5p4418_ssp_wr_byte(ssp_channel_t ch, uint8_t *txdata, uint8_t *rxdata)
{
    ssp_typedef *SSPx;
    SSPx = (ssp_typedef *)ssp_base_addr_table[ch];

    if (ch > SSP_CH2)
    {
        printf("%s : Error! invalid ssp channel\r\n", __FUNCTION__);
        return;
    }

    /* Put byte into TX FIFO buffer */
    SSPx->SSPDR = (uint32_t)*txdata;

    /* Enable SPI */
    SSPx->SSPCR1 |= (1 << 1);

    /* Wait until TX FIFO is empty */
    while (!(SSPx->SSPSR & SSP_SR_MASK_TFE));

    /* Wait until RX FIFO is not empty */
    while(!(SSPx->SSPSR & SSP_SR_MASK_RNE));

    /* Read RX FIFO */
   *rxdata = SSPx->SSPDR;

    /* Disable SPI */
    SSPx->SSPCR1 &= ~(1 << 1);
}
