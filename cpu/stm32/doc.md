@defgroup       cpu_stm32 STM32
@ingroup        cpu
@brief          All STM32 families code and definitions

This module contains the code and definitions for STM32 cpu
families supported by RIOT: F0, F1, F2, F3, F4, F7, G0, G4, L0, L1, L4, L5 and WB.

All constants described below can be configured using `CFLAGS` from the command
line or in the application Makefile.

Clock configuration
===================

All STM32 cpus have clock configuration code and macros. It is recommend to read
the reference manual of the CPU of interest to learn about the corresponding
clock tree and the possible configurations.

For all families, different input clock sources are available for the SYSCLK
system clock (but only one can be used at a time):

- HSE (High Speed External): HSE depends on the board configuration and its
  presence is specified by the `CONFIG_BOARD_HAS_HSE` constant at board level.
  The `CONFIG_CLOCK_HSE` constant specifies the frequency of the external oscillator
  in Hz.
  To use HSE as system clock (SYSCLK), set `CONFIG_USE_CLOCK_HSE` constant to 1;

- HSI (High Speed Internal): HSI is either 8MHz or 16MHz, depending
  on the families.
  To use HSI as system clock (SYSCLK), set `CONFIG_USE_CLOCK_HSI` constant to 1;

- MSI (Multi-Speed Internal): This internal oscillator is only available on L0,
  L1, L4, L5 and WB families.
  To use MSI as system clock (SYSCLK), set `CONFIG_USE_CLOCK_MSI` constant to 1
  The MSI clock frequency can be configured using the `CONFIG_CLOCK_MSI`
  constant (in Hz);

- PLL (Phase-Locked Loop) provides a finely configurable clock source for
  SYSCLK and allows to reach precise and high clock speeds, depending on the
  input source. In RIOT, this is the default clock source for SYSCLK.
  To use the PLL as system clock (SYSCLK), set `CONFIG_USE_CLOCK_PLL` constant
  to 1 (default).

  If an HSE is provided by the board, it is automatically selected as input
  source for the PLL, otherwise HSI is selected.

  On L4 and WB, the PLL input source can be selected between HSE, HSI and MSI
  (default): to configure the PLL input source, just set
  `CONFIG_CLOCK_PLL_SRC_HSI`, `CONFIG_CLOCK_PLL_SRC_HSE` or
  `CONFIG_CLOCK_PLL_SRC_MSI` to 1.

  2 types of PLL are available on STM32:
  - for stm32f[2|4|7], stm32g[0|4] and stm32[l4|l5|wb], the PLL can be
    configured with several parameters M, N, P, Q and R (optional). M is the
    input divider, N is the multiplier and P, Q and R are output dividers.
    On stm32f[2|4|7], the clock source from the P output divider is used as
    SYSCLK input, the clock source from the Q output divider can be used as
    48MHz.
    On stm32g[0|4] and stm32[l4|l5|wb], the clock source from the R output
    divider is used as SYSCLK input.

    Constants used to configure the PLL on these families are
    `CONFIG_CLOCK_PLL_M`, `CONFIG_CLOCK_PLL_N`, `CONFIG_CLOCK_PLL_P`,
    `CONFIG_CLOCK_PLL_Q` and `CONFIG_CLOCK_PLL_R`.

  - for stm32f[0|1|3] and stm32l[0|1], the PLL can be configured with a
    predivider parameter and a multiplier parameter.
    Constants used to configure the PLL on these families are
    `CONFIG_CLOCK_PLL_DIV` (l0, l1) or `CONFIG_CLOCK_PLL_PREDIV` (f0, f1, f3)
    and `CONFIG_CLOCK_PLL_MUL`.

Advanced Peripheral Bus (APB) clock prescalers:
-----------------------------------------------

These prescalers are used to compute the PCLK1 and PCLK2 clock values.
Use `CONFIG_CLOCK_APB1_DIV` and `CONFIG_CLOCK_APB2_DIV` constants to configure
the prescalers (just set the frequency factor).
APB2 is not available on f0 and g0 families so setting `CONFIG_CLOCK_APB2_DIV`
will have no effect on them.

MCO (Microcontroller Clock Output) parameters:
----------------------------------------------

For stm32f[2|4|7], clock outputs can be configured as follows:
- set `CONFIG_CLOCK_ENABLE_MCO1` to enable MCO1 (on PA8) and use
  `CONFIG_CLOCK_MCO1_PRE` constant to specify the MCO1 prescaler (default to 1).
  Possible sources are HSE, HSI and PLL (the default): set 1 to
  `CONFIG_CLOCK_MCO1_USE_HSE`, `CONFIG_CLOCK_MCO1_USE_HSI` or
  `CONFIG_CLOCK_MCO1_USE_PLL` to select the source;
- set `CONFIG_CLOCK_ENABLE_MCO2` to enable MCO1 (on PC9) and use
  `CONFIG_CLOCK_MCO2_PRE` constant to specify the MCO2 prescaler (default to 1).
  Possible sources are HSE, SYSCLK, PLLI2S and PLL (the default): set 1 to
  `CONFIG_CLOCK_MCO1_USE_HSE`, `CONFIG_CLOCK_MCO1_USE_SYSCLK`,
  `CONFIG_CLOCK_MCO1_USE_PLLI2S` or `CONFIG_CLOCK_MCO1_USE_PLL` to select the
  source;
