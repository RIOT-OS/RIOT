/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k60 Freescale Kinetis K60
 * @ingroup         cpu
 * @brief           CPU specific implementations for the Freescale Kinetis K60
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef CPU_CONF_H_
#define CPU_CONF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#if defined(CPU_MODEL_K60DN512VLL10) || defined(CPU_MODEL_K60DN256VLL10)

/* Rev. 2.x silicon */
#define K60_CPU_REV 2
#include "MK60D10.h"

/** The expected CPUID value, can be used to implement a check that we are
 * running on the right hardware */
#define K60_EXPECTED_CPUID 0x410fc241u

/* K60 rev 2.x replaced the RNG module in 1.x by the RNGA PRNG module */
#define KINETIS_RNGA            (RNG)

#elif defined(CPU_MODEL_K60DN512ZVLL10) || defined(CPU_MODEL_K60DN256ZVLL10)

/* Rev. 1.x silicon */
#define K60_CPU_REV 1
#include "MK60DZ10.h"

/** The expected CPUID value, can be used to implement a check that we are
 * running on the right hardware */
#define K60_EXPECTED_CPUID 0x410fc240u

/* K60 rev 1.x has the cryptographically strong RNGB module */
#define KINETIS_RNGB            (RNG)

#else
#error Unknown CPU model. Update Makefile.include in the board directory.
#endif

/* Compatibility definitions between the two different Freescale headers */
#include "MK60-comp.h"

/**
 * @name GPIO pin mux function numbers
 */
/** @{ */
#define PIN_MUX_FUNCTION_ANALOG 0
#define PIN_MUX_FUNCTION_GPIO 1
/** @} */
/**
 * @name GPIO interrupt flank settings
 */
/** @{ */
#define PIN_INTERRUPT_RISING 0b1001
#define PIN_INTERRUPT_FALLING 0b1010
#define PIN_INTERRUPT_EDGE 0b1011
/** @} */
/**
 * @name Kernel stack size configuration
 *
 * TODO: Tune this
 * @{
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF    (1024)

#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT   (1024)
#endif

#define THREAD_STACKSIZE_IDLE      (256)
/** @} */

/**
 * @name Length and address for reading CPU_ID (named UID in Freescale documents)
 * @{
 */
#define CPUID_ID_LEN                    (16)
#define CPUID_ID_PTR                    ((void *)(&(SIM->UIDH)))
/** @} */

#ifndef UART0_BUFSIZE
/**
 * @brief UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 */
#define UART0_BUFSIZE                   (128)
#endif

/**
 * @name UART driver settings
 */
/** @{ */
/** UART typedef from CPU header. */
#define KINETIS_UART                    UART_Type
/** @} */

/**
 * @name Clock settings for the LPTMR0 timer
 * @{
 */
#define LPTIMER_DEV                      (LPTMR0) /**< LPTIMER hardware module */
#define LPTIMER_CLKEN()                  (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_LPTIMER_SHIFT) = 1)    /**< Enable LPTMR0 clock gate */
#define LPTIMER_CLKDIS()                 (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_LPTIMER_SHIFT) = 0)    /**< Disable LPTMR0 clock gate */
#define LPTIMER_CLKSRC_MCGIRCLK          0    /**< internal reference clock (4MHz) */
#define LPTIMER_CLKSRC_LPO               1    /**< PMC 1kHz output */
#define LPTIMER_CLKSRC_ERCLK32K          2    /**< RTC clock 32768Hz */
#define LPTIMER_CLKSRC_OSCERCLK          3    /**< system oscillator output, clock from RF-Part */

#ifndef LPTIMER_CLKSRC
#define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_ERCLK32K    /**< default clock source */
#endif

#if (LPTIMER_CLKSRC == LPTIMER_CLKSRC_MCGIRCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_OSCERCLK)
#define LPTIMER_CLK_PRESCALE    1
#define LPTIMER_SPEED           1000000
#elif (LPTIMER_CLKSRC == LPTIMER_CLKSRC_ERCLK32K)
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           32768
#else
#define LPTIMER_CLK_PRESCALE    0
#define LPTIMER_SPEED           1000
#endif

