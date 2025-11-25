/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 *
 * @{
 *
 * @file
 * @brief       Flexcomm interrupt dispatch.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph_conf.h"
#include "vectors_qn908x.h"
#include "flexcomm.h"

#include "vendor/drivers/fsl_clock.h"

#define ENABLE_DEBUG 0

#include "debug.h"

int flexcomm_init(FLEXCOMM_Type *dev, flexcom_pselid_t mode)
{
    static const clock_ip_name_t flexcomm_clocks[] = FLEXCOMM_CLOCKS;
    int flexcomm_num = flexcomm_instance_from_addr(dev);

    if (flexcomm_num < 0 || flexcomm_num >= (int)ARRAY_SIZE(flexcomm_clocks)) {
        DEBUG("Invalid flexcomm_num: %d\n", flexcomm_num);
        return -1;
    }
    CLOCK_EnableClock(flexcomm_clocks[flexcomm_num]);
    /* Reset the flexcomm. */
    SYSCON->RST_SW_SET = 1u << flexcomm_num;
    SYSCON->RST_SW_CLR = 1u << flexcomm_num;

    /* Check that the mode is present in the FLEXCOMM.
     * Bits 4, 5 and 6 tell whether the UART, SPI and I2C respectively are
     * present. */
    if ((dev->PSELID & (1u << (mode + 3))) == 0) {
        DEBUG("Mode %d not present in FLEXCOMM%d\n", (int)mode, flexcomm_num);
        return -1;
    }

    /* This also locks the peripheral so it can't be changed until device or
     * peripheral is reset. */
    dev->PSELID = (dev->PSELID & ~FLEXCOMM_PSELID_PERSEL_MASK) |
                  FLEXCOMM_PSELID_LOCK_MASK |
                  FLEXCOMM_PSELID_PERSEL(mode);
    return flexcomm_num;
}

int flexcomm_instance_from_addr(const FLEXCOMM_Type *dev)
{
    static const FLEXCOMM_Type *flexcomm_addrs[] = FLEXCOMM_BASE_PTRS;

    for (uint8_t i = 0; i < ARRAY_SIZE(flexcomm_addrs); i++) {
        if (flexcomm_addrs[i] == dev) {
            return i;
        }
    }
    DEBUG("Invalid FLEXCOMM address.\n");
    return -1;
}

#ifdef MODULE_PERIPH_UART
extern void isr_flexcomm_uart(USART_Type *dev, uint32_t flexcomm_num);
#endif /* MODULE_PERIPH_UART */

#ifdef MODULE_PERIPH_SPI
extern void isr_flexcomm_spi(SPI_Type *dev, uint32_t flexcomm_num);
#endif /* MODULE_PERIPH_SPI */

#ifdef MODULE_PERIPH_I2C
extern void isr_flexcomm_i2c(I2C_Type *dev, uint32_t flexcomm_num);
#endif /* MODULE_PERIPH_I2C */

/**
 * @brief General Flexcomm interrupt handler dispatch.
 *
 * The driver that should get an interrupt from the flexcomm depends on the
 * currently configured one, which can be obtained from the PSELID.
 */
static void isr_flexcomm(void *flexcomm, uint32_t flexcomm_num)
{
    switch (((FLEXCOMM_Type *)flexcomm)->PSELID & FLEXCOMM_PSELID_PERSEL_MASK) {
#ifdef MODULE_PERIPH_UART
    case FLEXCOMM_ID_UART:
        isr_flexcomm_uart((USART_Type *)(flexcomm), flexcomm_num);
        return;
#endif  /* MODULE_PERIPH_UART */
#ifdef MODULE_PERIPH_SPI
    case FLEXCOMM_ID_SPI:
        isr_flexcomm_spi((SPI_Type *)(flexcomm), flexcomm_num);
        return;
#endif  /* MODULE_PERIPH_SPI */
#ifdef MODULE_PERIPH_I2C
    case FLEXCOMM_ID_I2C:
        isr_flexcomm_i2c((I2C_Type *)(flexcomm), flexcomm_num);
        return;
#endif  /* MODULE_PERIPH_I2C */
    default:
        cortexm_isr_end();
    }
}

#ifdef FLEXCOMM0
void isr_flexcomm0(void)
{
    isr_flexcomm(FLEXCOMM0, 0);
}
#endif /* FLEXCOMM0 */

#ifdef FLEXCOMM1
void isr_flexcomm1(void)
{
    isr_flexcomm(FLEXCOMM1, 1);
}
#endif /* FLEXCOMM1 */

#ifdef FLEXCOMM2
void isr_flexcomm2(void)
{
    isr_flexcomm(FLEXCOMM2, 2);
}
#endif /* FLEXCOMM2 */

#ifdef FLEXCOMM3
void isr_flexcomm3(void)
{
    isr_flexcomm(FLEXCOMM3, 3);
}
#endif /* FLEXCOMM3 */
