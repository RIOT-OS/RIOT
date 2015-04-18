/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdint.h>
#include "cpu.h"
#include "board.h"

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Implementation of K60 CPU initialization.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

extern void *_vector_rom[];

/** @brief Current core clock frequency */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current system clock frequency */
uint32_t SystemSysClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current bus clock frequency */
uint32_t SystemBusClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current FlexBus clock frequency */
uint32_t SystemFlexBusClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Current flash clock frequency */
uint32_t SystemFlashClock = DEFAULT_SYSTEM_CLOCK;
/** @brief Number of full PIT ticks in one microsecond. */
uint32_t PIT_ticks_per_usec = (DEFAULT_SYSTEM_CLOCK / 1000000ul);

/**
 * @brief Check the running CPU identification to find if we are running on the
 *        wrong hardware.
 */
static void check_running_cpu_revision(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* Check that we are running on the CPU that this code was built for */
    check_running_cpu_revision();

    /* configure the vector table location to internal flash */
    SCB->VTOR = (uint32_t)_vector_rom;

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);

}

static void check_running_cpu_revision(void)
{
    /* Check that the running CPU revision matches the compiled revision */
    if (SCB->CPUID != K60_EXPECTED_CPUID) {
        uint32_t CPUID = SCB->CPUID; /* This is only to ease debugging, type
                   * "print /x CPUID" in gdb */
        uint32_t SILICON_REVISION = (SCB->CPUID & SCB_CPUID_REVISION_Msk) + 1;
        (void)CPUID; /* prevents compiler warnings about an unused variable. */
        (void)SILICON_REVISION;

        /* Running on the wrong CPU, the clock initialization is different
         * between silicon revision 1.x and 2.x (LSB of CPUID) */
        /* If you unexpectedly end up on this line when debugging:
         * Rebuild the code using the correct value for K60_CPU_REV */
        __ASM volatile ("bkpt #99\n");

        while (1);
    }
}

void SystemCoreClockUpdate(void)
{
    /* Variable to store output clock frequency of the MCG module */
    uint32_t MCGOUT_clock;

    if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) {
        /* Output of FLL or PLL is selected */
        if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) {
            /* FLL is selected */
            if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) {
                /* External reference clock is selected */
#if K60_CPU_REV == 1
                /* rev.1 silicon */
                if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) {
                    /* System oscillator drives MCG clock */
                    MCGOUT_clock = CPU_XTAL_CLK_HZ;
                }
                else {
                    /* RTC 32 kHz oscillator drives MCG clock */
                    MCGOUT_clock = CPU_XTAL32k_CLK_HZ;
                }

#else /* K60_CPU_REV */

                /* rev.2 silicon */
                if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
                    /* System oscillator drives MCG clock */
                    MCGOUT_clock = CPU_XTAL_CLK_HZ;
                }
                else {
                    /* RTC 32 kHz oscillator drives MCG clock */
                    MCGOUT_clock = CPU_XTAL32k_CLK_HZ;
                }

