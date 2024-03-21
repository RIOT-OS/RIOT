/*
 * Copyright (C) 2013 INRIA
 *               2014 Freie Universität Berlin
 *               2016 TriaGnoSys GmbH
 *               2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 OTA keys S.A.
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of the kernel cpu functions
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Víctor Ariño <victor.arino@zii.aero>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Oleg Artamonov <oleg@unwds.com>
 * @author      Francisco Molina <francisco.molina@inria.cl>
 *
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "stdio_base.h"
#include "stmclk.h"
#include "periph_cpu.h"
#include "periph/init.h"
#include "periph/gpio.h"
#include "periph/vbat.h"
#include "board.h"
#include "pm_layered.h"

#if defined (CPU_FAM_STM32L4) || defined (CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5)
#define BIT_APB_PWREN       RCC_APB1ENR1_PWREN
#elif defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32C0)
#define BIT_APB_PWREN       RCC_APBENR1_PWREN
#elif !defined(CPU_FAM_STM32MP1)
#define BIT_APB_PWREN       RCC_APB1ENR_PWREN
#endif

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32L1)

#define STM32_CPU_MAX_GPIOS    (12U)

#if defined(CPU_FAM_STM32L1)
#define GPIO_CLK              (AHB)
#define GPIO_CLK_ENR          (RCC->AHBENR)
#define GPIO_CLK_ENR_MASK     (0x0000FFFF)
#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
#define GPIO_CLK              (AHB)
#define GPIO_CLK_ENR          (RCC->AHBENR)
#define GPIO_CLK_ENR_MASK     (0xFFFF0000)
#elif defined(CPU_FAM_STM32WL)
#define GPIO_CLK              (AHB2)
#define GPIO_CLK_ENR          (RCC->AHB2ENR)
#define GPIO_CLK_ENR_MASK     (0x00000087)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7)
#define GPIO_CLK              (AHB1)
#define GPIO_CLK_ENR          (RCC->AHB1ENR)
#define GPIO_CLK_ENR_MASK     (0x0000FFFF)
#elif defined(CPU_FAM_STM32F1)
#define GPIO_CLK              (APB2)
#define GPIO_CLK_ENR          (RCC->APB2ENR)
#define GPIO_CLK_ENR_MASK     (0x000001FC)
#endif

#ifndef DISABLE_JTAG
#define DISABLE_JTAG 0
#endif

/**
 * @brief   Initialize gpio to AIN
 *
 * stm32f need to have all there pins initialized to AIN so the consumption
 * of the input Schmitt trigger is saved when running in STOP mode.
 *
 * @see https://comm.eefocus.com/media/download/index/id-1013834
 */
static void _gpio_init_ain(void)
{
    uint32_t ahb_gpio_clocks;

    /* enable GPIO clock and save GPIO clock configuration */
    ahb_gpio_clocks = GPIO_CLK_ENR & GPIO_CLK_ENR_MASK;
    periph_clk_en(GPIO_CLK, GPIO_CLK_ENR_MASK);

    /* switch all GPIOs to AIN mode to minimize power consumption */
    for (uint8_t i = 0; i < STM32_CPU_MAX_GPIOS; i++) {
        GPIO_TypeDef *port;
        port = (GPIO_TypeDef *)(GPIOA_BASE + i*(GPIOB_BASE - GPIOA_BASE));
        if (IS_GPIO_ALL_INSTANCE(port)) {
            if (!DISABLE_JTAG) {
#if defined(CPU_FAM_STM32F1)
                switch (i) {
                    /* preserve JTAG pins on PORTA and PORTB */
                    case 0:
                        port->CRL = GPIO_CRL_CNF;
                        port->CRH = GPIO_CRH_CNF & 0x000FFFFF;
                        break;
                    case 1:
                        port->CRL = GPIO_CRL_CNF & 0xFFF00FFF;
                        port->CRH = GPIO_CRH_CNF;
                        break;
                    default:
                        port->CRL = GPIO_CRL_CNF;
                        port->CRH = GPIO_CRH_CNF;
                        break;
                }
#else /* ! defined(CPU_FAM_STM32F1) */
                switch (i) {
                    /* preserve JTAG pins on PORTA and PORTB */
                    case 0:
                        port->MODER = 0xABFFFFFF;
                        break;
                    case 1:
                        port->MODER = 0xFFFFFEBF;
                        break;
                    default:
                        port->MODER = 0xFFFFFFFF;
                        break;
                }
#endif /* defined(CPU_FAM_STM32F1) */
            }
            else {
#if defined(CPU_FAM_STM32F1)
                port->CRL = GPIO_CRL_CNF;
                port->CRH = GPIO_CRH_CNF;
#else
                port->MODER = 0xFFFFFFFF;
#endif
            }
        }
    }

    /* restore GPIO clocks */
    periph_clk_en(GPIO_CLK, ahb_gpio_clocks);
}
#endif