/** IRQ priority for hwtimer interrupts */
#define LPTIMER_IRQ_PRIO          1
/** IRQ channel for hwtimer interrupts */
#define LPTIMER_IRQ_CHAN          LPTMR0_IRQn

#if K60_CPU_REV == 1
/*
 * The CNR register latching in LPTMR0 was added in silicon rev 2.x. With
 * rev 1.x we do not need to do anything in order to read the current timer counter
 * value
 */
#define LPTIMER_CNR_NEEDS_LATCHING 0

#elif K60_CPU_REV == 2

#define LPTIMER_CNR_NEEDS_LATCHING 1

#endif
/** @} */

/**
 * @name Power mode hardware details
 */
/** @{ */
#if K60_CPU_REV == 1
#define KINETIS_PMCTRL MC->PMCTRL
#define KINETIS_PMCTRL_SET_MODE(x) (KINETIS_PMCTRL = MC_PMCTRL_LPLLSM(x) | MC_PMCTRL_LPWUI_MASK)
/* Clear LLS protection, clear VLPS, VLPW, VLPR protection */
/* Note: This register can only be written once after each reset, so we must
 * enable all power modes that we wish to use. */
#define KINETIS_UNLOCK_PMPROT() (MC->PMPROT |= MC_PMPROT_ALLS_MASK | MC_PMPROT_AVLP_MASK)
#elif K60_CPU_REV == 2
#define KINETIS_PMCTRL SMC->PMCTRL
#define KINETIS_PMCTRL_SET_MODE(x) (KINETIS_PMCTRL = SMC_PMCTRL_STOPM(x) | SMC_PMCTRL_LPWUI_MASK)
#define KINETIS_PMPROT_UNLOCK() (SMC->PMPROT |= SMC_PMPROT_ALLS_MASK | SMC_PMPROT_AVLP_MASK)
#else
#error Unknown K60 CPU revision!
#endif

/**
 * @name STOP mode bitfield values
 * @{
 */
/** @brief Normal STOP */
#define KINETIS_POWER_MODE_NORMAL (0b000)
/** @brief VLPS STOP */
#define KINETIS_POWER_MODE_VLPS   (0b010)
/** @brief LLS STOP */
#define KINETIS_POWER_MODE_LLS    (0b011)
/** @} */

/**
 * @brief Wake up source number for the LPTMR0
 *
 * In order to let the hwtimer wake the CPU from low power modes, we need to
 * enable this wake up source.
 */
#define KINETIS_LLWU_WAKEUP_MODULE_LPTMR 0

/**
 * @brief IRQn name to enable LLWU IRQ in NVIC
 */
#define KINETIS_LLWU_IRQ LLW_IRQn

/**
 * @brief Enable clock gate on LLWU module.
 */
#define LLWU_UNLOCK() (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_LLWU_SHIFT) = 1)

/**
 * @brief Internal modules whose interrupts are mapped to LLWU wake up sources.
 *
 * Other modules CAN NOT be used to wake the CPU from LLS or VLLSx power modes.
 */
typedef enum llwu_wakeup_module {
    KINETIS_LPM_WAKEUP_MODULE_LPTMR = 0,
    KINETIS_LPM_WAKEUP_MODULE_CMP0 = 1,
    KINETIS_LPM_WAKEUP_MODULE_CMP1 = 2,
    KINETIS_LPM_WAKEUP_MODULE_CMP2 = 3,
    KINETIS_LPM_WAKEUP_MODULE_TSI = 4,
    KINETIS_LPM_WAKEUP_MODULE_RTC_ALARM = 5,
    KINETIS_LPM_WAKEUP_MODULE_RESERVED = 6,
    KINETIS_LPM_WAKEUP_MODULE_RTC_SECONDS = 7,
    KINETIS_LPM_WAKEUP_MODULE_END,
} llwu_wakeup_module_t;