#endif /* K60_CPU_REV */
                uint8_t divider = (uint8_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
                /* Calculate the divided FLL reference clock */
                MCGOUT_clock /= divider;

                if ((MCG->C2 & MCG_C2_RANGE0_MASK) != 0x0u) {
                    /* If high range is enabled, additional 32 divider is active */
                    MCGOUT_clock /= 32u;
                }
            }
            else {
                /* The slow internal reference clock is selected */
                MCGOUT_clock = CPU_INT_SLOW_CLK_HZ;
            }

            /* Select correct multiplier to calculate the MCG output clock  */
            switch (MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) {
                case (0x0u):
                    MCGOUT_clock *= 640u;
                    break;

                case (MCG_C4_DRST_DRS(0b01)): /* 0x20u */
                    MCGOUT_clock *= 1280u;
                    break;

                case (MCG_C4_DRST_DRS(0b10)): /* 0x40u */
                    MCGOUT_clock *= 1920u;
                    break;

                case (MCG_C4_DRST_DRS(0b11)): /* 0x60u */
                    MCGOUT_clock *= 2560u;
                    break;

                case (MCG_C4_DMX32_MASK): /* 0x80u */
                    MCGOUT_clock *= 732u;
                    break;

                case (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0b01)): /* 0xA0u */
                    MCGOUT_clock *= 1464u;
                    break;

                case (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0b10)): /* 0xC0u */
                    MCGOUT_clock *= 2197u;
                    break;

                case (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0b11)): /* 0xE0u */
                    MCGOUT_clock *= 2929u;
                    break;

                default:
                    break;
            }
        }
        else {
            /* PLL is selected */
            /* Calculate the PLL reference clock */
            uint8_t divider = (1u + (MCG->C5 & MCG_C5_PRDIV0_MASK));
            MCGOUT_clock = (uint32_t)(CPU_XTAL_CLK_HZ / divider);
            /* Calculate the MCG output clock */
            divider = ((MCG->C6 & MCG_C6_VDIV0_MASK) + 24u);
            MCGOUT_clock *= divider;
        }
    }
    else if ((MCG->C1 & MCG_C1_CLKS_MASK) == MCG_C1_CLKS(0b01)) {   /* 0x40u */
        /* Internal reference clock is selected */
        if ((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u) {
            /* Slow internal reference clock selected */
            MCGOUT_clock = CPU_INT_SLOW_CLK_HZ;
        }
        else {
            /* Fast internal reference clock selected */
#if K60_CPU_REV == 1
            /* rev.1 silicon */
            MCGOUT_clock = CPU_INT_FAST_CLK_HZ;
#else /* K60_CPU_REV */
            /* rev.2 silicon */
            MCGOUT_clock = CPU_INT_FAST_CLK_HZ /
                           (1 << ((MCG->SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT));
#endif /* K60_CPU_REV */
        }
    }
    else if ((MCG->C1 & MCG_C1_CLKS_MASK) == MCG_C1_CLKS(0b10)) {   /* 0x80u */
        /* External reference clock is selected */
#if K60_CPU_REV == 1
        /* rev.1 silicon */
        if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) {
            /* System oscillator drives MCG clock */
            MCGOUT_clock = CPU_XTAL_CLK_HZ;
        }
        else {
            /* RTC 32 kHz oscillator drives MCG clock */
            MCGOUT_clock = CPU_XTAL32k_CLK_HZ;
        }

#else /* K60_CPU_REV */

        /* rev.2 silicon */
        if ((MCG->C7 & MCG_C7_OSCSEL_MASK) == 0x0u) {
            /* System oscillator drives MCG clock */
            MCGOUT_clock = CPU_XTAL_CLK_HZ;
        }
        else {
            /* RTC 32 kHz oscillator drives MCG clock */
            MCGOUT_clock = CPU_XTAL32k_CLK_HZ;
        }

#endif /* K60_CPU_REV */
    }
    else {
        /* Reserved value */
        return;
    }

    /* Core clock and system clock use the same divider setting */
    SystemCoreClock = SystemSysClock = (MCGOUT_clock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK)
                                        >> SIM_CLKDIV1_OUTDIV1_SHIFT)));
    SystemBusClock = (MCGOUT_clock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >>
                                            SIM_CLKDIV1_OUTDIV2_SHIFT)));
    SystemFlexBusClock = (MCGOUT_clock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV3_MASK) >>
                                          SIM_CLKDIV1_OUTDIV3_SHIFT)));
    SystemFlashClock = (MCGOUT_clock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >>
                                        SIM_CLKDIV1_OUTDIV4_SHIFT)));

    /* Module helper variables */
    if (SystemBusClock >= 1000000) {
        /* PIT module clock_delay_usec scale factor */
        PIT_ticks_per_usec = (SystemBusClock + 500000) / 1000000; /* Rounded to nearest integer */
    }
    else {
        /* less than 1 MHz clock frequency on the PIT module, round up. */
        PIT_ticks_per_usec = 1;
    }
}

/** @} */