/**
 * @brief get the value of a register in a glitch resistant fashion
 *
 * This very teniously avoids optimization, even optimized it's better than
 * nothing but periodic review should establish that it doesn't get optimized.
 */
MAYBE_UNUSED
__attribute__((always_inline))
static inline uint32_t _multi_read_reg32(volatile uint32_t *addr, bool *glitch)
{
    uint32_t value = *addr;
// cppcheck-suppress duplicateExpression
// cppcheck-suppress knownConditionTrueFalse
    if (*addr != value || *addr != value) {
        /* (reason: volatile pointer forces multiple reads for glitch resistance,
         glitch may force different value) */
        *glitch = true;
    }

    return value;
}

/**
 * @brief    Check RDP level is what the designer intended.
 *
 * RDP stands for "ReaDout Protection."
 *
 * The STM32L4 readout protection feature offers three levels of protection
 * for all SRAM2 and Flash memory as well as the backup registers:
 *
 *  - Level 0 (RDP0) means “no protection”. This is the factory default. Read,
 *    Write and Erase operations are permitted in the SRAM2 and Flash memory
 *    as well as the backup registers. Option bytes are changeable in Level 0.
 *
 *  - Level 1 (RDP1) ensures read protection of the chip’s memories which
 *    includes the Flash memory and the backup registers as well as the SRAM2
 *    content. Whenever a debugger access is detected or Boot mode is not set
 *    to a Flash memory area, any access to the Flash memory, the backup
 *    registers or to the SRAM2 generates a system hard fault which blocks all
 *    code execution until the next power-on reset. Option bytes can still be
 *    modified in Level 1.
 *
 *  - Level 2 (RDP2) provides the same protection features for the SRAM2,
 *    Flash memory and Backup registers as described for Level 1. However,
 *    there are three major differences. The JTAG/SWD debugger connection is
 *    disabled (even at the ST factory, to ensure that there are no
 *    backdoors), the Boot mode is forced to User Flash memory REGARDLESS of
 *    what the boot 0/1 settings are, and Level 2 is permanent. Once set to
 *    Level 2, there is no going back; RDP/WRP option bytes can no longer be
 *    changed, as well as ALL the other option bytes.
 *
 * By way of background, changing the level of RDP protection is only
 * permitted when the current protection level is ‘1’. Changing the protection
 * level from '1' to '0' should automatically erase the entire user flash
 * memory, SRAM2 and backup registers.
 *
 * The issue is that while Level 0 is 0xAA and Level 2 is 0xCC, Level 1 is any
 * other number. So when OxCC is set and the chip is physically or
 * electrically perturbed, flipping any bit will "fool" the CPU into thinking
 * that it is in Level 1, allowing JTAG access and the changing of option
 * bits.
 *
 * Think of this as a STM32-specific version of the Rowhammer attack.
 *
 * RDP may not be set correctly due to manufacturing error, glitch or
 * intentional attack.  It's done thrice to reduce the probability of a
 * glitch attack succeeding amongst all of the multireads desgned to make it
 * tougher.
 *
 * This would be best served with a random delay at the beginning of the
 * function.  But a consistent strategy for all chips is tough.
 *
 * To set the RDP bytes, the J-Flash utility or the STM32 Unlock (from J-Link)
 * utility, both provided by the manufacturer.
 *
 * You can also set the option bytes from code:
 *
 *  1. Unlock the option bytes by writing the correct keys to FLASH_OPTKEYR and
 *     clearing OPTLOCK
 *  2. Set the desired option values in FLASH_OPTCR
 *  3. Set OPTSTRT in FLASH_OPTCR
 *
 * This is the generic procedure for all option bytes. However, setting the
 * RDP level in this fashion will immediately lock the CPU and force a reboot
 * (and in some cases a clearing of the flash memory).
 */

/* RDP only defined for particular families. Kconfig sets this as necessary */
#if defined(STM32_OPTION_BYTES)

#ifndef CONFIG_STM32_RDP
#define CONFIG_STM32_RDP 0
#endif

static bool _rdp_ok(void)
{
    if (CONFIG_STM32_RDP == 0) {
        return true;
    }
    bool glitch = false;
    uint32_t read1 = _multi_read_reg32(STM32_OPTION_BYTES, &glitch);
    uint32_t read2 = _multi_read_reg32(STM32_OPTION_BYTES, &glitch);
    uint32_t read3 = _multi_read_reg32(STM32_OPTION_BYTES, &glitch);
    if (glitch) {
        return false;
    }

    switch (CONFIG_STM32_RDP) {
    case 1:
        return GET_RDP(read1) == 0xAA ||
               GET_RDP(read2) == 0xAA ||
               GET_RDP(read3) == 0xAA;
    case 2:
        return GET_RDP(read1) != 0xCC ||
               GET_RDP(read2) != 0xCC ||
               GET_RDP(read3) != 0xCC;
    default:
        return false;
    }
}

static void _rdp_check(void)
{
    if (!_rdp_ok()) {
        /* halt execution */
         while (1) {
            pm_set(0);
         }
    }
}

