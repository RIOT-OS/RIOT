#ifndef SX1276_PARAMS_H_
#define SX1276_PARAMS_H_

#ifndef SX1276_PARAM_DIO0
#define SX1276_PARAM_DIO0 GPIO_PIN(PA, 13)
#endif

#ifndef SX1276_PARAM_DIO1 
#define SX1276_PARAM_DIO1 GPIO_PIN(PA, 7)
#endif

#ifndef SX1276_PARAM_DIO2 
#define SX1276_PARAM_DIO2 GPIO_PIN(PA, 6)
#endif

#ifndef SX1276_PARAM_DIO3 
#define SX1276_PARAM_DIO3 GPIO_PIN(PA, 18)
#endif

#ifndef SX1276_PARAM_RESET 
#define SX1276_PARAM_RESET GPIO_PIN(PA, 28)
#endif

#ifndef SX1276_PARAM_SPI
#define SX1276_PARAM_SPI (SPI_DEV(1))
#endif

#ifndef SX1276_PARAM_SPI_SPEED
#define SX1276_PARAM_SPI_SPEED (SPI_CLK_1MHZ)
#endif

#ifndef SX1276_PARAM_SPI_NSS 
#define SX1276_PARAM_SPI_NSS GPIO_PIN(PA, 19)
#endif

#ifndef SX1276_PARAM_SPI_MODE 
#define SX1276_PARAM_SPI_MODE SPI_MODE_0
#endif

//TODO: add PARAMS_DEFAULT
#define SX1276_PARAMS_DEFAULT    {.spi = SX1276_PARAM_SPI, \
                                  .nss_pin = SX1276_PARAM_SPI_NSS, \
                                  .reset_pin = SX1276_PARAM_RESET, \
                                  .dio0_pin = SX1276_PARAM_DIO0, \
                                  .dio1_pin = SX1276_PARAM_DIO1, \
                                  .dio2_pin = SX1276_PARAM_DIO2, \
                                  .dio3_pin = SX1276_PARAM_DIO3}

static const sx1276_params_t sx1276_params[] =
{
#ifdef SX1276_PARAMS_BOARD
    SX1276_PARAMS_BOARD,
#else
    SX1276_PARAMS_DEFAULT,
#endif
};


#endif
