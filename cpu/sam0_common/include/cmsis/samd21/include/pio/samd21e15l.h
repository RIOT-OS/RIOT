/**
 * \file
 *
 * \brief Peripheral I/O description for SAMD21E15L
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD21E15L_PIO_
#define _SAMD21E15L_PIO_

#define PIN_PA02                           2  /**< \brief Pin Number for PA02 */
#define PORT_PA02                 (1ul <<  2) /**< \brief PORT Mask  for PA02 */
#define PIN_PA03                           3  /**< \brief Pin Number for PA03 */
#define PORT_PA03                 (1ul <<  3) /**< \brief PORT Mask  for PA03 */
#define PIN_PA04                           4  /**< \brief Pin Number for PA04 */
#define PORT_PA04                 (1ul <<  4) /**< \brief PORT Mask  for PA04 */
#define PIN_PA05                           5  /**< \brief Pin Number for PA05 */
#define PORT_PA05                 (1ul <<  5) /**< \brief PORT Mask  for PA05 */
#define PIN_PA06                           6  /**< \brief Pin Number for PA06 */
#define PORT_PA06                 (1ul <<  6) /**< \brief PORT Mask  for PA06 */
#define PIN_PA07                           7  /**< \brief Pin Number for PA07 */
#define PORT_PA07                 (1ul <<  7) /**< \brief PORT Mask  for PA07 */
#define PIN_PA08                           8  /**< \brief Pin Number for PA08 */
#define PORT_PA08                 (1ul <<  8) /**< \brief PORT Mask  for PA08 */
#define PIN_PA09                           9  /**< \brief Pin Number for PA09 */
#define PORT_PA09                 (1ul <<  9) /**< \brief PORT Mask  for PA09 */
#define PIN_PA10                          10  /**< \brief Pin Number for PA10 */
#define PORT_PA10                 (1ul << 10) /**< \brief PORT Mask  for PA10 */
#define PIN_PA11                          11  /**< \brief Pin Number for PA11 */
#define PORT_PA11                 (1ul << 11) /**< \brief PORT Mask  for PA11 */
#define PIN_PA14                          14  /**< \brief Pin Number for PA14 */
#define PORT_PA14                 (1ul << 14) /**< \brief PORT Mask  for PA14 */
#define PIN_PA15                          15  /**< \brief Pin Number for PA15 */
#define PORT_PA15                 (1ul << 15) /**< \brief PORT Mask  for PA15 */
#define PIN_PA16                          16  /**< \brief Pin Number for PA16 */
#define PORT_PA16                 (1ul << 16) /**< \brief PORT Mask  for PA16 */
#define PIN_PA17                          17  /**< \brief Pin Number for PA17 */
#define PORT_PA17                 (1ul << 17) /**< \brief PORT Mask  for PA17 */
#define PIN_PA18                          18  /**< \brief Pin Number for PA18 */
#define PORT_PA18                 (1ul << 18) /**< \brief PORT Mask  for PA18 */
#define PIN_PA19                          19  /**< \brief Pin Number for PA19 */
#define PORT_PA19                 (1ul << 19) /**< \brief PORT Mask  for PA19 */
#define PIN_PA22                          22  /**< \brief Pin Number for PA22 */
#define PORT_PA22                 (1ul << 22) /**< \brief PORT Mask  for PA22 */
#define PIN_PA23                          23  /**< \brief Pin Number for PA23 */
#define PORT_PA23                 (1ul << 23) /**< \brief PORT Mask  for PA23 */
#define PIN_PA24                          24  /**< \brief Pin Number for PA24 */
#define PORT_PA24                 (1ul << 24) /**< \brief PORT Mask  for PA24 */
#define PIN_PA25                          25  /**< \brief Pin Number for PA25 */
#define PORT_PA25                 (1ul << 25) /**< \brief PORT Mask  for PA25 */
#define PIN_PA30                          30  /**< \brief Pin Number for PA30 */
#define PORT_PA30                 (1ul << 30) /**< \brief PORT Mask  for PA30 */
#define PIN_PA31                          31  /**< \brief Pin Number for PA31 */
#define PORT_PA31                 (1ul << 31) /**< \brief PORT Mask  for PA31 */
#define PIN_PB02                          34  /**< \brief Pin Number for PB02 */
#define PORT_PB02                 (1ul <<  2) /**< \brief PORT Mask  for PB02 */
#define PIN_PB03                          35  /**< \brief Pin Number for PB03 */
#define PORT_PB03                 (1ul <<  3) /**< \brief PORT Mask  for PB03 */
#define PIN_PB04                          36  /**< \brief Pin Number for PB04 */
#define PORT_PB04                 (1ul <<  4) /**< \brief PORT Mask  for PB04 */
#define PIN_PB05                          37  /**< \brief Pin Number for PB05 */
#define PORT_PB05                 (1ul <<  5) /**< \brief PORT Mask  for PB05 */
/* ========== PORT definition for GCLK peripheral ========== */
#define PIN_PA14H_GCLK_IO0                14L  /**< \brief GCLK signal: IO0 on PA14 mux H */
#define MUX_PA14H_GCLK_IO0                 7L
#define PINMUX_PA14H_GCLK_IO0      ((PIN_PA14H_GCLK_IO0 << 16) | MUX_PA14H_GCLK_IO0)
#define PORT_PA14H_GCLK_IO0        (1ul << 14)
#define PIN_PA30H_GCLK_IO0                30L  /**< \brief GCLK signal: IO0 on PA30 mux H */
#define MUX_PA30H_GCLK_IO0                 7L
#define PINMUX_PA30H_GCLK_IO0      ((PIN_PA30H_GCLK_IO0 << 16) | MUX_PA30H_GCLK_IO0)
#define PORT_PA30H_GCLK_IO0        (1ul << 30)
#define PIN_PA15H_GCLK_IO1                15L  /**< \brief GCLK signal: IO1 on PA15 mux H */
#define MUX_PA15H_GCLK_IO1                 7L
#define PINMUX_PA15H_GCLK_IO1      ((PIN_PA15H_GCLK_IO1 << 16) | MUX_PA15H_GCLK_IO1)
#define PORT_PA15H_GCLK_IO1        (1ul << 15)
#define PIN_PA16H_GCLK_IO2                16L  /**< \brief GCLK signal: IO2 on PA16 mux H */
#define MUX_PA16H_GCLK_IO2                 7L
#define PINMUX_PA16H_GCLK_IO2      ((PIN_PA16H_GCLK_IO2 << 16) | MUX_PA16H_GCLK_IO2)
#define PORT_PA16H_GCLK_IO2        (1ul << 16)
#define PIN_PA17H_GCLK_IO3                17L  /**< \brief GCLK signal: IO3 on PA17 mux H */
#define MUX_PA17H_GCLK_IO3                 7L
#define PINMUX_PA17H_GCLK_IO3      ((PIN_PA17H_GCLK_IO3 << 16) | MUX_PA17H_GCLK_IO3)
#define PORT_PA17H_GCLK_IO3        (1ul << 17)
#define PIN_PA10H_GCLK_IO4                10L  /**< \brief GCLK signal: IO4 on PA10 mux H */
#define MUX_PA10H_GCLK_IO4                 7L
#define PINMUX_PA10H_GCLK_IO4      ((PIN_PA10H_GCLK_IO4 << 16) | MUX_PA10H_GCLK_IO4)
#define PORT_PA10H_GCLK_IO4        (1ul << 10)
#define PIN_PA11H_GCLK_IO5                11L  /**< \brief GCLK signal: IO5 on PA11 mux H */
#define MUX_PA11H_GCLK_IO5                 7L
#define PINMUX_PA11H_GCLK_IO5      ((PIN_PA11H_GCLK_IO5 << 16) | MUX_PA11H_GCLK_IO5)
#define PORT_PA11H_GCLK_IO5        (1ul << 11)
#define PIN_PA22H_GCLK_IO6                22L  /**< \brief GCLK signal: IO6 on PA22 mux H */
#define MUX_PA22H_GCLK_IO6                 7L
#define PINMUX_PA22H_GCLK_IO6      ((PIN_PA22H_GCLK_IO6 << 16) | MUX_PA22H_GCLK_IO6)
#define PORT_PA22H_GCLK_IO6        (1ul << 22)
#define PIN_PA23H_GCLK_IO7                23L  /**< \brief GCLK signal: IO7 on PA23 mux H */
#define MUX_PA23H_GCLK_IO7                 7L
#define PINMUX_PA23H_GCLK_IO7      ((PIN_PA23H_GCLK_IO7 << 16) | MUX_PA23H_GCLK_IO7)
#define PORT_PA23H_GCLK_IO7        (1ul << 23)
/* ========== PORT definition for EIC peripheral ========== */
#define PIN_PA16A_EIC_EXTINT0             16L  /**< \brief EIC signal: EXTINT0 on PA16 mux A */
#define MUX_PA16A_EIC_EXTINT0              0L
#define PINMUX_PA16A_EIC_EXTINT0   ((PIN_PA16A_EIC_EXTINT0 << 16) | MUX_PA16A_EIC_EXTINT0)
#define PORT_PA16A_EIC_EXTINT0     (1ul << 16)
#define PIN_PA17A_EIC_EXTINT1             17L  /**< \brief EIC signal: EXTINT1 on PA17 mux A */
#define MUX_PA17A_EIC_EXTINT1              0L
#define PINMUX_PA17A_EIC_EXTINT1   ((PIN_PA17A_EIC_EXTINT1 << 16) | MUX_PA17A_EIC_EXTINT1)
#define PORT_PA17A_EIC_EXTINT1     (1ul << 17)
#define PIN_PA02A_EIC_EXTINT2              2L  /**< \brief EIC signal: EXTINT2 on PA02 mux A */
#define MUX_PA02A_EIC_EXTINT2              0L
#define PINMUX_PA02A_EIC_EXTINT2   ((PIN_PA02A_EIC_EXTINT2 << 16) | MUX_PA02A_EIC_EXTINT2)
#define PORT_PA02A_EIC_EXTINT2     (1ul <<  2)
#define PIN_PA18A_EIC_EXTINT2             18L  /**< \brief EIC signal: EXTINT2 on PA18 mux A */
#define MUX_PA18A_EIC_EXTINT2              0L
#define PINMUX_PA18A_EIC_EXTINT2   ((PIN_PA18A_EIC_EXTINT2 << 16) | MUX_PA18A_EIC_EXTINT2)
#define PORT_PA18A_EIC_EXTINT2     (1ul << 18)
#define PIN_PB02A_EIC_EXTINT2             34L  /**< \brief EIC signal: EXTINT2 on PB02 mux A */
#define MUX_PB02A_EIC_EXTINT2              0L
#define PINMUX_PB02A_EIC_EXTINT2   ((PIN_PB02A_EIC_EXTINT2 << 16) | MUX_PB02A_EIC_EXTINT2)
#define PORT_PB02A_EIC_EXTINT2     (1ul <<  2)
#define PIN_PA03A_EIC_EXTINT3              3L  /**< \brief EIC signal: EXTINT3 on PA03 mux A */
#define MUX_PA03A_EIC_EXTINT3              0L
#define PINMUX_PA03A_EIC_EXTINT3   ((PIN_PA03A_EIC_EXTINT3 << 16) | MUX_PA03A_EIC_EXTINT3)
#define PORT_PA03A_EIC_EXTINT3     (1ul <<  3)
#define PIN_PA19A_EIC_EXTINT3             19L  /**< \brief EIC signal: EXTINT3 on PA19 mux A */
#define MUX_PA19A_EIC_EXTINT3              0L
#define PINMUX_PA19A_EIC_EXTINT3   ((PIN_PA19A_EIC_EXTINT3 << 16) | MUX_PA19A_EIC_EXTINT3)
#define PORT_PA19A_EIC_EXTINT3     (1ul << 19)
#define PIN_PB03A_EIC_EXTINT3             35L  /**< \brief EIC signal: EXTINT3 on PB03 mux A */
#define MUX_PB03A_EIC_EXTINT3              0L
#define PINMUX_PB03A_EIC_EXTINT3   ((PIN_PB03A_EIC_EXTINT3 << 16) | MUX_PB03A_EIC_EXTINT3)
#define PORT_PB03A_EIC_EXTINT3     (1ul <<  3)
#define PIN_PA04A_EIC_EXTINT4              4L  /**< \brief EIC signal: EXTINT4 on PA04 mux A */
#define MUX_PA04A_EIC_EXTINT4              0L
#define PINMUX_PA04A_EIC_EXTINT4   ((PIN_PA04A_EIC_EXTINT4 << 16) | MUX_PA04A_EIC_EXTINT4)
#define PORT_PA04A_EIC_EXTINT4     (1ul <<  4)
#define PIN_PB04A_EIC_EXTINT4             36L  /**< \brief EIC signal: EXTINT4 on PB04 mux A */
#define MUX_PB04A_EIC_EXTINT4              0L
#define PINMUX_PB04A_EIC_EXTINT4   ((PIN_PB04A_EIC_EXTINT4 << 16) | MUX_PB04A_EIC_EXTINT4)
#define PORT_PB04A_EIC_EXTINT4     (1ul <<  4)
#define PIN_PA05A_EIC_EXTINT5              5L  /**< \brief EIC signal: EXTINT5 on PA05 mux A */
#define MUX_PA05A_EIC_EXTINT5              0L
#define PINMUX_PA05A_EIC_EXTINT5   ((PIN_PA05A_EIC_EXTINT5 << 16) | MUX_PA05A_EIC_EXTINT5)
#define PORT_PA05A_EIC_EXTINT5     (1ul <<  5)
#define PIN_PB05A_EIC_EXTINT5             37L  /**< \brief EIC signal: EXTINT5 on PB05 mux A */
#define MUX_PB05A_EIC_EXTINT5              0L
#define PINMUX_PB05A_EIC_EXTINT5   ((PIN_PB05A_EIC_EXTINT5 << 16) | MUX_PB05A_EIC_EXTINT5)
#define PORT_PB05A_EIC_EXTINT5     (1ul <<  5)
#define PIN_PA06A_EIC_EXTINT6              6L  /**< \brief EIC signal: EXTINT6 on PA06 mux A */
#define MUX_PA06A_EIC_EXTINT6              0L
#define PINMUX_PA06A_EIC_EXTINT6   ((PIN_PA06A_EIC_EXTINT6 << 16) | MUX_PA06A_EIC_EXTINT6)
#define PORT_PA06A_EIC_EXTINT6     (1ul <<  6)
#define PIN_PA22A_EIC_EXTINT6             22L  /**< \brief EIC signal: EXTINT6 on PA22 mux A */
#define MUX_PA22A_EIC_EXTINT6              0L
#define PINMUX_PA22A_EIC_EXTINT6   ((PIN_PA22A_EIC_EXTINT6 << 16) | MUX_PA22A_EIC_EXTINT6)
#define PORT_PA22A_EIC_EXTINT6     (1ul << 22)
#define PIN_PA07A_EIC_EXTINT7              7L  /**< \brief EIC signal: EXTINT7 on PA07 mux A */
#define MUX_PA07A_EIC_EXTINT7              0L
#define PINMUX_PA07A_EIC_EXTINT7   ((PIN_PA07A_EIC_EXTINT7 << 16) | MUX_PA07A_EIC_EXTINT7)
#define PORT_PA07A_EIC_EXTINT7     (1ul <<  7)
#define PIN_PA23A_EIC_EXTINT7             23L  /**< \brief EIC signal: EXTINT7 on PA23 mux A */
#define MUX_PA23A_EIC_EXTINT7              0L
#define PINMUX_PA23A_EIC_EXTINT7   ((PIN_PA23A_EIC_EXTINT7 << 16) | MUX_PA23A_EIC_EXTINT7)
#define PORT_PA23A_EIC_EXTINT7     (1ul << 23)
#define PIN_PA09A_EIC_EXTINT9              9L  /**< \brief EIC signal: EXTINT9 on PA09 mux A */
#define MUX_PA09A_EIC_EXTINT9              0L
#define PINMUX_PA09A_EIC_EXTINT9   ((PIN_PA09A_EIC_EXTINT9 << 16) | MUX_PA09A_EIC_EXTINT9)
#define PORT_PA09A_EIC_EXTINT9     (1ul <<  9)
#define PIN_PA10A_EIC_EXTINT10            10L  /**< \brief EIC signal: EXTINT10 on PA10 mux A */
#define MUX_PA10A_EIC_EXTINT10             0L
#define PINMUX_PA10A_EIC_EXTINT10  ((PIN_PA10A_EIC_EXTINT10 << 16) | MUX_PA10A_EIC_EXTINT10)
#define PORT_PA10A_EIC_EXTINT10    (1ul << 10)
#define PIN_PA30A_EIC_EXTINT10            30L  /**< \brief EIC signal: EXTINT10 on PA30 mux A */
#define MUX_PA30A_EIC_EXTINT10             0L
#define PINMUX_PA30A_EIC_EXTINT10  ((PIN_PA30A_EIC_EXTINT10 << 16) | MUX_PA30A_EIC_EXTINT10)
#define PORT_PA30A_EIC_EXTINT10    (1ul << 30)
#define PIN_PA11A_EIC_EXTINT11            11L  /**< \brief EIC signal: EXTINT11 on PA11 mux A */
#define MUX_PA11A_EIC_EXTINT11             0L
#define PINMUX_PA11A_EIC_EXTINT11  ((PIN_PA11A_EIC_EXTINT11 << 16) | MUX_PA11A_EIC_EXTINT11)
#define PORT_PA11A_EIC_EXTINT11    (1ul << 11)
#define PIN_PA31A_EIC_EXTINT11            31L  /**< \brief EIC signal: EXTINT11 on PA31 mux A */
#define MUX_PA31A_EIC_EXTINT11             0L
#define PINMUX_PA31A_EIC_EXTINT11  ((PIN_PA31A_EIC_EXTINT11 << 16) | MUX_PA31A_EIC_EXTINT11)
#define PORT_PA31A_EIC_EXTINT11    (1ul << 31)
#define PIN_PA24A_EIC_EXTINT12            24L  /**< \brief EIC signal: EXTINT12 on PA24 mux A */
#define MUX_PA24A_EIC_EXTINT12             0L
#define PINMUX_PA24A_EIC_EXTINT12  ((PIN_PA24A_EIC_EXTINT12 << 16) | MUX_PA24A_EIC_EXTINT12)
#define PORT_PA24A_EIC_EXTINT12    (1ul << 24)
#define PIN_PA25A_EIC_EXTINT13            25L  /**< \brief EIC signal: EXTINT13 on PA25 mux A */
#define MUX_PA25A_EIC_EXTINT13             0L
#define PINMUX_PA25A_EIC_EXTINT13  ((PIN_PA25A_EIC_EXTINT13 << 16) | MUX_PA25A_EIC_EXTINT13)
#define PORT_PA25A_EIC_EXTINT13    (1ul << 25)
#define PIN_PA14A_EIC_EXTINT14            14L  /**< \brief EIC signal: EXTINT14 on PA14 mux A */
#define MUX_PA14A_EIC_EXTINT14             0L
#define PINMUX_PA14A_EIC_EXTINT14  ((PIN_PA14A_EIC_EXTINT14 << 16) | MUX_PA14A_EIC_EXTINT14)
#define PORT_PA14A_EIC_EXTINT14    (1ul << 14)
#define PIN_PA15A_EIC_EXTINT15            15L  /**< \brief EIC signal: EXTINT15 on PA15 mux A */
#define MUX_PA15A_EIC_EXTINT15             0L
#define PINMUX_PA15A_EIC_EXTINT15  ((PIN_PA15A_EIC_EXTINT15 << 16) | MUX_PA15A_EIC_EXTINT15)
#define PORT_PA15A_EIC_EXTINT15    (1ul << 15)
#define PIN_PA08A_EIC_NMI                  8L  /**< \brief EIC signal: NMI on PA08 mux A */
#define MUX_PA08A_EIC_NMI                  0L
#define PINMUX_PA08A_EIC_NMI       ((PIN_PA08A_EIC_NMI << 16) | MUX_PA08A_EIC_NMI)
#define PORT_PA08A_EIC_NMI         (1ul <<  8)
/* ========== PORT definition for SERCOM0 peripheral ========== */
#define PIN_PA04D_SERCOM0_PAD0             4L  /**< \brief SERCOM0 signal: PAD0 on PA04 mux D */
#define MUX_PA04D_SERCOM0_PAD0             3L
#define PINMUX_PA04D_SERCOM0_PAD0  ((PIN_PA04D_SERCOM0_PAD0 << 16) | MUX_PA04D_SERCOM0_PAD0)
#define PORT_PA04D_SERCOM0_PAD0    (1ul <<  4)
#define PIN_PA08C_SERCOM0_PAD0             8L  /**< \brief SERCOM0 signal: PAD0 on PA08 mux C */
#define MUX_PA08C_SERCOM0_PAD0             2L
#define PINMUX_PA08C_SERCOM0_PAD0  ((PIN_PA08C_SERCOM0_PAD0 << 16) | MUX_PA08C_SERCOM0_PAD0)
#define PORT_PA08C_SERCOM0_PAD0    (1ul <<  8)
#define PIN_PA05D_SERCOM0_PAD1             5L  /**< \brief SERCOM0 signal: PAD1 on PA05 mux D */
#define MUX_PA05D_SERCOM0_PAD1             3L
#define PINMUX_PA05D_SERCOM0_PAD1  ((PIN_PA05D_SERCOM0_PAD1 << 16) | MUX_PA05D_SERCOM0_PAD1)
#define PORT_PA05D_SERCOM0_PAD1    (1ul <<  5)
#define PIN_PA09C_SERCOM0_PAD1             9L  /**< \brief SERCOM0 signal: PAD1 on PA09 mux C */
#define MUX_PA09C_SERCOM0_PAD1             2L
#define PINMUX_PA09C_SERCOM0_PAD1  ((PIN_PA09C_SERCOM0_PAD1 << 16) | MUX_PA09C_SERCOM0_PAD1)
#define PORT_PA09C_SERCOM0_PAD1    (1ul <<  9)
#define PIN_PA06D_SERCOM0_PAD2             6L  /**< \brief SERCOM0 signal: PAD2 on PA06 mux D */
#define MUX_PA06D_SERCOM0_PAD2             3L
#define PINMUX_PA06D_SERCOM0_PAD2  ((PIN_PA06D_SERCOM0_PAD2 << 16) | MUX_PA06D_SERCOM0_PAD2)
#define PORT_PA06D_SERCOM0_PAD2    (1ul <<  6)
#define PIN_PA10C_SERCOM0_PAD2            10L  /**< \brief SERCOM0 signal: PAD2 on PA10 mux C */
#define MUX_PA10C_SERCOM0_PAD2             2L
#define PINMUX_PA10C_SERCOM0_PAD2  ((PIN_PA10C_SERCOM0_PAD2 << 16) | MUX_PA10C_SERCOM0_PAD2)
#define PORT_PA10C_SERCOM0_PAD2    (1ul << 10)
#define PIN_PA07D_SERCOM0_PAD3             7L  /**< \brief SERCOM0 signal: PAD3 on PA07 mux D */
#define MUX_PA07D_SERCOM0_PAD3             3L
#define PINMUX_PA07D_SERCOM0_PAD3  ((PIN_PA07D_SERCOM0_PAD3 << 16) | MUX_PA07D_SERCOM0_PAD3)
#define PORT_PA07D_SERCOM0_PAD3    (1ul <<  7)
#define PIN_PA11C_SERCOM0_PAD3            11L  /**< \brief SERCOM0 signal: PAD3 on PA11 mux C */
#define MUX_PA11C_SERCOM0_PAD3             2L
#define PINMUX_PA11C_SERCOM0_PAD3  ((PIN_PA11C_SERCOM0_PAD3 << 16) | MUX_PA11C_SERCOM0_PAD3)
#define PORT_PA11C_SERCOM0_PAD3    (1ul << 11)
/* ========== PORT definition for SERCOM1 peripheral ========== */
#define PIN_PA16C_SERCOM1_PAD0            16L  /**< \brief SERCOM1 signal: PAD0 on PA16 mux C */
#define MUX_PA16C_SERCOM1_PAD0             2L
#define PINMUX_PA16C_SERCOM1_PAD0  ((PIN_PA16C_SERCOM1_PAD0 << 16) | MUX_PA16C_SERCOM1_PAD0)
#define PORT_PA16C_SERCOM1_PAD0    (1ul << 16)
#define PIN_PA17C_SERCOM1_PAD1            17L  /**< \brief SERCOM1 signal: PAD1 on PA17 mux C */
#define MUX_PA17C_SERCOM1_PAD1             2L
#define PINMUX_PA17C_SERCOM1_PAD1  ((PIN_PA17C_SERCOM1_PAD1 << 16) | MUX_PA17C_SERCOM1_PAD1)
#define PORT_PA17C_SERCOM1_PAD1    (1ul << 17)
#define PIN_PA30D_SERCOM1_PAD2            30L  /**< \brief SERCOM1 signal: PAD2 on PA30 mux D */
#define MUX_PA30D_SERCOM1_PAD2             3L
#define PINMUX_PA30D_SERCOM1_PAD2  ((PIN_PA30D_SERCOM1_PAD2 << 16) | MUX_PA30D_SERCOM1_PAD2)
#define PORT_PA30D_SERCOM1_PAD2    (1ul << 30)
#define PIN_PA18C_SERCOM1_PAD2            18L  /**< \brief SERCOM1 signal: PAD2 on PA18 mux C */
#define MUX_PA18C_SERCOM1_PAD2             2L
#define PINMUX_PA18C_SERCOM1_PAD2  ((PIN_PA18C_SERCOM1_PAD2 << 16) | MUX_PA18C_SERCOM1_PAD2)
#define PORT_PA18C_SERCOM1_PAD2    (1ul << 18)
#define PIN_PA31D_SERCOM1_PAD3            31L  /**< \brief SERCOM1 signal: PAD3 on PA31 mux D */
#define MUX_PA31D_SERCOM1_PAD3             3L
#define PINMUX_PA31D_SERCOM1_PAD3  ((PIN_PA31D_SERCOM1_PAD3 << 16) | MUX_PA31D_SERCOM1_PAD3)
#define PORT_PA31D_SERCOM1_PAD3    (1ul << 31)
#define PIN_PA19C_SERCOM1_PAD3            19L  /**< \brief SERCOM1 signal: PAD3 on PA19 mux C */
#define MUX_PA19C_SERCOM1_PAD3             2L
#define PINMUX_PA19C_SERCOM1_PAD3  ((PIN_PA19C_SERCOM1_PAD3 << 16) | MUX_PA19C_SERCOM1_PAD3)
#define PORT_PA19C_SERCOM1_PAD3    (1ul << 19)
/* ========== PORT definition for SERCOM2 peripheral ========== */
#define PIN_PA08D_SERCOM2_PAD0             8L  /**< \brief SERCOM2 signal: PAD0 on PA08 mux D */
#define MUX_PA08D_SERCOM2_PAD0             3L
#define PINMUX_PA08D_SERCOM2_PAD0  ((PIN_PA08D_SERCOM2_PAD0 << 16) | MUX_PA08D_SERCOM2_PAD0)
#define PORT_PA08D_SERCOM2_PAD0    (1ul <<  8)
#define PIN_PA09D_SERCOM2_PAD1             9L  /**< \brief SERCOM2 signal: PAD1 on PA09 mux D */
#define MUX_PA09D_SERCOM2_PAD1             3L
#define PINMUX_PA09D_SERCOM2_PAD1  ((PIN_PA09D_SERCOM2_PAD1 << 16) | MUX_PA09D_SERCOM2_PAD1)
#define PORT_PA09D_SERCOM2_PAD1    (1ul <<  9)
#define PIN_PA10D_SERCOM2_PAD2            10L  /**< \brief SERCOM2 signal: PAD2 on PA10 mux D */
#define MUX_PA10D_SERCOM2_PAD2             3L
#define PINMUX_PA10D_SERCOM2_PAD2  ((PIN_PA10D_SERCOM2_PAD2 << 16) | MUX_PA10D_SERCOM2_PAD2)
#define PORT_PA10D_SERCOM2_PAD2    (1ul << 10)
#define PIN_PA14C_SERCOM2_PAD2            14L  /**< \brief SERCOM2 signal: PAD2 on PA14 mux C */
#define MUX_PA14C_SERCOM2_PAD2             2L
#define PINMUX_PA14C_SERCOM2_PAD2  ((PIN_PA14C_SERCOM2_PAD2 << 16) | MUX_PA14C_SERCOM2_PAD2)
#define PORT_PA14C_SERCOM2_PAD2    (1ul << 14)
#define PIN_PA11D_SERCOM2_PAD3            11L  /**< \brief SERCOM2 signal: PAD3 on PA11 mux D */
#define MUX_PA11D_SERCOM2_PAD3             3L
#define PINMUX_PA11D_SERCOM2_PAD3  ((PIN_PA11D_SERCOM2_PAD3 << 16) | MUX_PA11D_SERCOM2_PAD3)
#define PORT_PA11D_SERCOM2_PAD3    (1ul << 11)
#define PIN_PA15C_SERCOM2_PAD3            15L  /**< \brief SERCOM2 signal: PAD3 on PA15 mux C */
#define MUX_PA15C_SERCOM2_PAD3             2L
#define PINMUX_PA15C_SERCOM2_PAD3  ((PIN_PA15C_SERCOM2_PAD3 << 16) | MUX_PA15C_SERCOM2_PAD3)
#define PORT_PA15C_SERCOM2_PAD3    (1ul << 15)
/* ========== PORT definition for SERCOM3 peripheral ========== */
#define PIN_PA16D_SERCOM3_PAD0            16L  /**< \brief SERCOM3 signal: PAD0 on PA16 mux D */
#define MUX_PA16D_SERCOM3_PAD0             3L
#define PINMUX_PA16D_SERCOM3_PAD0  ((PIN_PA16D_SERCOM3_PAD0 << 16) | MUX_PA16D_SERCOM3_PAD0)
#define PORT_PA16D_SERCOM3_PAD0    (1ul << 16)
#define PIN_PA22C_SERCOM3_PAD0            22L  /**< \brief SERCOM3 signal: PAD0 on PA22 mux C */
#define MUX_PA22C_SERCOM3_PAD0             2L
#define PINMUX_PA22C_SERCOM3_PAD0  ((PIN_PA22C_SERCOM3_PAD0 << 16) | MUX_PA22C_SERCOM3_PAD0)
#define PORT_PA22C_SERCOM3_PAD0    (1ul << 22)
#define PIN_PA17D_SERCOM3_PAD1            17L  /**< \brief SERCOM3 signal: PAD1 on PA17 mux D */
#define MUX_PA17D_SERCOM3_PAD1             3L
#define PINMUX_PA17D_SERCOM3_PAD1  ((PIN_PA17D_SERCOM3_PAD1 << 16) | MUX_PA17D_SERCOM3_PAD1)
#define PORT_PA17D_SERCOM3_PAD1    (1ul << 17)
#define PIN_PA23C_SERCOM3_PAD1            23L  /**< \brief SERCOM3 signal: PAD1 on PA23 mux C */
#define MUX_PA23C_SERCOM3_PAD1             2L
#define PINMUX_PA23C_SERCOM3_PAD1  ((PIN_PA23C_SERCOM3_PAD1 << 16) | MUX_PA23C_SERCOM3_PAD1)
#define PORT_PA23C_SERCOM3_PAD1    (1ul << 23)
#define PIN_PA18D_SERCOM3_PAD2            18L  /**< \brief SERCOM3 signal: PAD2 on PA18 mux D */
#define MUX_PA18D_SERCOM3_PAD2             3L
#define PINMUX_PA18D_SERCOM3_PAD2  ((PIN_PA18D_SERCOM3_PAD2 << 16) | MUX_PA18D_SERCOM3_PAD2)
#define PORT_PA18D_SERCOM3_PAD2    (1ul << 18)
#define PIN_PA24C_SERCOM3_PAD2            24L  /**< \brief SERCOM3 signal: PAD2 on PA24 mux C */
#define MUX_PA24C_SERCOM3_PAD2             2L
#define PINMUX_PA24C_SERCOM3_PAD2  ((PIN_PA24C_SERCOM3_PAD2 << 16) | MUX_PA24C_SERCOM3_PAD2)
#define PORT_PA24C_SERCOM3_PAD2    (1ul << 24)
#define PIN_PA19D_SERCOM3_PAD3            19L  /**< \brief SERCOM3 signal: PAD3 on PA19 mux D */
#define MUX_PA19D_SERCOM3_PAD3             3L
#define PINMUX_PA19D_SERCOM3_PAD3  ((PIN_PA19D_SERCOM3_PAD3 << 16) | MUX_PA19D_SERCOM3_PAD3)
#define PORT_PA19D_SERCOM3_PAD3    (1ul << 19)
#define PIN_PA25C_SERCOM3_PAD3            25L  /**< \brief SERCOM3 signal: PAD3 on PA25 mux C */
#define MUX_PA25C_SERCOM3_PAD3             2L
#define PINMUX_PA25C_SERCOM3_PAD3  ((PIN_PA25C_SERCOM3_PAD3 << 16) | MUX_PA25C_SERCOM3_PAD3)
#define PORT_PA25C_SERCOM3_PAD3    (1ul << 25)
/* ========== PORT definition for TCC0 peripheral ========== */
#define PIN_PA04E_TCC0_WO0                 4L  /**< \brief TCC0 signal: WO0 on PA04 mux E */
#define MUX_PA04E_TCC0_WO0                 4L
#define PINMUX_PA04E_TCC0_WO0      ((PIN_PA04E_TCC0_WO0 << 16) | MUX_PA04E_TCC0_WO0)
#define PORT_PA04E_TCC0_WO0        (1ul <<  4)
#define PIN_PA08E_TCC0_WO0                 8L  /**< \brief TCC0 signal: WO0 on PA08 mux E */
#define MUX_PA08E_TCC0_WO0                 4L
#define PINMUX_PA08E_TCC0_WO0      ((PIN_PA08E_TCC0_WO0 << 16) | MUX_PA08E_TCC0_WO0)
#define PORT_PA08E_TCC0_WO0        (1ul <<  8)
#define PIN_PA05E_TCC0_WO1                 5L  /**< \brief TCC0 signal: WO1 on PA05 mux E */
#define MUX_PA05E_TCC0_WO1                 4L
#define PINMUX_PA05E_TCC0_WO1      ((PIN_PA05E_TCC0_WO1 << 16) | MUX_PA05E_TCC0_WO1)
#define PORT_PA05E_TCC0_WO1        (1ul <<  5)
#define PIN_PA09E_TCC0_WO1                 9L  /**< \brief TCC0 signal: WO1 on PA09 mux E */
#define MUX_PA09E_TCC0_WO1                 4L
#define PINMUX_PA09E_TCC0_WO1      ((PIN_PA09E_TCC0_WO1 << 16) | MUX_PA09E_TCC0_WO1)
#define PORT_PA09E_TCC0_WO1        (1ul <<  9)
#define PIN_PA10F_TCC0_WO2                10L  /**< \brief TCC0 signal: WO2 on PA10 mux F */
#define MUX_PA10F_TCC0_WO2                 5L
#define PINMUX_PA10F_TCC0_WO2      ((PIN_PA10F_TCC0_WO2 << 16) | MUX_PA10F_TCC0_WO2)
#define PORT_PA10F_TCC0_WO2        (1ul << 10)
#define PIN_PA18F_TCC0_WO2                18L  /**< \brief TCC0 signal: WO2 on PA18 mux F */
#define MUX_PA18F_TCC0_WO2                 5L
#define PINMUX_PA18F_TCC0_WO2      ((PIN_PA18F_TCC0_WO2 << 16) | MUX_PA18F_TCC0_WO2)
#define PORT_PA18F_TCC0_WO2        (1ul << 18)
#define PIN_PA11F_TCC0_WO3                11L  /**< \brief TCC0 signal: WO3 on PA11 mux F */
#define MUX_PA11F_TCC0_WO3                 5L
#define PINMUX_PA11F_TCC0_WO3      ((PIN_PA11F_TCC0_WO3 << 16) | MUX_PA11F_TCC0_WO3)
#define PORT_PA11F_TCC0_WO3        (1ul << 11)
#define PIN_PA19F_TCC0_WO3                19L  /**< \brief TCC0 signal: WO3 on PA19 mux F */
#define MUX_PA19F_TCC0_WO3                 5L
#define PINMUX_PA19F_TCC0_WO3      ((PIN_PA19F_TCC0_WO3 << 16) | MUX_PA19F_TCC0_WO3)
#define PORT_PA19F_TCC0_WO3        (1ul << 19)
#define PIN_PA22F_TCC0_WO4                22L  /**< \brief TCC0 signal: WO4 on PA22 mux F */
#define MUX_PA22F_TCC0_WO4                 5L
#define PINMUX_PA22F_TCC0_WO4      ((PIN_PA22F_TCC0_WO4 << 16) | MUX_PA22F_TCC0_WO4)
#define PORT_PA22F_TCC0_WO4        (1ul << 22)
#define PIN_PA14F_TCC0_WO4                14L  /**< \brief TCC0 signal: WO4 on PA14 mux F */
#define MUX_PA14F_TCC0_WO4                 5L
#define PINMUX_PA14F_TCC0_WO4      ((PIN_PA14F_TCC0_WO4 << 16) | MUX_PA14F_TCC0_WO4)
#define PORT_PA14F_TCC0_WO4        (1ul << 14)
#define PIN_PA23F_TCC0_WO5                23L  /**< \brief TCC0 signal: WO5 on PA23 mux F */
#define MUX_PA23F_TCC0_WO5                 5L
#define PINMUX_PA23F_TCC0_WO5      ((PIN_PA23F_TCC0_WO5 << 16) | MUX_PA23F_TCC0_WO5)
#define PORT_PA23F_TCC0_WO5        (1ul << 23)
#define PIN_PA15F_TCC0_WO5                15L  /**< \brief TCC0 signal: WO5 on PA15 mux F */
#define MUX_PA15F_TCC0_WO5                 5L
#define PINMUX_PA15F_TCC0_WO5      ((PIN_PA15F_TCC0_WO5 << 16) | MUX_PA15F_TCC0_WO5)
#define PORT_PA15F_TCC0_WO5        (1ul << 15)
#define PIN_PA16F_TCC0_WO6                16L  /**< \brief TCC0 signal: WO6 on PA16 mux F */
#define MUX_PA16F_TCC0_WO6                 5L
#define PINMUX_PA16F_TCC0_WO6      ((PIN_PA16F_TCC0_WO6 << 16) | MUX_PA16F_TCC0_WO6)
#define PORT_PA16F_TCC0_WO6        (1ul << 16)
#define PIN_PA17F_TCC0_WO7                17L  /**< \brief TCC0 signal: WO7 on PA17 mux F */
#define MUX_PA17F_TCC0_WO7                 5L
#define PINMUX_PA17F_TCC0_WO7      ((PIN_PA17F_TCC0_WO7 << 16) | MUX_PA17F_TCC0_WO7)
#define PORT_PA17F_TCC0_WO7        (1ul << 17)
/* ========== PORT definition for TCC1 peripheral ========== */
#define PIN_PA06E_TCC1_WO0                 6L  /**< \brief TCC1 signal: WO0 on PA06 mux E */
#define MUX_PA06E_TCC1_WO0                 4L
#define PINMUX_PA06E_TCC1_WO0      ((PIN_PA06E_TCC1_WO0 << 16) | MUX_PA06E_TCC1_WO0)
#define PORT_PA06E_TCC1_WO0        (1ul <<  6)
#define PIN_PA10E_TCC1_WO0                10L  /**< \brief TCC1 signal: WO0 on PA10 mux E */
#define MUX_PA10E_TCC1_WO0                 4L
#define PINMUX_PA10E_TCC1_WO0      ((PIN_PA10E_TCC1_WO0 << 16) | MUX_PA10E_TCC1_WO0)
#define PORT_PA10E_TCC1_WO0        (1ul << 10)
#define PIN_PA30E_TCC1_WO0                30L  /**< \brief TCC1 signal: WO0 on PA30 mux E */
#define MUX_PA30E_TCC1_WO0                 4L
#define PINMUX_PA30E_TCC1_WO0      ((PIN_PA30E_TCC1_WO0 << 16) | MUX_PA30E_TCC1_WO0)
#define PORT_PA30E_TCC1_WO0        (1ul << 30)
#define PIN_PA07E_TCC1_WO1                 7L  /**< \brief TCC1 signal: WO1 on PA07 mux E */
#define MUX_PA07E_TCC1_WO1                 4L
#define PINMUX_PA07E_TCC1_WO1      ((PIN_PA07E_TCC1_WO1 << 16) | MUX_PA07E_TCC1_WO1)
#define PORT_PA07E_TCC1_WO1        (1ul <<  7)
#define PIN_PA11E_TCC1_WO1                11L  /**< \brief TCC1 signal: WO1 on PA11 mux E */
#define MUX_PA11E_TCC1_WO1                 4L
#define PINMUX_PA11E_TCC1_WO1      ((PIN_PA11E_TCC1_WO1 << 16) | MUX_PA11E_TCC1_WO1)
#define PORT_PA11E_TCC1_WO1        (1ul << 11)
#define PIN_PA31E_TCC1_WO1                31L  /**< \brief TCC1 signal: WO1 on PA31 mux E */
#define MUX_PA31E_TCC1_WO1                 4L
#define PINMUX_PA31E_TCC1_WO1      ((PIN_PA31E_TCC1_WO1 << 16) | MUX_PA31E_TCC1_WO1)
#define PORT_PA31E_TCC1_WO1        (1ul << 31)
#define PIN_PA08F_TCC1_WO2                 8L  /**< \brief TCC1 signal: WO2 on PA08 mux F */
#define MUX_PA08F_TCC1_WO2                 5L
#define PINMUX_PA08F_TCC1_WO2      ((PIN_PA08F_TCC1_WO2 << 16) | MUX_PA08F_TCC1_WO2)
#define PORT_PA08F_TCC1_WO2        (1ul <<  8)
#define PIN_PA24F_TCC1_WO2                24L  /**< \brief TCC1 signal: WO2 on PA24 mux F */
#define MUX_PA24F_TCC1_WO2                 5L
#define PINMUX_PA24F_TCC1_WO2      ((PIN_PA24F_TCC1_WO2 << 16) | MUX_PA24F_TCC1_WO2)
#define PORT_PA24F_TCC1_WO2        (1ul << 24)
#define PIN_PA09F_TCC1_WO3                 9L  /**< \brief TCC1 signal: WO3 on PA09 mux F */
#define MUX_PA09F_TCC1_WO3                 5L
#define PINMUX_PA09F_TCC1_WO3      ((PIN_PA09F_TCC1_WO3 << 16) | MUX_PA09F_TCC1_WO3)
#define PORT_PA09F_TCC1_WO3        (1ul <<  9)
#define PIN_PA25F_TCC1_WO3                25L  /**< \brief TCC1 signal: WO3 on PA25 mux F */
#define MUX_PA25F_TCC1_WO3                 5L
#define PINMUX_PA25F_TCC1_WO3      ((PIN_PA25F_TCC1_WO3 << 16) | MUX_PA25F_TCC1_WO3)
#define PORT_PA25F_TCC1_WO3        (1ul << 25)
/* ========== PORT definition for TCC2 peripheral ========== */
#define PIN_PA16E_TCC2_WO0                16L  /**< \brief TCC2 signal: WO0 on PA16 mux E */
#define MUX_PA16E_TCC2_WO0                 4L
#define PINMUX_PA16E_TCC2_WO0      ((PIN_PA16E_TCC2_WO0 << 16) | MUX_PA16E_TCC2_WO0)
#define PORT_PA16E_TCC2_WO0        (1ul << 16)
#define PIN_PA17E_TCC2_WO1                17L  /**< \brief TCC2 signal: WO1 on PA17 mux E */
#define MUX_PA17E_TCC2_WO1                 4L
#define PINMUX_PA17E_TCC2_WO1      ((PIN_PA17E_TCC2_WO1 << 16) | MUX_PA17E_TCC2_WO1)
#define PORT_PA17E_TCC2_WO1        (1ul << 17)
/* ========== PORT definition for TC3 peripheral ========== */
#define PIN_PA18E_TC3_WO0                 18L  /**< \brief TC3 signal: WO0 on PA18 mux E */
#define MUX_PA18E_TC3_WO0                  4L
#define PINMUX_PA18E_TC3_WO0       ((PIN_PA18E_TC3_WO0 << 16) | MUX_PA18E_TC3_WO0)
#define PORT_PA18E_TC3_WO0         (1ul << 18)
#define PIN_PA14E_TC3_WO0                 14L  /**< \brief TC3 signal: WO0 on PA14 mux E */
#define MUX_PA14E_TC3_WO0                  4L
#define PINMUX_PA14E_TC3_WO0       ((PIN_PA14E_TC3_WO0 << 16) | MUX_PA14E_TC3_WO0)
#define PORT_PA14E_TC3_WO0         (1ul << 14)
#define PIN_PA19E_TC3_WO1                 19L  /**< \brief TC3 signal: WO1 on PA19 mux E */
#define MUX_PA19E_TC3_WO1                  4L
#define PINMUX_PA19E_TC3_WO1       ((PIN_PA19E_TC3_WO1 << 16) | MUX_PA19E_TC3_WO1)
#define PORT_PA19E_TC3_WO1         (1ul << 19)
#define PIN_PA15E_TC3_WO1                 15L  /**< \brief TC3 signal: WO1 on PA15 mux E */
#define MUX_PA15E_TC3_WO1                  4L
#define PINMUX_PA15E_TC3_WO1       ((PIN_PA15E_TC3_WO1 << 16) | MUX_PA15E_TC3_WO1)
#define PORT_PA15E_TC3_WO1         (1ul << 15)
/* ========== PORT definition for TC4 peripheral ========== */
#define PIN_PA22E_TC4_WO0                 22L  /**< \brief TC4 signal: WO0 on PA22 mux E */
#define MUX_PA22E_TC4_WO0                  4L
#define PINMUX_PA22E_TC4_WO0       ((PIN_PA22E_TC4_WO0 << 16) | MUX_PA22E_TC4_WO0)
#define PORT_PA22E_TC4_WO0         (1ul << 22)
#define PIN_PA23E_TC4_WO1                 23L  /**< \brief TC4 signal: WO1 on PA23 mux E */
#define MUX_PA23E_TC4_WO1                  4L
#define PINMUX_PA23E_TC4_WO1       ((PIN_PA23E_TC4_WO1 << 16) | MUX_PA23E_TC4_WO1)
#define PORT_PA23E_TC4_WO1         (1ul << 23)
/* ========== PORT definition for TC5 peripheral ========== */
#define PIN_PA24E_TC5_WO0                 24L  /**< \brief TC5 signal: WO0 on PA24 mux E */
#define MUX_PA24E_TC5_WO0                  4L
#define PINMUX_PA24E_TC5_WO0       ((PIN_PA24E_TC5_WO0 << 16) | MUX_PA24E_TC5_WO0)
#define PORT_PA24E_TC5_WO0         (1ul << 24)
#define PIN_PA25E_TC5_WO1                 25L  /**< \brief TC5 signal: WO1 on PA25 mux E */
#define MUX_PA25E_TC5_WO1                  4L
#define PINMUX_PA25E_TC5_WO1       ((PIN_PA25E_TC5_WO1 << 16) | MUX_PA25E_TC5_WO1)
#define PORT_PA25E_TC5_WO1         (1ul << 25)
/* ========== PORT definition for ADC peripheral ========== */
#define PIN_PA02B_ADC_AIN0                 2L  /**< \brief ADC signal: AIN0 on PA02 mux B */
#define MUX_PA02B_ADC_AIN0                 1L
#define PINMUX_PA02B_ADC_AIN0      ((PIN_PA02B_ADC_AIN0 << 16) | MUX_PA02B_ADC_AIN0)
#define PORT_PA02B_ADC_AIN0        (1ul <<  2)
#define PIN_PA03B_ADC_AIN1                 3L  /**< \brief ADC signal: AIN1 on PA03 mux B */
#define MUX_PA03B_ADC_AIN1                 1L
#define PINMUX_PA03B_ADC_AIN1      ((PIN_PA03B_ADC_AIN1 << 16) | MUX_PA03B_ADC_AIN1)
#define PORT_PA03B_ADC_AIN1        (1ul <<  3)
#define PIN_PA04B_ADC_AIN4                 4L  /**< \brief ADC signal: AIN4 on PA04 mux B */
#define MUX_PA04B_ADC_AIN4                 1L
#define PINMUX_PA04B_ADC_AIN4      ((PIN_PA04B_ADC_AIN4 << 16) | MUX_PA04B_ADC_AIN4)
#define PORT_PA04B_ADC_AIN4        (1ul <<  4)
#define PIN_PA05B_ADC_AIN5                 5L  /**< \brief ADC signal: AIN5 on PA05 mux B */
#define MUX_PA05B_ADC_AIN5                 1L
#define PINMUX_PA05B_ADC_AIN5      ((PIN_PA05B_ADC_AIN5 << 16) | MUX_PA05B_ADC_AIN5)
#define PORT_PA05B_ADC_AIN5        (1ul <<  5)
#define PIN_PA06B_ADC_AIN6                 6L  /**< \brief ADC signal: AIN6 on PA06 mux B */
#define MUX_PA06B_ADC_AIN6                 1L
#define PINMUX_PA06B_ADC_AIN6      ((PIN_PA06B_ADC_AIN6 << 16) | MUX_PA06B_ADC_AIN6)
#define PORT_PA06B_ADC_AIN6        (1ul <<  6)
#define PIN_PA07B_ADC_AIN7                 7L  /**< \brief ADC signal: AIN7 on PA07 mux B */
#define MUX_PA07B_ADC_AIN7                 1L
#define PINMUX_PA07B_ADC_AIN7      ((PIN_PA07B_ADC_AIN7 << 16) | MUX_PA07B_ADC_AIN7)
#define PORT_PA07B_ADC_AIN7        (1ul <<  7)
#define PIN_PB02B_ADC_AIN10               34L  /**< \brief ADC signal: AIN10 on PB02 mux B */
#define MUX_PB02B_ADC_AIN10                1L
#define PINMUX_PB02B_ADC_AIN10     ((PIN_PB02B_ADC_AIN10 << 16) | MUX_PB02B_ADC_AIN10)
#define PORT_PB02B_ADC_AIN10       (1ul <<  2)
#define PIN_PB03B_ADC_AIN11               35L  /**< \brief ADC signal: AIN11 on PB03 mux B */
#define MUX_PB03B_ADC_AIN11                1L
#define PINMUX_PB03B_ADC_AIN11     ((PIN_PB03B_ADC_AIN11 << 16) | MUX_PB03B_ADC_AIN11)
#define PORT_PB03B_ADC_AIN11       (1ul <<  3)
#define PIN_PB04B_ADC_AIN12               36L  /**< \brief ADC signal: AIN12 on PB04 mux B */
#define MUX_PB04B_ADC_AIN12                1L
#define PINMUX_PB04B_ADC_AIN12     ((PIN_PB04B_ADC_AIN12 << 16) | MUX_PB04B_ADC_AIN12)
#define PORT_PB04B_ADC_AIN12       (1ul <<  4)
#define PIN_PB05B_ADC_AIN13               37L  /**< \brief ADC signal: AIN13 on PB05 mux B */
#define MUX_PB05B_ADC_AIN13                1L
#define PINMUX_PB05B_ADC_AIN13     ((PIN_PB05B_ADC_AIN13 << 16) | MUX_PB05B_ADC_AIN13)
#define PORT_PB05B_ADC_AIN13       (1ul <<  5)
#define PIN_PA08B_ADC_AIN16                8L  /**< \brief ADC signal: AIN16 on PA08 mux B */
#define MUX_PA08B_ADC_AIN16                1L
#define PINMUX_PA08B_ADC_AIN16     ((PIN_PA08B_ADC_AIN16 << 16) | MUX_PA08B_ADC_AIN16)
#define PORT_PA08B_ADC_AIN16       (1ul <<  8)
#define PIN_PA09B_ADC_AIN17                9L  /**< \brief ADC signal: AIN17 on PA09 mux B */
#define MUX_PA09B_ADC_AIN17                1L
#define PINMUX_PA09B_ADC_AIN17     ((PIN_PA09B_ADC_AIN17 << 16) | MUX_PA09B_ADC_AIN17)
#define PORT_PA09B_ADC_AIN17       (1ul <<  9)
#define PIN_PA10B_ADC_AIN18               10L  /**< \brief ADC signal: AIN18 on PA10 mux B */
#define MUX_PA10B_ADC_AIN18                1L
#define PINMUX_PA10B_ADC_AIN18     ((PIN_PA10B_ADC_AIN18 << 16) | MUX_PA10B_ADC_AIN18)
#define PORT_PA10B_ADC_AIN18       (1ul << 10)
#define PIN_PA11B_ADC_AIN19               11L  /**< \brief ADC signal: AIN19 on PA11 mux B */
#define MUX_PA11B_ADC_AIN19                1L
#define PINMUX_PA11B_ADC_AIN19     ((PIN_PA11B_ADC_AIN19 << 16) | MUX_PA11B_ADC_AIN19)
#define PORT_PA11B_ADC_AIN19       (1ul << 11)
#define PIN_PA04B_ADC_VREFP                4L  /**< \brief ADC signal: VREFP on PA04 mux B */
#define MUX_PA04B_ADC_VREFP                1L
#define PINMUX_PA04B_ADC_VREFP     ((PIN_PA04B_ADC_VREFP << 16) | MUX_PA04B_ADC_VREFP)
#define PORT_PA04B_ADC_VREFP       (1ul <<  4)
/* ========== PORT definition for AC peripheral ========== */
#define PIN_PA04B_AC_AIN0                  4L  /**< \brief AC signal: AIN0 on PA04 mux B */
#define MUX_PA04B_AC_AIN0                  1L
#define PINMUX_PA04B_AC_AIN0       ((PIN_PA04B_AC_AIN0 << 16) | MUX_PA04B_AC_AIN0)
#define PORT_PA04B_AC_AIN0         (1ul <<  4)
#define PIN_PA05B_AC_AIN1                  5L  /**< \brief AC signal: AIN1 on PA05 mux B */
#define MUX_PA05B_AC_AIN1                  1L
#define PINMUX_PA05B_AC_AIN1       ((PIN_PA05B_AC_AIN1 << 16) | MUX_PA05B_AC_AIN1)
#define PORT_PA05B_AC_AIN1         (1ul <<  5)
#define PIN_PA06B_AC_AIN2                  6L  /**< \brief AC signal: AIN2 on PA06 mux B */
#define MUX_PA06B_AC_AIN2                  1L
#define PINMUX_PA06B_AC_AIN2       ((PIN_PA06B_AC_AIN2 << 16) | MUX_PA06B_AC_AIN2)
#define PORT_PA06B_AC_AIN2         (1ul <<  6)
#define PIN_PA07B_AC_AIN3                  7L  /**< \brief AC signal: AIN3 on PA07 mux B */
#define MUX_PA07B_AC_AIN3                  1L
#define PINMUX_PA07B_AC_AIN3       ((PIN_PA07B_AC_AIN3 << 16) | MUX_PA07B_AC_AIN3)
#define PORT_PA07B_AC_AIN3         (1ul <<  7)
#define PIN_PA18H_AC_CMP0                 18L  /**< \brief AC signal: CMP0 on PA18 mux H */
#define MUX_PA18H_AC_CMP0                  7L
#define PINMUX_PA18H_AC_CMP0       ((PIN_PA18H_AC_CMP0 << 16) | MUX_PA18H_AC_CMP0)
#define PORT_PA18H_AC_CMP0         (1ul << 18)
#define PIN_PA19H_AC_CMP1                 19L  /**< \brief AC signal: CMP1 on PA19 mux H */
#define MUX_PA19H_AC_CMP1                  7L
#define PINMUX_PA19H_AC_CMP1       ((PIN_PA19H_AC_CMP1 << 16) | MUX_PA19H_AC_CMP1)
#define PORT_PA19H_AC_CMP1         (1ul << 19)
/* ========== PORT definition for DAC peripheral ========== */
#define PIN_PA02B_DAC_VOUT                 2L  /**< \brief DAC signal: VOUT on PA02 mux B */
#define MUX_PA02B_DAC_VOUT                 1L
#define PINMUX_PA02B_DAC_VOUT      ((PIN_PA02B_DAC_VOUT << 16) | MUX_PA02B_DAC_VOUT)
#define PORT_PA02B_DAC_VOUT        (1ul <<  2)
#define PIN_PA03B_DAC_VREFP                3L  /**< \brief DAC signal: VREFP on PA03 mux B */
#define MUX_PA03B_DAC_VREFP                1L
#define PINMUX_PA03B_DAC_VREFP     ((PIN_PA03B_DAC_VREFP << 16) | MUX_PA03B_DAC_VREFP)
#define PORT_PA03B_DAC_VREFP       (1ul <<  3)
/* ========== PORT definition for AC1 peripheral ========== */
#define PIN_PB04B_AC1_AIN0                36L  /**< \brief AC1 signal: AIN0 on PB04 mux B */
#define MUX_PB04B_AC1_AIN0                 1L
#define PINMUX_PB04B_AC1_AIN0      ((PIN_PB04B_AC1_AIN0 << 16) | MUX_PB04B_AC1_AIN0)
#define PORT_PB04B_AC1_AIN0        (1ul <<  4)
#define PIN_PB05B_AC1_AIN1                37L  /**< \brief AC1 signal: AIN1 on PB05 mux B */
#define MUX_PB05B_AC1_AIN1                 1L
#define PINMUX_PB05B_AC1_AIN1      ((PIN_PB05B_AC1_AIN1 << 16) | MUX_PB05B_AC1_AIN1)
#define PORT_PB05B_AC1_AIN1        (1ul <<  5)
#define PIN_PB02B_AC1_AIN2                34L  /**< \brief AC1 signal: AIN2 on PB02 mux B */
#define MUX_PB02B_AC1_AIN2                 1L
#define PINMUX_PB02B_AC1_AIN2      ((PIN_PB02B_AC1_AIN2 << 16) | MUX_PB02B_AC1_AIN2)
#define PORT_PB02B_AC1_AIN2        (1ul <<  2)
#define PIN_PB03B_AC1_AIN3                35L  /**< \brief AC1 signal: AIN3 on PB03 mux B */
#define MUX_PB03B_AC1_AIN3                 1L
#define PINMUX_PB03B_AC1_AIN3      ((PIN_PB03B_AC1_AIN3 << 16) | MUX_PB03B_AC1_AIN3)
#define PORT_PB03B_AC1_AIN3        (1ul <<  3)
#define PIN_PA24H_AC1_CMP0                24L  /**< \brief AC1 signal: CMP0 on PA24 mux H */
#define MUX_PA24H_AC1_CMP0                 7L
#define PINMUX_PA24H_AC1_CMP0      ((PIN_PA24H_AC1_CMP0 << 16) | MUX_PA24H_AC1_CMP0)
#define PORT_PA24H_AC1_CMP0        (1ul << 24)
#define PIN_PA25H_AC1_CMP1                25L  /**< \brief AC1 signal: CMP1 on PA25 mux H */
#define MUX_PA25H_AC1_CMP1                 7L
#define PINMUX_PA25H_AC1_CMP1      ((PIN_PA25H_AC1_CMP1 << 16) | MUX_PA25H_AC1_CMP1)
#define PORT_PA25H_AC1_CMP1        (1ul << 25)

#endif /* _SAMD21E15L_PIO_ */