#endif /* STM32_OPTION_BYTES */

/**
 * @brief   Initialize HW debug pins for Sub-GHz Radio
 */
void _wlx5xx_init_subghz_debug_pins(void)
{
#if IS_ACTIVE(CONFIG_STM32_WLX5XX_SUBGHZ_DEBUG)
    /* SUBGHZSPI Debug */
    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT_AF);

    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT_AF);

    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT_AF);

    gpio_init(CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT,
                 CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT_AF);

    /* Sub-GHz Radio Debug */
    gpio_init(CPU_STM32WL_SUBGHZ_RF_BUSY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_RF_BUSY,
                 CPU_STM32WL_SUBGHZ_RF_BUSY_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY_AF);

    gpio_init(CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY, GPIO_OUT);
    gpio_init_af(CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY,
                 CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY_AF);
#endif
}

static void swj_init(void)
{
#if defined(CPU_FAM_STM32F1)
    /* Only if the selected SWJ config differs from the reset value, we
     * actually need to do something. Since both sides are compile time
     * constants, this hole code gets optimized out by default */
    if (CONFIG_AFIO_MAPR_SWJ_CFG != SWJ_CFG_FULL_SWJ) {
        /* The remapping periph clock must first be enabled */
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
        /* Handling of the MAPR register is a bit involved due to the
         * write-only nature of the SWJ_CFG field, which returns undefined
         * garbage on read. `afio_mapr_read()` will read the current MAPR
         * value, but clear the SWF_CFG vield. `afio_mapr_wriote()` will then
         * write the value read back, but apply the `SWF_CFG` configuration
         * from `CONFIG_AFIO_MAPR_SWJ_CFG` first.*/
        afio_mapr_write(afio_mapr_read());
    }
#endif
}

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* enable PWR module */
#if defined(CPU_FAM_STM32U5)
    periph_clk_en(AHB3, RCC_AHB3ENR_PWREN);
#elif !defined(CPU_FAM_STM32WB) && !defined(CPU_FAM_STM32MP1) &&  \
    !defined(CPU_FAM_STM32WL)
    periph_clk_en(APB1, BIT_APB_PWREN);
#endif
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32L1)
    _gpio_init_ain();
    _rdp_check();
#endif
#if !defined(CPU_FAM_STM32MP1) || IS_USED(MODULE_STM32MP1_ENG_MODE)
    /* initialize the system clock as configured in the periph_conf.h */
    stmclk_init_sysclk();
#endif
#ifdef MODULE_PERIPH_DMA
    /*  initialize DMA streams */
    dma_init();
#endif
    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    swj_init();

    /* trigger static peripheral initialization */
    periph_init();

    if (IS_ACTIVE(CONFIG_STM32_WLX5XX_SUBGHZ_DEBUG)) {
        _wlx5xx_init_subghz_debug_pins();
    }
}

void backup_ram_init(void)
{
/* see reference manual "Battery backup domain" */
#if defined(RCC_APB1ENR_PWREN)
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
#elif defined(RCC_APBENR1_PWREN)
    periph_clk_en(APB1, RCC_APBENR1_PWREN);
#elif defined(RCC_APB1ENR1_PWREN)
    periph_clk_en(APB1, RCC_APB1ENR1_PWREN);
#elif defined(RCC_AHB3ENR_PWREN)
    periph_clk_en(AHB3, RCC_AHB3ENR_PWREN);
#endif
    stmclk_dbp_unlock();
#if defined(RCC_AHB1ENR_BKPSRAMEN)
    periph_clk_en(AHB1, RCC_AHB1ENR_BKPSRAMEN);
#endif
}

#ifndef BACKUP_RAM_MAGIC
#define BACKUP_RAM_MAGIC    {'R', 'I', 'O', 'T'}
#endif

__attribute__((unused))
static inline bool _backup_battery_connected(void) {
#if IS_USED(MODULE_PERIPH_VBAT)
    vbat_init(); /* early use of VBAT requires init() */
    return !vbat_is_empty();
#endif
    return false;
}

bool cpu_woke_from_backup(void) {
#if IS_ACTIVE(CPU_HAS_BACKUP_RAM)
    static const char _signature[] BACKUP_RAM_DATA = BACKUP_RAM_MAGIC;
    if (_backup_battery_connected()) {
        /* in case the board has a backup battery the regulator must be on
        to mitigate (unexpected) outage of VDD, so RTC register and
        backup domain register contents are not lost */
        pm_backup_regulator_on();
    }
    else {
#ifndef RIOTBOOT
        /* switch off regulator to save power */
        pm_backup_regulator_off();
#endif
    }
    for (unsigned i = 0; i < sizeof(_signature); i++) {
        if (_signature[i] != ((char[])BACKUP_RAM_MAGIC)[i]) {
            return false;
        }
    }
    return true;
#endif /* CPU_HAS_BACKUP_RAM */
    return false;
}
