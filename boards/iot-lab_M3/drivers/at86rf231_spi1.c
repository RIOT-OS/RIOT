#include <stdio.h>
#include <stddef.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "spi.h"
#include "periph_conf.h"
#include "at86rf231_spi1.h"

void at86rf231_spi1_init(void)
{
    // SPI_InitTypeDef SPI_InitStructure;
    /* RCC */
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    // RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    /* GPIO */

    /* Configure SPI MASTER pins */
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIOA->CRL &= ~(0xf << (5 * 4));
    GPIOA->CRL |= (0xB << (5 * 4));
    GPIOA->CRL &= ~(0xf << (7 * 4));
    GPIOA->CRL |= (0xB << (7 * 4));

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);
    gpio_init_in(GPIO_6, GPIO_NOPULL);

    /* SPI
     * NOTE: APB2 is 72MHz, prescaler 16 => SPI @ 4.5 MHz, radio spi max is 7.5MHz
     * Clock idle low, rising edge
     */
    // SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    // SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    // SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    // SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    // SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    // SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    // SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    // SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    // SPI_InitStructure.SPI_CRCPolynomial = 7;
    //SPI_Init(SPI1, &SPI_InitStructure);
#warning implement spi
    /* Enable interrupt */
    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
    /* Enable SPI */
    // SPI_Cmd(SPI1, ENABLE);
}

uint8_t at86rf231_spi_transfer_byte(uint8_t byte)
{
    char ret;

    spi_transfer_byte(SPI_0, byte?byte:0, byte?0:&ret );

    return ret;
}

void at86rf231_spi_transfer(const uint8_t *data_out, uint8_t *data_in, uint16_t length)
{
    spi_transfer_bytes(SPI_0, (char*)data_out, (char*)data_in, length);
}
