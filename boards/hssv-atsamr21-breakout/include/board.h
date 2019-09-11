#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(1)
#define XTIMER_CHAN         (0)
/** @} */

/**
 * @name    AT86RF233 configuration
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PB, 15)

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

/**
 * @brief Define exposed pin aliases
 */
#define HSSV_ATSAMR21_SPI_CS       GPIO_PIN(PA, 17)
#define HSSV_ATSAMR21_SPI_MOSI     GPIO_PIN(PA, 18)
#define HSSV_ATSAMR21_SPI_MISO     GPIO_PIN(PA, 16)
#define HSSV_ATSAMR21_SPI_SCLK     GPIO_PIN(PA, 19)
#define HSSV_ATSAMR21_GPIO         GPIO_PIN(PA, 28)
#define HSSV_ATSAMR21_RX1          GPIO_PIN(PA, 7)
#define HSSV_ATSAMR21_TX1          GPIO_PIN(PA, 8)
#define HSSV_ATSAMR21_FEM_RX_TX    GPIO_PIN(PA, 13)
#define HSSV_ATSAMR21_ANT_DIV_P    GPIO_PIN(PA, 12)
//#define HSSV_ATSAMR21_USB_DM       GPIO_PIN(PA, 24)
//#define HSSV_ATSAMR21_USB_DP       GPIO_PIN(PA, 25)
#define HSSV_ATSAMR21_RX           GPIO_PIN(PA, 15)
#define HSSV_ATSAMR21_TX           GPIO_PIN(PA, 14)
#define HSSV_ATSAMR21_AREF         GPIO_PIN(PA, 4)
#define HSSV_ATSAMR21_ADC2         GPIO_PIN(PA, 5)
#define HSSV_ATSAMR21_ADC1         GPIO_PIN(PA, 6)
//#define HSSV_ATSAMR21_SWCLK        GPIO_PIN(PA, 30)
//#define HSSV_ATSAMR21_SWDIO        GPIO_PIN(PA, 31)
#define HSSV_ATSAMR21_SCL          GPIO_PIN(PA, 23)
#define HSSV_ATSAMR21_SDA          GPIO_PIN(PA, 22)

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