/**
 * @brief enum that maps physical pins to wakeup pin numbers in LLWU module
 *
 * Other pins CAN NOT be used to wake the CPU from LLS or VLLSx power modes.
 */
typedef enum llwu_wakeup_pin {
    KINETIS_LPM_WAKEUP_PIN_PTE1 = 0,
    KINETIS_LPM_WAKEUP_PIN_PTE2 = 1,
    KINETIS_LPM_WAKEUP_PIN_PTE4 = 2,
    KINETIS_LPM_WAKEUP_PIN_PTA4 = 3,
    KINETIS_LPM_WAKEUP_PIN_PTA13 = 4,
    KINETIS_LPM_WAKEUP_PIN_PTB0 = 5,
    KINETIS_LPM_WAKEUP_PIN_PTC1 = 6,
    KINETIS_LPM_WAKEUP_PIN_PTC3 = 7,
    KINETIS_LPM_WAKEUP_PIN_PTC4 = 8,
    KINETIS_LPM_WAKEUP_PIN_PTC5 = 9,
    KINETIS_LPM_WAKEUP_PIN_PTC6 = 10,
    KINETIS_LPM_WAKEUP_PIN_PTC11 = 11,
    KINETIS_LPM_WAKEUP_PIN_PTD0 = 12,
    KINETIS_LPM_WAKEUP_PIN_PTD2 = 13,
    KINETIS_LPM_WAKEUP_PIN_PTD4 = 14,
    KINETIS_LPM_WAKEUP_PIN_PTD6 = 15,
    KINETIS_LPM_WAKEUP_PIN_END
} llwu_wakeup_pin_t;

/** @} */

/** @name K60 PORT ISR names
 * @{ */
#define ISR_PORT_A isr_porta_pin_detect
#define ISR_PORT_B isr_portb_pin_detect
#define ISR_PORT_C isr_portc_pin_detect
#define ISR_PORT_D isr_portd_pin_detect
#define ISR_PORT_E isr_porte_pin_detect
/** @} */

/** @brief Number of packets in transceiver queue */
#define TRANSCEIVER_BUFFER_SIZE (3)

/**
 * @name Bit band macros
 * @{
 */
/* Generic bitband conversion routine */
/** @brief Convert bit-band region address and bit number to bit-band alias address
 *
 * @param[in] addr base address in non-bit-banded memory
 * @param[in] bit  bit number within the word
 *
 * @return Address of the bit within the bit-band memory region
 */
#define BITBAND_ADDR(addr, bit) ((((uint32_t) (addr)) & 0xF0000000u) + 0x2000000 + ((((uint32_t) (addr)) & 0xFFFFF) << 5) + ((bit) << 2))

/**
 * @brief Bitband 32 bit access to variable stored in SRAM_U
 *
 * @note SRAM_L is not bit band aliased on the K60, only SRAM_U (0x20000000 and up)
 * @note var must be declared 'volatile'
 */
#define BITBAND_VAR32(var, bit) (*((uint32_t volatile*) BITBAND_ADDR(&(var), (bit))))

/**
 * @brief Bitband 16 bit access to variable stored in SRAM_U
 *
 * @note SRAM_L is not bit band aliased on the K60, only SRAM_U (0x20000000 and up)
 * @note var must be declared 'volatile'
 */
#define BITBAND_VAR16(var, bit) (*((uint16_t volatile*) BITBAND_ADDR(&(var), (bit))))

/**
 * @brief Bitband 8 bit access to variable stored in SRAM_U
 *
 * @note SRAM_L is not bit band aliased on the K60, only SRAM_U (0x20000000 and up)
 * @note var must be declared 'volatile'
 */
#define BITBAND_VAR8(var, bit) (*((uint8_t volatile*) BITBAND_ADDR(&(var), (bit))))

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H_ */
/** @} */
