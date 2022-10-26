/**************************************************************************//**
 * @file
 * @brief EFR32ZG23 GPIO register and bit field definitions
 ******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EFR32ZG23_GPIO_H
#define EFR32ZG23_GPIO_H
#define GPIO_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/

#include "efr32zg23_gpio_port.h"

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< ACMP0 pin enable                                   */
  __IOM uint32_t ACMPOUTROUTE;                       /**< ACMPOUT port/pin select                            */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_ACMPROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< CMU pin enable                                     */
  __IOM uint32_t CLKIN0ROUTE;                        /**< CLKIN0 port/pin select                             */
  __IOM uint32_t CLKOUT0ROUTE;                       /**< CLKOUT0 port/pin select                            */
  __IOM uint32_t CLKOUT1ROUTE;                       /**< CLKOUT1 port/pin select                            */
  __IOM uint32_t CLKOUT2ROUTE;                       /**< CLKOUT2 port/pin select                            */
  uint32_t       RESERVED0[2U];                      /**< Reserved for future use                            */
} GPIO_CMUROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< EUSART0 pin enable                                 */
  __IOM uint32_t CSROUTE;                            /**< CS port/pin select                                 */
  __IOM uint32_t CTSROUTE;                           /**< CTS port/pin select                                */
  __IOM uint32_t RTSROUTE;                           /**< RTS port/pin select                                */
  __IOM uint32_t RXROUTE;                            /**< RX port/pin select                                 */
  __IOM uint32_t SCLKROUTE;                          /**< SCLK port/pin select                               */
  __IOM uint32_t TXROUTE;                            /**< TX port/pin select                                 */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_EUSARTROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< FRC pin enable                                     */
  __IOM uint32_t DCLKROUTE;                          /**< DCLK port/pin select                               */
  __IOM uint32_t DFRAMEROUTE;                        /**< DFRAME port/pin select                             */
  __IOM uint32_t DOUTROUTE;                          /**< DOUT port/pin select                               */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_FRCROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< I2C0 pin enable                                    */
  __IOM uint32_t SCLROUTE;                           /**< SCL port/pin select                                */
  __IOM uint32_t SDAROUTE;                           /**< SDA port/pin select                                */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_I2CROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< KEYSCAN pin enable                                 */
  __IOM uint32_t COLOUT0ROUTE;                       /**< COLOUT0 port/pin select                            */
  __IOM uint32_t COLOUT1ROUTE;                       /**< COLOUT1 port/pin select                            */
  __IOM uint32_t COLOUT2ROUTE;                       /**< COLOUT2 port/pin select                            */
  __IOM uint32_t COLOUT3ROUTE;                       /**< COLOUT3 port/pin select                            */
  __IOM uint32_t COLOUT4ROUTE;                       /**< COLOUT4 port/pin select                            */
  __IOM uint32_t COLOUT5ROUTE;                       /**< COLOUT5 port/pin select                            */
  __IOM uint32_t COLOUT6ROUTE;                       /**< COLOUT6 port/pin select                            */
  __IOM uint32_t COLOUT7ROUTE;                       /**< COLOUT7 port/pin select                            */
  __IOM uint32_t ROWSENSE0ROUTE;                     /**< ROWSENSE0 port/pin select                          */
  __IOM uint32_t ROWSENSE1ROUTE;                     /**< ROWSENSE1 port/pin select                          */
  __IOM uint32_t ROWSENSE2ROUTE;                     /**< ROWSENSE2 port/pin select                          */
  __IOM uint32_t ROWSENSE3ROUTE;                     /**< ROWSENSE3 port/pin select                          */
  __IOM uint32_t ROWSENSE4ROUTE;                     /**< ROWSENSE4 port/pin select                          */
  __IOM uint32_t ROWSENSE5ROUTE;                     /**< ROWSENSE5 port/pin select                          */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_KEYSCANROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< LESENSE pin enable                                 */
  __IOM uint32_t CH0OUTROUTE;                        /**< CH0OUT port/pin select                             */
  __IOM uint32_t CH1OUTROUTE;                        /**< CH1OUT port/pin select                             */
  __IOM uint32_t CH2OUTROUTE;                        /**< CH2OUT port/pin select                             */
  __IOM uint32_t CH3OUTROUTE;                        /**< CH3OUT port/pin select                             */
  __IOM uint32_t CH4OUTROUTE;                        /**< CH4OUT port/pin select                             */
  __IOM uint32_t CH5OUTROUTE;                        /**< CH5OUT port/pin select                             */
  __IOM uint32_t CH6OUTROUTE;                        /**< CH6OUT port/pin select                             */
  __IOM uint32_t CH7OUTROUTE;                        /**< CH7OUT port/pin select                             */
  __IOM uint32_t CH8OUTROUTE;                        /**< CH8OUT port/pin select                             */
  __IOM uint32_t CH9OUTROUTE;                        /**< CH9OUT port/pin select                             */
  __IOM uint32_t CH10OUTROUTE;                       /**< CH10OUT port/pin select                            */
  __IOM uint32_t CH11OUTROUTE;                       /**< CH11OUT port/pin select                            */
  __IOM uint32_t CH12OUTROUTE;                       /**< CH12OUT port/pin select                            */
  __IOM uint32_t CH13OUTROUTE;                       /**< CH13OUT port/pin select                            */
  __IOM uint32_t CH14OUTROUTE;                       /**< CH14OUT port/pin select                            */
  __IOM uint32_t CH15OUTROUTE;                       /**< CH15OUT port/pin select                            */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_LESENSEROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< LETIMER pin enable                                 */
  __IOM uint32_t OUT0ROUTE;                          /**< OUT0 port/pin select                               */
  __IOM uint32_t OUT1ROUTE;                          /**< OUT1 port/pin select                               */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_LETIMERROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< MODEM pin enable                                   */
  __IOM uint32_t ANT0ROUTE;                          /**< ANT0 port/pin select                               */
  __IOM uint32_t ANT1ROUTE;                          /**< ANT1 port/pin select                               */
  __IOM uint32_t ANTROLLOVERROUTE;                   /**< ANTROLLOVER port/pin select                        */
  __IOM uint32_t ANTRR0ROUTE;                        /**< ANTRR0 port/pin select                             */
  __IOM uint32_t ANTRR1ROUTE;                        /**< ANTRR1 port/pin select                             */
  __IOM uint32_t ANTRR2ROUTE;                        /**< ANTRR2 port/pin select                             */
  __IOM uint32_t ANTRR3ROUTE;                        /**< ANTRR3 port/pin select                             */
  __IOM uint32_t ANTRR4ROUTE;                        /**< ANTRR4 port/pin select                             */
  __IOM uint32_t ANTRR5ROUTE;                        /**< ANTRR5 port/pin select                             */
  __IOM uint32_t ANTSWENROUTE;                       /**< ANTSWEN port/pin select                            */
  __IOM uint32_t ANTSWUSROUTE;                       /**< ANTSWUS port/pin select                            */
  __IOM uint32_t ANTTRIGROUTE;                       /**< ANTTRIG port/pin select                            */
  __IOM uint32_t ANTTRIGSTOPROUTE;                   /**< ANTTRIGSTOP port/pin select                        */
  __IOM uint32_t DCLKROUTE;                          /**< DCLK port/pin select                               */
  __IOM uint32_t DINROUTE;                           /**< DIN port/pin select                                */
  __IOM uint32_t DOUTROUTE;                          /**< DOUT port/pin select                               */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_MODEMROUTE_TypeDef;

typedef struct {
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t S0INROUTE;                          /**< S0IN port/pin select                               */
  __IOM uint32_t S1INROUTE;                          /**< S1IN port/pin select                               */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
} GPIO_PCNTROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< PRS0 pin enable                                    */
  __IOM uint32_t ASYNCH0ROUTE;                       /**< ASYNCH0 port/pin select                            */
  __IOM uint32_t ASYNCH1ROUTE;                       /**< ASYNCH1 port/pin select                            */
  __IOM uint32_t ASYNCH2ROUTE;                       /**< ASYNCH2 port/pin select                            */
  __IOM uint32_t ASYNCH3ROUTE;                       /**< ASYNCH3 port/pin select                            */
  __IOM uint32_t ASYNCH4ROUTE;                       /**< ASYNCH4 port/pin select                            */
  __IOM uint32_t ASYNCH5ROUTE;                       /**< ASYNCH5 port/pin select                            */
  __IOM uint32_t ASYNCH6ROUTE;                       /**< ASYNCH6 port/pin select                            */
  __IOM uint32_t ASYNCH7ROUTE;                       /**< ASYNCH7 port/pin select                            */
  __IOM uint32_t ASYNCH8ROUTE;                       /**< ASYNCH8 port/pin select                            */
  __IOM uint32_t ASYNCH9ROUTE;                       /**< ASYNCH9 port/pin select                            */
  __IOM uint32_t ASYNCH10ROUTE;                      /**< ASYNCH10 port/pin select                           */
  __IOM uint32_t ASYNCH11ROUTE;                      /**< ASYNCH11 port/pin select                           */
  __IOM uint32_t SYNCH0ROUTE;                        /**< SYNCH0 port/pin select                             */
  __IOM uint32_t SYNCH1ROUTE;                        /**< SYNCH1 port/pin select                             */
  __IOM uint32_t SYNCH2ROUTE;                        /**< SYNCH2 port/pin select                             */
  __IOM uint32_t SYNCH3ROUTE;                        /**< SYNCH3 port/pin select                             */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_PRSROUTE_TypeDef;

typedef struct {
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IOM uint32_t BUFOUTREQINASYNCROUTE;              /**< BUFOUTREQINASYNC port/pin select                   */
  uint32_t       RESERVED1[1U];                      /**< Reserved for future use                            */
} GPIO_SYXOROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< TIMER0 pin enable                                  */
  __IOM uint32_t CC0ROUTE;                           /**< CC0 port/pin select                                */
  __IOM uint32_t CC1ROUTE;                           /**< CC1 port/pin select                                */
  __IOM uint32_t CC2ROUTE;                           /**< CC2 port/pin select                                */
  __IOM uint32_t CDTI0ROUTE;                         /**< CDTI0 port/pin select                              */
  __IOM uint32_t CDTI1ROUTE;                         /**< CDTI1 port/pin select                              */
  __IOM uint32_t CDTI2ROUTE;                         /**< CDTI2 port/pin select                              */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_TIMERROUTE_TypeDef;

typedef struct {
  __IOM uint32_t ROUTEEN;                            /**< USART0 pin enable                                  */
  __IOM uint32_t CSROUTE;                            /**< CS port/pin select                                 */
  __IOM uint32_t CTSROUTE;                           /**< CTS port/pin select                                */
  __IOM uint32_t RTSROUTE;                           /**< RTS port/pin select                                */
  __IOM uint32_t RXROUTE;                            /**< RX port/pin select                                 */
  __IOM uint32_t CLKROUTE;                           /**< SCLK port/pin select                               */
  __IOM uint32_t TXROUTE;                            /**< TX port/pin select                                 */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
} GPIO_USARTROUTE_TypeDef;

typedef struct {
  __IM uint32_t             IPVERSION;               /**< main                                               */
  uint32_t                  RESERVED0[11U];          /**< Reserved for future use                            */
  GPIO_PORT_TypeDef         P[4U];                   /**<                                                    */
  uint32_t                  RESERVED1[132U];         /**< Reserved for future use                            */
  __IOM uint32_t            LOCK;                    /**< Lock Register                                      */
  uint32_t                  RESERVED2[3U];           /**< Reserved for future use                            */
  __IM uint32_t             GPIOLOCKSTATUS;          /**< Lock Status                                        */
  uint32_t                  RESERVED3[3U];           /**< Reserved for future use                            */
  __IOM uint32_t            ABUSALLOC;               /**< A Bus allocation                                   */
  __IOM uint32_t            BBUSALLOC;               /**< B Bus allocation                                   */
  __IOM uint32_t            CDBUSALLOC;              /**< CD Bus allocation                                  */
  uint32_t                  RESERVED4[53U];          /**< Reserved for future use                            */
  __IOM uint32_t            EXTIPSELL;               /**< External Interrupt Port Select Low                 */
  __IOM uint32_t            EXTIPSELH;               /**< External interrupt Port Select High                */
  __IOM uint32_t            EXTIPINSELL;             /**< External Interrupt Pin Select Low                  */
  __IOM uint32_t            EXTIPINSELH;             /**< External Interrupt Pin Select High                 */
  __IOM uint32_t            EXTIRISE;                /**< External Interrupt Rising Edge Trigger             */
  __IOM uint32_t            EXTIFALL;                /**< External Interrupt Falling Edge Trigger            */
  uint32_t                  RESERVED5[2U];           /**< Reserved for future use                            */
  __IOM uint32_t            IF;                      /**< Interrupt Flag                                     */
  __IOM uint32_t            IEN;                     /**< Interrupt Enable                                   */
  uint32_t                  RESERVED6[1U];           /**< Reserved for future use                            */
  __IOM uint32_t            EM4WUEN;                 /**< EM4 wakeup enable                                  */
  __IOM uint32_t            EM4WUPOL;                /**< EM4 wakeup polarity                                */
  uint32_t                  RESERVED7[3U];           /**< Reserved for future use                            */
  __IOM uint32_t            DBGROUTEPEN;             /**< Debugger Route Pin enable                          */
  __IOM uint32_t            TRACEROUTEPEN;           /**< Trace Route Pin Enable                             */
  uint32_t                  RESERVED8[2U];           /**< Reserved for future use                            */
  uint32_t                  RESERVED9[4U];           /**< Reserved for future use                            */
  __IOM uint32_t            LCDSEG;                  /**< LCD Segment Enable                                 */
  uint32_t                  RESERVED10[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            LCDCOM;                  /**< LCD Common Enable                                  */
  uint32_t                  RESERVED11[3U];          /**< Reserved for future use                            */
  GPIO_ACMPROUTE_TypeDef    ACMPROUTE[2U];           /**< acmp0 DBUS config registers                        */
  GPIO_CMUROUTE_TypeDef     CMUROUTE;                /**< cmu DBUS config registers                          */
  uint32_t                  RESERVED12[4U];          /**< Reserved for future use                            */
  GPIO_EUSARTROUTE_TypeDef  EUSARTROUTE[3U];         /**< eusart0 DBUS config registers                      */
  GPIO_FRCROUTE_TypeDef     FRCROUTE;                /**< frc DBUS config registers                          */
  GPIO_I2CROUTE_TypeDef     I2CROUTE[2U];            /**< i2c0 DBUS config registers                         */
  GPIO_KEYSCANROUTE_TypeDef KEYSCANROUTE;            /**< keyscan DBUS config registers                      */
  GPIO_LESENSEROUTE_TypeDef LESENSEROUTE;            /**< lesense DBUS config registers                      */
  GPIO_LETIMERROUTE_TypeDef LETIMERROUTE;            /**< letimer DBUS config registers                      */
  GPIO_MODEMROUTE_TypeDef   MODEMROUTE;              /**< modem DBUS config registers                        */
  GPIO_PCNTROUTE_TypeDef    PCNTROUTE[1U];           /**< pcnt0 DBUS config registers                        */
  GPIO_PRSROUTE_TypeDef     PRSROUTE[1U];            /**< prs0 DBUS config registers                         */
  uint32_t                  RESERVED13[23U];         /**< Reserved for future use                            */
  GPIO_SYXOROUTE_TypeDef    SYXOROUTE[1U];           /**< syxo0 DBUS config registers                        */
  GPIO_TIMERROUTE_TypeDef   TIMERROUTE[5U];          /**< timer0 DBUS config registers                       */
  GPIO_USARTROUTE_TypeDef   USARTROUTE[1U];          /**< usart0 DBUS config registers                       */
  uint32_t                  RESERVED14[530U];        /**< Reserved for future use                            */
  __IM uint32_t             IPVERSION_SET;           /**< main                                               */
  uint32_t                  RESERVED15[11U];         /**< Reserved for future use                            */
  GPIO_PORT_TypeDef         P_SET[4U];               /**<                                                    */
  uint32_t                  RESERVED16[132U];        /**< Reserved for future use                            */
  __IOM uint32_t            LOCK_SET;                /**< Lock Register                                      */
  uint32_t                  RESERVED17[3U];          /**< Reserved for future use                            */
  __IM uint32_t             GPIOLOCKSTATUS_SET;      /**< Lock Status                                        */
  uint32_t                  RESERVED18[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            ABUSALLOC_SET;           /**< A Bus allocation                                   */
  __IOM uint32_t            BBUSALLOC_SET;           /**< B Bus allocation                                   */
  __IOM uint32_t            CDBUSALLOC_SET;          /**< CD Bus allocation                                  */
  uint32_t                  RESERVED19[53U];         /**< Reserved for future use                            */
  __IOM uint32_t            EXTIPSELL_SET;           /**< External Interrupt Port Select Low                 */
  __IOM uint32_t            EXTIPSELH_SET;           /**< External interrupt Port Select High                */
  __IOM uint32_t            EXTIPINSELL_SET;         /**< External Interrupt Pin Select Low                  */
  __IOM uint32_t            EXTIPINSELH_SET;         /**< External Interrupt Pin Select High                 */
  __IOM uint32_t            EXTIRISE_SET;            /**< External Interrupt Rising Edge Trigger             */
  __IOM uint32_t            EXTIFALL_SET;            /**< External Interrupt Falling Edge Trigger            */
  uint32_t                  RESERVED20[2U];          /**< Reserved for future use                            */
  __IOM uint32_t            IF_SET;                  /**< Interrupt Flag                                     */
  __IOM uint32_t            IEN_SET;                 /**< Interrupt Enable                                   */
  uint32_t                  RESERVED21[1U];          /**< Reserved for future use                            */
  __IOM uint32_t            EM4WUEN_SET;             /**< EM4 wakeup enable                                  */
  __IOM uint32_t            EM4WUPOL_SET;            /**< EM4 wakeup polarity                                */
  uint32_t                  RESERVED22[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            DBGROUTEPEN_SET;         /**< Debugger Route Pin enable                          */
  __IOM uint32_t            TRACEROUTEPEN_SET;       /**< Trace Route Pin Enable                             */
  uint32_t                  RESERVED23[2U];          /**< Reserved for future use                            */
  uint32_t                  RESERVED24[4U];          /**< Reserved for future use                            */
  __IOM uint32_t            LCDSEG_SET;              /**< LCD Segment Enable                                 */
  uint32_t                  RESERVED25[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            LCDCOM_SET;              /**< LCD Common Enable                                  */
  uint32_t                  RESERVED26[3U];          /**< Reserved for future use                            */
  GPIO_ACMPROUTE_TypeDef    ACMPROUTE_SET[2U];       /**< acmp0 DBUS config registers                        */
  GPIO_CMUROUTE_TypeDef     CMUROUTE_SET;            /**< cmu DBUS config registers                          */
  uint32_t                  RESERVED27[4U];          /**< Reserved for future use                            */
  GPIO_EUSARTROUTE_TypeDef  EUSARTROUTE_SET[3U];     /**< eusart0 DBUS config registers                      */
  GPIO_FRCROUTE_TypeDef     FRCROUTE_SET;            /**< frc DBUS config registers                          */
  GPIO_I2CROUTE_TypeDef     I2CROUTE_SET[2U];        /**< i2c0 DBUS config registers                         */
  GPIO_KEYSCANROUTE_TypeDef KEYSCANROUTE_SET;        /**< keyscan DBUS config registers                      */
  GPIO_LESENSEROUTE_TypeDef LESENSEROUTE_SET;        /**< lesense DBUS config registers                      */
  GPIO_LETIMERROUTE_TypeDef LETIMERROUTE_SET;        /**< letimer DBUS config registers                      */
  GPIO_MODEMROUTE_TypeDef   MODEMROUTE_SET;          /**< modem DBUS config registers                        */
  GPIO_PCNTROUTE_TypeDef    PCNTROUTE_SET[1U];       /**< pcnt0 DBUS config registers                        */
  GPIO_PRSROUTE_TypeDef     PRSROUTE_SET[1U];        /**< prs0 DBUS config registers                         */
  uint32_t                  RESERVED28[23U];         /**< Reserved for future use                            */
  GPIO_SYXOROUTE_TypeDef    SYXOROUTE_SET[1U];       /**< syxo0 DBUS config registers                        */
  GPIO_TIMERROUTE_TypeDef   TIMERROUTE_SET[5U];      /**< timer0 DBUS config registers                       */
  GPIO_USARTROUTE_TypeDef   USARTROUTE_SET[1U];      /**< usart0 DBUS config registers                       */
  uint32_t                  RESERVED29[530U];        /**< Reserved for future use                            */
  __IM uint32_t             IPVERSION_CLR;           /**< main                                               */
  uint32_t                  RESERVED30[11U];         /**< Reserved for future use                            */
  GPIO_PORT_TypeDef         P_CLR[4U];               /**<                                                    */
  uint32_t                  RESERVED31[132U];        /**< Reserved for future use                            */
  __IOM uint32_t            LOCK_CLR;                /**< Lock Register                                      */
  uint32_t                  RESERVED32[3U];          /**< Reserved for future use                            */
  __IM uint32_t             GPIOLOCKSTATUS_CLR;      /**< Lock Status                                        */
  uint32_t                  RESERVED33[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            ABUSALLOC_CLR;           /**< A Bus allocation                                   */
  __IOM uint32_t            BBUSALLOC_CLR;           /**< B Bus allocation                                   */
  __IOM uint32_t            CDBUSALLOC_CLR;          /**< CD Bus allocation                                  */
  uint32_t                  RESERVED34[53U];         /**< Reserved for future use                            */
  __IOM uint32_t            EXTIPSELL_CLR;           /**< External Interrupt Port Select Low                 */
  __IOM uint32_t            EXTIPSELH_CLR;           /**< External interrupt Port Select High                */
  __IOM uint32_t            EXTIPINSELL_CLR;         /**< External Interrupt Pin Select Low                  */
  __IOM uint32_t            EXTIPINSELH_CLR;         /**< External Interrupt Pin Select High                 */
  __IOM uint32_t            EXTIRISE_CLR;            /**< External Interrupt Rising Edge Trigger             */
  __IOM uint32_t            EXTIFALL_CLR;            /**< External Interrupt Falling Edge Trigger            */
  uint32_t                  RESERVED35[2U];          /**< Reserved for future use                            */
  __IOM uint32_t            IF_CLR;                  /**< Interrupt Flag                                     */
  __IOM uint32_t            IEN_CLR;                 /**< Interrupt Enable                                   */
  uint32_t                  RESERVED36[1U];          /**< Reserved for future use                            */
  __IOM uint32_t            EM4WUEN_CLR;             /**< EM4 wakeup enable                                  */
  __IOM uint32_t            EM4WUPOL_CLR;            /**< EM4 wakeup polarity                                */
  uint32_t                  RESERVED37[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            DBGROUTEPEN_CLR;         /**< Debugger Route Pin enable                          */
  __IOM uint32_t            TRACEROUTEPEN_CLR;       /**< Trace Route Pin Enable                             */
  uint32_t                  RESERVED38[2U];          /**< Reserved for future use                            */
  uint32_t                  RESERVED39[4U];          /**< Reserved for future use                            */
  __IOM uint32_t            LCDSEG_CLR;              /**< LCD Segment Enable                                 */
  uint32_t                  RESERVED40[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            LCDCOM_CLR;              /**< LCD Common Enable                                  */
  uint32_t                  RESERVED41[3U];          /**< Reserved for future use                            */
  GPIO_ACMPROUTE_TypeDef    ACMPROUTE_CLR[2U];       /**< acmp0 DBUS config registers                        */
  GPIO_CMUROUTE_TypeDef     CMUROUTE_CLR;            /**< cmu DBUS config registers                          */
  uint32_t                  RESERVED42[4U];          /**< Reserved for future use                            */
  GPIO_EUSARTROUTE_TypeDef  EUSARTROUTE_CLR[3U];     /**< eusart0 DBUS config registers                      */
  GPIO_FRCROUTE_TypeDef     FRCROUTE_CLR;            /**< frc DBUS config registers                          */
  GPIO_I2CROUTE_TypeDef     I2CROUTE_CLR[2U];        /**< i2c0 DBUS config registers                         */
  GPIO_KEYSCANROUTE_TypeDef KEYSCANROUTE_CLR;        /**< keyscan DBUS config registers                      */
  GPIO_LESENSEROUTE_TypeDef LESENSEROUTE_CLR;        /**< lesense DBUS config registers                      */
  GPIO_LETIMERROUTE_TypeDef LETIMERROUTE_CLR;        /**< letimer DBUS config registers                      */
  GPIO_MODEMROUTE_TypeDef   MODEMROUTE_CLR;          /**< modem DBUS config registers                        */
  GPIO_PCNTROUTE_TypeDef    PCNTROUTE_CLR[1U];       /**< pcnt0 DBUS config registers                        */
  GPIO_PRSROUTE_TypeDef     PRSROUTE_CLR[1U];        /**< prs0 DBUS config registers                         */
  uint32_t                  RESERVED43[23U];         /**< Reserved for future use                            */
  GPIO_SYXOROUTE_TypeDef    SYXOROUTE_CLR[1U];       /**< syxo0 DBUS config registers                        */
  GPIO_TIMERROUTE_TypeDef   TIMERROUTE_CLR[5U];      /**< timer0 DBUS config registers                       */
  GPIO_USARTROUTE_TypeDef   USARTROUTE_CLR[1U];      /**< usart0 DBUS config registers                       */
  uint32_t                  RESERVED44[530U];        /**< Reserved for future use                            */
  __IM uint32_t             IPVERSION_TGL;           /**< main                                               */
  uint32_t                  RESERVED45[11U];         /**< Reserved for future use                            */
  GPIO_PORT_TypeDef         P_TGL[4U];               /**<                                                    */
  uint32_t                  RESERVED46[132U];        /**< Reserved for future use                            */
  __IOM uint32_t            LOCK_TGL;                /**< Lock Register                                      */
  uint32_t                  RESERVED47[3U];          /**< Reserved for future use                            */
  __IM uint32_t             GPIOLOCKSTATUS_TGL;      /**< Lock Status                                        */
  uint32_t                  RESERVED48[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            ABUSALLOC_TGL;           /**< A Bus allocation                                   */
  __IOM uint32_t            BBUSALLOC_TGL;           /**< B Bus allocation                                   */
  __IOM uint32_t            CDBUSALLOC_TGL;          /**< CD Bus allocation                                  */
  uint32_t                  RESERVED49[53U];         /**< Reserved for future use                            */
  __IOM uint32_t            EXTIPSELL_TGL;           /**< External Interrupt Port Select Low                 */
  __IOM uint32_t            EXTIPSELH_TGL;           /**< External interrupt Port Select High                */
  __IOM uint32_t            EXTIPINSELL_TGL;         /**< External Interrupt Pin Select Low                  */
  __IOM uint32_t            EXTIPINSELH_TGL;         /**< External Interrupt Pin Select High                 */
  __IOM uint32_t            EXTIRISE_TGL;            /**< External Interrupt Rising Edge Trigger             */
  __IOM uint32_t            EXTIFALL_TGL;            /**< External Interrupt Falling Edge Trigger            */
  uint32_t                  RESERVED50[2U];          /**< Reserved for future use                            */
  __IOM uint32_t            IF_TGL;                  /**< Interrupt Flag                                     */
  __IOM uint32_t            IEN_TGL;                 /**< Interrupt Enable                                   */
  uint32_t                  RESERVED51[1U];          /**< Reserved for future use                            */
  __IOM uint32_t            EM4WUEN_TGL;             /**< EM4 wakeup enable                                  */
  __IOM uint32_t            EM4WUPOL_TGL;            /**< EM4 wakeup polarity                                */
  uint32_t                  RESERVED52[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            DBGROUTEPEN_TGL;         /**< Debugger Route Pin enable                          */
  __IOM uint32_t            TRACEROUTEPEN_TGL;       /**< Trace Route Pin Enable                             */
  uint32_t                  RESERVED53[2U];          /**< Reserved for future use                            */
  uint32_t                  RESERVED54[4U];          /**< Reserved for future use                            */
  __IOM uint32_t            LCDSEG_TGL;              /**< LCD Segment Enable                                 */
  uint32_t                  RESERVED55[3U];          /**< Reserved for future use                            */
  __IOM uint32_t            LCDCOM_TGL;              /**< LCD Common Enable                                  */
  uint32_t                  RESERVED56[3U];          /**< Reserved for future use                            */
  GPIO_ACMPROUTE_TypeDef    ACMPROUTE_TGL[2U];       /**< acmp0 DBUS config registers                        */
  GPIO_CMUROUTE_TypeDef     CMUROUTE_TGL;            /**< cmu DBUS config registers                          */
  uint32_t                  RESERVED57[4U];          /**< Reserved for future use                            */
  GPIO_EUSARTROUTE_TypeDef  EUSARTROUTE_TGL[3U];     /**< eusart0 DBUS config registers                      */
  GPIO_FRCROUTE_TypeDef     FRCROUTE_TGL;            /**< frc DBUS config registers                          */
  GPIO_I2CROUTE_TypeDef     I2CROUTE_TGL[2U];        /**< i2c0 DBUS config registers                         */
  GPIO_KEYSCANROUTE_TypeDef KEYSCANROUTE_TGL;        /**< keyscan DBUS config registers                      */
  GPIO_LESENSEROUTE_TypeDef LESENSEROUTE_TGL;        /**< lesense DBUS config registers                      */
  GPIO_LETIMERROUTE_TypeDef LETIMERROUTE_TGL;        /**< letimer DBUS config registers                      */
  GPIO_MODEMROUTE_TypeDef   MODEMROUTE_TGL;          /**< modem DBUS config registers                        */
  GPIO_PCNTROUTE_TypeDef    PCNTROUTE_TGL[1U];       /**< pcnt0 DBUS config registers                        */
  GPIO_PRSROUTE_TypeDef     PRSROUTE_TGL[1U];        /**< prs0 DBUS config registers                         */
  uint32_t                  RESERVED58[23U];         /**< Reserved for future use                            */
  GPIO_SYXOROUTE_TypeDef    SYXOROUTE_TGL[1U];       /**< syxo0 DBUS config registers                        */
  GPIO_TIMERROUTE_TypeDef   TIMERROUTE_TGL[5U];      /**< timer0 DBUS config registers                       */
  GPIO_USARTROUTE_TypeDef   USARTROUTE_TGL[1U];      /**< usart0 DBUS config registers                       */
} GPIO_TypeDef;

/* Bit fields for GPIO IPVERSION */
#define _GPIO_IPVERSION_RESETVALUE                       0x00000002UL                             /**< Default value for GPIO_IPVERSION            */
#define _GPIO_IPVERSION_MASK                             0xFFFFFFFFUL                             /**< Mask for GPIO_IPVERSION                     */
#define _GPIO_IPVERSION_IPVERSION_SHIFT                  0                                        /**< Shift value for GPIO_IPVERSION              */
#define _GPIO_IPVERSION_IPVERSION_MASK                   0xFFFFFFFFUL                             /**< Bit mask for GPIO_IPVERSION                 */
#define _GPIO_IPVERSION_IPVERSION_DEFAULT                0x00000002UL                             /**< Mode DEFAULT for GPIO_IPVERSION             */
#define GPIO_IPVERSION_IPVERSION_DEFAULT                 (_GPIO_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_IPVERSION     */
#define GPIO_PORTA                                       0x00000000UL                             /**< PORTA index                                  */
#define GPIO_PORTB                                       0x00000001UL                             /**< PORTB index                                  */
#define GPIO_PORTC                                       0x00000002UL                             /**< PORTC index                                  */
#define GPIO_PORTD                                       0x00000003UL                             /**< PORTD index                                  */

/* Bit fields for GPIO LOCK */
#define _GPIO_LOCK_RESETVALUE                            0x0000A534UL                      /**< Default value for GPIO_LOCK                 */
#define _GPIO_LOCK_MASK                                  0x0000FFFFUL                      /**< Mask for GPIO_LOCK                          */
#define _GPIO_LOCK_LOCKKEY_SHIFT                         0                                 /**< Shift value for GPIO_LOCKKEY                */
#define _GPIO_LOCK_LOCKKEY_MASK                          0xFFFFUL                          /**< Bit mask for GPIO_LOCKKEY                   */
#define _GPIO_LOCK_LOCKKEY_DEFAULT                       0x0000A534UL                      /**< Mode DEFAULT for GPIO_LOCK                  */
#define _GPIO_LOCK_LOCKKEY_UNLOCK                        0x0000A534UL                      /**< Mode UNLOCK for GPIO_LOCK                   */
#define GPIO_LOCK_LOCKKEY_DEFAULT                        (_GPIO_LOCK_LOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LOCK          */
#define GPIO_LOCK_LOCKKEY_UNLOCK                         (_GPIO_LOCK_LOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for GPIO_LOCK           */

/* Bit fields for GPIO GPIOLOCKSTATUS */
#define _GPIO_GPIOLOCKSTATUS_RESETVALUE                  0x00000000UL                              /**< Default value for GPIO_GPIOLOCKSTATUS       */
#define _GPIO_GPIOLOCKSTATUS_MASK                        0x00000001UL                              /**< Mask for GPIO_GPIOLOCKSTATUS                */
#define GPIO_GPIOLOCKSTATUS_LOCK                         (0x1UL << 0)                              /**< GPIO LOCK status                            */
#define _GPIO_GPIOLOCKSTATUS_LOCK_SHIFT                  0                                         /**< Shift value for GPIO_LOCK                   */
#define _GPIO_GPIOLOCKSTATUS_LOCK_MASK                   0x1UL                                     /**< Bit mask for GPIO_LOCK                      */
#define _GPIO_GPIOLOCKSTATUS_LOCK_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_GPIOLOCKSTATUS        */
#define _GPIO_GPIOLOCKSTATUS_LOCK_UNLOCKED               0x00000000UL                              /**< Mode UNLOCKED for GPIO_GPIOLOCKSTATUS       */
#define _GPIO_GPIOLOCKSTATUS_LOCK_LOCKED                 0x00000001UL                              /**< Mode LOCKED for GPIO_GPIOLOCKSTATUS         */
#define GPIO_GPIOLOCKSTATUS_LOCK_DEFAULT                 (_GPIO_GPIOLOCKSTATUS_LOCK_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_GPIOLOCKSTATUS*/
#define GPIO_GPIOLOCKSTATUS_LOCK_UNLOCKED                (_GPIO_GPIOLOCKSTATUS_LOCK_UNLOCKED << 0) /**< Shifted mode UNLOCKED for GPIO_GPIOLOCKSTATUS*/
#define GPIO_GPIOLOCKSTATUS_LOCK_LOCKED                  (_GPIO_GPIOLOCKSTATUS_LOCK_LOCKED << 0)   /**< Shifted mode LOCKED for GPIO_GPIOLOCKSTATUS */

/* Bit fields for GPIO ABUSALLOC */
#define _GPIO_ABUSALLOC_RESETVALUE                       0x00000000UL                           /**< Default value for GPIO_ABUSALLOC            */
#define _GPIO_ABUSALLOC_MASK                             0x0F0F0F0FUL                           /**< Mask for GPIO_ABUSALLOC                     */
#define _GPIO_ABUSALLOC_AEVEN0_SHIFT                     0                                      /**< Shift value for GPIO_AEVEN0                 */
#define _GPIO_ABUSALLOC_AEVEN0_MASK                      0xFUL                                  /**< Bit mask for GPIO_AEVEN0                    */
#define _GPIO_ABUSALLOC_AEVEN0_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for GPIO_ABUSALLOC             */
#define _GPIO_ABUSALLOC_AEVEN0_TRISTATE                  0x00000000UL                           /**< Mode TRISTATE for GPIO_ABUSALLOC            */
#define _GPIO_ABUSALLOC_AEVEN0_ADC0                      0x00000001UL                           /**< Mode ADC0 for GPIO_ABUSALLOC                */
#define _GPIO_ABUSALLOC_AEVEN0_ACMP0                     0x00000002UL                           /**< Mode ACMP0 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AEVEN0_ACMP1                     0x00000003UL                           /**< Mode ACMP1 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AEVEN0_VDAC0CH0                  0x00000004UL                           /**< Mode VDAC0CH0 for GPIO_ABUSALLOC            */
#define GPIO_ABUSALLOC_AEVEN0_DEFAULT                    (_GPIO_ABUSALLOC_AEVEN0_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_ABUSALLOC     */
#define GPIO_ABUSALLOC_AEVEN0_TRISTATE                   (_GPIO_ABUSALLOC_AEVEN0_TRISTATE << 0) /**< Shifted mode TRISTATE for GPIO_ABUSALLOC    */
#define GPIO_ABUSALLOC_AEVEN0_ADC0                       (_GPIO_ABUSALLOC_AEVEN0_ADC0 << 0)     /**< Shifted mode ADC0 for GPIO_ABUSALLOC        */
#define GPIO_ABUSALLOC_AEVEN0_ACMP0                      (_GPIO_ABUSALLOC_AEVEN0_ACMP0 << 0)    /**< Shifted mode ACMP0 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AEVEN0_ACMP1                      (_GPIO_ABUSALLOC_AEVEN0_ACMP1 << 0)    /**< Shifted mode ACMP1 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AEVEN0_VDAC0CH0                   (_GPIO_ABUSALLOC_AEVEN0_VDAC0CH0 << 0) /**< Shifted mode VDAC0CH0 for GPIO_ABUSALLOC    */
#define _GPIO_ABUSALLOC_AEVEN1_SHIFT                     8                                      /**< Shift value for GPIO_AEVEN1                 */
#define _GPIO_ABUSALLOC_AEVEN1_MASK                      0xF00UL                                /**< Bit mask for GPIO_AEVEN1                    */
#define _GPIO_ABUSALLOC_AEVEN1_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for GPIO_ABUSALLOC             */
#define _GPIO_ABUSALLOC_AEVEN1_TRISTATE                  0x00000000UL                           /**< Mode TRISTATE for GPIO_ABUSALLOC            */
#define _GPIO_ABUSALLOC_AEVEN1_ADC0                      0x00000001UL                           /**< Mode ADC0 for GPIO_ABUSALLOC                */
#define _GPIO_ABUSALLOC_AEVEN1_ACMP0                     0x00000002UL                           /**< Mode ACMP0 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AEVEN1_ACMP1                     0x00000003UL                           /**< Mode ACMP1 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AEVEN1_VDAC0CH1                  0x00000004UL                           /**< Mode VDAC0CH1 for GPIO_ABUSALLOC            */
#define GPIO_ABUSALLOC_AEVEN1_DEFAULT                    (_GPIO_ABUSALLOC_AEVEN1_DEFAULT << 8)  /**< Shifted mode DEFAULT for GPIO_ABUSALLOC     */
#define GPIO_ABUSALLOC_AEVEN1_TRISTATE                   (_GPIO_ABUSALLOC_AEVEN1_TRISTATE << 8) /**< Shifted mode TRISTATE for GPIO_ABUSALLOC    */
#define GPIO_ABUSALLOC_AEVEN1_ADC0                       (_GPIO_ABUSALLOC_AEVEN1_ADC0 << 8)     /**< Shifted mode ADC0 for GPIO_ABUSALLOC        */
#define GPIO_ABUSALLOC_AEVEN1_ACMP0                      (_GPIO_ABUSALLOC_AEVEN1_ACMP0 << 8)    /**< Shifted mode ACMP0 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AEVEN1_ACMP1                      (_GPIO_ABUSALLOC_AEVEN1_ACMP1 << 8)    /**< Shifted mode ACMP1 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AEVEN1_VDAC0CH1                   (_GPIO_ABUSALLOC_AEVEN1_VDAC0CH1 << 8) /**< Shifted mode VDAC0CH1 for GPIO_ABUSALLOC    */
#define _GPIO_ABUSALLOC_AODD0_SHIFT                      16                                     /**< Shift value for GPIO_AODD0                  */
#define _GPIO_ABUSALLOC_AODD0_MASK                       0xF0000UL                              /**< Bit mask for GPIO_AODD0                     */
#define _GPIO_ABUSALLOC_AODD0_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for GPIO_ABUSALLOC             */
#define _GPIO_ABUSALLOC_AODD0_TRISTATE                   0x00000000UL                           /**< Mode TRISTATE for GPIO_ABUSALLOC            */
#define _GPIO_ABUSALLOC_AODD0_ADC0                       0x00000001UL                           /**< Mode ADC0 for GPIO_ABUSALLOC                */
#define _GPIO_ABUSALLOC_AODD0_ACMP0                      0x00000002UL                           /**< Mode ACMP0 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AODD0_ACMP1                      0x00000003UL                           /**< Mode ACMP1 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AODD0_VDAC0CH0                   0x00000004UL                           /**< Mode VDAC0CH0 for GPIO_ABUSALLOC            */
#define GPIO_ABUSALLOC_AODD0_DEFAULT                     (_GPIO_ABUSALLOC_AODD0_DEFAULT << 16)  /**< Shifted mode DEFAULT for GPIO_ABUSALLOC     */
#define GPIO_ABUSALLOC_AODD0_TRISTATE                    (_GPIO_ABUSALLOC_AODD0_TRISTATE << 16) /**< Shifted mode TRISTATE for GPIO_ABUSALLOC    */
#define GPIO_ABUSALLOC_AODD0_ADC0                        (_GPIO_ABUSALLOC_AODD0_ADC0 << 16)     /**< Shifted mode ADC0 for GPIO_ABUSALLOC        */
#define GPIO_ABUSALLOC_AODD0_ACMP0                       (_GPIO_ABUSALLOC_AODD0_ACMP0 << 16)    /**< Shifted mode ACMP0 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AODD0_ACMP1                       (_GPIO_ABUSALLOC_AODD0_ACMP1 << 16)    /**< Shifted mode ACMP1 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AODD0_VDAC0CH0                    (_GPIO_ABUSALLOC_AODD0_VDAC0CH0 << 16) /**< Shifted mode VDAC0CH0 for GPIO_ABUSALLOC    */
#define _GPIO_ABUSALLOC_AODD1_SHIFT                      24                                     /**< Shift value for GPIO_AODD1                  */
#define _GPIO_ABUSALLOC_AODD1_MASK                       0xF000000UL                            /**< Bit mask for GPIO_AODD1                     */
#define _GPIO_ABUSALLOC_AODD1_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for GPIO_ABUSALLOC             */
#define _GPIO_ABUSALLOC_AODD1_TRISTATE                   0x00000000UL                           /**< Mode TRISTATE for GPIO_ABUSALLOC            */
#define _GPIO_ABUSALLOC_AODD1_ADC0                       0x00000001UL                           /**< Mode ADC0 for GPIO_ABUSALLOC                */
#define _GPIO_ABUSALLOC_AODD1_ACMP0                      0x00000002UL                           /**< Mode ACMP0 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AODD1_ACMP1                      0x00000003UL                           /**< Mode ACMP1 for GPIO_ABUSALLOC               */
#define _GPIO_ABUSALLOC_AODD1_VDAC0CH1                   0x00000004UL                           /**< Mode VDAC0CH1 for GPIO_ABUSALLOC            */
#define GPIO_ABUSALLOC_AODD1_DEFAULT                     (_GPIO_ABUSALLOC_AODD1_DEFAULT << 24)  /**< Shifted mode DEFAULT for GPIO_ABUSALLOC     */
#define GPIO_ABUSALLOC_AODD1_TRISTATE                    (_GPIO_ABUSALLOC_AODD1_TRISTATE << 24) /**< Shifted mode TRISTATE for GPIO_ABUSALLOC    */
#define GPIO_ABUSALLOC_AODD1_ADC0                        (_GPIO_ABUSALLOC_AODD1_ADC0 << 24)     /**< Shifted mode ADC0 for GPIO_ABUSALLOC        */
#define GPIO_ABUSALLOC_AODD1_ACMP0                       (_GPIO_ABUSALLOC_AODD1_ACMP0 << 24)    /**< Shifted mode ACMP0 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AODD1_ACMP1                       (_GPIO_ABUSALLOC_AODD1_ACMP1 << 24)    /**< Shifted mode ACMP1 for GPIO_ABUSALLOC       */
#define GPIO_ABUSALLOC_AODD1_VDAC0CH1                    (_GPIO_ABUSALLOC_AODD1_VDAC0CH1 << 24) /**< Shifted mode VDAC0CH1 for GPIO_ABUSALLOC    */

/* Bit fields for GPIO BBUSALLOC */
#define _GPIO_BBUSALLOC_RESETVALUE                       0x00000000UL                           /**< Default value for GPIO_BBUSALLOC            */
#define _GPIO_BBUSALLOC_MASK                             0x0F0F0F0FUL                           /**< Mask for GPIO_BBUSALLOC                     */
#define _GPIO_BBUSALLOC_BEVEN0_SHIFT                     0                                      /**< Shift value for GPIO_BEVEN0                 */
#define _GPIO_BBUSALLOC_BEVEN0_MASK                      0xFUL                                  /**< Bit mask for GPIO_BEVEN0                    */
#define _GPIO_BBUSALLOC_BEVEN0_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for GPIO_BBUSALLOC             */
#define _GPIO_BBUSALLOC_BEVEN0_TRISTATE                  0x00000000UL                           /**< Mode TRISTATE for GPIO_BBUSALLOC            */
#define _GPIO_BBUSALLOC_BEVEN0_ADC0                      0x00000001UL                           /**< Mode ADC0 for GPIO_BBUSALLOC                */
#define _GPIO_BBUSALLOC_BEVEN0_ACMP0                     0x00000002UL                           /**< Mode ACMP0 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BEVEN0_ACMP1                     0x00000003UL                           /**< Mode ACMP1 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BEVEN0_VDAC0CH0                  0x00000004UL                           /**< Mode VDAC0CH0 for GPIO_BBUSALLOC            */
#define GPIO_BBUSALLOC_BEVEN0_DEFAULT                    (_GPIO_BBUSALLOC_BEVEN0_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_BBUSALLOC     */
#define GPIO_BBUSALLOC_BEVEN0_TRISTATE                   (_GPIO_BBUSALLOC_BEVEN0_TRISTATE << 0) /**< Shifted mode TRISTATE for GPIO_BBUSALLOC    */
#define GPIO_BBUSALLOC_BEVEN0_ADC0                       (_GPIO_BBUSALLOC_BEVEN0_ADC0 << 0)     /**< Shifted mode ADC0 for GPIO_BBUSALLOC        */
#define GPIO_BBUSALLOC_BEVEN0_ACMP0                      (_GPIO_BBUSALLOC_BEVEN0_ACMP0 << 0)    /**< Shifted mode ACMP0 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BEVEN0_ACMP1                      (_GPIO_BBUSALLOC_BEVEN0_ACMP1 << 0)    /**< Shifted mode ACMP1 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BEVEN0_VDAC0CH0                   (_GPIO_BBUSALLOC_BEVEN0_VDAC0CH0 << 0) /**< Shifted mode VDAC0CH0 for GPIO_BBUSALLOC    */
#define _GPIO_BBUSALLOC_BEVEN1_SHIFT                     8                                      /**< Shift value for GPIO_BEVEN1                 */
#define _GPIO_BBUSALLOC_BEVEN1_MASK                      0xF00UL                                /**< Bit mask for GPIO_BEVEN1                    */
#define _GPIO_BBUSALLOC_BEVEN1_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for GPIO_BBUSALLOC             */
#define _GPIO_BBUSALLOC_BEVEN1_TRISTATE                  0x00000000UL                           /**< Mode TRISTATE for GPIO_BBUSALLOC            */
#define _GPIO_BBUSALLOC_BEVEN1_ADC0                      0x00000001UL                           /**< Mode ADC0 for GPIO_BBUSALLOC                */
#define _GPIO_BBUSALLOC_BEVEN1_ACMP0                     0x00000002UL                           /**< Mode ACMP0 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BEVEN1_ACMP1                     0x00000003UL                           /**< Mode ACMP1 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BEVEN1_VDAC0CH1                  0x00000004UL                           /**< Mode VDAC0CH1 for GPIO_BBUSALLOC            */
#define GPIO_BBUSALLOC_BEVEN1_DEFAULT                    (_GPIO_BBUSALLOC_BEVEN1_DEFAULT << 8)  /**< Shifted mode DEFAULT for GPIO_BBUSALLOC     */
#define GPIO_BBUSALLOC_BEVEN1_TRISTATE                   (_GPIO_BBUSALLOC_BEVEN1_TRISTATE << 8) /**< Shifted mode TRISTATE for GPIO_BBUSALLOC    */
#define GPIO_BBUSALLOC_BEVEN1_ADC0                       (_GPIO_BBUSALLOC_BEVEN1_ADC0 << 8)     /**< Shifted mode ADC0 for GPIO_BBUSALLOC        */
#define GPIO_BBUSALLOC_BEVEN1_ACMP0                      (_GPIO_BBUSALLOC_BEVEN1_ACMP0 << 8)    /**< Shifted mode ACMP0 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BEVEN1_ACMP1                      (_GPIO_BBUSALLOC_BEVEN1_ACMP1 << 8)    /**< Shifted mode ACMP1 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BEVEN1_VDAC0CH1                   (_GPIO_BBUSALLOC_BEVEN1_VDAC0CH1 << 8) /**< Shifted mode VDAC0CH1 for GPIO_BBUSALLOC    */
#define _GPIO_BBUSALLOC_BODD0_SHIFT                      16                                     /**< Shift value for GPIO_BODD0                  */
#define _GPIO_BBUSALLOC_BODD0_MASK                       0xF0000UL                              /**< Bit mask for GPIO_BODD0                     */
#define _GPIO_BBUSALLOC_BODD0_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for GPIO_BBUSALLOC             */
#define _GPIO_BBUSALLOC_BODD0_TRISTATE                   0x00000000UL                           /**< Mode TRISTATE for GPIO_BBUSALLOC            */
#define _GPIO_BBUSALLOC_BODD0_ADC0                       0x00000001UL                           /**< Mode ADC0 for GPIO_BBUSALLOC                */
#define _GPIO_BBUSALLOC_BODD0_ACMP0                      0x00000002UL                           /**< Mode ACMP0 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BODD0_ACMP1                      0x00000003UL                           /**< Mode ACMP1 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BODD0_VDAC0CH0                   0x00000004UL                           /**< Mode VDAC0CH0 for GPIO_BBUSALLOC            */
#define GPIO_BBUSALLOC_BODD0_DEFAULT                     (_GPIO_BBUSALLOC_BODD0_DEFAULT << 16)  /**< Shifted mode DEFAULT for GPIO_BBUSALLOC     */
#define GPIO_BBUSALLOC_BODD0_TRISTATE                    (_GPIO_BBUSALLOC_BODD0_TRISTATE << 16) /**< Shifted mode TRISTATE for GPIO_BBUSALLOC    */
#define GPIO_BBUSALLOC_BODD0_ADC0                        (_GPIO_BBUSALLOC_BODD0_ADC0 << 16)     /**< Shifted mode ADC0 for GPIO_BBUSALLOC        */
#define GPIO_BBUSALLOC_BODD0_ACMP0                       (_GPIO_BBUSALLOC_BODD0_ACMP0 << 16)    /**< Shifted mode ACMP0 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BODD0_ACMP1                       (_GPIO_BBUSALLOC_BODD0_ACMP1 << 16)    /**< Shifted mode ACMP1 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BODD0_VDAC0CH0                    (_GPIO_BBUSALLOC_BODD0_VDAC0CH0 << 16) /**< Shifted mode VDAC0CH0 for GPIO_BBUSALLOC    */
#define _GPIO_BBUSALLOC_BODD1_SHIFT                      24                                     /**< Shift value for GPIO_BODD1                  */
#define _GPIO_BBUSALLOC_BODD1_MASK                       0xF000000UL                            /**< Bit mask for GPIO_BODD1                     */
#define _GPIO_BBUSALLOC_BODD1_DEFAULT                    0x00000000UL                           /**< Mode DEFAULT for GPIO_BBUSALLOC             */
#define _GPIO_BBUSALLOC_BODD1_TRISTATE                   0x00000000UL                           /**< Mode TRISTATE for GPIO_BBUSALLOC            */
#define _GPIO_BBUSALLOC_BODD1_ADC0                       0x00000001UL                           /**< Mode ADC0 for GPIO_BBUSALLOC                */
#define _GPIO_BBUSALLOC_BODD1_ACMP0                      0x00000002UL                           /**< Mode ACMP0 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BODD1_ACMP1                      0x00000003UL                           /**< Mode ACMP1 for GPIO_BBUSALLOC               */
#define _GPIO_BBUSALLOC_BODD1_VDAC0CH1                   0x00000004UL                           /**< Mode VDAC0CH1 for GPIO_BBUSALLOC            */
#define GPIO_BBUSALLOC_BODD1_DEFAULT                     (_GPIO_BBUSALLOC_BODD1_DEFAULT << 24)  /**< Shifted mode DEFAULT for GPIO_BBUSALLOC     */
#define GPIO_BBUSALLOC_BODD1_TRISTATE                    (_GPIO_BBUSALLOC_BODD1_TRISTATE << 24) /**< Shifted mode TRISTATE for GPIO_BBUSALLOC    */
#define GPIO_BBUSALLOC_BODD1_ADC0                        (_GPIO_BBUSALLOC_BODD1_ADC0 << 24)     /**< Shifted mode ADC0 for GPIO_BBUSALLOC        */
#define GPIO_BBUSALLOC_BODD1_ACMP0                       (_GPIO_BBUSALLOC_BODD1_ACMP0 << 24)    /**< Shifted mode ACMP0 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BODD1_ACMP1                       (_GPIO_BBUSALLOC_BODD1_ACMP1 << 24)    /**< Shifted mode ACMP1 for GPIO_BBUSALLOC       */
#define GPIO_BBUSALLOC_BODD1_VDAC0CH1                    (_GPIO_BBUSALLOC_BODD1_VDAC0CH1 << 24) /**< Shifted mode VDAC0CH1 for GPIO_BBUSALLOC    */

/* Bit fields for GPIO CDBUSALLOC */
#define _GPIO_CDBUSALLOC_RESETVALUE                      0x00000000UL                             /**< Default value for GPIO_CDBUSALLOC           */
#define _GPIO_CDBUSALLOC_MASK                            0x0F0F0F0FUL                             /**< Mask for GPIO_CDBUSALLOC                    */
#define _GPIO_CDBUSALLOC_CDEVEN0_SHIFT                   0                                        /**< Shift value for GPIO_CDEVEN0                */
#define _GPIO_CDBUSALLOC_CDEVEN0_MASK                    0xFUL                                    /**< Bit mask for GPIO_CDEVEN0                   */
#define _GPIO_CDBUSALLOC_CDEVEN0_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_CDBUSALLOC            */
#define _GPIO_CDBUSALLOC_CDEVEN0_TRISTATE                0x00000000UL                             /**< Mode TRISTATE for GPIO_CDBUSALLOC           */
#define _GPIO_CDBUSALLOC_CDEVEN0_ADC0                    0x00000001UL                             /**< Mode ADC0 for GPIO_CDBUSALLOC               */
#define _GPIO_CDBUSALLOC_CDEVEN0_ACMP0                   0x00000002UL                             /**< Mode ACMP0 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDEVEN0_ACMP1                   0x00000003UL                             /**< Mode ACMP1 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDEVEN0_VDAC0CH0                0x00000004UL                             /**< Mode VDAC0CH0 for GPIO_CDBUSALLOC           */
#define GPIO_CDBUSALLOC_CDEVEN0_DEFAULT                  (_GPIO_CDBUSALLOC_CDEVEN0_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_CDBUSALLOC    */
#define GPIO_CDBUSALLOC_CDEVEN0_TRISTATE                 (_GPIO_CDBUSALLOC_CDEVEN0_TRISTATE << 0) /**< Shifted mode TRISTATE for GPIO_CDBUSALLOC   */
#define GPIO_CDBUSALLOC_CDEVEN0_ADC0                     (_GPIO_CDBUSALLOC_CDEVEN0_ADC0 << 0)     /**< Shifted mode ADC0 for GPIO_CDBUSALLOC       */
#define GPIO_CDBUSALLOC_CDEVEN0_ACMP0                    (_GPIO_CDBUSALLOC_CDEVEN0_ACMP0 << 0)    /**< Shifted mode ACMP0 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDEVEN0_ACMP1                    (_GPIO_CDBUSALLOC_CDEVEN0_ACMP1 << 0)    /**< Shifted mode ACMP1 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDEVEN0_VDAC0CH0                 (_GPIO_CDBUSALLOC_CDEVEN0_VDAC0CH0 << 0) /**< Shifted mode VDAC0CH0 for GPIO_CDBUSALLOC   */
#define _GPIO_CDBUSALLOC_CDEVEN1_SHIFT                   8                                        /**< Shift value for GPIO_CDEVEN1                */
#define _GPIO_CDBUSALLOC_CDEVEN1_MASK                    0xF00UL                                  /**< Bit mask for GPIO_CDEVEN1                   */
#define _GPIO_CDBUSALLOC_CDEVEN1_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_CDBUSALLOC            */
#define _GPIO_CDBUSALLOC_CDEVEN1_TRISTATE                0x00000000UL                             /**< Mode TRISTATE for GPIO_CDBUSALLOC           */
#define _GPIO_CDBUSALLOC_CDEVEN1_ADC0                    0x00000001UL                             /**< Mode ADC0 for GPIO_CDBUSALLOC               */
#define _GPIO_CDBUSALLOC_CDEVEN1_ACMP0                   0x00000002UL                             /**< Mode ACMP0 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDEVEN1_ACMP1                   0x00000003UL                             /**< Mode ACMP1 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDEVEN1_VDAC0CH1                0x00000004UL                             /**< Mode VDAC0CH1 for GPIO_CDBUSALLOC           */
#define GPIO_CDBUSALLOC_CDEVEN1_DEFAULT                  (_GPIO_CDBUSALLOC_CDEVEN1_DEFAULT << 8)  /**< Shifted mode DEFAULT for GPIO_CDBUSALLOC    */
#define GPIO_CDBUSALLOC_CDEVEN1_TRISTATE                 (_GPIO_CDBUSALLOC_CDEVEN1_TRISTATE << 8) /**< Shifted mode TRISTATE for GPIO_CDBUSALLOC   */
#define GPIO_CDBUSALLOC_CDEVEN1_ADC0                     (_GPIO_CDBUSALLOC_CDEVEN1_ADC0 << 8)     /**< Shifted mode ADC0 for GPIO_CDBUSALLOC       */
#define GPIO_CDBUSALLOC_CDEVEN1_ACMP0                    (_GPIO_CDBUSALLOC_CDEVEN1_ACMP0 << 8)    /**< Shifted mode ACMP0 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDEVEN1_ACMP1                    (_GPIO_CDBUSALLOC_CDEVEN1_ACMP1 << 8)    /**< Shifted mode ACMP1 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDEVEN1_VDAC0CH1                 (_GPIO_CDBUSALLOC_CDEVEN1_VDAC0CH1 << 8) /**< Shifted mode VDAC0CH1 for GPIO_CDBUSALLOC   */
#define _GPIO_CDBUSALLOC_CDODD0_SHIFT                    16                                       /**< Shift value for GPIO_CDODD0                 */
#define _GPIO_CDBUSALLOC_CDODD0_MASK                     0xF0000UL                                /**< Bit mask for GPIO_CDODD0                    */
#define _GPIO_CDBUSALLOC_CDODD0_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for GPIO_CDBUSALLOC            */
#define _GPIO_CDBUSALLOC_CDODD0_TRISTATE                 0x00000000UL                             /**< Mode TRISTATE for GPIO_CDBUSALLOC           */
#define _GPIO_CDBUSALLOC_CDODD0_ADC0                     0x00000001UL                             /**< Mode ADC0 for GPIO_CDBUSALLOC               */
#define _GPIO_CDBUSALLOC_CDODD0_ACMP0                    0x00000002UL                             /**< Mode ACMP0 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDODD0_ACMP1                    0x00000003UL                             /**< Mode ACMP1 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDODD0_VDAC0CH0                 0x00000004UL                             /**< Mode VDAC0CH0 for GPIO_CDBUSALLOC           */
#define GPIO_CDBUSALLOC_CDODD0_DEFAULT                   (_GPIO_CDBUSALLOC_CDODD0_DEFAULT << 16)  /**< Shifted mode DEFAULT for GPIO_CDBUSALLOC    */
#define GPIO_CDBUSALLOC_CDODD0_TRISTATE                  (_GPIO_CDBUSALLOC_CDODD0_TRISTATE << 16) /**< Shifted mode TRISTATE for GPIO_CDBUSALLOC   */
#define GPIO_CDBUSALLOC_CDODD0_ADC0                      (_GPIO_CDBUSALLOC_CDODD0_ADC0 << 16)     /**< Shifted mode ADC0 for GPIO_CDBUSALLOC       */
#define GPIO_CDBUSALLOC_CDODD0_ACMP0                     (_GPIO_CDBUSALLOC_CDODD0_ACMP0 << 16)    /**< Shifted mode ACMP0 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDODD0_ACMP1                     (_GPIO_CDBUSALLOC_CDODD0_ACMP1 << 16)    /**< Shifted mode ACMP1 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDODD0_VDAC0CH0                  (_GPIO_CDBUSALLOC_CDODD0_VDAC0CH0 << 16) /**< Shifted mode VDAC0CH0 for GPIO_CDBUSALLOC   */
#define _GPIO_CDBUSALLOC_CDODD1_SHIFT                    24                                       /**< Shift value for GPIO_CDODD1                 */
#define _GPIO_CDBUSALLOC_CDODD1_MASK                     0xF000000UL                              /**< Bit mask for GPIO_CDODD1                    */
#define _GPIO_CDBUSALLOC_CDODD1_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for GPIO_CDBUSALLOC            */
#define _GPIO_CDBUSALLOC_CDODD1_TRISTATE                 0x00000000UL                             /**< Mode TRISTATE for GPIO_CDBUSALLOC           */
#define _GPIO_CDBUSALLOC_CDODD1_ADC0                     0x00000001UL                             /**< Mode ADC0 for GPIO_CDBUSALLOC               */
#define _GPIO_CDBUSALLOC_CDODD1_ACMP0                    0x00000002UL                             /**< Mode ACMP0 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDODD1_ACMP1                    0x00000003UL                             /**< Mode ACMP1 for GPIO_CDBUSALLOC              */
#define _GPIO_CDBUSALLOC_CDODD1_VDAC0CH1                 0x00000004UL                             /**< Mode VDAC0CH1 for GPIO_CDBUSALLOC           */
#define GPIO_CDBUSALLOC_CDODD1_DEFAULT                   (_GPIO_CDBUSALLOC_CDODD1_DEFAULT << 24)  /**< Shifted mode DEFAULT for GPIO_CDBUSALLOC    */
#define GPIO_CDBUSALLOC_CDODD1_TRISTATE                  (_GPIO_CDBUSALLOC_CDODD1_TRISTATE << 24) /**< Shifted mode TRISTATE for GPIO_CDBUSALLOC   */
#define GPIO_CDBUSALLOC_CDODD1_ADC0                      (_GPIO_CDBUSALLOC_CDODD1_ADC0 << 24)     /**< Shifted mode ADC0 for GPIO_CDBUSALLOC       */
#define GPIO_CDBUSALLOC_CDODD1_ACMP0                     (_GPIO_CDBUSALLOC_CDODD1_ACMP0 << 24)    /**< Shifted mode ACMP0 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDODD1_ACMP1                     (_GPIO_CDBUSALLOC_CDODD1_ACMP1 << 24)    /**< Shifted mode ACMP1 for GPIO_CDBUSALLOC      */
#define GPIO_CDBUSALLOC_CDODD1_VDAC0CH1                  (_GPIO_CDBUSALLOC_CDODD1_VDAC0CH1 << 24) /**< Shifted mode VDAC0CH1 for GPIO_CDBUSALLOC   */

/* Bit fields for GPIO EXTIPSELL */
#define _GPIO_EXTIPSELL_RESETVALUE                       0x00000000UL                              /**< Default value for GPIO_EXTIPSELL            */
#define _GPIO_EXTIPSELL_MASK                             0x33333333UL                              /**< Mask for GPIO_EXTIPSELL                     */
#define _GPIO_EXTIPSELL_EXTIPSEL0_SHIFT                  0                                         /**< Shift value for GPIO_EXTIPSEL0              */
#define _GPIO_EXTIPSELL_EXTIPSEL0_MASK                   0x3UL                                     /**< Bit mask for GPIO_EXTIPSEL0                 */
#define _GPIO_EXTIPSELL_EXTIPSEL0_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL0_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL0_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL0_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL0_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL0_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL0_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL0_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL0_PORTA << 0)    /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL0_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL0_PORTB << 0)    /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL0_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL0_PORTC << 0)    /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL0_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL0_PORTD << 0)    /**< Shifted mode PORTD for GPIO_EXTIPSELL       */
#define _GPIO_EXTIPSELL_EXTIPSEL1_SHIFT                  4                                         /**< Shift value for GPIO_EXTIPSEL1              */
#define _GPIO_EXTIPSELL_EXTIPSEL1_MASK                   0x30UL                                    /**< Bit mask for GPIO_EXTIPSEL1                 */
#define _GPIO_EXTIPSELL_EXTIPSEL1_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL1_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL1_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL1_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL1_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL1_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL1_DEFAULT << 4)  /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL1_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL1_PORTA << 4)    /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL1_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL1_PORTB << 4)    /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL1_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL1_PORTC << 4)    /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL1_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL1_PORTD << 4)    /**< Shifted mode PORTD for GPIO_EXTIPSELL       */
#define _GPIO_EXTIPSELL_EXTIPSEL2_SHIFT                  8                                         /**< Shift value for GPIO_EXTIPSEL2              */
#define _GPIO_EXTIPSELL_EXTIPSEL2_MASK                   0x300UL                                   /**< Bit mask for GPIO_EXTIPSEL2                 */
#define _GPIO_EXTIPSELL_EXTIPSEL2_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL2_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL2_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL2_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL2_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL2_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL2_DEFAULT << 8)  /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL2_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL2_PORTA << 8)    /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL2_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL2_PORTB << 8)    /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL2_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL2_PORTC << 8)    /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL2_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL2_PORTD << 8)    /**< Shifted mode PORTD for GPIO_EXTIPSELL       */
#define _GPIO_EXTIPSELL_EXTIPSEL3_SHIFT                  12                                        /**< Shift value for GPIO_EXTIPSEL3              */
#define _GPIO_EXTIPSELL_EXTIPSEL3_MASK                   0x3000UL                                  /**< Bit mask for GPIO_EXTIPSEL3                 */
#define _GPIO_EXTIPSELL_EXTIPSEL3_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL3_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL3_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL3_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL3_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL3_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL3_DEFAULT << 12) /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL3_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL3_PORTA << 12)   /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL3_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL3_PORTB << 12)   /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL3_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL3_PORTC << 12)   /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL3_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL3_PORTD << 12)   /**< Shifted mode PORTD for GPIO_EXTIPSELL       */
#define _GPIO_EXTIPSELL_EXTIPSEL4_SHIFT                  16                                        /**< Shift value for GPIO_EXTIPSEL4              */
#define _GPIO_EXTIPSELL_EXTIPSEL4_MASK                   0x30000UL                                 /**< Bit mask for GPIO_EXTIPSEL4                 */
#define _GPIO_EXTIPSELL_EXTIPSEL4_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL4_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL4_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL4_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL4_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL4_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL4_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL4_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL4_PORTA << 16)   /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL4_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL4_PORTB << 16)   /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL4_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL4_PORTC << 16)   /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL4_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL4_PORTD << 16)   /**< Shifted mode PORTD for GPIO_EXTIPSELL       */
#define _GPIO_EXTIPSELL_EXTIPSEL5_SHIFT                  20                                        /**< Shift value for GPIO_EXTIPSEL5              */
#define _GPIO_EXTIPSELL_EXTIPSEL5_MASK                   0x300000UL                                /**< Bit mask for GPIO_EXTIPSEL5                 */
#define _GPIO_EXTIPSELL_EXTIPSEL5_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL5_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL5_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL5_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL5_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL5_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL5_DEFAULT << 20) /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL5_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL5_PORTA << 20)   /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL5_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL5_PORTB << 20)   /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL5_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL5_PORTC << 20)   /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL5_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL5_PORTD << 20)   /**< Shifted mode PORTD for GPIO_EXTIPSELL       */
#define _GPIO_EXTIPSELL_EXTIPSEL6_SHIFT                  24                                        /**< Shift value for GPIO_EXTIPSEL6              */
#define _GPIO_EXTIPSELL_EXTIPSEL6_MASK                   0x3000000UL                               /**< Bit mask for GPIO_EXTIPSEL6                 */
#define _GPIO_EXTIPSELL_EXTIPSEL6_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL6_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL6_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL6_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL6_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL6_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL6_DEFAULT << 24) /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL6_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL6_PORTA << 24)   /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL6_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL6_PORTB << 24)   /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL6_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL6_PORTC << 24)   /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL6_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL6_PORTD << 24)   /**< Shifted mode PORTD for GPIO_EXTIPSELL       */
#define _GPIO_EXTIPSELL_EXTIPSEL7_SHIFT                  28                                        /**< Shift value for GPIO_EXTIPSEL7              */
#define _GPIO_EXTIPSELL_EXTIPSEL7_MASK                   0x30000000UL                              /**< Bit mask for GPIO_EXTIPSEL7                 */
#define _GPIO_EXTIPSELL_EXTIPSEL7_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELL             */
#define _GPIO_EXTIPSELL_EXTIPSEL7_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL7_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL7_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELL               */
#define _GPIO_EXTIPSELL_EXTIPSEL7_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELL               */
#define GPIO_EXTIPSELL_EXTIPSEL7_DEFAULT                 (_GPIO_EXTIPSELL_EXTIPSEL7_DEFAULT << 28) /**< Shifted mode DEFAULT for GPIO_EXTIPSELL     */
#define GPIO_EXTIPSELL_EXTIPSEL7_PORTA                   (_GPIO_EXTIPSELL_EXTIPSEL7_PORTA << 28)   /**< Shifted mode PORTA for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL7_PORTB                   (_GPIO_EXTIPSELL_EXTIPSEL7_PORTB << 28)   /**< Shifted mode PORTB for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL7_PORTC                   (_GPIO_EXTIPSELL_EXTIPSEL7_PORTC << 28)   /**< Shifted mode PORTC for GPIO_EXTIPSELL       */
#define GPIO_EXTIPSELL_EXTIPSEL7_PORTD                   (_GPIO_EXTIPSELL_EXTIPSEL7_PORTD << 28)   /**< Shifted mode PORTD for GPIO_EXTIPSELL       */

/* Bit fields for GPIO EXTIPSELH */
#define _GPIO_EXTIPSELH_RESETVALUE                       0x00000000UL                              /**< Default value for GPIO_EXTIPSELH            */
#define _GPIO_EXTIPSELH_MASK                             0x00003333UL                              /**< Mask for GPIO_EXTIPSELH                     */
#define _GPIO_EXTIPSELH_EXTIPSEL0_SHIFT                  0                                         /**< Shift value for GPIO_EXTIPSEL0              */
#define _GPIO_EXTIPSELH_EXTIPSEL0_MASK                   0x3UL                                     /**< Bit mask for GPIO_EXTIPSEL0                 */
#define _GPIO_EXTIPSELH_EXTIPSEL0_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELH             */
#define _GPIO_EXTIPSELH_EXTIPSEL0_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL0_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL0_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL0_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELH               */
#define GPIO_EXTIPSELH_EXTIPSEL0_DEFAULT                 (_GPIO_EXTIPSELH_EXTIPSEL0_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_EXTIPSELH     */
#define GPIO_EXTIPSELH_EXTIPSEL0_PORTA                   (_GPIO_EXTIPSELH_EXTIPSEL0_PORTA << 0)    /**< Shifted mode PORTA for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL0_PORTB                   (_GPIO_EXTIPSELH_EXTIPSEL0_PORTB << 0)    /**< Shifted mode PORTB for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL0_PORTC                   (_GPIO_EXTIPSELH_EXTIPSEL0_PORTC << 0)    /**< Shifted mode PORTC for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL0_PORTD                   (_GPIO_EXTIPSELH_EXTIPSEL0_PORTD << 0)    /**< Shifted mode PORTD for GPIO_EXTIPSELH       */
#define _GPIO_EXTIPSELH_EXTIPSEL1_SHIFT                  4                                         /**< Shift value for GPIO_EXTIPSEL1              */
#define _GPIO_EXTIPSELH_EXTIPSEL1_MASK                   0x30UL                                    /**< Bit mask for GPIO_EXTIPSEL1                 */
#define _GPIO_EXTIPSELH_EXTIPSEL1_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELH             */
#define _GPIO_EXTIPSELH_EXTIPSEL1_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL1_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL1_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL1_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELH               */
#define GPIO_EXTIPSELH_EXTIPSEL1_DEFAULT                 (_GPIO_EXTIPSELH_EXTIPSEL1_DEFAULT << 4)  /**< Shifted mode DEFAULT for GPIO_EXTIPSELH     */
#define GPIO_EXTIPSELH_EXTIPSEL1_PORTA                   (_GPIO_EXTIPSELH_EXTIPSEL1_PORTA << 4)    /**< Shifted mode PORTA for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL1_PORTB                   (_GPIO_EXTIPSELH_EXTIPSEL1_PORTB << 4)    /**< Shifted mode PORTB for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL1_PORTC                   (_GPIO_EXTIPSELH_EXTIPSEL1_PORTC << 4)    /**< Shifted mode PORTC for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL1_PORTD                   (_GPIO_EXTIPSELH_EXTIPSEL1_PORTD << 4)    /**< Shifted mode PORTD for GPIO_EXTIPSELH       */
#define _GPIO_EXTIPSELH_EXTIPSEL2_SHIFT                  8                                         /**< Shift value for GPIO_EXTIPSEL2              */
#define _GPIO_EXTIPSELH_EXTIPSEL2_MASK                   0x300UL                                   /**< Bit mask for GPIO_EXTIPSEL2                 */
#define _GPIO_EXTIPSELH_EXTIPSEL2_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELH             */
#define _GPIO_EXTIPSELH_EXTIPSEL2_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL2_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL2_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL2_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELH               */
#define GPIO_EXTIPSELH_EXTIPSEL2_DEFAULT                 (_GPIO_EXTIPSELH_EXTIPSEL2_DEFAULT << 8)  /**< Shifted mode DEFAULT for GPIO_EXTIPSELH     */
#define GPIO_EXTIPSELH_EXTIPSEL2_PORTA                   (_GPIO_EXTIPSELH_EXTIPSEL2_PORTA << 8)    /**< Shifted mode PORTA for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL2_PORTB                   (_GPIO_EXTIPSELH_EXTIPSEL2_PORTB << 8)    /**< Shifted mode PORTB for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL2_PORTC                   (_GPIO_EXTIPSELH_EXTIPSEL2_PORTC << 8)    /**< Shifted mode PORTC for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL2_PORTD                   (_GPIO_EXTIPSELH_EXTIPSEL2_PORTD << 8)    /**< Shifted mode PORTD for GPIO_EXTIPSELH       */
#define _GPIO_EXTIPSELH_EXTIPSEL3_SHIFT                  12                                        /**< Shift value for GPIO_EXTIPSEL3              */
#define _GPIO_EXTIPSELH_EXTIPSEL3_MASK                   0x3000UL                                  /**< Bit mask for GPIO_EXTIPSEL3                 */
#define _GPIO_EXTIPSELH_EXTIPSEL3_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EXTIPSELH             */
#define _GPIO_EXTIPSELH_EXTIPSEL3_PORTA                  0x00000000UL                              /**< Mode PORTA for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL3_PORTB                  0x00000001UL                              /**< Mode PORTB for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL3_PORTC                  0x00000002UL                              /**< Mode PORTC for GPIO_EXTIPSELH               */
#define _GPIO_EXTIPSELH_EXTIPSEL3_PORTD                  0x00000003UL                              /**< Mode PORTD for GPIO_EXTIPSELH               */
#define GPIO_EXTIPSELH_EXTIPSEL3_DEFAULT                 (_GPIO_EXTIPSELH_EXTIPSEL3_DEFAULT << 12) /**< Shifted mode DEFAULT for GPIO_EXTIPSELH     */
#define GPIO_EXTIPSELH_EXTIPSEL3_PORTA                   (_GPIO_EXTIPSELH_EXTIPSEL3_PORTA << 12)   /**< Shifted mode PORTA for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL3_PORTB                   (_GPIO_EXTIPSELH_EXTIPSEL3_PORTB << 12)   /**< Shifted mode PORTB for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL3_PORTC                   (_GPIO_EXTIPSELH_EXTIPSEL3_PORTC << 12)   /**< Shifted mode PORTC for GPIO_EXTIPSELH       */
#define GPIO_EXTIPSELH_EXTIPSEL3_PORTD                   (_GPIO_EXTIPSELH_EXTIPSEL3_PORTD << 12)   /**< Shifted mode PORTD for GPIO_EXTIPSELH       */

/* Bit fields for GPIO EXTIPINSELL */
#define _GPIO_EXTIPINSELL_RESETVALUE                     0x00000000UL                                  /**< Default value for GPIO_EXTIPINSELL          */
#define _GPIO_EXTIPINSELL_MASK                           0x33333333UL                                  /**< Mask for GPIO_EXTIPINSELL                   */
#define _GPIO_EXTIPINSELL_EXTIPINSEL0_SHIFT              0                                             /**< Shift value for GPIO_EXTIPINSEL0            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL0_MASK               0x3UL                                         /**< Bit mask for GPIO_EXTIPINSEL0               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL0_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL0_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL0_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL0_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL0_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL0_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL0_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL0_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL0_PIN0 << 0)     /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL0_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL0_PIN1 << 0)     /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL0_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL0_PIN2 << 0)     /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL0_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL0_PIN3 << 0)     /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */
#define _GPIO_EXTIPINSELL_EXTIPINSEL1_SHIFT              4                                             /**< Shift value for GPIO_EXTIPINSEL1            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL1_MASK               0x30UL                                        /**< Bit mask for GPIO_EXTIPINSEL1               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL1_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL1_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL1_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL1_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL1_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL1_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL1_DEFAULT << 4)  /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL1_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL1_PIN0 << 4)     /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL1_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL1_PIN1 << 4)     /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL1_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL1_PIN2 << 4)     /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL1_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL1_PIN3 << 4)     /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */
#define _GPIO_EXTIPINSELL_EXTIPINSEL2_SHIFT              8                                             /**< Shift value for GPIO_EXTIPINSEL2            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL2_MASK               0x300UL                                       /**< Bit mask for GPIO_EXTIPINSEL2               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL2_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL2_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL2_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL2_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL2_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL2_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL2_DEFAULT << 8)  /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL2_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL2_PIN0 << 8)     /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL2_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL2_PIN1 << 8)     /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL2_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL2_PIN2 << 8)     /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL2_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL2_PIN3 << 8)     /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */
#define _GPIO_EXTIPINSELL_EXTIPINSEL3_SHIFT              12                                            /**< Shift value for GPIO_EXTIPINSEL3            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL3_MASK               0x3000UL                                      /**< Bit mask for GPIO_EXTIPINSEL3               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL3_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL3_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL3_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL3_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL3_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL3_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL3_DEFAULT << 12) /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL3_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL3_PIN0 << 12)    /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL3_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL3_PIN1 << 12)    /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL3_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL3_PIN2 << 12)    /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL3_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL3_PIN3 << 12)    /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */
#define _GPIO_EXTIPINSELL_EXTIPINSEL4_SHIFT              16                                            /**< Shift value for GPIO_EXTIPINSEL4            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL4_MASK               0x30000UL                                     /**< Bit mask for GPIO_EXTIPINSEL4               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL4_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL4_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL4_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL4_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL4_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL4_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL4_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL4_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL4_PIN0 << 16)    /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL4_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL4_PIN1 << 16)    /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL4_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL4_PIN2 << 16)    /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL4_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL4_PIN3 << 16)    /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */
#define _GPIO_EXTIPINSELL_EXTIPINSEL5_SHIFT              20                                            /**< Shift value for GPIO_EXTIPINSEL5            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL5_MASK               0x300000UL                                    /**< Bit mask for GPIO_EXTIPINSEL5               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL5_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL5_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL5_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL5_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL5_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL5_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL5_DEFAULT << 20) /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL5_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL5_PIN0 << 20)    /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL5_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL5_PIN1 << 20)    /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL5_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL5_PIN2 << 20)    /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL5_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL5_PIN3 << 20)    /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */
#define _GPIO_EXTIPINSELL_EXTIPINSEL6_SHIFT              24                                            /**< Shift value for GPIO_EXTIPINSEL6            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL6_MASK               0x3000000UL                                   /**< Bit mask for GPIO_EXTIPINSEL6               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL6_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL6_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL6_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL6_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL6_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL6_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL6_DEFAULT << 24) /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL6_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL6_PIN0 << 24)    /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL6_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL6_PIN1 << 24)    /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL6_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL6_PIN2 << 24)    /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL6_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL6_PIN3 << 24)    /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */
#define _GPIO_EXTIPINSELL_EXTIPINSEL7_SHIFT              28                                            /**< Shift value for GPIO_EXTIPINSEL7            */
#define _GPIO_EXTIPINSELL_EXTIPINSEL7_MASK               0x30000000UL                                  /**< Bit mask for GPIO_EXTIPINSEL7               */
#define _GPIO_EXTIPINSELL_EXTIPINSEL7_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELL           */
#define _GPIO_EXTIPINSELL_EXTIPINSEL7_PIN0               0x00000000UL                                  /**< Mode PIN0 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL7_PIN1               0x00000001UL                                  /**< Mode PIN1 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL7_PIN2               0x00000002UL                                  /**< Mode PIN2 for GPIO_EXTIPINSELL              */
#define _GPIO_EXTIPINSELL_EXTIPINSEL7_PIN3               0x00000003UL                                  /**< Mode PIN3 for GPIO_EXTIPINSELL              */
#define GPIO_EXTIPINSELL_EXTIPINSEL7_DEFAULT             (_GPIO_EXTIPINSELL_EXTIPINSEL7_DEFAULT << 28) /**< Shifted mode DEFAULT for GPIO_EXTIPINSELL   */
#define GPIO_EXTIPINSELL_EXTIPINSEL7_PIN0                (_GPIO_EXTIPINSELL_EXTIPINSEL7_PIN0 << 28)    /**< Shifted mode PIN0 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL7_PIN1                (_GPIO_EXTIPINSELL_EXTIPINSEL7_PIN1 << 28)    /**< Shifted mode PIN1 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL7_PIN2                (_GPIO_EXTIPINSELL_EXTIPINSEL7_PIN2 << 28)    /**< Shifted mode PIN2 for GPIO_EXTIPINSELL      */
#define GPIO_EXTIPINSELL_EXTIPINSEL7_PIN3                (_GPIO_EXTIPINSELL_EXTIPINSEL7_PIN3 << 28)    /**< Shifted mode PIN3 for GPIO_EXTIPINSELL      */

/* Bit fields for GPIO EXTIPINSELH */
#define _GPIO_EXTIPINSELH_RESETVALUE                     0x00000000UL                                  /**< Default value for GPIO_EXTIPINSELH          */
#define _GPIO_EXTIPINSELH_MASK                           0x00003333UL                                  /**< Mask for GPIO_EXTIPINSELH                   */
#define _GPIO_EXTIPINSELH_EXTIPINSEL0_SHIFT              0                                             /**< Shift value for GPIO_EXTIPINSEL0            */
#define _GPIO_EXTIPINSELH_EXTIPINSEL0_MASK               0x3UL                                         /**< Bit mask for GPIO_EXTIPINSEL0               */
#define _GPIO_EXTIPINSELH_EXTIPINSEL0_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELH           */
#define _GPIO_EXTIPINSELH_EXTIPINSEL0_PIN8               0x00000000UL                                  /**< Mode PIN8 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL0_PIN9               0x00000001UL                                  /**< Mode PIN9 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL0_PIN10              0x00000002UL                                  /**< Mode PIN10 for GPIO_EXTIPINSELH             */
#define _GPIO_EXTIPINSELH_EXTIPINSEL0_PIN11              0x00000003UL                                  /**< Mode PIN11 for GPIO_EXTIPINSELH             */
#define GPIO_EXTIPINSELH_EXTIPINSEL0_DEFAULT             (_GPIO_EXTIPINSELH_EXTIPINSEL0_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_EXTIPINSELH   */
#define GPIO_EXTIPINSELH_EXTIPINSEL0_PIN8                (_GPIO_EXTIPINSELH_EXTIPINSEL0_PIN8 << 0)     /**< Shifted mode PIN8 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL0_PIN9                (_GPIO_EXTIPINSELH_EXTIPINSEL0_PIN9 << 0)     /**< Shifted mode PIN9 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL0_PIN10               (_GPIO_EXTIPINSELH_EXTIPINSEL0_PIN10 << 0)    /**< Shifted mode PIN10 for GPIO_EXTIPINSELH     */
#define GPIO_EXTIPINSELH_EXTIPINSEL0_PIN11               (_GPIO_EXTIPINSELH_EXTIPINSEL0_PIN11 << 0)    /**< Shifted mode PIN11 for GPIO_EXTIPINSELH     */
#define _GPIO_EXTIPINSELH_EXTIPINSEL1_SHIFT              4                                             /**< Shift value for GPIO_EXTIPINSEL1            */
#define _GPIO_EXTIPINSELH_EXTIPINSEL1_MASK               0x30UL                                        /**< Bit mask for GPIO_EXTIPINSEL1               */
#define _GPIO_EXTIPINSELH_EXTIPINSEL1_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELH           */
#define _GPIO_EXTIPINSELH_EXTIPINSEL1_PIN8               0x00000000UL                                  /**< Mode PIN8 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL1_PIN9               0x00000001UL                                  /**< Mode PIN9 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL1_PIN10              0x00000002UL                                  /**< Mode PIN10 for GPIO_EXTIPINSELH             */
#define _GPIO_EXTIPINSELH_EXTIPINSEL1_PIN11              0x00000003UL                                  /**< Mode PIN11 for GPIO_EXTIPINSELH             */
#define GPIO_EXTIPINSELH_EXTIPINSEL1_DEFAULT             (_GPIO_EXTIPINSELH_EXTIPINSEL1_DEFAULT << 4)  /**< Shifted mode DEFAULT for GPIO_EXTIPINSELH   */
#define GPIO_EXTIPINSELH_EXTIPINSEL1_PIN8                (_GPIO_EXTIPINSELH_EXTIPINSEL1_PIN8 << 4)     /**< Shifted mode PIN8 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL1_PIN9                (_GPIO_EXTIPINSELH_EXTIPINSEL1_PIN9 << 4)     /**< Shifted mode PIN9 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL1_PIN10               (_GPIO_EXTIPINSELH_EXTIPINSEL1_PIN10 << 4)    /**< Shifted mode PIN10 for GPIO_EXTIPINSELH     */
#define GPIO_EXTIPINSELH_EXTIPINSEL1_PIN11               (_GPIO_EXTIPINSELH_EXTIPINSEL1_PIN11 << 4)    /**< Shifted mode PIN11 for GPIO_EXTIPINSELH     */
#define _GPIO_EXTIPINSELH_EXTIPINSEL2_SHIFT              8                                             /**< Shift value for GPIO_EXTIPINSEL2            */
#define _GPIO_EXTIPINSELH_EXTIPINSEL2_MASK               0x300UL                                       /**< Bit mask for GPIO_EXTIPINSEL2               */
#define _GPIO_EXTIPINSELH_EXTIPINSEL2_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELH           */
#define _GPIO_EXTIPINSELH_EXTIPINSEL2_PIN8               0x00000000UL                                  /**< Mode PIN8 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL2_PIN9               0x00000001UL                                  /**< Mode PIN9 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL2_PIN10              0x00000002UL                                  /**< Mode PIN10 for GPIO_EXTIPINSELH             */
#define _GPIO_EXTIPINSELH_EXTIPINSEL2_PIN11              0x00000003UL                                  /**< Mode PIN11 for GPIO_EXTIPINSELH             */
#define GPIO_EXTIPINSELH_EXTIPINSEL2_DEFAULT             (_GPIO_EXTIPINSELH_EXTIPINSEL2_DEFAULT << 8)  /**< Shifted mode DEFAULT for GPIO_EXTIPINSELH   */
#define GPIO_EXTIPINSELH_EXTIPINSEL2_PIN8                (_GPIO_EXTIPINSELH_EXTIPINSEL2_PIN8 << 8)     /**< Shifted mode PIN8 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL2_PIN9                (_GPIO_EXTIPINSELH_EXTIPINSEL2_PIN9 << 8)     /**< Shifted mode PIN9 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL2_PIN10               (_GPIO_EXTIPINSELH_EXTIPINSEL2_PIN10 << 8)    /**< Shifted mode PIN10 for GPIO_EXTIPINSELH     */
#define GPIO_EXTIPINSELH_EXTIPINSEL2_PIN11               (_GPIO_EXTIPINSELH_EXTIPINSEL2_PIN11 << 8)    /**< Shifted mode PIN11 for GPIO_EXTIPINSELH     */
#define _GPIO_EXTIPINSELH_EXTIPINSEL3_SHIFT              12                                            /**< Shift value for GPIO_EXTIPINSEL3            */
#define _GPIO_EXTIPINSELH_EXTIPINSEL3_MASK               0x3000UL                                      /**< Bit mask for GPIO_EXTIPINSEL3               */
#define _GPIO_EXTIPINSELH_EXTIPINSEL3_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_EXTIPINSELH           */
#define _GPIO_EXTIPINSELH_EXTIPINSEL3_PIN8               0x00000000UL                                  /**< Mode PIN8 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL3_PIN9               0x00000001UL                                  /**< Mode PIN9 for GPIO_EXTIPINSELH              */
#define _GPIO_EXTIPINSELH_EXTIPINSEL3_PIN10              0x00000002UL                                  /**< Mode PIN10 for GPIO_EXTIPINSELH             */
#define _GPIO_EXTIPINSELH_EXTIPINSEL3_PIN11              0x00000003UL                                  /**< Mode PIN11 for GPIO_EXTIPINSELH             */
#define GPIO_EXTIPINSELH_EXTIPINSEL3_DEFAULT             (_GPIO_EXTIPINSELH_EXTIPINSEL3_DEFAULT << 12) /**< Shifted mode DEFAULT for GPIO_EXTIPINSELH   */
#define GPIO_EXTIPINSELH_EXTIPINSEL3_PIN8                (_GPIO_EXTIPINSELH_EXTIPINSEL3_PIN8 << 12)    /**< Shifted mode PIN8 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL3_PIN9                (_GPIO_EXTIPINSELH_EXTIPINSEL3_PIN9 << 12)    /**< Shifted mode PIN9 for GPIO_EXTIPINSELH      */
#define GPIO_EXTIPINSELH_EXTIPINSEL3_PIN10               (_GPIO_EXTIPINSELH_EXTIPINSEL3_PIN10 << 12)   /**< Shifted mode PIN10 for GPIO_EXTIPINSELH     */
#define GPIO_EXTIPINSELH_EXTIPINSEL3_PIN11               (_GPIO_EXTIPINSELH_EXTIPINSEL3_PIN11 << 12)   /**< Shifted mode PIN11 for GPIO_EXTIPINSELH     */

/* Bit fields for GPIO EXTIRISE */
#define _GPIO_EXTIRISE_RESETVALUE                        0x00000000UL                           /**< Default value for GPIO_EXTIRISE             */
#define _GPIO_EXTIRISE_MASK                              0x00000FFFUL                           /**< Mask for GPIO_EXTIRISE                      */
#define _GPIO_EXTIRISE_EXTIRISE_SHIFT                    0                                      /**< Shift value for GPIO_EXTIRISE               */
#define _GPIO_EXTIRISE_EXTIRISE_MASK                     0xFFFUL                                /**< Bit mask for GPIO_EXTIRISE                  */
#define _GPIO_EXTIRISE_EXTIRISE_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for GPIO_EXTIRISE              */
#define GPIO_EXTIRISE_EXTIRISE_DEFAULT                   (_GPIO_EXTIRISE_EXTIRISE_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EXTIRISE      */

/* Bit fields for GPIO EXTIFALL */
#define _GPIO_EXTIFALL_RESETVALUE                        0x00000000UL                           /**< Default value for GPIO_EXTIFALL             */
#define _GPIO_EXTIFALL_MASK                              0x00000FFFUL                           /**< Mask for GPIO_EXTIFALL                      */
#define _GPIO_EXTIFALL_EXTIFALL_SHIFT                    0                                      /**< Shift value for GPIO_EXTIFALL               */
#define _GPIO_EXTIFALL_EXTIFALL_MASK                     0xFFFUL                                /**< Bit mask for GPIO_EXTIFALL                  */
#define _GPIO_EXTIFALL_EXTIFALL_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for GPIO_EXTIFALL              */
#define GPIO_EXTIFALL_EXTIFALL_DEFAULT                   (_GPIO_EXTIFALL_EXTIFALL_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EXTIFALL      */

/* Bit fields for GPIO IF */
#define _GPIO_IF_RESETVALUE                              0x00000000UL                     /**< Default value for GPIO_IF                   */
#define _GPIO_IF_MASK                                    0x0FFF0FFFUL                     /**< Mask for GPIO_IF                            */
#define GPIO_IF_EXTIF0                                   (0x1UL << 0)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF0_SHIFT                            0                                /**< Shift value for GPIO_EXTIF0                 */
#define _GPIO_IF_EXTIF0_MASK                             0x1UL                            /**< Bit mask for GPIO_EXTIF0                    */
#define _GPIO_IF_EXTIF0_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF0_DEFAULT                           (_GPIO_IF_EXTIF0_DEFAULT << 0)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF1                                   (0x1UL << 1)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF1_SHIFT                            1                                /**< Shift value for GPIO_EXTIF1                 */
#define _GPIO_IF_EXTIF1_MASK                             0x2UL                            /**< Bit mask for GPIO_EXTIF1                    */
#define _GPIO_IF_EXTIF1_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF1_DEFAULT                           (_GPIO_IF_EXTIF1_DEFAULT << 1)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF2                                   (0x1UL << 2)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF2_SHIFT                            2                                /**< Shift value for GPIO_EXTIF2                 */
#define _GPIO_IF_EXTIF2_MASK                             0x4UL                            /**< Bit mask for GPIO_EXTIF2                    */
#define _GPIO_IF_EXTIF2_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF2_DEFAULT                           (_GPIO_IF_EXTIF2_DEFAULT << 2)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF3                                   (0x1UL << 3)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF3_SHIFT                            3                                /**< Shift value for GPIO_EXTIF3                 */
#define _GPIO_IF_EXTIF3_MASK                             0x8UL                            /**< Bit mask for GPIO_EXTIF3                    */
#define _GPIO_IF_EXTIF3_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF3_DEFAULT                           (_GPIO_IF_EXTIF3_DEFAULT << 3)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF4                                   (0x1UL << 4)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF4_SHIFT                            4                                /**< Shift value for GPIO_EXTIF4                 */
#define _GPIO_IF_EXTIF4_MASK                             0x10UL                           /**< Bit mask for GPIO_EXTIF4                    */
#define _GPIO_IF_EXTIF4_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF4_DEFAULT                           (_GPIO_IF_EXTIF4_DEFAULT << 4)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF5                                   (0x1UL << 5)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF5_SHIFT                            5                                /**< Shift value for GPIO_EXTIF5                 */
#define _GPIO_IF_EXTIF5_MASK                             0x20UL                           /**< Bit mask for GPIO_EXTIF5                    */
#define _GPIO_IF_EXTIF5_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF5_DEFAULT                           (_GPIO_IF_EXTIF5_DEFAULT << 5)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF6                                   (0x1UL << 6)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF6_SHIFT                            6                                /**< Shift value for GPIO_EXTIF6                 */
#define _GPIO_IF_EXTIF6_MASK                             0x40UL                           /**< Bit mask for GPIO_EXTIF6                    */
#define _GPIO_IF_EXTIF6_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF6_DEFAULT                           (_GPIO_IF_EXTIF6_DEFAULT << 6)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF7                                   (0x1UL << 7)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF7_SHIFT                            7                                /**< Shift value for GPIO_EXTIF7                 */
#define _GPIO_IF_EXTIF7_MASK                             0x80UL                           /**< Bit mask for GPIO_EXTIF7                    */
#define _GPIO_IF_EXTIF7_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF7_DEFAULT                           (_GPIO_IF_EXTIF7_DEFAULT << 7)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF8                                   (0x1UL << 8)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF8_SHIFT                            8                                /**< Shift value for GPIO_EXTIF8                 */
#define _GPIO_IF_EXTIF8_MASK                             0x100UL                          /**< Bit mask for GPIO_EXTIF8                    */
#define _GPIO_IF_EXTIF8_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF8_DEFAULT                           (_GPIO_IF_EXTIF8_DEFAULT << 8)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF9                                   (0x1UL << 9)                     /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF9_SHIFT                            9                                /**< Shift value for GPIO_EXTIF9                 */
#define _GPIO_IF_EXTIF9_MASK                             0x200UL                          /**< Bit mask for GPIO_EXTIF9                    */
#define _GPIO_IF_EXTIF9_DEFAULT                          0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF9_DEFAULT                           (_GPIO_IF_EXTIF9_DEFAULT << 9)   /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF10                                  (0x1UL << 10)                    /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF10_SHIFT                           10                               /**< Shift value for GPIO_EXTIF10                */
#define _GPIO_IF_EXTIF10_MASK                            0x400UL                          /**< Bit mask for GPIO_EXTIF10                   */
#define _GPIO_IF_EXTIF10_DEFAULT                         0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF10_DEFAULT                          (_GPIO_IF_EXTIF10_DEFAULT << 10) /**< Shifted mode DEFAULT for GPIO_IF            */
#define GPIO_IF_EXTIF11                                  (0x1UL << 11)                    /**< External Pin Flag                           */
#define _GPIO_IF_EXTIF11_SHIFT                           11                               /**< Shift value for GPIO_EXTIF11                */
#define _GPIO_IF_EXTIF11_MASK                            0x800UL                          /**< Bit mask for GPIO_EXTIF11                   */
#define _GPIO_IF_EXTIF11_DEFAULT                         0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EXTIF11_DEFAULT                          (_GPIO_IF_EXTIF11_DEFAULT << 11) /**< Shifted mode DEFAULT for GPIO_IF            */
#define _GPIO_IF_EM4WU_SHIFT                             16                               /**< Shift value for GPIO_EM4WU                  */
#define _GPIO_IF_EM4WU_MASK                              0xFFF0000UL                      /**< Bit mask for GPIO_EM4WU                     */
#define _GPIO_IF_EM4WU_DEFAULT                           0x00000000UL                     /**< Mode DEFAULT for GPIO_IF                    */
#define GPIO_IF_EM4WU_DEFAULT                            (_GPIO_IF_EM4WU_DEFAULT << 16)   /**< Shifted mode DEFAULT for GPIO_IF            */

/* Bit fields for GPIO IEN */
#define _GPIO_IEN_RESETVALUE                             0x00000000UL                         /**< Default value for GPIO_IEN                  */
#define _GPIO_IEN_MASK                                   0x0FFF0FFFUL                         /**< Mask for GPIO_IEN                           */
#define GPIO_IEN_EXTIEN0                                 (0x1UL << 0)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN0_SHIFT                          0                                    /**< Shift value for GPIO_EXTIEN0                */
#define _GPIO_IEN_EXTIEN0_MASK                           0x1UL                                /**< Bit mask for GPIO_EXTIEN0                   */
#define _GPIO_IEN_EXTIEN0_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN0_DEFAULT                         (_GPIO_IEN_EXTIEN0_DEFAULT << 0)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN1                                 (0x1UL << 1)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN1_SHIFT                          1                                    /**< Shift value for GPIO_EXTIEN1                */
#define _GPIO_IEN_EXTIEN1_MASK                           0x2UL                                /**< Bit mask for GPIO_EXTIEN1                   */
#define _GPIO_IEN_EXTIEN1_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN1_DEFAULT                         (_GPIO_IEN_EXTIEN1_DEFAULT << 1)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN2                                 (0x1UL << 2)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN2_SHIFT                          2                                    /**< Shift value for GPIO_EXTIEN2                */
#define _GPIO_IEN_EXTIEN2_MASK                           0x4UL                                /**< Bit mask for GPIO_EXTIEN2                   */
#define _GPIO_IEN_EXTIEN2_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN2_DEFAULT                         (_GPIO_IEN_EXTIEN2_DEFAULT << 2)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN3                                 (0x1UL << 3)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN3_SHIFT                          3                                    /**< Shift value for GPIO_EXTIEN3                */
#define _GPIO_IEN_EXTIEN3_MASK                           0x8UL                                /**< Bit mask for GPIO_EXTIEN3                   */
#define _GPIO_IEN_EXTIEN3_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN3_DEFAULT                         (_GPIO_IEN_EXTIEN3_DEFAULT << 3)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN4                                 (0x1UL << 4)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN4_SHIFT                          4                                    /**< Shift value for GPIO_EXTIEN4                */
#define _GPIO_IEN_EXTIEN4_MASK                           0x10UL                               /**< Bit mask for GPIO_EXTIEN4                   */
#define _GPIO_IEN_EXTIEN4_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN4_DEFAULT                         (_GPIO_IEN_EXTIEN4_DEFAULT << 4)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN5                                 (0x1UL << 5)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN5_SHIFT                          5                                    /**< Shift value for GPIO_EXTIEN5                */
#define _GPIO_IEN_EXTIEN5_MASK                           0x20UL                               /**< Bit mask for GPIO_EXTIEN5                   */
#define _GPIO_IEN_EXTIEN5_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN5_DEFAULT                         (_GPIO_IEN_EXTIEN5_DEFAULT << 5)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN6                                 (0x1UL << 6)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN6_SHIFT                          6                                    /**< Shift value for GPIO_EXTIEN6                */
#define _GPIO_IEN_EXTIEN6_MASK                           0x40UL                               /**< Bit mask for GPIO_EXTIEN6                   */
#define _GPIO_IEN_EXTIEN6_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN6_DEFAULT                         (_GPIO_IEN_EXTIEN6_DEFAULT << 6)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN7                                 (0x1UL << 7)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN7_SHIFT                          7                                    /**< Shift value for GPIO_EXTIEN7                */
#define _GPIO_IEN_EXTIEN7_MASK                           0x80UL                               /**< Bit mask for GPIO_EXTIEN7                   */
#define _GPIO_IEN_EXTIEN7_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN7_DEFAULT                         (_GPIO_IEN_EXTIEN7_DEFAULT << 7)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN8                                 (0x1UL << 8)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN8_SHIFT                          8                                    /**< Shift value for GPIO_EXTIEN8                */
#define _GPIO_IEN_EXTIEN8_MASK                           0x100UL                              /**< Bit mask for GPIO_EXTIEN8                   */
#define _GPIO_IEN_EXTIEN8_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN8_DEFAULT                         (_GPIO_IEN_EXTIEN8_DEFAULT << 8)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN9                                 (0x1UL << 9)                         /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN9_SHIFT                          9                                    /**< Shift value for GPIO_EXTIEN9                */
#define _GPIO_IEN_EXTIEN9_MASK                           0x200UL                              /**< Bit mask for GPIO_EXTIEN9                   */
#define _GPIO_IEN_EXTIEN9_DEFAULT                        0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN9_DEFAULT                         (_GPIO_IEN_EXTIEN9_DEFAULT << 9)     /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN10                                (0x1UL << 10)                        /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN10_SHIFT                         10                                   /**< Shift value for GPIO_EXTIEN10               */
#define _GPIO_IEN_EXTIEN10_MASK                          0x400UL                              /**< Bit mask for GPIO_EXTIEN10                  */
#define _GPIO_IEN_EXTIEN10_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN10_DEFAULT                        (_GPIO_IEN_EXTIEN10_DEFAULT << 10)   /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EXTIEN11                                (0x1UL << 11)                        /**< External Pin Enable                         */
#define _GPIO_IEN_EXTIEN11_SHIFT                         11                                   /**< Shift value for GPIO_EXTIEN11               */
#define _GPIO_IEN_EXTIEN11_MASK                          0x800UL                              /**< Bit mask for GPIO_EXTIEN11                  */
#define _GPIO_IEN_EXTIEN11_DEFAULT                       0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EXTIEN11_DEFAULT                        (_GPIO_IEN_EXTIEN11_DEFAULT << 11)   /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN0                               (0x1UL << 16)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN0_SHIFT                        16                                   /**< Shift value for GPIO_EM4WUIEN0              */
#define _GPIO_IEN_EM4WUIEN0_MASK                         0x10000UL                            /**< Bit mask for GPIO_EM4WUIEN0                 */
#define _GPIO_IEN_EM4WUIEN0_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN0_DEFAULT                       (_GPIO_IEN_EM4WUIEN0_DEFAULT << 16)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN1                               (0x1UL << 17)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN1_SHIFT                        17                                   /**< Shift value for GPIO_EM4WUIEN1              */
#define _GPIO_IEN_EM4WUIEN1_MASK                         0x20000UL                            /**< Bit mask for GPIO_EM4WUIEN1                 */
#define _GPIO_IEN_EM4WUIEN1_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN1_DEFAULT                       (_GPIO_IEN_EM4WUIEN1_DEFAULT << 17)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN2                               (0x1UL << 18)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN2_SHIFT                        18                                   /**< Shift value for GPIO_EM4WUIEN2              */
#define _GPIO_IEN_EM4WUIEN2_MASK                         0x40000UL                            /**< Bit mask for GPIO_EM4WUIEN2                 */
#define _GPIO_IEN_EM4WUIEN2_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN2_DEFAULT                       (_GPIO_IEN_EM4WUIEN2_DEFAULT << 18)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN3                               (0x1UL << 19)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN3_SHIFT                        19                                   /**< Shift value for GPIO_EM4WUIEN3              */
#define _GPIO_IEN_EM4WUIEN3_MASK                         0x80000UL                            /**< Bit mask for GPIO_EM4WUIEN3                 */
#define _GPIO_IEN_EM4WUIEN3_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN3_DEFAULT                       (_GPIO_IEN_EM4WUIEN3_DEFAULT << 19)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN4                               (0x1UL << 20)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN4_SHIFT                        20                                   /**< Shift value for GPIO_EM4WUIEN4              */
#define _GPIO_IEN_EM4WUIEN4_MASK                         0x100000UL                           /**< Bit mask for GPIO_EM4WUIEN4                 */
#define _GPIO_IEN_EM4WUIEN4_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN4_DEFAULT                       (_GPIO_IEN_EM4WUIEN4_DEFAULT << 20)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN5                               (0x1UL << 21)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN5_SHIFT                        21                                   /**< Shift value for GPIO_EM4WUIEN5              */
#define _GPIO_IEN_EM4WUIEN5_MASK                         0x200000UL                           /**< Bit mask for GPIO_EM4WUIEN5                 */
#define _GPIO_IEN_EM4WUIEN5_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN5_DEFAULT                       (_GPIO_IEN_EM4WUIEN5_DEFAULT << 21)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN6                               (0x1UL << 22)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN6_SHIFT                        22                                   /**< Shift value for GPIO_EM4WUIEN6              */
#define _GPIO_IEN_EM4WUIEN6_MASK                         0x400000UL                           /**< Bit mask for GPIO_EM4WUIEN6                 */
#define _GPIO_IEN_EM4WUIEN6_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN6_DEFAULT                       (_GPIO_IEN_EM4WUIEN6_DEFAULT << 22)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN7                               (0x1UL << 23)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN7_SHIFT                        23                                   /**< Shift value for GPIO_EM4WUIEN7              */
#define _GPIO_IEN_EM4WUIEN7_MASK                         0x800000UL                           /**< Bit mask for GPIO_EM4WUIEN7                 */
#define _GPIO_IEN_EM4WUIEN7_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN7_DEFAULT                       (_GPIO_IEN_EM4WUIEN7_DEFAULT << 23)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN8                               (0x1UL << 24)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN8_SHIFT                        24                                   /**< Shift value for GPIO_EM4WUIEN8              */
#define _GPIO_IEN_EM4WUIEN8_MASK                         0x1000000UL                          /**< Bit mask for GPIO_EM4WUIEN8                 */
#define _GPIO_IEN_EM4WUIEN8_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN8_DEFAULT                       (_GPIO_IEN_EM4WUIEN8_DEFAULT << 24)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN9                               (0x1UL << 25)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN9_SHIFT                        25                                   /**< Shift value for GPIO_EM4WUIEN9              */
#define _GPIO_IEN_EM4WUIEN9_MASK                         0x2000000UL                          /**< Bit mask for GPIO_EM4WUIEN9                 */
#define _GPIO_IEN_EM4WUIEN9_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN9_DEFAULT                       (_GPIO_IEN_EM4WUIEN9_DEFAULT << 25)  /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN10                              (0x1UL << 26)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN10_SHIFT                       26                                   /**< Shift value for GPIO_EM4WUIEN10             */
#define _GPIO_IEN_EM4WUIEN10_MASK                        0x4000000UL                          /**< Bit mask for GPIO_EM4WUIEN10                */
#define _GPIO_IEN_EM4WUIEN10_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN10_DEFAULT                      (_GPIO_IEN_EM4WUIEN10_DEFAULT << 26) /**< Shifted mode DEFAULT for GPIO_IEN           */
#define GPIO_IEN_EM4WUIEN11                              (0x1UL << 27)                        /**< EM4 Wake Up Interrupt En                    */
#define _GPIO_IEN_EM4WUIEN11_SHIFT                       27                                   /**< Shift value for GPIO_EM4WUIEN11             */
#define _GPIO_IEN_EM4WUIEN11_MASK                        0x8000000UL                          /**< Bit mask for GPIO_EM4WUIEN11                */
#define _GPIO_IEN_EM4WUIEN11_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for GPIO_IEN                   */
#define GPIO_IEN_EM4WUIEN11_DEFAULT                      (_GPIO_IEN_EM4WUIEN11_DEFAULT << 27) /**< Shifted mode DEFAULT for GPIO_IEN           */

/* Bit fields for GPIO EM4WUEN */
#define _GPIO_EM4WUEN_RESETVALUE                         0x00000000UL                          /**< Default value for GPIO_EM4WUEN              */
#define _GPIO_EM4WUEN_MASK                               0x0FFF0000UL                          /**< Mask for GPIO_EM4WUEN                       */
#define _GPIO_EM4WUEN_EM4WUEN_SHIFT                      16                                    /**< Shift value for GPIO_EM4WUEN                */
#define _GPIO_EM4WUEN_EM4WUEN_MASK                       0xFFF0000UL                           /**< Bit mask for GPIO_EM4WUEN                   */
#define _GPIO_EM4WUEN_EM4WUEN_DEFAULT                    0x00000000UL                          /**< Mode DEFAULT for GPIO_EM4WUEN               */
#define GPIO_EM4WUEN_EM4WUEN_DEFAULT                     (_GPIO_EM4WUEN_EM4WUEN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EM4WUEN       */

/* Bit fields for GPIO EM4WUPOL */
#define _GPIO_EM4WUPOL_RESETVALUE                        0x00000000UL                            /**< Default value for GPIO_EM4WUPOL             */
#define _GPIO_EM4WUPOL_MASK                              0x0FFF0000UL                            /**< Mask for GPIO_EM4WUPOL                      */
#define _GPIO_EM4WUPOL_EM4WUPOL_SHIFT                    16                                      /**< Shift value for GPIO_EM4WUPOL               */
#define _GPIO_EM4WUPOL_EM4WUPOL_MASK                     0xFFF0000UL                             /**< Bit mask for GPIO_EM4WUPOL                  */
#define _GPIO_EM4WUPOL_EM4WUPOL_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for GPIO_EM4WUPOL              */
#define GPIO_EM4WUPOL_EM4WUPOL_DEFAULT                   (_GPIO_EM4WUPOL_EM4WUPOL_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EM4WUPOL      */

/* Bit fields for GPIO DBGROUTEPEN */
#define _GPIO_DBGROUTEPEN_RESETVALUE                     0x0000000FUL                                 /**< Default value for GPIO_DBGROUTEPEN          */
#define _GPIO_DBGROUTEPEN_MASK                           0x0000000FUL                                 /**< Mask for GPIO_DBGROUTEPEN                   */
#define GPIO_DBGROUTEPEN_SWCLKTCKPEN                     (0x1UL << 0)                                 /**< Route Pin Enable                            */
#define _GPIO_DBGROUTEPEN_SWCLKTCKPEN_SHIFT              0                                            /**< Shift value for GPIO_SWCLKTCKPEN            */
#define _GPIO_DBGROUTEPEN_SWCLKTCKPEN_MASK               0x1UL                                        /**< Bit mask for GPIO_SWCLKTCKPEN               */
#define _GPIO_DBGROUTEPEN_SWCLKTCKPEN_DEFAULT            0x00000001UL                                 /**< Mode DEFAULT for GPIO_DBGROUTEPEN           */
#define GPIO_DBGROUTEPEN_SWCLKTCKPEN_DEFAULT             (_GPIO_DBGROUTEPEN_SWCLKTCKPEN_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_DBGROUTEPEN   */
#define GPIO_DBGROUTEPEN_SWDIOTMSPEN                     (0x1UL << 1)                                 /**< Route Location 0                            */
#define _GPIO_DBGROUTEPEN_SWDIOTMSPEN_SHIFT              1                                            /**< Shift value for GPIO_SWDIOTMSPEN            */
#define _GPIO_DBGROUTEPEN_SWDIOTMSPEN_MASK               0x2UL                                        /**< Bit mask for GPIO_SWDIOTMSPEN               */
#define _GPIO_DBGROUTEPEN_SWDIOTMSPEN_DEFAULT            0x00000001UL                                 /**< Mode DEFAULT for GPIO_DBGROUTEPEN           */
#define GPIO_DBGROUTEPEN_SWDIOTMSPEN_DEFAULT             (_GPIO_DBGROUTEPEN_SWDIOTMSPEN_DEFAULT << 1) /**< Shifted mode DEFAULT for GPIO_DBGROUTEPEN   */
#define GPIO_DBGROUTEPEN_TDOPEN                          (0x1UL << 2)                                 /**< JTAG Test Debug Output Pin Enable           */
#define _GPIO_DBGROUTEPEN_TDOPEN_SHIFT                   2                                            /**< Shift value for GPIO_TDOPEN                 */
#define _GPIO_DBGROUTEPEN_TDOPEN_MASK                    0x4UL                                        /**< Bit mask for GPIO_TDOPEN                    */
#define _GPIO_DBGROUTEPEN_TDOPEN_DEFAULT                 0x00000001UL                                 /**< Mode DEFAULT for GPIO_DBGROUTEPEN           */
#define GPIO_DBGROUTEPEN_TDOPEN_DEFAULT                  (_GPIO_DBGROUTEPEN_TDOPEN_DEFAULT << 2)      /**< Shifted mode DEFAULT for GPIO_DBGROUTEPEN   */
#define GPIO_DBGROUTEPEN_TDIPEN                          (0x1UL << 3)                                 /**< JTAG Test Debug Input Pin Enable            */
#define _GPIO_DBGROUTEPEN_TDIPEN_SHIFT                   3                                            /**< Shift value for GPIO_TDIPEN                 */
#define _GPIO_DBGROUTEPEN_TDIPEN_MASK                    0x8UL                                        /**< Bit mask for GPIO_TDIPEN                    */
#define _GPIO_DBGROUTEPEN_TDIPEN_DEFAULT                 0x00000001UL                                 /**< Mode DEFAULT for GPIO_DBGROUTEPEN           */
#define GPIO_DBGROUTEPEN_TDIPEN_DEFAULT                  (_GPIO_DBGROUTEPEN_TDIPEN_DEFAULT << 3)      /**< Shifted mode DEFAULT for GPIO_DBGROUTEPEN   */

/* Bit fields for GPIO TRACEROUTEPEN */
#define _GPIO_TRACEROUTEPEN_RESETVALUE                   0x00000000UL                                     /**< Default value for GPIO_TRACEROUTEPEN        */
#define _GPIO_TRACEROUTEPEN_MASK                         0x0000003FUL                                     /**< Mask for GPIO_TRACEROUTEPEN                 */
#define GPIO_TRACEROUTEPEN_SWVPEN                        (0x1UL << 0)                                     /**< Serial Wire Viewer Output Pin Enable        */
#define _GPIO_TRACEROUTEPEN_SWVPEN_SHIFT                 0                                                /**< Shift value for GPIO_SWVPEN                 */
#define _GPIO_TRACEROUTEPEN_SWVPEN_MASK                  0x1UL                                            /**< Bit mask for GPIO_SWVPEN                    */
#define _GPIO_TRACEROUTEPEN_SWVPEN_DEFAULT               0x00000000UL                                     /**< Mode DEFAULT for GPIO_TRACEROUTEPEN         */
#define GPIO_TRACEROUTEPEN_SWVPEN_DEFAULT                (_GPIO_TRACEROUTEPEN_SWVPEN_DEFAULT << 0)        /**< Shifted mode DEFAULT for GPIO_TRACEROUTEPEN */
#define GPIO_TRACEROUTEPEN_TRACECLKPEN                   (0x1UL << 1)                                     /**< Trace Clk Pin Enable                        */
#define _GPIO_TRACEROUTEPEN_TRACECLKPEN_SHIFT            1                                                /**< Shift value for GPIO_TRACECLKPEN            */
#define _GPIO_TRACEROUTEPEN_TRACECLKPEN_MASK             0x2UL                                            /**< Bit mask for GPIO_TRACECLKPEN               */
#define _GPIO_TRACEROUTEPEN_TRACECLKPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_TRACEROUTEPEN         */
#define GPIO_TRACEROUTEPEN_TRACECLKPEN_DEFAULT           (_GPIO_TRACEROUTEPEN_TRACECLKPEN_DEFAULT << 1)   /**< Shifted mode DEFAULT for GPIO_TRACEROUTEPEN */
#define GPIO_TRACEROUTEPEN_TRACEDATA0PEN                 (0x1UL << 2)                                     /**< Trace Data0 Pin Enable                      */
#define _GPIO_TRACEROUTEPEN_TRACEDATA0PEN_SHIFT          2                                                /**< Shift value for GPIO_TRACEDATA0PEN          */
#define _GPIO_TRACEROUTEPEN_TRACEDATA0PEN_MASK           0x4UL                                            /**< Bit mask for GPIO_TRACEDATA0PEN             */
#define _GPIO_TRACEROUTEPEN_TRACEDATA0PEN_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_TRACEROUTEPEN         */
#define GPIO_TRACEROUTEPEN_TRACEDATA0PEN_DEFAULT         (_GPIO_TRACEROUTEPEN_TRACEDATA0PEN_DEFAULT << 2) /**< Shifted mode DEFAULT for GPIO_TRACEROUTEPEN */
#define GPIO_TRACEROUTEPEN_TRACEDATA1PEN                 (0x1UL << 3)                                     /**< Trace Data1 Pin Enable                      */
#define _GPIO_TRACEROUTEPEN_TRACEDATA1PEN_SHIFT          3                                                /**< Shift value for GPIO_TRACEDATA1PEN          */
#define _GPIO_TRACEROUTEPEN_TRACEDATA1PEN_MASK           0x8UL                                            /**< Bit mask for GPIO_TRACEDATA1PEN             */
#define _GPIO_TRACEROUTEPEN_TRACEDATA1PEN_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_TRACEROUTEPEN         */
#define GPIO_TRACEROUTEPEN_TRACEDATA1PEN_DEFAULT         (_GPIO_TRACEROUTEPEN_TRACEDATA1PEN_DEFAULT << 3) /**< Shifted mode DEFAULT for GPIO_TRACEROUTEPEN */
#define GPIO_TRACEROUTEPEN_TRACEDATA2PEN                 (0x1UL << 4)                                     /**< Trace Data2 Pin Enable                      */
#define _GPIO_TRACEROUTEPEN_TRACEDATA2PEN_SHIFT          4                                                /**< Shift value for GPIO_TRACEDATA2PEN          */
#define _GPIO_TRACEROUTEPEN_TRACEDATA2PEN_MASK           0x10UL                                           /**< Bit mask for GPIO_TRACEDATA2PEN             */
#define _GPIO_TRACEROUTEPEN_TRACEDATA2PEN_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_TRACEROUTEPEN         */
#define GPIO_TRACEROUTEPEN_TRACEDATA2PEN_DEFAULT         (_GPIO_TRACEROUTEPEN_TRACEDATA2PEN_DEFAULT << 4) /**< Shifted mode DEFAULT for GPIO_TRACEROUTEPEN */
#define GPIO_TRACEROUTEPEN_TRACEDATA3PEN                 (0x1UL << 5)                                     /**< Trace Data3 Pin Enable                      */
#define _GPIO_TRACEROUTEPEN_TRACEDATA3PEN_SHIFT          5                                                /**< Shift value for GPIO_TRACEDATA3PEN          */
#define _GPIO_TRACEROUTEPEN_TRACEDATA3PEN_MASK           0x20UL                                           /**< Bit mask for GPIO_TRACEDATA3PEN             */
#define _GPIO_TRACEROUTEPEN_TRACEDATA3PEN_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_TRACEROUTEPEN         */
#define GPIO_TRACEROUTEPEN_TRACEDATA3PEN_DEFAULT         (_GPIO_TRACEROUTEPEN_TRACEDATA3PEN_DEFAULT << 5) /**< Shifted mode DEFAULT for GPIO_TRACEROUTEPEN */

/* Bit fields for GPIO LCDSEG */
#define _GPIO_LCDSEG_RESETVALUE                          0x00000000UL                            /**< Default value for GPIO_LCDSEG               */
#define _GPIO_LCDSEG_MASK                                0x000FFFFFUL                            /**< Mask for GPIO_LCDSEG                        */
#define _GPIO_LCDSEG_LCDSEGALLOC_SHIFT                   0                                       /**< Shift value for GPIO_LCDSEGALLOC            */
#define _GPIO_LCDSEG_LCDSEGALLOC_MASK                    0xFFFFFUL                               /**< Bit mask for GPIO_LCDSEGALLOC               */
#define _GPIO_LCDSEG_LCDSEGALLOC_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_LCDSEG                */
#define GPIO_LCDSEG_LCDSEGALLOC_DEFAULT                  (_GPIO_LCDSEG_LCDSEGALLOC_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LCDSEG        */

/* Bit fields for GPIO LCDCOM */
#define _GPIO_LCDCOM_RESETVALUE                          0x00000000UL                            /**< Default value for GPIO_LCDCOM               */
#define _GPIO_LCDCOM_MASK                                0x0000000FUL                            /**< Mask for GPIO_LCDCOM                        */
#define _GPIO_LCDCOM_LCDCOMALLOC_SHIFT                   0                                       /**< Shift value for GPIO_LCDCOMALLOC            */
#define _GPIO_LCDCOM_LCDCOMALLOC_MASK                    0xFUL                                   /**< Bit mask for GPIO_LCDCOMALLOC               */
#define _GPIO_LCDCOM_LCDCOMALLOC_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_LCDCOM                */
#define GPIO_LCDCOM_LCDCOMALLOC_DEFAULT                  (_GPIO_LCDCOM_LCDCOMALLOC_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LCDCOM        */

/* Bit fields for GPIO_ACMP ROUTEEN */
#define _GPIO_ACMP_ROUTEEN_RESETVALUE                    0x00000000UL                                 /**< Default value for GPIO_ACMP_ROUTEEN         */
#define _GPIO_ACMP_ROUTEEN_MASK                          0x00000001UL                                 /**< Mask for GPIO_ACMP_ROUTEEN                  */
#define GPIO_ACMP_ROUTEEN_ACMPOUTPEN                     (0x1UL << 0)                                 /**< ACMPOUT pin enable control bit              */
#define _GPIO_ACMP_ROUTEEN_ACMPOUTPEN_SHIFT              0                                            /**< Shift value for GPIO_ACMPOUTPEN             */
#define _GPIO_ACMP_ROUTEEN_ACMPOUTPEN_MASK               0x1UL                                        /**< Bit mask for GPIO_ACMPOUTPEN                */
#define _GPIO_ACMP_ROUTEEN_ACMPOUTPEN_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for GPIO_ACMP_ROUTEEN          */
#define GPIO_ACMP_ROUTEEN_ACMPOUTPEN_DEFAULT             (_GPIO_ACMP_ROUTEEN_ACMPOUTPEN_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_ACMP_ROUTEEN  */

/* Bit fields for GPIO_ACMP ACMPOUTROUTE */
#define _GPIO_ACMP_ACMPOUTROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_ACMP_ACMPOUTROUTE    */
#define _GPIO_ACMP_ACMPOUTROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_ACMP_ACMPOUTROUTE             */
#define _GPIO_ACMP_ACMPOUTROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_ACMP_ACMPOUTROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_ACMP_ACMPOUTROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_ACMP_ACMPOUTROUTE     */
#define GPIO_ACMP_ACMPOUTROUTE_PORT_DEFAULT              (_GPIO_ACMP_ACMPOUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_ACMP_ACMPOUTROUTE*/
#define _GPIO_ACMP_ACMPOUTROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_ACMP_ACMPOUTROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_ACMP_ACMPOUTROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_ACMP_ACMPOUTROUTE     */
#define GPIO_ACMP_ACMPOUTROUTE_PIN_DEFAULT               (_GPIO_ACMP_ACMPOUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_ACMP_ACMPOUTROUTE*/

/* Bit fields for GPIO_CMU ROUTEEN */
#define _GPIO_CMU_ROUTEEN_RESETVALUE                     0x00000000UL                                /**< Default value for GPIO_CMU_ROUTEEN          */
#define _GPIO_CMU_ROUTEEN_MASK                           0x0000000FUL                                /**< Mask for GPIO_CMU_ROUTEEN                   */
#define GPIO_CMU_ROUTEEN_CLKOUT0PEN                      (0x1UL << 0)                                /**< CLKOUT0 pin enable control bit              */
#define _GPIO_CMU_ROUTEEN_CLKOUT0PEN_SHIFT               0                                           /**< Shift value for GPIO_CLKOUT0PEN             */
#define _GPIO_CMU_ROUTEEN_CLKOUT0PEN_MASK                0x1UL                                       /**< Bit mask for GPIO_CLKOUT0PEN                */
#define _GPIO_CMU_ROUTEEN_CLKOUT0PEN_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_CMU_ROUTEEN           */
#define GPIO_CMU_ROUTEEN_CLKOUT0PEN_DEFAULT              (_GPIO_CMU_ROUTEEN_CLKOUT0PEN_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_CMU_ROUTEEN   */
#define GPIO_CMU_ROUTEEN_CLKOUT1PEN                      (0x1UL << 1)                                /**< CLKOUT1 pin enable control bit              */
#define _GPIO_CMU_ROUTEEN_CLKOUT1PEN_SHIFT               1                                           /**< Shift value for GPIO_CLKOUT1PEN             */
#define _GPIO_CMU_ROUTEEN_CLKOUT1PEN_MASK                0x2UL                                       /**< Bit mask for GPIO_CLKOUT1PEN                */
#define _GPIO_CMU_ROUTEEN_CLKOUT1PEN_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_CMU_ROUTEEN           */
#define GPIO_CMU_ROUTEEN_CLKOUT1PEN_DEFAULT              (_GPIO_CMU_ROUTEEN_CLKOUT1PEN_DEFAULT << 1) /**< Shifted mode DEFAULT for GPIO_CMU_ROUTEEN   */
#define GPIO_CMU_ROUTEEN_CLKOUT2PEN                      (0x1UL << 2)                                /**< CLKOUT2 pin enable control bit              */
#define _GPIO_CMU_ROUTEEN_CLKOUT2PEN_SHIFT               2                                           /**< Shift value for GPIO_CLKOUT2PEN             */
#define _GPIO_CMU_ROUTEEN_CLKOUT2PEN_MASK                0x4UL                                       /**< Bit mask for GPIO_CLKOUT2PEN                */
#define _GPIO_CMU_ROUTEEN_CLKOUT2PEN_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_CMU_ROUTEEN           */
#define GPIO_CMU_ROUTEEN_CLKOUT2PEN_DEFAULT              (_GPIO_CMU_ROUTEEN_CLKOUT2PEN_DEFAULT << 2) /**< Shifted mode DEFAULT for GPIO_CMU_ROUTEEN   */

/* Bit fields for GPIO_CMU CLKIN0ROUTE */
#define _GPIO_CMU_CLKIN0ROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_CMU_CLKIN0ROUTE      */
#define _GPIO_CMU_CLKIN0ROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_CMU_CLKIN0ROUTE               */
#define _GPIO_CMU_CLKIN0ROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_CMU_CLKIN0ROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_CMU_CLKIN0ROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_CMU_CLKIN0ROUTE       */
#define GPIO_CMU_CLKIN0ROUTE_PORT_DEFAULT                (_GPIO_CMU_CLKIN0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_CMU_CLKIN0ROUTE*/
#define _GPIO_CMU_CLKIN0ROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_CMU_CLKIN0ROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_CMU_CLKIN0ROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_CMU_CLKIN0ROUTE       */
#define GPIO_CMU_CLKIN0ROUTE_PIN_DEFAULT                 (_GPIO_CMU_CLKIN0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_CMU_CLKIN0ROUTE*/

/* Bit fields for GPIO_CMU CLKOUT0ROUTE */
#define _GPIO_CMU_CLKOUT0ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_CMU_CLKOUT0ROUTE     */
#define _GPIO_CMU_CLKOUT0ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_CMU_CLKOUT0ROUTE              */
#define _GPIO_CMU_CLKOUT0ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_CMU_CLKOUT0ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_CMU_CLKOUT0ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_CMU_CLKOUT0ROUTE      */
#define GPIO_CMU_CLKOUT0ROUTE_PORT_DEFAULT               (_GPIO_CMU_CLKOUT0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_CMU_CLKOUT0ROUTE*/
#define _GPIO_CMU_CLKOUT0ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_CMU_CLKOUT0ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_CMU_CLKOUT0ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_CMU_CLKOUT0ROUTE      */
#define GPIO_CMU_CLKOUT0ROUTE_PIN_DEFAULT                (_GPIO_CMU_CLKOUT0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_CMU_CLKOUT0ROUTE*/

/* Bit fields for GPIO_CMU CLKOUT1ROUTE */
#define _GPIO_CMU_CLKOUT1ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_CMU_CLKOUT1ROUTE     */
#define _GPIO_CMU_CLKOUT1ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_CMU_CLKOUT1ROUTE              */
#define _GPIO_CMU_CLKOUT1ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_CMU_CLKOUT1ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_CMU_CLKOUT1ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_CMU_CLKOUT1ROUTE      */
#define GPIO_CMU_CLKOUT1ROUTE_PORT_DEFAULT               (_GPIO_CMU_CLKOUT1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_CMU_CLKOUT1ROUTE*/
#define _GPIO_CMU_CLKOUT1ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_CMU_CLKOUT1ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_CMU_CLKOUT1ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_CMU_CLKOUT1ROUTE      */
#define GPIO_CMU_CLKOUT1ROUTE_PIN_DEFAULT                (_GPIO_CMU_CLKOUT1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_CMU_CLKOUT1ROUTE*/

/* Bit fields for GPIO_CMU CLKOUT2ROUTE */
#define _GPIO_CMU_CLKOUT2ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_CMU_CLKOUT2ROUTE     */
#define _GPIO_CMU_CLKOUT2ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_CMU_CLKOUT2ROUTE              */
#define _GPIO_CMU_CLKOUT2ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_CMU_CLKOUT2ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_CMU_CLKOUT2ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_CMU_CLKOUT2ROUTE      */
#define GPIO_CMU_CLKOUT2ROUTE_PORT_DEFAULT               (_GPIO_CMU_CLKOUT2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_CMU_CLKOUT2ROUTE*/
#define _GPIO_CMU_CLKOUT2ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_CMU_CLKOUT2ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_CMU_CLKOUT2ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_CMU_CLKOUT2ROUTE      */
#define GPIO_CMU_CLKOUT2ROUTE_PIN_DEFAULT                (_GPIO_CMU_CLKOUT2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_CMU_CLKOUT2ROUTE*/

/* Bit fields for GPIO_EUSART ROUTEEN */
#define _GPIO_EUSART_ROUTEEN_RESETVALUE                  0x00000000UL                                /**< Default value for GPIO_EUSART_ROUTEEN       */
#define _GPIO_EUSART_ROUTEEN_MASK                        0x0000001FUL                                /**< Mask for GPIO_EUSART_ROUTEEN                */
#define GPIO_EUSART_ROUTEEN_CSPEN                        (0x1UL << 0)                                /**< CS pin enable control bit                   */
#define _GPIO_EUSART_ROUTEEN_CSPEN_SHIFT                 0                                           /**< Shift value for GPIO_CSPEN                  */
#define _GPIO_EUSART_ROUTEEN_CSPEN_MASK                  0x1UL                                       /**< Bit mask for GPIO_CSPEN                     */
#define _GPIO_EUSART_ROUTEEN_CSPEN_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for GPIO_EUSART_ROUTEEN        */
#define GPIO_EUSART_ROUTEEN_CSPEN_DEFAULT                (_GPIO_EUSART_ROUTEEN_CSPEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for GPIO_EUSART_ROUTEEN*/
#define GPIO_EUSART_ROUTEEN_RTSPEN                       (0x1UL << 1)                                /**< RTS pin enable control bit                  */
#define _GPIO_EUSART_ROUTEEN_RTSPEN_SHIFT                1                                           /**< Shift value for GPIO_RTSPEN                 */
#define _GPIO_EUSART_ROUTEEN_RTSPEN_MASK                 0x2UL                                       /**< Bit mask for GPIO_RTSPEN                    */
#define _GPIO_EUSART_ROUTEEN_RTSPEN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_EUSART_ROUTEEN        */
#define GPIO_EUSART_ROUTEEN_RTSPEN_DEFAULT               (_GPIO_EUSART_ROUTEEN_RTSPEN_DEFAULT << 1)  /**< Shifted mode DEFAULT for GPIO_EUSART_ROUTEEN*/
#define GPIO_EUSART_ROUTEEN_RXPEN                        (0x1UL << 2)                                /**< RX pin enable control bit                   */
#define _GPIO_EUSART_ROUTEEN_RXPEN_SHIFT                 2                                           /**< Shift value for GPIO_RXPEN                  */
#define _GPIO_EUSART_ROUTEEN_RXPEN_MASK                  0x4UL                                       /**< Bit mask for GPIO_RXPEN                     */
#define _GPIO_EUSART_ROUTEEN_RXPEN_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for GPIO_EUSART_ROUTEEN        */
#define GPIO_EUSART_ROUTEEN_RXPEN_DEFAULT                (_GPIO_EUSART_ROUTEEN_RXPEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for GPIO_EUSART_ROUTEEN*/
#define GPIO_EUSART_ROUTEEN_SCLKPEN                      (0x1UL << 3)                                /**< SCLK pin enable control bit                 */
#define _GPIO_EUSART_ROUTEEN_SCLKPEN_SHIFT               3                                           /**< Shift value for GPIO_SCLKPEN                */
#define _GPIO_EUSART_ROUTEEN_SCLKPEN_MASK                0x8UL                                       /**< Bit mask for GPIO_SCLKPEN                   */
#define _GPIO_EUSART_ROUTEEN_SCLKPEN_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_EUSART_ROUTEEN        */
#define GPIO_EUSART_ROUTEEN_SCLKPEN_DEFAULT              (_GPIO_EUSART_ROUTEEN_SCLKPEN_DEFAULT << 3) /**< Shifted mode DEFAULT for GPIO_EUSART_ROUTEEN*/
#define GPIO_EUSART_ROUTEEN_TXPEN                        (0x1UL << 4)                                /**< TX pin enable control bit                   */
#define _GPIO_EUSART_ROUTEEN_TXPEN_SHIFT                 4                                           /**< Shift value for GPIO_TXPEN                  */
#define _GPIO_EUSART_ROUTEEN_TXPEN_MASK                  0x10UL                                      /**< Bit mask for GPIO_TXPEN                     */
#define _GPIO_EUSART_ROUTEEN_TXPEN_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for GPIO_EUSART_ROUTEEN        */
#define GPIO_EUSART_ROUTEEN_TXPEN_DEFAULT                (_GPIO_EUSART_ROUTEEN_TXPEN_DEFAULT << 4)   /**< Shifted mode DEFAULT for GPIO_EUSART_ROUTEEN*/

/* Bit fields for GPIO_EUSART CSROUTE */
#define _GPIO_EUSART_CSROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_EUSART_CSROUTE       */
#define _GPIO_EUSART_CSROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_EUSART_CSROUTE                */
#define _GPIO_EUSART_CSROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_EUSART_CSROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_EUSART_CSROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_EUSART_CSROUTE        */
#define GPIO_EUSART_CSROUTE_PORT_DEFAULT                 (_GPIO_EUSART_CSROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EUSART_CSROUTE*/
#define _GPIO_EUSART_CSROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_EUSART_CSROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_EUSART_CSROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_EUSART_CSROUTE        */
#define GPIO_EUSART_CSROUTE_PIN_DEFAULT                  (_GPIO_EUSART_CSROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EUSART_CSROUTE*/

/* Bit fields for GPIO_EUSART CTSROUTE */
#define _GPIO_EUSART_CTSROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_EUSART_CTSROUTE      */
#define _GPIO_EUSART_CTSROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_EUSART_CTSROUTE               */
#define _GPIO_EUSART_CTSROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_EUSART_CTSROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_EUSART_CTSROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_EUSART_CTSROUTE       */
#define GPIO_EUSART_CTSROUTE_PORT_DEFAULT                (_GPIO_EUSART_CTSROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EUSART_CTSROUTE*/
#define _GPIO_EUSART_CTSROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_EUSART_CTSROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_EUSART_CTSROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EUSART_CTSROUTE       */
#define GPIO_EUSART_CTSROUTE_PIN_DEFAULT                 (_GPIO_EUSART_CTSROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EUSART_CTSROUTE*/

/* Bit fields for GPIO_EUSART RTSROUTE */
#define _GPIO_EUSART_RTSROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_EUSART_RTSROUTE      */
#define _GPIO_EUSART_RTSROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_EUSART_RTSROUTE               */
#define _GPIO_EUSART_RTSROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_EUSART_RTSROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_EUSART_RTSROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_EUSART_RTSROUTE       */
#define GPIO_EUSART_RTSROUTE_PORT_DEFAULT                (_GPIO_EUSART_RTSROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EUSART_RTSROUTE*/
#define _GPIO_EUSART_RTSROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_EUSART_RTSROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_EUSART_RTSROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_EUSART_RTSROUTE       */
#define GPIO_EUSART_RTSROUTE_PIN_DEFAULT                 (_GPIO_EUSART_RTSROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EUSART_RTSROUTE*/

/* Bit fields for GPIO_EUSART RXROUTE */
#define _GPIO_EUSART_RXROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_EUSART_RXROUTE       */
#define _GPIO_EUSART_RXROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_EUSART_RXROUTE                */
#define _GPIO_EUSART_RXROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_EUSART_RXROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_EUSART_RXROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_EUSART_RXROUTE        */
#define GPIO_EUSART_RXROUTE_PORT_DEFAULT                 (_GPIO_EUSART_RXROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EUSART_RXROUTE*/
#define _GPIO_EUSART_RXROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_EUSART_RXROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_EUSART_RXROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_EUSART_RXROUTE        */
#define GPIO_EUSART_RXROUTE_PIN_DEFAULT                  (_GPIO_EUSART_RXROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EUSART_RXROUTE*/

/* Bit fields for GPIO_EUSART SCLKROUTE */
#define _GPIO_EUSART_SCLKROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_EUSART_SCLKROUTE     */
#define _GPIO_EUSART_SCLKROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_EUSART_SCLKROUTE              */
#define _GPIO_EUSART_SCLKROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_EUSART_SCLKROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_EUSART_SCLKROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_EUSART_SCLKROUTE      */
#define GPIO_EUSART_SCLKROUTE_PORT_DEFAULT               (_GPIO_EUSART_SCLKROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EUSART_SCLKROUTE*/
#define _GPIO_EUSART_SCLKROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_EUSART_SCLKROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_EUSART_SCLKROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_EUSART_SCLKROUTE      */
#define GPIO_EUSART_SCLKROUTE_PIN_DEFAULT                (_GPIO_EUSART_SCLKROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EUSART_SCLKROUTE*/

/* Bit fields for GPIO_EUSART TXROUTE */
#define _GPIO_EUSART_TXROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_EUSART_TXROUTE       */
#define _GPIO_EUSART_TXROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_EUSART_TXROUTE                */
#define _GPIO_EUSART_TXROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_EUSART_TXROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_EUSART_TXROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_EUSART_TXROUTE        */
#define GPIO_EUSART_TXROUTE_PORT_DEFAULT                 (_GPIO_EUSART_TXROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_EUSART_TXROUTE*/
#define _GPIO_EUSART_TXROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_EUSART_TXROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_EUSART_TXROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_EUSART_TXROUTE        */
#define GPIO_EUSART_TXROUTE_PIN_DEFAULT                  (_GPIO_EUSART_TXROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_EUSART_TXROUTE*/

/* Bit fields for GPIO_FRC ROUTEEN */
#define _GPIO_FRC_ROUTEEN_RESETVALUE                     0x00000000UL                               /**< Default value for GPIO_FRC_ROUTEEN          */
#define _GPIO_FRC_ROUTEEN_MASK                           0x00000007UL                               /**< Mask for GPIO_FRC_ROUTEEN                   */
#define GPIO_FRC_ROUTEEN_DCLKPEN                         (0x1UL << 0)                               /**< DCLK pin enable control bit                 */
#define _GPIO_FRC_ROUTEEN_DCLKPEN_SHIFT                  0                                          /**< Shift value for GPIO_DCLKPEN                */
#define _GPIO_FRC_ROUTEEN_DCLKPEN_MASK                   0x1UL                                      /**< Bit mask for GPIO_DCLKPEN                   */
#define _GPIO_FRC_ROUTEEN_DCLKPEN_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for GPIO_FRC_ROUTEEN           */
#define GPIO_FRC_ROUTEEN_DCLKPEN_DEFAULT                 (_GPIO_FRC_ROUTEEN_DCLKPEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for GPIO_FRC_ROUTEEN   */
#define GPIO_FRC_ROUTEEN_DFRAMEPEN                       (0x1UL << 1)                               /**< DFRAME pin enable control bit               */
#define _GPIO_FRC_ROUTEEN_DFRAMEPEN_SHIFT                1                                          /**< Shift value for GPIO_DFRAMEPEN              */
#define _GPIO_FRC_ROUTEEN_DFRAMEPEN_MASK                 0x2UL                                      /**< Bit mask for GPIO_DFRAMEPEN                 */
#define _GPIO_FRC_ROUTEEN_DFRAMEPEN_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_FRC_ROUTEEN           */
#define GPIO_FRC_ROUTEEN_DFRAMEPEN_DEFAULT               (_GPIO_FRC_ROUTEEN_DFRAMEPEN_DEFAULT << 1) /**< Shifted mode DEFAULT for GPIO_FRC_ROUTEEN   */
#define GPIO_FRC_ROUTEEN_DOUTPEN                         (0x1UL << 2)                               /**< DOUT pin enable control bit                 */
#define _GPIO_FRC_ROUTEEN_DOUTPEN_SHIFT                  2                                          /**< Shift value for GPIO_DOUTPEN                */
#define _GPIO_FRC_ROUTEEN_DOUTPEN_MASK                   0x4UL                                      /**< Bit mask for GPIO_DOUTPEN                   */
#define _GPIO_FRC_ROUTEEN_DOUTPEN_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for GPIO_FRC_ROUTEEN           */
#define GPIO_FRC_ROUTEEN_DOUTPEN_DEFAULT                 (_GPIO_FRC_ROUTEEN_DOUTPEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for GPIO_FRC_ROUTEEN   */

/* Bit fields for GPIO_FRC DCLKROUTE */
#define _GPIO_FRC_DCLKROUTE_RESETVALUE                   0x00000000UL                            /**< Default value for GPIO_FRC_DCLKROUTE        */
#define _GPIO_FRC_DCLKROUTE_MASK                         0x000F0003UL                            /**< Mask for GPIO_FRC_DCLKROUTE                 */
#define _GPIO_FRC_DCLKROUTE_PORT_SHIFT                   0                                       /**< Shift value for GPIO_PORT                   */
#define _GPIO_FRC_DCLKROUTE_PORT_MASK                    0x3UL                                   /**< Bit mask for GPIO_PORT                      */
#define _GPIO_FRC_DCLKROUTE_PORT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_FRC_DCLKROUTE         */
#define GPIO_FRC_DCLKROUTE_PORT_DEFAULT                  (_GPIO_FRC_DCLKROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_FRC_DCLKROUTE */
#define _GPIO_FRC_DCLKROUTE_PIN_SHIFT                    16                                      /**< Shift value for GPIO_PIN                    */
#define _GPIO_FRC_DCLKROUTE_PIN_MASK                     0xF0000UL                               /**< Bit mask for GPIO_PIN                       */
#define _GPIO_FRC_DCLKROUTE_PIN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for GPIO_FRC_DCLKROUTE         */
#define GPIO_FRC_DCLKROUTE_PIN_DEFAULT                   (_GPIO_FRC_DCLKROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_FRC_DCLKROUTE */

/* Bit fields for GPIO_FRC DFRAMEROUTE */
#define _GPIO_FRC_DFRAMEROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_FRC_DFRAMEROUTE      */
#define _GPIO_FRC_DFRAMEROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_FRC_DFRAMEROUTE               */
#define _GPIO_FRC_DFRAMEROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_FRC_DFRAMEROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_FRC_DFRAMEROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_FRC_DFRAMEROUTE       */
#define GPIO_FRC_DFRAMEROUTE_PORT_DEFAULT                (_GPIO_FRC_DFRAMEROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_FRC_DFRAMEROUTE*/
#define _GPIO_FRC_DFRAMEROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_FRC_DFRAMEROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_FRC_DFRAMEROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_FRC_DFRAMEROUTE       */
#define GPIO_FRC_DFRAMEROUTE_PIN_DEFAULT                 (_GPIO_FRC_DFRAMEROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_FRC_DFRAMEROUTE*/

/* Bit fields for GPIO_FRC DOUTROUTE */
#define _GPIO_FRC_DOUTROUTE_RESETVALUE                   0x00000000UL                            /**< Default value for GPIO_FRC_DOUTROUTE        */
#define _GPIO_FRC_DOUTROUTE_MASK                         0x000F0003UL                            /**< Mask for GPIO_FRC_DOUTROUTE                 */
#define _GPIO_FRC_DOUTROUTE_PORT_SHIFT                   0                                       /**< Shift value for GPIO_PORT                   */
#define _GPIO_FRC_DOUTROUTE_PORT_MASK                    0x3UL                                   /**< Bit mask for GPIO_PORT                      */
#define _GPIO_FRC_DOUTROUTE_PORT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_FRC_DOUTROUTE         */
#define GPIO_FRC_DOUTROUTE_PORT_DEFAULT                  (_GPIO_FRC_DOUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_FRC_DOUTROUTE */
#define _GPIO_FRC_DOUTROUTE_PIN_SHIFT                    16                                      /**< Shift value for GPIO_PIN                    */
#define _GPIO_FRC_DOUTROUTE_PIN_MASK                     0xF0000UL                               /**< Bit mask for GPIO_PIN                       */
#define _GPIO_FRC_DOUTROUTE_PIN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for GPIO_FRC_DOUTROUTE         */
#define GPIO_FRC_DOUTROUTE_PIN_DEFAULT                   (_GPIO_FRC_DOUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_FRC_DOUTROUTE */

/* Bit fields for GPIO_I2C ROUTEEN */
#define _GPIO_I2C_ROUTEEN_RESETVALUE                     0x00000000UL                            /**< Default value for GPIO_I2C_ROUTEEN          */
#define _GPIO_I2C_ROUTEEN_MASK                           0x00000003UL                            /**< Mask for GPIO_I2C_ROUTEEN                   */
#define GPIO_I2C_ROUTEEN_SCLPEN                          (0x1UL << 0)                            /**< SCL pin enable control bit                  */
#define _GPIO_I2C_ROUTEEN_SCLPEN_SHIFT                   0                                       /**< Shift value for GPIO_SCLPEN                 */
#define _GPIO_I2C_ROUTEEN_SCLPEN_MASK                    0x1UL                                   /**< Bit mask for GPIO_SCLPEN                    */
#define _GPIO_I2C_ROUTEEN_SCLPEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_I2C_ROUTEEN           */
#define GPIO_I2C_ROUTEEN_SCLPEN_DEFAULT                  (_GPIO_I2C_ROUTEEN_SCLPEN_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_I2C_ROUTEEN   */
#define GPIO_I2C_ROUTEEN_SDAPEN                          (0x1UL << 1)                            /**< SDA pin enable control bit                  */
#define _GPIO_I2C_ROUTEEN_SDAPEN_SHIFT                   1                                       /**< Shift value for GPIO_SDAPEN                 */
#define _GPIO_I2C_ROUTEEN_SDAPEN_MASK                    0x2UL                                   /**< Bit mask for GPIO_SDAPEN                    */
#define _GPIO_I2C_ROUTEEN_SDAPEN_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_I2C_ROUTEEN           */
#define GPIO_I2C_ROUTEEN_SDAPEN_DEFAULT                  (_GPIO_I2C_ROUTEEN_SDAPEN_DEFAULT << 1) /**< Shifted mode DEFAULT for GPIO_I2C_ROUTEEN   */

/* Bit fields for GPIO_I2C SCLROUTE */
#define _GPIO_I2C_SCLROUTE_RESETVALUE                    0x00000000UL                           /**< Default value for GPIO_I2C_SCLROUTE         */
#define _GPIO_I2C_SCLROUTE_MASK                          0x000F0003UL                           /**< Mask for GPIO_I2C_SCLROUTE                  */
#define _GPIO_I2C_SCLROUTE_PORT_SHIFT                    0                                      /**< Shift value for GPIO_PORT                   */
#define _GPIO_I2C_SCLROUTE_PORT_MASK                     0x3UL                                  /**< Bit mask for GPIO_PORT                      */
#define _GPIO_I2C_SCLROUTE_PORT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for GPIO_I2C_SCLROUTE          */
#define GPIO_I2C_SCLROUTE_PORT_DEFAULT                   (_GPIO_I2C_SCLROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_I2C_SCLROUTE  */
#define _GPIO_I2C_SCLROUTE_PIN_SHIFT                     16                                     /**< Shift value for GPIO_PIN                    */
#define _GPIO_I2C_SCLROUTE_PIN_MASK                      0xF0000UL                              /**< Bit mask for GPIO_PIN                       */
#define _GPIO_I2C_SCLROUTE_PIN_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for GPIO_I2C_SCLROUTE          */
#define GPIO_I2C_SCLROUTE_PIN_DEFAULT                    (_GPIO_I2C_SCLROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_I2C_SCLROUTE  */

/* Bit fields for GPIO_I2C SDAROUTE */
#define _GPIO_I2C_SDAROUTE_RESETVALUE                    0x00000000UL                           /**< Default value for GPIO_I2C_SDAROUTE         */
#define _GPIO_I2C_SDAROUTE_MASK                          0x000F0003UL                           /**< Mask for GPIO_I2C_SDAROUTE                  */
#define _GPIO_I2C_SDAROUTE_PORT_SHIFT                    0                                      /**< Shift value for GPIO_PORT                   */
#define _GPIO_I2C_SDAROUTE_PORT_MASK                     0x3UL                                  /**< Bit mask for GPIO_PORT                      */
#define _GPIO_I2C_SDAROUTE_PORT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for GPIO_I2C_SDAROUTE          */
#define GPIO_I2C_SDAROUTE_PORT_DEFAULT                   (_GPIO_I2C_SDAROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_I2C_SDAROUTE  */
#define _GPIO_I2C_SDAROUTE_PIN_SHIFT                     16                                     /**< Shift value for GPIO_PIN                    */
#define _GPIO_I2C_SDAROUTE_PIN_MASK                      0xF0000UL                              /**< Bit mask for GPIO_PIN                       */
#define _GPIO_I2C_SDAROUTE_PIN_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for GPIO_I2C_SDAROUTE          */
#define GPIO_I2C_SDAROUTE_PIN_DEFAULT                    (_GPIO_I2C_SDAROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_I2C_SDAROUTE  */

/* Bit fields for GPIO_KEYSCAN ROUTEEN */
#define _GPIO_KEYSCAN_ROUTEEN_RESETVALUE                 0x00000000UL                                    /**< Default value for GPIO_KEYSCAN_ROUTEEN      */
#define _GPIO_KEYSCAN_ROUTEEN_MASK                       0x000000FFUL                                    /**< Mask for GPIO_KEYSCAN_ROUTEEN               */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT0PEN                  (0x1UL << 0)                                    /**< COLOUT0 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT0PEN_SHIFT           0                                               /**< Shift value for GPIO_COLOUT0PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT0PEN_MASK            0x1UL                                           /**< Bit mask for GPIO_COLOUT0PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT0PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT0PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT0PEN_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/
#define GPIO_KEYSCAN_ROUTEEN_COLOUT1PEN                  (0x1UL << 1)                                    /**< COLOUT1 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT1PEN_SHIFT           1                                               /**< Shift value for GPIO_COLOUT1PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT1PEN_MASK            0x2UL                                           /**< Bit mask for GPIO_COLOUT1PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT1PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT1PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT1PEN_DEFAULT << 1) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/
#define GPIO_KEYSCAN_ROUTEEN_COLOUT2PEN                  (0x1UL << 2)                                    /**< COLOUT2 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT2PEN_SHIFT           2                                               /**< Shift value for GPIO_COLOUT2PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT2PEN_MASK            0x4UL                                           /**< Bit mask for GPIO_COLOUT2PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT2PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT2PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT2PEN_DEFAULT << 2) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/
#define GPIO_KEYSCAN_ROUTEEN_COLOUT3PEN                  (0x1UL << 3)                                    /**< COLOUT3 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT3PEN_SHIFT           3                                               /**< Shift value for GPIO_COLOUT3PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT3PEN_MASK            0x8UL                                           /**< Bit mask for GPIO_COLOUT3PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT3PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT3PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT3PEN_DEFAULT << 3) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/
#define GPIO_KEYSCAN_ROUTEEN_COLOUT4PEN                  (0x1UL << 4)                                    /**< COLOUT4 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT4PEN_SHIFT           4                                               /**< Shift value for GPIO_COLOUT4PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT4PEN_MASK            0x10UL                                          /**< Bit mask for GPIO_COLOUT4PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT4PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT4PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT4PEN_DEFAULT << 4) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/
#define GPIO_KEYSCAN_ROUTEEN_COLOUT5PEN                  (0x1UL << 5)                                    /**< COLOUT5 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT5PEN_SHIFT           5                                               /**< Shift value for GPIO_COLOUT5PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT5PEN_MASK            0x20UL                                          /**< Bit mask for GPIO_COLOUT5PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT5PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT5PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT5PEN_DEFAULT << 5) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/
#define GPIO_KEYSCAN_ROUTEEN_COLOUT6PEN                  (0x1UL << 6)                                    /**< COLOUT6 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT6PEN_SHIFT           6                                               /**< Shift value for GPIO_COLOUT6PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT6PEN_MASK            0x40UL                                          /**< Bit mask for GPIO_COLOUT6PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT6PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT6PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT6PEN_DEFAULT << 6) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/
#define GPIO_KEYSCAN_ROUTEEN_COLOUT7PEN                  (0x1UL << 7)                                    /**< COLOUT7 pin enable control bit              */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT7PEN_SHIFT           7                                               /**< Shift value for GPIO_COLOUT7PEN             */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT7PEN_MASK            0x80UL                                          /**< Bit mask for GPIO_COLOUT7PEN                */
#define _GPIO_KEYSCAN_ROUTEEN_COLOUT7PEN_DEFAULT         0x00000000UL                                    /**< Mode DEFAULT for GPIO_KEYSCAN_ROUTEEN       */
#define GPIO_KEYSCAN_ROUTEEN_COLOUT7PEN_DEFAULT          (_GPIO_KEYSCAN_ROUTEEN_COLOUT7PEN_DEFAULT << 7) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROUTEEN*/

/* Bit fields for GPIO_KEYSCAN COLOUT0ROUTE */
#define _GPIO_KEYSCAN_COLOUT0ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT0ROUTE */
#define _GPIO_KEYSCAN_COLOUT0ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT0ROUTE          */
#define _GPIO_KEYSCAN_COLOUT0ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT0ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT0ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT0ROUTE  */
#define GPIO_KEYSCAN_COLOUT0ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT0ROUTE*/
#define _GPIO_KEYSCAN_COLOUT0ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT0ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT0ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT0ROUTE  */
#define GPIO_KEYSCAN_COLOUT0ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT0ROUTE*/

/* Bit fields for GPIO_KEYSCAN COLOUT1ROUTE */
#define _GPIO_KEYSCAN_COLOUT1ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT1ROUTE */
#define _GPIO_KEYSCAN_COLOUT1ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT1ROUTE          */
#define _GPIO_KEYSCAN_COLOUT1ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT1ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT1ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT1ROUTE  */
#define GPIO_KEYSCAN_COLOUT1ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT1ROUTE*/
#define _GPIO_KEYSCAN_COLOUT1ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT1ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT1ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT1ROUTE  */
#define GPIO_KEYSCAN_COLOUT1ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT1ROUTE*/

/* Bit fields for GPIO_KEYSCAN COLOUT2ROUTE */
#define _GPIO_KEYSCAN_COLOUT2ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT2ROUTE */
#define _GPIO_KEYSCAN_COLOUT2ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT2ROUTE          */
#define _GPIO_KEYSCAN_COLOUT2ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT2ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT2ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT2ROUTE  */
#define GPIO_KEYSCAN_COLOUT2ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT2ROUTE*/
#define _GPIO_KEYSCAN_COLOUT2ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT2ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT2ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT2ROUTE  */
#define GPIO_KEYSCAN_COLOUT2ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT2ROUTE*/

/* Bit fields for GPIO_KEYSCAN COLOUT3ROUTE */
#define _GPIO_KEYSCAN_COLOUT3ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT3ROUTE */
#define _GPIO_KEYSCAN_COLOUT3ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT3ROUTE          */
#define _GPIO_KEYSCAN_COLOUT3ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT3ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT3ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT3ROUTE  */
#define GPIO_KEYSCAN_COLOUT3ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT3ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT3ROUTE*/
#define _GPIO_KEYSCAN_COLOUT3ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT3ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT3ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT3ROUTE  */
#define GPIO_KEYSCAN_COLOUT3ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT3ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT3ROUTE*/

/* Bit fields for GPIO_KEYSCAN COLOUT4ROUTE */
#define _GPIO_KEYSCAN_COLOUT4ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT4ROUTE */
#define _GPIO_KEYSCAN_COLOUT4ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT4ROUTE          */
#define _GPIO_KEYSCAN_COLOUT4ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT4ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT4ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT4ROUTE  */
#define GPIO_KEYSCAN_COLOUT4ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT4ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT4ROUTE*/
#define _GPIO_KEYSCAN_COLOUT4ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT4ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT4ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT4ROUTE  */
#define GPIO_KEYSCAN_COLOUT4ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT4ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT4ROUTE*/

/* Bit fields for GPIO_KEYSCAN COLOUT5ROUTE */
#define _GPIO_KEYSCAN_COLOUT5ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT5ROUTE */
#define _GPIO_KEYSCAN_COLOUT5ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT5ROUTE          */
#define _GPIO_KEYSCAN_COLOUT5ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT5ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT5ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT5ROUTE  */
#define GPIO_KEYSCAN_COLOUT5ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT5ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT5ROUTE*/
#define _GPIO_KEYSCAN_COLOUT5ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT5ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT5ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT5ROUTE  */
#define GPIO_KEYSCAN_COLOUT5ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT5ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT5ROUTE*/

/* Bit fields for GPIO_KEYSCAN COLOUT6ROUTE */
#define _GPIO_KEYSCAN_COLOUT6ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT6ROUTE */
#define _GPIO_KEYSCAN_COLOUT6ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT6ROUTE          */
#define _GPIO_KEYSCAN_COLOUT6ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT6ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT6ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT6ROUTE  */
#define GPIO_KEYSCAN_COLOUT6ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT6ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT6ROUTE*/
#define _GPIO_KEYSCAN_COLOUT6ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT6ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT6ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT6ROUTE  */
#define GPIO_KEYSCAN_COLOUT6ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT6ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT6ROUTE*/

/* Bit fields for GPIO_KEYSCAN COLOUT7ROUTE */
#define _GPIO_KEYSCAN_COLOUT7ROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_KEYSCAN_COLOUT7ROUTE */
#define _GPIO_KEYSCAN_COLOUT7ROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_KEYSCAN_COLOUT7ROUTE          */
#define _GPIO_KEYSCAN_COLOUT7ROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_COLOUT7ROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_COLOUT7ROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT7ROUTE  */
#define GPIO_KEYSCAN_COLOUT7ROUTE_PORT_DEFAULT           (_GPIO_KEYSCAN_COLOUT7ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT7ROUTE*/
#define _GPIO_KEYSCAN_COLOUT7ROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_COLOUT7ROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_COLOUT7ROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_KEYSCAN_COLOUT7ROUTE  */
#define GPIO_KEYSCAN_COLOUT7ROUTE_PIN_DEFAULT            (_GPIO_KEYSCAN_COLOUT7ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_COLOUT7ROUTE*/

/* Bit fields for GPIO_KEYSCAN ROWSENSE0ROUTE */
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_KEYSCAN_ROWSENSE0ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_KEYSCAN_ROWSENSE0ROUTE        */
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE0ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE0ROUTE_PORT_DEFAULT         (_GPIO_KEYSCAN_ROWSENSE0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE0ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_ROWSENSE0ROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE0ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE0ROUTE_PIN_DEFAULT          (_GPIO_KEYSCAN_ROWSENSE0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE0ROUTE*/

/* Bit fields for GPIO_KEYSCAN ROWSENSE1ROUTE */
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_KEYSCAN_ROWSENSE1ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_KEYSCAN_ROWSENSE1ROUTE        */
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE1ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE1ROUTE_PORT_DEFAULT         (_GPIO_KEYSCAN_ROWSENSE1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE1ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_ROWSENSE1ROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE1ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE1ROUTE_PIN_DEFAULT          (_GPIO_KEYSCAN_ROWSENSE1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE1ROUTE*/

/* Bit fields for GPIO_KEYSCAN ROWSENSE2ROUTE */
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_KEYSCAN_ROWSENSE2ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_KEYSCAN_ROWSENSE2ROUTE        */
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE2ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE2ROUTE_PORT_DEFAULT         (_GPIO_KEYSCAN_ROWSENSE2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE2ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_ROWSENSE2ROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE2ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE2ROUTE_PIN_DEFAULT          (_GPIO_KEYSCAN_ROWSENSE2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE2ROUTE*/

/* Bit fields for GPIO_KEYSCAN ROWSENSE3ROUTE */
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_KEYSCAN_ROWSENSE3ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_KEYSCAN_ROWSENSE3ROUTE        */
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE3ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE3ROUTE_PORT_DEFAULT         (_GPIO_KEYSCAN_ROWSENSE3ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE3ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_ROWSENSE3ROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE3ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE3ROUTE_PIN_DEFAULT          (_GPIO_KEYSCAN_ROWSENSE3ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE3ROUTE*/

/* Bit fields for GPIO_KEYSCAN ROWSENSE4ROUTE */
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_KEYSCAN_ROWSENSE4ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_KEYSCAN_ROWSENSE4ROUTE        */
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE4ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE4ROUTE_PORT_DEFAULT         (_GPIO_KEYSCAN_ROWSENSE4ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE4ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_ROWSENSE4ROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE4ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE4ROUTE_PIN_DEFAULT          (_GPIO_KEYSCAN_ROWSENSE4ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE4ROUTE*/

/* Bit fields for GPIO_KEYSCAN ROWSENSE5ROUTE */
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_KEYSCAN_ROWSENSE5ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_KEYSCAN_ROWSENSE5ROUTE        */
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE5ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE5ROUTE_PORT_DEFAULT         (_GPIO_KEYSCAN_ROWSENSE5ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE5ROUTE*/
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_KEYSCAN_ROWSENSE5ROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_KEYSCAN_ROWSENSE5ROUTE*/
#define GPIO_KEYSCAN_ROWSENSE5ROUTE_PIN_DEFAULT          (_GPIO_KEYSCAN_ROWSENSE5ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_KEYSCAN_ROWSENSE5ROUTE*/

/* Bit fields for GPIO_LESENSE ROUTEEN */
#define _GPIO_LESENSE_ROUTEEN_RESETVALUE                 0x00000000UL                                     /**< Default value for GPIO_LESENSE_ROUTEEN      */
#define _GPIO_LESENSE_ROUTEEN_MASK                       0x0000FFFFUL                                     /**< Mask for GPIO_LESENSE_ROUTEEN               */
#define GPIO_LESENSE_ROUTEEN_CH0OUTPEN                   (0x1UL << 0)                                     /**< CH0OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH0OUTPEN_SHIFT            0                                                /**< Shift value for GPIO_CH0OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH0OUTPEN_MASK             0x1UL                                            /**< Bit mask for GPIO_CH0OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH0OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH0OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH0OUTPEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH1OUTPEN                   (0x1UL << 1)                                     /**< CH1OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH1OUTPEN_SHIFT            1                                                /**< Shift value for GPIO_CH1OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH1OUTPEN_MASK             0x2UL                                            /**< Bit mask for GPIO_CH1OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH1OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH1OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH1OUTPEN_DEFAULT << 1)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH2OUTPEN                   (0x1UL << 2)                                     /**< CH2OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH2OUTPEN_SHIFT            2                                                /**< Shift value for GPIO_CH2OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH2OUTPEN_MASK             0x4UL                                            /**< Bit mask for GPIO_CH2OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH2OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH2OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH2OUTPEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH3OUTPEN                   (0x1UL << 3)                                     /**< CH3OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH3OUTPEN_SHIFT            3                                                /**< Shift value for GPIO_CH3OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH3OUTPEN_MASK             0x8UL                                            /**< Bit mask for GPIO_CH3OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH3OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH3OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH3OUTPEN_DEFAULT << 3)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH4OUTPEN                   (0x1UL << 4)                                     /**< CH4OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH4OUTPEN_SHIFT            4                                                /**< Shift value for GPIO_CH4OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH4OUTPEN_MASK             0x10UL                                           /**< Bit mask for GPIO_CH4OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH4OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH4OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH4OUTPEN_DEFAULT << 4)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH5OUTPEN                   (0x1UL << 5)                                     /**< CH5OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH5OUTPEN_SHIFT            5                                                /**< Shift value for GPIO_CH5OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH5OUTPEN_MASK             0x20UL                                           /**< Bit mask for GPIO_CH5OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH5OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH5OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH5OUTPEN_DEFAULT << 5)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH6OUTPEN                   (0x1UL << 6)                                     /**< CH6OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH6OUTPEN_SHIFT            6                                                /**< Shift value for GPIO_CH6OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH6OUTPEN_MASK             0x40UL                                           /**< Bit mask for GPIO_CH6OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH6OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH6OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH6OUTPEN_DEFAULT << 6)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH7OUTPEN                   (0x1UL << 7)                                     /**< CH7OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH7OUTPEN_SHIFT            7                                                /**< Shift value for GPIO_CH7OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH7OUTPEN_MASK             0x80UL                                           /**< Bit mask for GPIO_CH7OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH7OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH7OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH7OUTPEN_DEFAULT << 7)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH8OUTPEN                   (0x1UL << 8)                                     /**< CH8OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH8OUTPEN_SHIFT            8                                                /**< Shift value for GPIO_CH8OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH8OUTPEN_MASK             0x100UL                                          /**< Bit mask for GPIO_CH8OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH8OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH8OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH8OUTPEN_DEFAULT << 8)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH9OUTPEN                   (0x1UL << 9)                                     /**< CH9OUT pin enable control bit               */
#define _GPIO_LESENSE_ROUTEEN_CH9OUTPEN_SHIFT            9                                                /**< Shift value for GPIO_CH9OUTPEN              */
#define _GPIO_LESENSE_ROUTEEN_CH9OUTPEN_MASK             0x200UL                                          /**< Bit mask for GPIO_CH9OUTPEN                 */
#define _GPIO_LESENSE_ROUTEEN_CH9OUTPEN_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH9OUTPEN_DEFAULT           (_GPIO_LESENSE_ROUTEEN_CH9OUTPEN_DEFAULT << 9)   /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH10OUTPEN                  (0x1UL << 10)                                    /**< CH10OUT pin enable control bit              */
#define _GPIO_LESENSE_ROUTEEN_CH10OUTPEN_SHIFT           10                                               /**< Shift value for GPIO_CH10OUTPEN             */
#define _GPIO_LESENSE_ROUTEEN_CH10OUTPEN_MASK            0x400UL                                          /**< Bit mask for GPIO_CH10OUTPEN                */
#define _GPIO_LESENSE_ROUTEEN_CH10OUTPEN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH10OUTPEN_DEFAULT          (_GPIO_LESENSE_ROUTEEN_CH10OUTPEN_DEFAULT << 10) /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH11OUTPEN                  (0x1UL << 11)                                    /**< CH11OUT pin enable control bit              */
#define _GPIO_LESENSE_ROUTEEN_CH11OUTPEN_SHIFT           11                                               /**< Shift value for GPIO_CH11OUTPEN             */
#define _GPIO_LESENSE_ROUTEEN_CH11OUTPEN_MASK            0x800UL                                          /**< Bit mask for GPIO_CH11OUTPEN                */
#define _GPIO_LESENSE_ROUTEEN_CH11OUTPEN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH11OUTPEN_DEFAULT          (_GPIO_LESENSE_ROUTEEN_CH11OUTPEN_DEFAULT << 11) /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH12OUTPEN                  (0x1UL << 12)                                    /**< CH12OUT pin enable control bit              */
#define _GPIO_LESENSE_ROUTEEN_CH12OUTPEN_SHIFT           12                                               /**< Shift value for GPIO_CH12OUTPEN             */
#define _GPIO_LESENSE_ROUTEEN_CH12OUTPEN_MASK            0x1000UL                                         /**< Bit mask for GPIO_CH12OUTPEN                */
#define _GPIO_LESENSE_ROUTEEN_CH12OUTPEN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH12OUTPEN_DEFAULT          (_GPIO_LESENSE_ROUTEEN_CH12OUTPEN_DEFAULT << 12) /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH13OUTPEN                  (0x1UL << 13)                                    /**< CH13OUT pin enable control bit              */
#define _GPIO_LESENSE_ROUTEEN_CH13OUTPEN_SHIFT           13                                               /**< Shift value for GPIO_CH13OUTPEN             */
#define _GPIO_LESENSE_ROUTEEN_CH13OUTPEN_MASK            0x2000UL                                         /**< Bit mask for GPIO_CH13OUTPEN                */
#define _GPIO_LESENSE_ROUTEEN_CH13OUTPEN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH13OUTPEN_DEFAULT          (_GPIO_LESENSE_ROUTEEN_CH13OUTPEN_DEFAULT << 13) /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH14OUTPEN                  (0x1UL << 14)                                    /**< CH14OUT pin enable control bit              */
#define _GPIO_LESENSE_ROUTEEN_CH14OUTPEN_SHIFT           14                                               /**< Shift value for GPIO_CH14OUTPEN             */
#define _GPIO_LESENSE_ROUTEEN_CH14OUTPEN_MASK            0x4000UL                                         /**< Bit mask for GPIO_CH14OUTPEN                */
#define _GPIO_LESENSE_ROUTEEN_CH14OUTPEN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH14OUTPEN_DEFAULT          (_GPIO_LESENSE_ROUTEEN_CH14OUTPEN_DEFAULT << 14) /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/
#define GPIO_LESENSE_ROUTEEN_CH15OUTPEN                  (0x1UL << 15)                                    /**< CH15OUT pin enable control bit              */
#define _GPIO_LESENSE_ROUTEEN_CH15OUTPEN_SHIFT           15                                               /**< Shift value for GPIO_CH15OUTPEN             */
#define _GPIO_LESENSE_ROUTEEN_CH15OUTPEN_MASK            0x8000UL                                         /**< Bit mask for GPIO_CH15OUTPEN                */
#define _GPIO_LESENSE_ROUTEEN_CH15OUTPEN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_LESENSE_ROUTEEN       */
#define GPIO_LESENSE_ROUTEEN_CH15OUTPEN_DEFAULT          (_GPIO_LESENSE_ROUTEEN_CH15OUTPEN_DEFAULT << 15) /**< Shifted mode DEFAULT for GPIO_LESENSE_ROUTEEN*/

/* Bit fields for GPIO_LESENSE CH0OUTROUTE */
#define _GPIO_LESENSE_CH0OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH0OUTROUTE  */
#define _GPIO_LESENSE_CH0OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH0OUTROUTE           */
#define _GPIO_LESENSE_CH0OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH0OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH0OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH0OUTROUTE   */
#define GPIO_LESENSE_CH0OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH0OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH0OUTROUTE*/
#define _GPIO_LESENSE_CH0OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH0OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH0OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH0OUTROUTE   */
#define GPIO_LESENSE_CH0OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH0OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH0OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH1OUTROUTE */
#define _GPIO_LESENSE_CH1OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH1OUTROUTE  */
#define _GPIO_LESENSE_CH1OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH1OUTROUTE           */
#define _GPIO_LESENSE_CH1OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH1OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH1OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH1OUTROUTE   */
#define GPIO_LESENSE_CH1OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH1OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH1OUTROUTE*/
#define _GPIO_LESENSE_CH1OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH1OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH1OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH1OUTROUTE   */
#define GPIO_LESENSE_CH1OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH1OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH1OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH2OUTROUTE */
#define _GPIO_LESENSE_CH2OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH2OUTROUTE  */
#define _GPIO_LESENSE_CH2OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH2OUTROUTE           */
#define _GPIO_LESENSE_CH2OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH2OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH2OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH2OUTROUTE   */
#define GPIO_LESENSE_CH2OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH2OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH2OUTROUTE*/
#define _GPIO_LESENSE_CH2OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH2OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH2OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH2OUTROUTE   */
#define GPIO_LESENSE_CH2OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH2OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH2OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH3OUTROUTE */
#define _GPIO_LESENSE_CH3OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH3OUTROUTE  */
#define _GPIO_LESENSE_CH3OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH3OUTROUTE           */
#define _GPIO_LESENSE_CH3OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH3OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH3OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH3OUTROUTE   */
#define GPIO_LESENSE_CH3OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH3OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH3OUTROUTE*/
#define _GPIO_LESENSE_CH3OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH3OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH3OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH3OUTROUTE   */
#define GPIO_LESENSE_CH3OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH3OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH3OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH4OUTROUTE */
#define _GPIO_LESENSE_CH4OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH4OUTROUTE  */
#define _GPIO_LESENSE_CH4OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH4OUTROUTE           */
#define _GPIO_LESENSE_CH4OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH4OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH4OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH4OUTROUTE   */
#define GPIO_LESENSE_CH4OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH4OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH4OUTROUTE*/
#define _GPIO_LESENSE_CH4OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH4OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH4OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH4OUTROUTE   */
#define GPIO_LESENSE_CH4OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH4OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH4OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH5OUTROUTE */
#define _GPIO_LESENSE_CH5OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH5OUTROUTE  */
#define _GPIO_LESENSE_CH5OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH5OUTROUTE           */
#define _GPIO_LESENSE_CH5OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH5OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH5OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH5OUTROUTE   */
#define GPIO_LESENSE_CH5OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH5OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH5OUTROUTE*/
#define _GPIO_LESENSE_CH5OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH5OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH5OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH5OUTROUTE   */
#define GPIO_LESENSE_CH5OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH5OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH5OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH6OUTROUTE */
#define _GPIO_LESENSE_CH6OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH6OUTROUTE  */
#define _GPIO_LESENSE_CH6OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH6OUTROUTE           */
#define _GPIO_LESENSE_CH6OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH6OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH6OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH6OUTROUTE   */
#define GPIO_LESENSE_CH6OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH6OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH6OUTROUTE*/
#define _GPIO_LESENSE_CH6OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH6OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH6OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH6OUTROUTE   */
#define GPIO_LESENSE_CH6OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH6OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH6OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH7OUTROUTE */
#define _GPIO_LESENSE_CH7OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH7OUTROUTE  */
#define _GPIO_LESENSE_CH7OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH7OUTROUTE           */
#define _GPIO_LESENSE_CH7OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH7OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH7OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH7OUTROUTE   */
#define GPIO_LESENSE_CH7OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH7OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH7OUTROUTE*/
#define _GPIO_LESENSE_CH7OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH7OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH7OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH7OUTROUTE   */
#define GPIO_LESENSE_CH7OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH7OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH7OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH8OUTROUTE */
#define _GPIO_LESENSE_CH8OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH8OUTROUTE  */
#define _GPIO_LESENSE_CH8OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH8OUTROUTE           */
#define _GPIO_LESENSE_CH8OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH8OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH8OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH8OUTROUTE   */
#define GPIO_LESENSE_CH8OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH8OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH8OUTROUTE*/
#define _GPIO_LESENSE_CH8OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH8OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH8OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH8OUTROUTE   */
#define GPIO_LESENSE_CH8OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH8OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH8OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH9OUTROUTE */
#define _GPIO_LESENSE_CH9OUTROUTE_RESETVALUE             0x00000000UL                                  /**< Default value for GPIO_LESENSE_CH9OUTROUTE  */
#define _GPIO_LESENSE_CH9OUTROUTE_MASK                   0x000F0003UL                                  /**< Mask for GPIO_LESENSE_CH9OUTROUTE           */
#define _GPIO_LESENSE_CH9OUTROUTE_PORT_SHIFT             0                                             /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH9OUTROUTE_PORT_MASK              0x3UL                                         /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH9OUTROUTE_PORT_DEFAULT           0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH9OUTROUTE   */
#define GPIO_LESENSE_CH9OUTROUTE_PORT_DEFAULT            (_GPIO_LESENSE_CH9OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH9OUTROUTE*/
#define _GPIO_LESENSE_CH9OUTROUTE_PIN_SHIFT              16                                            /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH9OUTROUTE_PIN_MASK               0xF0000UL                                     /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH9OUTROUTE_PIN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_LESENSE_CH9OUTROUTE   */
#define GPIO_LESENSE_CH9OUTROUTE_PIN_DEFAULT             (_GPIO_LESENSE_CH9OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH9OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH10OUTROUTE */
#define _GPIO_LESENSE_CH10OUTROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_LESENSE_CH10OUTROUTE */
#define _GPIO_LESENSE_CH10OUTROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_LESENSE_CH10OUTROUTE          */
#define _GPIO_LESENSE_CH10OUTROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH10OUTROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH10OUTROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH10OUTROUTE  */
#define GPIO_LESENSE_CH10OUTROUTE_PORT_DEFAULT           (_GPIO_LESENSE_CH10OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH10OUTROUTE*/
#define _GPIO_LESENSE_CH10OUTROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH10OUTROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH10OUTROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH10OUTROUTE  */
#define GPIO_LESENSE_CH10OUTROUTE_PIN_DEFAULT            (_GPIO_LESENSE_CH10OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH10OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH11OUTROUTE */
#define _GPIO_LESENSE_CH11OUTROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_LESENSE_CH11OUTROUTE */
#define _GPIO_LESENSE_CH11OUTROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_LESENSE_CH11OUTROUTE          */
#define _GPIO_LESENSE_CH11OUTROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH11OUTROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH11OUTROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH11OUTROUTE  */
#define GPIO_LESENSE_CH11OUTROUTE_PORT_DEFAULT           (_GPIO_LESENSE_CH11OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH11OUTROUTE*/
#define _GPIO_LESENSE_CH11OUTROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH11OUTROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH11OUTROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH11OUTROUTE  */
#define GPIO_LESENSE_CH11OUTROUTE_PIN_DEFAULT            (_GPIO_LESENSE_CH11OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH11OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH12OUTROUTE */
#define _GPIO_LESENSE_CH12OUTROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_LESENSE_CH12OUTROUTE */
#define _GPIO_LESENSE_CH12OUTROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_LESENSE_CH12OUTROUTE          */
#define _GPIO_LESENSE_CH12OUTROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH12OUTROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH12OUTROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH12OUTROUTE  */
#define GPIO_LESENSE_CH12OUTROUTE_PORT_DEFAULT           (_GPIO_LESENSE_CH12OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH12OUTROUTE*/
#define _GPIO_LESENSE_CH12OUTROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH12OUTROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH12OUTROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH12OUTROUTE  */
#define GPIO_LESENSE_CH12OUTROUTE_PIN_DEFAULT            (_GPIO_LESENSE_CH12OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH12OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH13OUTROUTE */
#define _GPIO_LESENSE_CH13OUTROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_LESENSE_CH13OUTROUTE */
#define _GPIO_LESENSE_CH13OUTROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_LESENSE_CH13OUTROUTE          */
#define _GPIO_LESENSE_CH13OUTROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH13OUTROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH13OUTROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH13OUTROUTE  */
#define GPIO_LESENSE_CH13OUTROUTE_PORT_DEFAULT           (_GPIO_LESENSE_CH13OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH13OUTROUTE*/
#define _GPIO_LESENSE_CH13OUTROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH13OUTROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH13OUTROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH13OUTROUTE  */
#define GPIO_LESENSE_CH13OUTROUTE_PIN_DEFAULT            (_GPIO_LESENSE_CH13OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH13OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH14OUTROUTE */
#define _GPIO_LESENSE_CH14OUTROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_LESENSE_CH14OUTROUTE */
#define _GPIO_LESENSE_CH14OUTROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_LESENSE_CH14OUTROUTE          */
#define _GPIO_LESENSE_CH14OUTROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH14OUTROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH14OUTROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH14OUTROUTE  */
#define GPIO_LESENSE_CH14OUTROUTE_PORT_DEFAULT           (_GPIO_LESENSE_CH14OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH14OUTROUTE*/
#define _GPIO_LESENSE_CH14OUTROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH14OUTROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH14OUTROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH14OUTROUTE  */
#define GPIO_LESENSE_CH14OUTROUTE_PIN_DEFAULT            (_GPIO_LESENSE_CH14OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH14OUTROUTE*/

/* Bit fields for GPIO_LESENSE CH15OUTROUTE */
#define _GPIO_LESENSE_CH15OUTROUTE_RESETVALUE            0x00000000UL                                   /**< Default value for GPIO_LESENSE_CH15OUTROUTE */
#define _GPIO_LESENSE_CH15OUTROUTE_MASK                  0x000F0003UL                                   /**< Mask for GPIO_LESENSE_CH15OUTROUTE          */
#define _GPIO_LESENSE_CH15OUTROUTE_PORT_SHIFT            0                                              /**< Shift value for GPIO_PORT                   */
#define _GPIO_LESENSE_CH15OUTROUTE_PORT_MASK             0x3UL                                          /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LESENSE_CH15OUTROUTE_PORT_DEFAULT          0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH15OUTROUTE  */
#define GPIO_LESENSE_CH15OUTROUTE_PORT_DEFAULT           (_GPIO_LESENSE_CH15OUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH15OUTROUTE*/
#define _GPIO_LESENSE_CH15OUTROUTE_PIN_SHIFT             16                                             /**< Shift value for GPIO_PIN                    */
#define _GPIO_LESENSE_CH15OUTROUTE_PIN_MASK              0xF0000UL                                      /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LESENSE_CH15OUTROUTE_PIN_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for GPIO_LESENSE_CH15OUTROUTE  */
#define GPIO_LESENSE_CH15OUTROUTE_PIN_DEFAULT            (_GPIO_LESENSE_CH15OUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LESENSE_CH15OUTROUTE*/

/* Bit fields for GPIO_LETIMER ROUTEEN */
#define _GPIO_LETIMER_ROUTEEN_RESETVALUE                 0x00000000UL                                 /**< Default value for GPIO_LETIMER_ROUTEEN      */
#define _GPIO_LETIMER_ROUTEEN_MASK                       0x00000003UL                                 /**< Mask for GPIO_LETIMER_ROUTEEN               */
#define GPIO_LETIMER_ROUTEEN_OUT0PEN                     (0x1UL << 0)                                 /**< OUT0 pin enable control bit                 */
#define _GPIO_LETIMER_ROUTEEN_OUT0PEN_SHIFT              0                                            /**< Shift value for GPIO_OUT0PEN                */
#define _GPIO_LETIMER_ROUTEEN_OUT0PEN_MASK               0x1UL                                        /**< Bit mask for GPIO_OUT0PEN                   */
#define _GPIO_LETIMER_ROUTEEN_OUT0PEN_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for GPIO_LETIMER_ROUTEEN       */
#define GPIO_LETIMER_ROUTEEN_OUT0PEN_DEFAULT             (_GPIO_LETIMER_ROUTEEN_OUT0PEN_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LETIMER_ROUTEEN*/
#define GPIO_LETIMER_ROUTEEN_OUT1PEN                     (0x1UL << 1)                                 /**< OUT1 pin enable control bit                 */
#define _GPIO_LETIMER_ROUTEEN_OUT1PEN_SHIFT              1                                            /**< Shift value for GPIO_OUT1PEN                */
#define _GPIO_LETIMER_ROUTEEN_OUT1PEN_MASK               0x2UL                                        /**< Bit mask for GPIO_OUT1PEN                   */
#define _GPIO_LETIMER_ROUTEEN_OUT1PEN_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for GPIO_LETIMER_ROUTEEN       */
#define GPIO_LETIMER_ROUTEEN_OUT1PEN_DEFAULT             (_GPIO_LETIMER_ROUTEEN_OUT1PEN_DEFAULT << 1) /**< Shifted mode DEFAULT for GPIO_LETIMER_ROUTEEN*/

/* Bit fields for GPIO_LETIMER OUT0ROUTE */
#define _GPIO_LETIMER_OUT0ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_LETIMER_OUT0ROUTE    */
#define _GPIO_LETIMER_OUT0ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_LETIMER_OUT0ROUTE             */
#define _GPIO_LETIMER_OUT0ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_LETIMER_OUT0ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LETIMER_OUT0ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_LETIMER_OUT0ROUTE     */
#define GPIO_LETIMER_OUT0ROUTE_PORT_DEFAULT              (_GPIO_LETIMER_OUT0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LETIMER_OUT0ROUTE*/
#define _GPIO_LETIMER_OUT0ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_LETIMER_OUT0ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LETIMER_OUT0ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_LETIMER_OUT0ROUTE     */
#define GPIO_LETIMER_OUT0ROUTE_PIN_DEFAULT               (_GPIO_LETIMER_OUT0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LETIMER_OUT0ROUTE*/

/* Bit fields for GPIO_LETIMER OUT1ROUTE */
#define _GPIO_LETIMER_OUT1ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_LETIMER_OUT1ROUTE    */
#define _GPIO_LETIMER_OUT1ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_LETIMER_OUT1ROUTE             */
#define _GPIO_LETIMER_OUT1ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_LETIMER_OUT1ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_LETIMER_OUT1ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_LETIMER_OUT1ROUTE     */
#define GPIO_LETIMER_OUT1ROUTE_PORT_DEFAULT              (_GPIO_LETIMER_OUT1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_LETIMER_OUT1ROUTE*/
#define _GPIO_LETIMER_OUT1ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_LETIMER_OUT1ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_LETIMER_OUT1ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_LETIMER_OUT1ROUTE     */
#define GPIO_LETIMER_OUT1ROUTE_PIN_DEFAULT               (_GPIO_LETIMER_OUT1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_LETIMER_OUT1ROUTE*/

/* Bit fields for GPIO_MODEM ROUTEEN */
#define _GPIO_MODEM_ROUTEEN_RESETVALUE                   0x00000000UL                                       /**< Default value for GPIO_MODEM_ROUTEEN        */
#define _GPIO_MODEM_ROUTEEN_MASK                         0x00007FFFUL                                       /**< Mask for GPIO_MODEM_ROUTEEN                 */
#define GPIO_MODEM_ROUTEEN_ANT0PEN                       (0x1UL << 0)                                       /**< ANT0 pin enable control bit                 */
#define _GPIO_MODEM_ROUTEEN_ANT0PEN_SHIFT                0                                                  /**< Shift value for GPIO_ANT0PEN                */
#define _GPIO_MODEM_ROUTEEN_ANT0PEN_MASK                 0x1UL                                              /**< Bit mask for GPIO_ANT0PEN                   */
#define _GPIO_MODEM_ROUTEEN_ANT0PEN_DEFAULT              0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANT0PEN_DEFAULT               (_GPIO_MODEM_ROUTEEN_ANT0PEN_DEFAULT << 0)         /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANT1PEN                       (0x1UL << 1)                                       /**< ANT1 pin enable control bit                 */
#define _GPIO_MODEM_ROUTEEN_ANT1PEN_SHIFT                1                                                  /**< Shift value for GPIO_ANT1PEN                */
#define _GPIO_MODEM_ROUTEEN_ANT1PEN_MASK                 0x2UL                                              /**< Bit mask for GPIO_ANT1PEN                   */
#define _GPIO_MODEM_ROUTEEN_ANT1PEN_DEFAULT              0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANT1PEN_DEFAULT               (_GPIO_MODEM_ROUTEEN_ANT1PEN_DEFAULT << 1)         /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTROLLOVERPEN                (0x1UL << 2)                                       /**< ANTROLLOVER pin enable control bit          */
#define _GPIO_MODEM_ROUTEEN_ANTROLLOVERPEN_SHIFT         2                                                  /**< Shift value for GPIO_ANTROLLOVERPEN         */
#define _GPIO_MODEM_ROUTEEN_ANTROLLOVERPEN_MASK          0x4UL                                              /**< Bit mask for GPIO_ANTROLLOVERPEN            */
#define _GPIO_MODEM_ROUTEEN_ANTROLLOVERPEN_DEFAULT       0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTROLLOVERPEN_DEFAULT        (_GPIO_MODEM_ROUTEEN_ANTROLLOVERPEN_DEFAULT << 2)  /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTRR0PEN                     (0x1UL << 3)                                       /**< ANTRR0 pin enable control bit               */
#define _GPIO_MODEM_ROUTEEN_ANTRR0PEN_SHIFT              3                                                  /**< Shift value for GPIO_ANTRR0PEN              */
#define _GPIO_MODEM_ROUTEEN_ANTRR0PEN_MASK               0x8UL                                              /**< Bit mask for GPIO_ANTRR0PEN                 */
#define _GPIO_MODEM_ROUTEEN_ANTRR0PEN_DEFAULT            0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTRR0PEN_DEFAULT             (_GPIO_MODEM_ROUTEEN_ANTRR0PEN_DEFAULT << 3)       /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTRR1PEN                     (0x1UL << 4)                                       /**< ANTRR1 pin enable control bit               */
#define _GPIO_MODEM_ROUTEEN_ANTRR1PEN_SHIFT              4                                                  /**< Shift value for GPIO_ANTRR1PEN              */
#define _GPIO_MODEM_ROUTEEN_ANTRR1PEN_MASK               0x10UL                                             /**< Bit mask for GPIO_ANTRR1PEN                 */
#define _GPIO_MODEM_ROUTEEN_ANTRR1PEN_DEFAULT            0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTRR1PEN_DEFAULT             (_GPIO_MODEM_ROUTEEN_ANTRR1PEN_DEFAULT << 4)       /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTRR2PEN                     (0x1UL << 5)                                       /**< ANTRR2 pin enable control bit               */
#define _GPIO_MODEM_ROUTEEN_ANTRR2PEN_SHIFT              5                                                  /**< Shift value for GPIO_ANTRR2PEN              */
#define _GPIO_MODEM_ROUTEEN_ANTRR2PEN_MASK               0x20UL                                             /**< Bit mask for GPIO_ANTRR2PEN                 */
#define _GPIO_MODEM_ROUTEEN_ANTRR2PEN_DEFAULT            0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTRR2PEN_DEFAULT             (_GPIO_MODEM_ROUTEEN_ANTRR2PEN_DEFAULT << 5)       /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTRR3PEN                     (0x1UL << 6)                                       /**< ANTRR3 pin enable control bit               */
#define _GPIO_MODEM_ROUTEEN_ANTRR3PEN_SHIFT              6                                                  /**< Shift value for GPIO_ANTRR3PEN              */
#define _GPIO_MODEM_ROUTEEN_ANTRR3PEN_MASK               0x40UL                                             /**< Bit mask for GPIO_ANTRR3PEN                 */
#define _GPIO_MODEM_ROUTEEN_ANTRR3PEN_DEFAULT            0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTRR3PEN_DEFAULT             (_GPIO_MODEM_ROUTEEN_ANTRR3PEN_DEFAULT << 6)       /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTRR4PEN                     (0x1UL << 7)                                       /**< ANTRR4 pin enable control bit               */
#define _GPIO_MODEM_ROUTEEN_ANTRR4PEN_SHIFT              7                                                  /**< Shift value for GPIO_ANTRR4PEN              */
#define _GPIO_MODEM_ROUTEEN_ANTRR4PEN_MASK               0x80UL                                             /**< Bit mask for GPIO_ANTRR4PEN                 */
#define _GPIO_MODEM_ROUTEEN_ANTRR4PEN_DEFAULT            0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTRR4PEN_DEFAULT             (_GPIO_MODEM_ROUTEEN_ANTRR4PEN_DEFAULT << 7)       /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTRR5PEN                     (0x1UL << 8)                                       /**< ANTRR5 pin enable control bit               */
#define _GPIO_MODEM_ROUTEEN_ANTRR5PEN_SHIFT              8                                                  /**< Shift value for GPIO_ANTRR5PEN              */
#define _GPIO_MODEM_ROUTEEN_ANTRR5PEN_MASK               0x100UL                                            /**< Bit mask for GPIO_ANTRR5PEN                 */
#define _GPIO_MODEM_ROUTEEN_ANTRR5PEN_DEFAULT            0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTRR5PEN_DEFAULT             (_GPIO_MODEM_ROUTEEN_ANTRR5PEN_DEFAULT << 8)       /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTSWENPEN                    (0x1UL << 9)                                       /**< ANTSWEN pin enable control bit              */
#define _GPIO_MODEM_ROUTEEN_ANTSWENPEN_SHIFT             9                                                  /**< Shift value for GPIO_ANTSWENPEN             */
#define _GPIO_MODEM_ROUTEEN_ANTSWENPEN_MASK              0x200UL                                            /**< Bit mask for GPIO_ANTSWENPEN                */
#define _GPIO_MODEM_ROUTEEN_ANTSWENPEN_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTSWENPEN_DEFAULT            (_GPIO_MODEM_ROUTEEN_ANTSWENPEN_DEFAULT << 9)      /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTSWUSPEN                    (0x1UL << 10)                                      /**< ANTSWUS pin enable control bit              */
#define _GPIO_MODEM_ROUTEEN_ANTSWUSPEN_SHIFT             10                                                 /**< Shift value for GPIO_ANTSWUSPEN             */
#define _GPIO_MODEM_ROUTEEN_ANTSWUSPEN_MASK              0x400UL                                            /**< Bit mask for GPIO_ANTSWUSPEN                */
#define _GPIO_MODEM_ROUTEEN_ANTSWUSPEN_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTSWUSPEN_DEFAULT            (_GPIO_MODEM_ROUTEEN_ANTSWUSPEN_DEFAULT << 10)     /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTTRIGPEN                    (0x1UL << 11)                                      /**< ANTTRIG pin enable control bit              */
#define _GPIO_MODEM_ROUTEEN_ANTTRIGPEN_SHIFT             11                                                 /**< Shift value for GPIO_ANTTRIGPEN             */
#define _GPIO_MODEM_ROUTEEN_ANTTRIGPEN_MASK              0x800UL                                            /**< Bit mask for GPIO_ANTTRIGPEN                */
#define _GPIO_MODEM_ROUTEEN_ANTTRIGPEN_DEFAULT           0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTTRIGPEN_DEFAULT            (_GPIO_MODEM_ROUTEEN_ANTTRIGPEN_DEFAULT << 11)     /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_ANTTRIGSTOPPEN                (0x1UL << 12)                                      /**< ANTTRIGSTOP pin enable control bit          */
#define _GPIO_MODEM_ROUTEEN_ANTTRIGSTOPPEN_SHIFT         12                                                 /**< Shift value for GPIO_ANTTRIGSTOPPEN         */
#define _GPIO_MODEM_ROUTEEN_ANTTRIGSTOPPEN_MASK          0x1000UL                                           /**< Bit mask for GPIO_ANTTRIGSTOPPEN            */
#define _GPIO_MODEM_ROUTEEN_ANTTRIGSTOPPEN_DEFAULT       0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_ANTTRIGSTOPPEN_DEFAULT        (_GPIO_MODEM_ROUTEEN_ANTTRIGSTOPPEN_DEFAULT << 12) /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_DCLKPEN                       (0x1UL << 13)                                      /**< DCLK pin enable control bit                 */
#define _GPIO_MODEM_ROUTEEN_DCLKPEN_SHIFT                13                                                 /**< Shift value for GPIO_DCLKPEN                */
#define _GPIO_MODEM_ROUTEEN_DCLKPEN_MASK                 0x2000UL                                           /**< Bit mask for GPIO_DCLKPEN                   */
#define _GPIO_MODEM_ROUTEEN_DCLKPEN_DEFAULT              0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_DCLKPEN_DEFAULT               (_GPIO_MODEM_ROUTEEN_DCLKPEN_DEFAULT << 13)        /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */
#define GPIO_MODEM_ROUTEEN_DOUTPEN                       (0x1UL << 14)                                      /**< DOUT pin enable control bit                 */
#define _GPIO_MODEM_ROUTEEN_DOUTPEN_SHIFT                14                                                 /**< Shift value for GPIO_DOUTPEN                */
#define _GPIO_MODEM_ROUTEEN_DOUTPEN_MASK                 0x4000UL                                           /**< Bit mask for GPIO_DOUTPEN                   */
#define _GPIO_MODEM_ROUTEEN_DOUTPEN_DEFAULT              0x00000000UL                                       /**< Mode DEFAULT for GPIO_MODEM_ROUTEEN         */
#define GPIO_MODEM_ROUTEEN_DOUTPEN_DEFAULT               (_GPIO_MODEM_ROUTEEN_DOUTPEN_DEFAULT << 14)        /**< Shifted mode DEFAULT for GPIO_MODEM_ROUTEEN */

/* Bit fields for GPIO_MODEM ANT0ROUTE */
#define _GPIO_MODEM_ANT0ROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_MODEM_ANT0ROUTE      */
#define _GPIO_MODEM_ANT0ROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_MODEM_ANT0ROUTE               */
#define _GPIO_MODEM_ANT0ROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANT0ROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANT0ROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_ANT0ROUTE       */
#define GPIO_MODEM_ANT0ROUTE_PORT_DEFAULT                (_GPIO_MODEM_ANT0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANT0ROUTE*/
#define _GPIO_MODEM_ANT0ROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANT0ROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANT0ROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_ANT0ROUTE       */
#define GPIO_MODEM_ANT0ROUTE_PIN_DEFAULT                 (_GPIO_MODEM_ANT0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANT0ROUTE*/

/* Bit fields for GPIO_MODEM ANT1ROUTE */
#define _GPIO_MODEM_ANT1ROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_MODEM_ANT1ROUTE      */
#define _GPIO_MODEM_ANT1ROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_MODEM_ANT1ROUTE               */
#define _GPIO_MODEM_ANT1ROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANT1ROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANT1ROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_ANT1ROUTE       */
#define GPIO_MODEM_ANT1ROUTE_PORT_DEFAULT                (_GPIO_MODEM_ANT1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANT1ROUTE*/
#define _GPIO_MODEM_ANT1ROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANT1ROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANT1ROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_ANT1ROUTE       */
#define GPIO_MODEM_ANT1ROUTE_PIN_DEFAULT                 (_GPIO_MODEM_ANT1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANT1ROUTE*/

/* Bit fields for GPIO_MODEM ANTROLLOVERROUTE */
#define _GPIO_MODEM_ANTROLLOVERROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_MODEM_ANTROLLOVERROUTE*/
#define _GPIO_MODEM_ANTROLLOVERROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_MODEM_ANTROLLOVERROUTE        */
#define _GPIO_MODEM_ANTROLLOVERROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTROLLOVERROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTROLLOVERROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_MODEM_ANTROLLOVERROUTE*/
#define GPIO_MODEM_ANTROLLOVERROUTE_PORT_DEFAULT         (_GPIO_MODEM_ANTROLLOVERROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTROLLOVERROUTE*/
#define _GPIO_MODEM_ANTROLLOVERROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTROLLOVERROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTROLLOVERROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_MODEM_ANTROLLOVERROUTE*/
#define GPIO_MODEM_ANTROLLOVERROUTE_PIN_DEFAULT          (_GPIO_MODEM_ANTROLLOVERROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTROLLOVERROUTE*/

/* Bit fields for GPIO_MODEM ANTRR0ROUTE */
#define _GPIO_MODEM_ANTRR0ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_MODEM_ANTRR0ROUTE    */
#define _GPIO_MODEM_ANTRR0ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_MODEM_ANTRR0ROUTE             */
#define _GPIO_MODEM_ANTRR0ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTRR0ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTRR0ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR0ROUTE     */
#define GPIO_MODEM_ANTRR0ROUTE_PORT_DEFAULT              (_GPIO_MODEM_ANTRR0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR0ROUTE*/
#define _GPIO_MODEM_ANTRR0ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTRR0ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTRR0ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR0ROUTE     */
#define GPIO_MODEM_ANTRR0ROUTE_PIN_DEFAULT               (_GPIO_MODEM_ANTRR0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR0ROUTE*/

/* Bit fields for GPIO_MODEM ANTRR1ROUTE */
#define _GPIO_MODEM_ANTRR1ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_MODEM_ANTRR1ROUTE    */
#define _GPIO_MODEM_ANTRR1ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_MODEM_ANTRR1ROUTE             */
#define _GPIO_MODEM_ANTRR1ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTRR1ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTRR1ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR1ROUTE     */
#define GPIO_MODEM_ANTRR1ROUTE_PORT_DEFAULT              (_GPIO_MODEM_ANTRR1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR1ROUTE*/
#define _GPIO_MODEM_ANTRR1ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTRR1ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTRR1ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR1ROUTE     */
#define GPIO_MODEM_ANTRR1ROUTE_PIN_DEFAULT               (_GPIO_MODEM_ANTRR1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR1ROUTE*/

/* Bit fields for GPIO_MODEM ANTRR2ROUTE */
#define _GPIO_MODEM_ANTRR2ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_MODEM_ANTRR2ROUTE    */
#define _GPIO_MODEM_ANTRR2ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_MODEM_ANTRR2ROUTE             */
#define _GPIO_MODEM_ANTRR2ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTRR2ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTRR2ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR2ROUTE     */
#define GPIO_MODEM_ANTRR2ROUTE_PORT_DEFAULT              (_GPIO_MODEM_ANTRR2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR2ROUTE*/
#define _GPIO_MODEM_ANTRR2ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTRR2ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTRR2ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR2ROUTE     */
#define GPIO_MODEM_ANTRR2ROUTE_PIN_DEFAULT               (_GPIO_MODEM_ANTRR2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR2ROUTE*/

/* Bit fields for GPIO_MODEM ANTRR3ROUTE */
#define _GPIO_MODEM_ANTRR3ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_MODEM_ANTRR3ROUTE    */
#define _GPIO_MODEM_ANTRR3ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_MODEM_ANTRR3ROUTE             */
#define _GPIO_MODEM_ANTRR3ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTRR3ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTRR3ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR3ROUTE     */
#define GPIO_MODEM_ANTRR3ROUTE_PORT_DEFAULT              (_GPIO_MODEM_ANTRR3ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR3ROUTE*/
#define _GPIO_MODEM_ANTRR3ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTRR3ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTRR3ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR3ROUTE     */
#define GPIO_MODEM_ANTRR3ROUTE_PIN_DEFAULT               (_GPIO_MODEM_ANTRR3ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR3ROUTE*/

/* Bit fields for GPIO_MODEM ANTRR4ROUTE */
#define _GPIO_MODEM_ANTRR4ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_MODEM_ANTRR4ROUTE    */
#define _GPIO_MODEM_ANTRR4ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_MODEM_ANTRR4ROUTE             */
#define _GPIO_MODEM_ANTRR4ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTRR4ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTRR4ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR4ROUTE     */
#define GPIO_MODEM_ANTRR4ROUTE_PORT_DEFAULT              (_GPIO_MODEM_ANTRR4ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR4ROUTE*/
#define _GPIO_MODEM_ANTRR4ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTRR4ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTRR4ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR4ROUTE     */
#define GPIO_MODEM_ANTRR4ROUTE_PIN_DEFAULT               (_GPIO_MODEM_ANTRR4ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR4ROUTE*/

/* Bit fields for GPIO_MODEM ANTRR5ROUTE */
#define _GPIO_MODEM_ANTRR5ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_MODEM_ANTRR5ROUTE    */
#define _GPIO_MODEM_ANTRR5ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_MODEM_ANTRR5ROUTE             */
#define _GPIO_MODEM_ANTRR5ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTRR5ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTRR5ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR5ROUTE     */
#define GPIO_MODEM_ANTRR5ROUTE_PORT_DEFAULT              (_GPIO_MODEM_ANTRR5ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR5ROUTE*/
#define _GPIO_MODEM_ANTRR5ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTRR5ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTRR5ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_MODEM_ANTRR5ROUTE     */
#define GPIO_MODEM_ANTRR5ROUTE_PIN_DEFAULT               (_GPIO_MODEM_ANTRR5ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTRR5ROUTE*/

/* Bit fields for GPIO_MODEM ANTSWENROUTE */
#define _GPIO_MODEM_ANTSWENROUTE_RESETVALUE              0x00000000UL                                 /**< Default value for GPIO_MODEM_ANTSWENROUTE   */
#define _GPIO_MODEM_ANTSWENROUTE_MASK                    0x000F0003UL                                 /**< Mask for GPIO_MODEM_ANTSWENROUTE            */
#define _GPIO_MODEM_ANTSWENROUTE_PORT_SHIFT              0                                            /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTSWENROUTE_PORT_MASK               0x3UL                                        /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTSWENROUTE_PORT_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for GPIO_MODEM_ANTSWENROUTE    */
#define GPIO_MODEM_ANTSWENROUTE_PORT_DEFAULT             (_GPIO_MODEM_ANTSWENROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTSWENROUTE*/
#define _GPIO_MODEM_ANTSWENROUTE_PIN_SHIFT               16                                           /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTSWENROUTE_PIN_MASK                0xF0000UL                                    /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTSWENROUTE_PIN_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for GPIO_MODEM_ANTSWENROUTE    */
#define GPIO_MODEM_ANTSWENROUTE_PIN_DEFAULT              (_GPIO_MODEM_ANTSWENROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTSWENROUTE*/

/* Bit fields for GPIO_MODEM ANTSWUSROUTE */
#define _GPIO_MODEM_ANTSWUSROUTE_RESETVALUE              0x00000000UL                                 /**< Default value for GPIO_MODEM_ANTSWUSROUTE   */
#define _GPIO_MODEM_ANTSWUSROUTE_MASK                    0x000F0003UL                                 /**< Mask for GPIO_MODEM_ANTSWUSROUTE            */
#define _GPIO_MODEM_ANTSWUSROUTE_PORT_SHIFT              0                                            /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTSWUSROUTE_PORT_MASK               0x3UL                                        /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTSWUSROUTE_PORT_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for GPIO_MODEM_ANTSWUSROUTE    */
#define GPIO_MODEM_ANTSWUSROUTE_PORT_DEFAULT             (_GPIO_MODEM_ANTSWUSROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTSWUSROUTE*/
#define _GPIO_MODEM_ANTSWUSROUTE_PIN_SHIFT               16                                           /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTSWUSROUTE_PIN_MASK                0xF0000UL                                    /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTSWUSROUTE_PIN_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for GPIO_MODEM_ANTSWUSROUTE    */
#define GPIO_MODEM_ANTSWUSROUTE_PIN_DEFAULT              (_GPIO_MODEM_ANTSWUSROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTSWUSROUTE*/

/* Bit fields for GPIO_MODEM ANTTRIGROUTE */
#define _GPIO_MODEM_ANTTRIGROUTE_RESETVALUE              0x00000000UL                                 /**< Default value for GPIO_MODEM_ANTTRIGROUTE   */
#define _GPIO_MODEM_ANTTRIGROUTE_MASK                    0x000F0003UL                                 /**< Mask for GPIO_MODEM_ANTTRIGROUTE            */
#define _GPIO_MODEM_ANTTRIGROUTE_PORT_SHIFT              0                                            /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTTRIGROUTE_PORT_MASK               0x3UL                                        /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTTRIGROUTE_PORT_DEFAULT            0x00000000UL                                 /**< Mode DEFAULT for GPIO_MODEM_ANTTRIGROUTE    */
#define GPIO_MODEM_ANTTRIGROUTE_PORT_DEFAULT             (_GPIO_MODEM_ANTTRIGROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTTRIGROUTE*/
#define _GPIO_MODEM_ANTTRIGROUTE_PIN_SHIFT               16                                           /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTTRIGROUTE_PIN_MASK                0xF0000UL                                    /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTTRIGROUTE_PIN_DEFAULT             0x00000000UL                                 /**< Mode DEFAULT for GPIO_MODEM_ANTTRIGROUTE    */
#define GPIO_MODEM_ANTTRIGROUTE_PIN_DEFAULT              (_GPIO_MODEM_ANTTRIGROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTTRIGROUTE*/

/* Bit fields for GPIO_MODEM ANTTRIGSTOPROUTE */
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_RESETVALUE          0x00000000UL                                     /**< Default value for GPIO_MODEM_ANTTRIGSTOPROUTE*/
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_MASK                0x000F0003UL                                     /**< Mask for GPIO_MODEM_ANTTRIGSTOPROUTE        */
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_PORT_SHIFT          0                                                /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_PORT_MASK           0x3UL                                            /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_PORT_DEFAULT        0x00000000UL                                     /**< Mode DEFAULT for GPIO_MODEM_ANTTRIGSTOPROUTE*/
#define GPIO_MODEM_ANTTRIGSTOPROUTE_PORT_DEFAULT         (_GPIO_MODEM_ANTTRIGSTOPROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTTRIGSTOPROUTE*/
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_PIN_SHIFT           16                                               /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_PIN_MASK            0xF0000UL                                        /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_ANTTRIGSTOPROUTE_PIN_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for GPIO_MODEM_ANTTRIGSTOPROUTE*/
#define GPIO_MODEM_ANTTRIGSTOPROUTE_PIN_DEFAULT          (_GPIO_MODEM_ANTTRIGSTOPROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_ANTTRIGSTOPROUTE*/

/* Bit fields for GPIO_MODEM DCLKROUTE */
#define _GPIO_MODEM_DCLKROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_MODEM_DCLKROUTE      */
#define _GPIO_MODEM_DCLKROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_MODEM_DCLKROUTE               */
#define _GPIO_MODEM_DCLKROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_DCLKROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_DCLKROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_DCLKROUTE       */
#define GPIO_MODEM_DCLKROUTE_PORT_DEFAULT                (_GPIO_MODEM_DCLKROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_DCLKROUTE*/
#define _GPIO_MODEM_DCLKROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_DCLKROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_DCLKROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_DCLKROUTE       */
#define GPIO_MODEM_DCLKROUTE_PIN_DEFAULT                 (_GPIO_MODEM_DCLKROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_DCLKROUTE*/

/* Bit fields for GPIO_MODEM DINROUTE */
#define _GPIO_MODEM_DINROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_MODEM_DINROUTE       */
#define _GPIO_MODEM_DINROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_MODEM_DINROUTE                */
#define _GPIO_MODEM_DINROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_DINROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_DINROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_MODEM_DINROUTE        */
#define GPIO_MODEM_DINROUTE_PORT_DEFAULT                 (_GPIO_MODEM_DINROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_DINROUTE*/
#define _GPIO_MODEM_DINROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_DINROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_DINROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_MODEM_DINROUTE        */
#define GPIO_MODEM_DINROUTE_PIN_DEFAULT                  (_GPIO_MODEM_DINROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_DINROUTE*/

/* Bit fields for GPIO_MODEM DOUTROUTE */
#define _GPIO_MODEM_DOUTROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_MODEM_DOUTROUTE      */
#define _GPIO_MODEM_DOUTROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_MODEM_DOUTROUTE               */
#define _GPIO_MODEM_DOUTROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_MODEM_DOUTROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_MODEM_DOUTROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_DOUTROUTE       */
#define GPIO_MODEM_DOUTROUTE_PORT_DEFAULT                (_GPIO_MODEM_DOUTROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_MODEM_DOUTROUTE*/
#define _GPIO_MODEM_DOUTROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_MODEM_DOUTROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_MODEM_DOUTROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_MODEM_DOUTROUTE       */
#define GPIO_MODEM_DOUTROUTE_PIN_DEFAULT                 (_GPIO_MODEM_DOUTROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_MODEM_DOUTROUTE*/

/* Bit fields for GPIO_PCNT S0INROUTE */
#define _GPIO_PCNT_S0INROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_PCNT_S0INROUTE       */
#define _GPIO_PCNT_S0INROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_PCNT_S0INROUTE                */
#define _GPIO_PCNT_S0INROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_PCNT_S0INROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PCNT_S0INROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_PCNT_S0INROUTE        */
#define GPIO_PCNT_S0INROUTE_PORT_DEFAULT                 (_GPIO_PCNT_S0INROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PCNT_S0INROUTE*/
#define _GPIO_PCNT_S0INROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_PCNT_S0INROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PCNT_S0INROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_PCNT_S0INROUTE        */
#define GPIO_PCNT_S0INROUTE_PIN_DEFAULT                  (_GPIO_PCNT_S0INROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PCNT_S0INROUTE*/

/* Bit fields for GPIO_PCNT S1INROUTE */
#define _GPIO_PCNT_S1INROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_PCNT_S1INROUTE       */
#define _GPIO_PCNT_S1INROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_PCNT_S1INROUTE                */
#define _GPIO_PCNT_S1INROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_PCNT_S1INROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PCNT_S1INROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_PCNT_S1INROUTE        */
#define GPIO_PCNT_S1INROUTE_PORT_DEFAULT                 (_GPIO_PCNT_S1INROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PCNT_S1INROUTE*/
#define _GPIO_PCNT_S1INROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_PCNT_S1INROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PCNT_S1INROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_PCNT_S1INROUTE        */
#define GPIO_PCNT_S1INROUTE_PIN_DEFAULT                  (_GPIO_PCNT_S1INROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PCNT_S1INROUTE*/

/* Bit fields for GPIO_PRS ROUTEEN */
#define _GPIO_PRS_ROUTEEN_RESETVALUE                     0x00000000UL                                  /**< Default value for GPIO_PRS_ROUTEEN          */
#define _GPIO_PRS_ROUTEEN_MASK                           0x0000FFFFUL                                  /**< Mask for GPIO_PRS_ROUTEEN                   */
#define GPIO_PRS_ROUTEEN_ASYNCH0PEN                      (0x1UL << 0)                                  /**< ASYNCH0 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH0PEN_SHIFT               0                                             /**< Shift value for GPIO_ASYNCH0PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH0PEN_MASK                0x1UL                                         /**< Bit mask for GPIO_ASYNCH0PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH0PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH0PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH0PEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH1PEN                      (0x1UL << 1)                                  /**< ASYNCH1 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH1PEN_SHIFT               1                                             /**< Shift value for GPIO_ASYNCH1PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH1PEN_MASK                0x2UL                                         /**< Bit mask for GPIO_ASYNCH1PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH1PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH1PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH1PEN_DEFAULT << 1)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH2PEN                      (0x1UL << 2)                                  /**< ASYNCH2 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH2PEN_SHIFT               2                                             /**< Shift value for GPIO_ASYNCH2PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH2PEN_MASK                0x4UL                                         /**< Bit mask for GPIO_ASYNCH2PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH2PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH2PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH2PEN_DEFAULT << 2)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH3PEN                      (0x1UL << 3)                                  /**< ASYNCH3 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH3PEN_SHIFT               3                                             /**< Shift value for GPIO_ASYNCH3PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH3PEN_MASK                0x8UL                                         /**< Bit mask for GPIO_ASYNCH3PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH3PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH3PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH3PEN_DEFAULT << 3)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH4PEN                      (0x1UL << 4)                                  /**< ASYNCH4 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH4PEN_SHIFT               4                                             /**< Shift value for GPIO_ASYNCH4PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH4PEN_MASK                0x10UL                                        /**< Bit mask for GPIO_ASYNCH4PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH4PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH4PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH4PEN_DEFAULT << 4)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH5PEN                      (0x1UL << 5)                                  /**< ASYNCH5 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH5PEN_SHIFT               5                                             /**< Shift value for GPIO_ASYNCH5PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH5PEN_MASK                0x20UL                                        /**< Bit mask for GPIO_ASYNCH5PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH5PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH5PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH5PEN_DEFAULT << 5)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH6PEN                      (0x1UL << 6)                                  /**< ASYNCH6 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH6PEN_SHIFT               6                                             /**< Shift value for GPIO_ASYNCH6PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH6PEN_MASK                0x40UL                                        /**< Bit mask for GPIO_ASYNCH6PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH6PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH6PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH6PEN_DEFAULT << 6)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH7PEN                      (0x1UL << 7)                                  /**< ASYNCH7 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH7PEN_SHIFT               7                                             /**< Shift value for GPIO_ASYNCH7PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH7PEN_MASK                0x80UL                                        /**< Bit mask for GPIO_ASYNCH7PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH7PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH7PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH7PEN_DEFAULT << 7)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH8PEN                      (0x1UL << 8)                                  /**< ASYNCH8 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH8PEN_SHIFT               8                                             /**< Shift value for GPIO_ASYNCH8PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH8PEN_MASK                0x100UL                                       /**< Bit mask for GPIO_ASYNCH8PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH8PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH8PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH8PEN_DEFAULT << 8)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH9PEN                      (0x1UL << 9)                                  /**< ASYNCH9 pin enable control bit              */
#define _GPIO_PRS_ROUTEEN_ASYNCH9PEN_SHIFT               9                                             /**< Shift value for GPIO_ASYNCH9PEN             */
#define _GPIO_PRS_ROUTEEN_ASYNCH9PEN_MASK                0x200UL                                       /**< Bit mask for GPIO_ASYNCH9PEN                */
#define _GPIO_PRS_ROUTEEN_ASYNCH9PEN_DEFAULT             0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH9PEN_DEFAULT              (_GPIO_PRS_ROUTEEN_ASYNCH9PEN_DEFAULT << 9)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH10PEN                     (0x1UL << 10)                                 /**< ASYNCH10 pin enable control bit             */
#define _GPIO_PRS_ROUTEEN_ASYNCH10PEN_SHIFT              10                                            /**< Shift value for GPIO_ASYNCH10PEN            */
#define _GPIO_PRS_ROUTEEN_ASYNCH10PEN_MASK               0x400UL                                       /**< Bit mask for GPIO_ASYNCH10PEN               */
#define _GPIO_PRS_ROUTEEN_ASYNCH10PEN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH10PEN_DEFAULT             (_GPIO_PRS_ROUTEEN_ASYNCH10PEN_DEFAULT << 10) /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_ASYNCH11PEN                     (0x1UL << 11)                                 /**< ASYNCH11 pin enable control bit             */
#define _GPIO_PRS_ROUTEEN_ASYNCH11PEN_SHIFT              11                                            /**< Shift value for GPIO_ASYNCH11PEN            */
#define _GPIO_PRS_ROUTEEN_ASYNCH11PEN_MASK               0x800UL                                       /**< Bit mask for GPIO_ASYNCH11PEN               */
#define _GPIO_PRS_ROUTEEN_ASYNCH11PEN_DEFAULT            0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_ASYNCH11PEN_DEFAULT             (_GPIO_PRS_ROUTEEN_ASYNCH11PEN_DEFAULT << 11) /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_SYNCH0PEN                       (0x1UL << 12)                                 /**< SYNCH0 pin enable control bit               */
#define _GPIO_PRS_ROUTEEN_SYNCH0PEN_SHIFT                12                                            /**< Shift value for GPIO_SYNCH0PEN              */
#define _GPIO_PRS_ROUTEEN_SYNCH0PEN_MASK                 0x1000UL                                      /**< Bit mask for GPIO_SYNCH0PEN                 */
#define _GPIO_PRS_ROUTEEN_SYNCH0PEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_SYNCH0PEN_DEFAULT               (_GPIO_PRS_ROUTEEN_SYNCH0PEN_DEFAULT << 12)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_SYNCH1PEN                       (0x1UL << 13)                                 /**< SYNCH1 pin enable control bit               */
#define _GPIO_PRS_ROUTEEN_SYNCH1PEN_SHIFT                13                                            /**< Shift value for GPIO_SYNCH1PEN              */
#define _GPIO_PRS_ROUTEEN_SYNCH1PEN_MASK                 0x2000UL                                      /**< Bit mask for GPIO_SYNCH1PEN                 */
#define _GPIO_PRS_ROUTEEN_SYNCH1PEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_SYNCH1PEN_DEFAULT               (_GPIO_PRS_ROUTEEN_SYNCH1PEN_DEFAULT << 13)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_SYNCH2PEN                       (0x1UL << 14)                                 /**< SYNCH2 pin enable control bit               */
#define _GPIO_PRS_ROUTEEN_SYNCH2PEN_SHIFT                14                                            /**< Shift value for GPIO_SYNCH2PEN              */
#define _GPIO_PRS_ROUTEEN_SYNCH2PEN_MASK                 0x4000UL                                      /**< Bit mask for GPIO_SYNCH2PEN                 */
#define _GPIO_PRS_ROUTEEN_SYNCH2PEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_SYNCH2PEN_DEFAULT               (_GPIO_PRS_ROUTEEN_SYNCH2PEN_DEFAULT << 14)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */
#define GPIO_PRS_ROUTEEN_SYNCH3PEN                       (0x1UL << 15)                                 /**< SYNCH3 pin enable control bit               */
#define _GPIO_PRS_ROUTEEN_SYNCH3PEN_SHIFT                15                                            /**< Shift value for GPIO_SYNCH3PEN              */
#define _GPIO_PRS_ROUTEEN_SYNCH3PEN_MASK                 0x8000UL                                      /**< Bit mask for GPIO_SYNCH3PEN                 */
#define _GPIO_PRS_ROUTEEN_SYNCH3PEN_DEFAULT              0x00000000UL                                  /**< Mode DEFAULT for GPIO_PRS_ROUTEEN           */
#define GPIO_PRS_ROUTEEN_SYNCH3PEN_DEFAULT               (_GPIO_PRS_ROUTEEN_SYNCH3PEN_DEFAULT << 15)   /**< Shifted mode DEFAULT for GPIO_PRS_ROUTEEN   */

/* Bit fields for GPIO_PRS ASYNCH0ROUTE */
#define _GPIO_PRS_ASYNCH0ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH0ROUTE     */
#define _GPIO_PRS_ASYNCH0ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH0ROUTE              */
#define _GPIO_PRS_ASYNCH0ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH0ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH0ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH0ROUTE      */
#define GPIO_PRS_ASYNCH0ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH0ROUTE*/
#define _GPIO_PRS_ASYNCH0ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH0ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH0ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH0ROUTE      */
#define GPIO_PRS_ASYNCH0ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH0ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH1ROUTE */
#define _GPIO_PRS_ASYNCH1ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH1ROUTE     */
#define _GPIO_PRS_ASYNCH1ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH1ROUTE              */
#define _GPIO_PRS_ASYNCH1ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH1ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH1ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH1ROUTE      */
#define GPIO_PRS_ASYNCH1ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH1ROUTE*/
#define _GPIO_PRS_ASYNCH1ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH1ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH1ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH1ROUTE      */
#define GPIO_PRS_ASYNCH1ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH1ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH2ROUTE */
#define _GPIO_PRS_ASYNCH2ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH2ROUTE     */
#define _GPIO_PRS_ASYNCH2ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH2ROUTE              */
#define _GPIO_PRS_ASYNCH2ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH2ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH2ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH2ROUTE      */
#define GPIO_PRS_ASYNCH2ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH2ROUTE*/
#define _GPIO_PRS_ASYNCH2ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH2ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH2ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH2ROUTE      */
#define GPIO_PRS_ASYNCH2ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH2ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH3ROUTE */
#define _GPIO_PRS_ASYNCH3ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH3ROUTE     */
#define _GPIO_PRS_ASYNCH3ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH3ROUTE              */
#define _GPIO_PRS_ASYNCH3ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH3ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH3ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH3ROUTE      */
#define GPIO_PRS_ASYNCH3ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH3ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH3ROUTE*/
#define _GPIO_PRS_ASYNCH3ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH3ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH3ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH3ROUTE      */
#define GPIO_PRS_ASYNCH3ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH3ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH3ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH4ROUTE */
#define _GPIO_PRS_ASYNCH4ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH4ROUTE     */
#define _GPIO_PRS_ASYNCH4ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH4ROUTE              */
#define _GPIO_PRS_ASYNCH4ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH4ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH4ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH4ROUTE      */
#define GPIO_PRS_ASYNCH4ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH4ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH4ROUTE*/
#define _GPIO_PRS_ASYNCH4ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH4ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH4ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH4ROUTE      */
#define GPIO_PRS_ASYNCH4ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH4ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH4ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH5ROUTE */
#define _GPIO_PRS_ASYNCH5ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH5ROUTE     */
#define _GPIO_PRS_ASYNCH5ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH5ROUTE              */
#define _GPIO_PRS_ASYNCH5ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH5ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH5ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH5ROUTE      */
#define GPIO_PRS_ASYNCH5ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH5ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH5ROUTE*/
#define _GPIO_PRS_ASYNCH5ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH5ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH5ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH5ROUTE      */
#define GPIO_PRS_ASYNCH5ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH5ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH5ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH6ROUTE */
#define _GPIO_PRS_ASYNCH6ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH6ROUTE     */
#define _GPIO_PRS_ASYNCH6ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH6ROUTE              */
#define _GPIO_PRS_ASYNCH6ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH6ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH6ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH6ROUTE      */
#define GPIO_PRS_ASYNCH6ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH6ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH6ROUTE*/
#define _GPIO_PRS_ASYNCH6ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH6ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH6ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH6ROUTE      */
#define GPIO_PRS_ASYNCH6ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH6ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH6ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH7ROUTE */
#define _GPIO_PRS_ASYNCH7ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH7ROUTE     */
#define _GPIO_PRS_ASYNCH7ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH7ROUTE              */
#define _GPIO_PRS_ASYNCH7ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH7ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH7ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH7ROUTE      */
#define GPIO_PRS_ASYNCH7ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH7ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH7ROUTE*/
#define _GPIO_PRS_ASYNCH7ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH7ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH7ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH7ROUTE      */
#define GPIO_PRS_ASYNCH7ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH7ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH7ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH8ROUTE */
#define _GPIO_PRS_ASYNCH8ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH8ROUTE     */
#define _GPIO_PRS_ASYNCH8ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH8ROUTE              */
#define _GPIO_PRS_ASYNCH8ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH8ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH8ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH8ROUTE      */
#define GPIO_PRS_ASYNCH8ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH8ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH8ROUTE*/
#define _GPIO_PRS_ASYNCH8ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH8ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH8ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH8ROUTE      */
#define GPIO_PRS_ASYNCH8ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH8ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH8ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH9ROUTE */
#define _GPIO_PRS_ASYNCH9ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_PRS_ASYNCH9ROUTE     */
#define _GPIO_PRS_ASYNCH9ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_PRS_ASYNCH9ROUTE              */
#define _GPIO_PRS_ASYNCH9ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH9ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH9ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH9ROUTE      */
#define GPIO_PRS_ASYNCH9ROUTE_PORT_DEFAULT               (_GPIO_PRS_ASYNCH9ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH9ROUTE*/
#define _GPIO_PRS_ASYNCH9ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH9ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH9ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_PRS_ASYNCH9ROUTE      */
#define GPIO_PRS_ASYNCH9ROUTE_PIN_DEFAULT                (_GPIO_PRS_ASYNCH9ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH9ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH10ROUTE */
#define _GPIO_PRS_ASYNCH10ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_PRS_ASYNCH10ROUTE    */
#define _GPIO_PRS_ASYNCH10ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_PRS_ASYNCH10ROUTE             */
#define _GPIO_PRS_ASYNCH10ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH10ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH10ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_PRS_ASYNCH10ROUTE     */
#define GPIO_PRS_ASYNCH10ROUTE_PORT_DEFAULT              (_GPIO_PRS_ASYNCH10ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH10ROUTE*/
#define _GPIO_PRS_ASYNCH10ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH10ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH10ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_PRS_ASYNCH10ROUTE     */
#define GPIO_PRS_ASYNCH10ROUTE_PIN_DEFAULT               (_GPIO_PRS_ASYNCH10ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH10ROUTE*/

/* Bit fields for GPIO_PRS ASYNCH11ROUTE */
#define _GPIO_PRS_ASYNCH11ROUTE_RESETVALUE               0x00000000UL                                /**< Default value for GPIO_PRS_ASYNCH11ROUTE    */
#define _GPIO_PRS_ASYNCH11ROUTE_MASK                     0x000F0003UL                                /**< Mask for GPIO_PRS_ASYNCH11ROUTE             */
#define _GPIO_PRS_ASYNCH11ROUTE_PORT_SHIFT               0                                           /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_ASYNCH11ROUTE_PORT_MASK                0x3UL                                       /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_ASYNCH11ROUTE_PORT_DEFAULT             0x00000000UL                                /**< Mode DEFAULT for GPIO_PRS_ASYNCH11ROUTE     */
#define GPIO_PRS_ASYNCH11ROUTE_PORT_DEFAULT              (_GPIO_PRS_ASYNCH11ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH11ROUTE*/
#define _GPIO_PRS_ASYNCH11ROUTE_PIN_SHIFT                16                                          /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_ASYNCH11ROUTE_PIN_MASK                 0xF0000UL                                   /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_ASYNCH11ROUTE_PIN_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for GPIO_PRS_ASYNCH11ROUTE     */
#define GPIO_PRS_ASYNCH11ROUTE_PIN_DEFAULT               (_GPIO_PRS_ASYNCH11ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_ASYNCH11ROUTE*/

/* Bit fields for GPIO_PRS SYNCH0ROUTE */
#define _GPIO_PRS_SYNCH0ROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_PRS_SYNCH0ROUTE      */
#define _GPIO_PRS_SYNCH0ROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_PRS_SYNCH0ROUTE               */
#define _GPIO_PRS_SYNCH0ROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_SYNCH0ROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_SYNCH0ROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH0ROUTE       */
#define GPIO_PRS_SYNCH0ROUTE_PORT_DEFAULT                (_GPIO_PRS_SYNCH0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH0ROUTE*/
#define _GPIO_PRS_SYNCH0ROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_SYNCH0ROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_SYNCH0ROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH0ROUTE       */
#define GPIO_PRS_SYNCH0ROUTE_PIN_DEFAULT                 (_GPIO_PRS_SYNCH0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH0ROUTE*/

/* Bit fields for GPIO_PRS SYNCH1ROUTE */
#define _GPIO_PRS_SYNCH1ROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_PRS_SYNCH1ROUTE      */
#define _GPIO_PRS_SYNCH1ROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_PRS_SYNCH1ROUTE               */
#define _GPIO_PRS_SYNCH1ROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_SYNCH1ROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_SYNCH1ROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH1ROUTE       */
#define GPIO_PRS_SYNCH1ROUTE_PORT_DEFAULT                (_GPIO_PRS_SYNCH1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH1ROUTE*/
#define _GPIO_PRS_SYNCH1ROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_SYNCH1ROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_SYNCH1ROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH1ROUTE       */
#define GPIO_PRS_SYNCH1ROUTE_PIN_DEFAULT                 (_GPIO_PRS_SYNCH1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH1ROUTE*/

/* Bit fields for GPIO_PRS SYNCH2ROUTE */
#define _GPIO_PRS_SYNCH2ROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_PRS_SYNCH2ROUTE      */
#define _GPIO_PRS_SYNCH2ROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_PRS_SYNCH2ROUTE               */
#define _GPIO_PRS_SYNCH2ROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_SYNCH2ROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_SYNCH2ROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH2ROUTE       */
#define GPIO_PRS_SYNCH2ROUTE_PORT_DEFAULT                (_GPIO_PRS_SYNCH2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH2ROUTE*/
#define _GPIO_PRS_SYNCH2ROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_SYNCH2ROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_SYNCH2ROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH2ROUTE       */
#define GPIO_PRS_SYNCH2ROUTE_PIN_DEFAULT                 (_GPIO_PRS_SYNCH2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH2ROUTE*/

/* Bit fields for GPIO_PRS SYNCH3ROUTE */
#define _GPIO_PRS_SYNCH3ROUTE_RESETVALUE                 0x00000000UL                              /**< Default value for GPIO_PRS_SYNCH3ROUTE      */
#define _GPIO_PRS_SYNCH3ROUTE_MASK                       0x000F0003UL                              /**< Mask for GPIO_PRS_SYNCH3ROUTE               */
#define _GPIO_PRS_SYNCH3ROUTE_PORT_SHIFT                 0                                         /**< Shift value for GPIO_PORT                   */
#define _GPIO_PRS_SYNCH3ROUTE_PORT_MASK                  0x3UL                                     /**< Bit mask for GPIO_PORT                      */
#define _GPIO_PRS_SYNCH3ROUTE_PORT_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH3ROUTE       */
#define GPIO_PRS_SYNCH3ROUTE_PORT_DEFAULT                (_GPIO_PRS_SYNCH3ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH3ROUTE*/
#define _GPIO_PRS_SYNCH3ROUTE_PIN_SHIFT                  16                                        /**< Shift value for GPIO_PIN                    */
#define _GPIO_PRS_SYNCH3ROUTE_PIN_MASK                   0xF0000UL                                 /**< Bit mask for GPIO_PIN                       */
#define _GPIO_PRS_SYNCH3ROUTE_PIN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_PRS_SYNCH3ROUTE       */
#define GPIO_PRS_SYNCH3ROUTE_PIN_DEFAULT                 (_GPIO_PRS_SYNCH3ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_PRS_SYNCH3ROUTE*/

/* Bit fields for GPIO_SYXO BUFOUTREQINASYNCROUTE */
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_RESETVALUE      0x00000000UL                                         /**< Default value for GPIO_SYXO_BUFOUTREQINASYNCROUTE*/
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_MASK            0x000F0003UL                                         /**< Mask for GPIO_SYXO_BUFOUTREQINASYNCROUTE    */
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_PORT_SHIFT      0                                                    /**< Shift value for GPIO_PORT                   */
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_PORT_MASK       0x3UL                                                /**< Bit mask for GPIO_PORT                      */
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_PORT_DEFAULT    0x00000000UL                                         /**< Mode DEFAULT for GPIO_SYXO_BUFOUTREQINASYNCROUTE*/
#define GPIO_SYXO_BUFOUTREQINASYNCROUTE_PORT_DEFAULT     (_GPIO_SYXO_BUFOUTREQINASYNCROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_SYXO_BUFOUTREQINASYNCROUTE*/
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_PIN_SHIFT       16                                                   /**< Shift value for GPIO_PIN                    */
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_PIN_MASK        0xF0000UL                                            /**< Bit mask for GPIO_PIN                       */
#define _GPIO_SYXO_BUFOUTREQINASYNCROUTE_PIN_DEFAULT     0x00000000UL                                         /**< Mode DEFAULT for GPIO_SYXO_BUFOUTREQINASYNCROUTE*/
#define GPIO_SYXO_BUFOUTREQINASYNCROUTE_PIN_DEFAULT      (_GPIO_SYXO_BUFOUTREQINASYNCROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_SYXO_BUFOUTREQINASYNCROUTE*/

/* Bit fields for GPIO_TIMER ROUTEEN */
#define _GPIO_TIMER_ROUTEEN_RESETVALUE                   0x00000000UL                               /**< Default value for GPIO_TIMER_ROUTEEN        */
#define _GPIO_TIMER_ROUTEEN_MASK                         0x0000003FUL                               /**< Mask for GPIO_TIMER_ROUTEEN                 */
#define GPIO_TIMER_ROUTEEN_CC0PEN                        (0x1UL << 0)                               /**< CC0 pin enable control bit                  */
#define _GPIO_TIMER_ROUTEEN_CC0PEN_SHIFT                 0                                          /**< Shift value for GPIO_CC0PEN                 */
#define _GPIO_TIMER_ROUTEEN_CC0PEN_MASK                  0x1UL                                      /**< Bit mask for GPIO_CC0PEN                    */
#define _GPIO_TIMER_ROUTEEN_CC0PEN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_ROUTEEN         */
#define GPIO_TIMER_ROUTEEN_CC0PEN_DEFAULT                (_GPIO_TIMER_ROUTEEN_CC0PEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_TIMER_ROUTEEN */
#define GPIO_TIMER_ROUTEEN_CC1PEN                        (0x1UL << 1)                               /**< CC1 pin enable control bit                  */
#define _GPIO_TIMER_ROUTEEN_CC1PEN_SHIFT                 1                                          /**< Shift value for GPIO_CC1PEN                 */
#define _GPIO_TIMER_ROUTEEN_CC1PEN_MASK                  0x2UL                                      /**< Bit mask for GPIO_CC1PEN                    */
#define _GPIO_TIMER_ROUTEEN_CC1PEN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_ROUTEEN         */
#define GPIO_TIMER_ROUTEEN_CC1PEN_DEFAULT                (_GPIO_TIMER_ROUTEEN_CC1PEN_DEFAULT << 1)  /**< Shifted mode DEFAULT for GPIO_TIMER_ROUTEEN */
#define GPIO_TIMER_ROUTEEN_CC2PEN                        (0x1UL << 2)                               /**< CC2 pin enable control bit                  */
#define _GPIO_TIMER_ROUTEEN_CC2PEN_SHIFT                 2                                          /**< Shift value for GPIO_CC2PEN                 */
#define _GPIO_TIMER_ROUTEEN_CC2PEN_MASK                  0x4UL                                      /**< Bit mask for GPIO_CC2PEN                    */
#define _GPIO_TIMER_ROUTEEN_CC2PEN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_ROUTEEN         */
#define GPIO_TIMER_ROUTEEN_CC2PEN_DEFAULT                (_GPIO_TIMER_ROUTEEN_CC2PEN_DEFAULT << 2)  /**< Shifted mode DEFAULT for GPIO_TIMER_ROUTEEN */
#define GPIO_TIMER_ROUTEEN_CCC0PEN                       (0x1UL << 3)                               /**< CDTI0 pin enable control bit                */
#define _GPIO_TIMER_ROUTEEN_CCC0PEN_SHIFT                3                                          /**< Shift value for GPIO_CCC0PEN                */
#define _GPIO_TIMER_ROUTEEN_CCC0PEN_MASK                 0x8UL                                      /**< Bit mask for GPIO_CCC0PEN                   */
#define _GPIO_TIMER_ROUTEEN_CCC0PEN_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_ROUTEEN         */
#define GPIO_TIMER_ROUTEEN_CCC0PEN_DEFAULT               (_GPIO_TIMER_ROUTEEN_CCC0PEN_DEFAULT << 3) /**< Shifted mode DEFAULT for GPIO_TIMER_ROUTEEN */
#define GPIO_TIMER_ROUTEEN_CCC1PEN                       (0x1UL << 4)                               /**< CDTI1 pin enable control bit                */
#define _GPIO_TIMER_ROUTEEN_CCC1PEN_SHIFT                4                                          /**< Shift value for GPIO_CCC1PEN                */
#define _GPIO_TIMER_ROUTEEN_CCC1PEN_MASK                 0x10UL                                     /**< Bit mask for GPIO_CCC1PEN                   */
#define _GPIO_TIMER_ROUTEEN_CCC1PEN_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_ROUTEEN         */
#define GPIO_TIMER_ROUTEEN_CCC1PEN_DEFAULT               (_GPIO_TIMER_ROUTEEN_CCC1PEN_DEFAULT << 4) /**< Shifted mode DEFAULT for GPIO_TIMER_ROUTEEN */
#define GPIO_TIMER_ROUTEEN_CCC2PEN                       (0x1UL << 5)                               /**< CDTI2 pin enable control bit                */
#define _GPIO_TIMER_ROUTEEN_CCC2PEN_SHIFT                5                                          /**< Shift value for GPIO_CCC2PEN                */
#define _GPIO_TIMER_ROUTEEN_CCC2PEN_MASK                 0x20UL                                     /**< Bit mask for GPIO_CCC2PEN                   */
#define _GPIO_TIMER_ROUTEEN_CCC2PEN_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_ROUTEEN         */
#define GPIO_TIMER_ROUTEEN_CCC2PEN_DEFAULT               (_GPIO_TIMER_ROUTEEN_CCC2PEN_DEFAULT << 5) /**< Shifted mode DEFAULT for GPIO_TIMER_ROUTEEN */

/* Bit fields for GPIO_TIMER CC0ROUTE */
#define _GPIO_TIMER_CC0ROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_TIMER_CC0ROUTE       */
#define _GPIO_TIMER_CC0ROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_TIMER_CC0ROUTE                */
#define _GPIO_TIMER_CC0ROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CC0ROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_TIMER_CC0ROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_TIMER_CC0ROUTE        */
#define GPIO_TIMER_CC0ROUTE_PORT_DEFAULT                 (_GPIO_TIMER_CC0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_TIMER_CC0ROUTE*/
#define _GPIO_TIMER_CC0ROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CC0ROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_TIMER_CC0ROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_TIMER_CC0ROUTE        */
#define GPIO_TIMER_CC0ROUTE_PIN_DEFAULT                  (_GPIO_TIMER_CC0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_TIMER_CC0ROUTE*/

/* Bit fields for GPIO_TIMER CC1ROUTE */
#define _GPIO_TIMER_CC1ROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_TIMER_CC1ROUTE       */
#define _GPIO_TIMER_CC1ROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_TIMER_CC1ROUTE                */
#define _GPIO_TIMER_CC1ROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CC1ROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_TIMER_CC1ROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_TIMER_CC1ROUTE        */
#define GPIO_TIMER_CC1ROUTE_PORT_DEFAULT                 (_GPIO_TIMER_CC1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_TIMER_CC1ROUTE*/
#define _GPIO_TIMER_CC1ROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CC1ROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_TIMER_CC1ROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_TIMER_CC1ROUTE        */
#define GPIO_TIMER_CC1ROUTE_PIN_DEFAULT                  (_GPIO_TIMER_CC1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_TIMER_CC1ROUTE*/

/* Bit fields for GPIO_TIMER CC2ROUTE */
#define _GPIO_TIMER_CC2ROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_TIMER_CC2ROUTE       */
#define _GPIO_TIMER_CC2ROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_TIMER_CC2ROUTE                */
#define _GPIO_TIMER_CC2ROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CC2ROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_TIMER_CC2ROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_TIMER_CC2ROUTE        */
#define GPIO_TIMER_CC2ROUTE_PORT_DEFAULT                 (_GPIO_TIMER_CC2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_TIMER_CC2ROUTE*/
#define _GPIO_TIMER_CC2ROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CC2ROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_TIMER_CC2ROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_TIMER_CC2ROUTE        */
#define GPIO_TIMER_CC2ROUTE_PIN_DEFAULT                  (_GPIO_TIMER_CC2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_TIMER_CC2ROUTE*/

/* Bit fields for GPIO_TIMER CDTI0ROUTE */
#define _GPIO_TIMER_CDTI0ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_TIMER_CDTI0ROUTE     */
#define _GPIO_TIMER_CDTI0ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_TIMER_CDTI0ROUTE              */
#define _GPIO_TIMER_CDTI0ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CDTI0ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_TIMER_CDTI0ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_CDTI0ROUTE      */
#define GPIO_TIMER_CDTI0ROUTE_PORT_DEFAULT               (_GPIO_TIMER_CDTI0ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_TIMER_CDTI0ROUTE*/
#define _GPIO_TIMER_CDTI0ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CDTI0ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_TIMER_CDTI0ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_CDTI0ROUTE      */
#define GPIO_TIMER_CDTI0ROUTE_PIN_DEFAULT                (_GPIO_TIMER_CDTI0ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_TIMER_CDTI0ROUTE*/

/* Bit fields for GPIO_TIMER CDTI1ROUTE */
#define _GPIO_TIMER_CDTI1ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_TIMER_CDTI1ROUTE     */
#define _GPIO_TIMER_CDTI1ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_TIMER_CDTI1ROUTE              */
#define _GPIO_TIMER_CDTI1ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CDTI1ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_TIMER_CDTI1ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_CDTI1ROUTE      */
#define GPIO_TIMER_CDTI1ROUTE_PORT_DEFAULT               (_GPIO_TIMER_CDTI1ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_TIMER_CDTI1ROUTE*/
#define _GPIO_TIMER_CDTI1ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CDTI1ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_TIMER_CDTI1ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_CDTI1ROUTE      */
#define GPIO_TIMER_CDTI1ROUTE_PIN_DEFAULT                (_GPIO_TIMER_CDTI1ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_TIMER_CDTI1ROUTE*/

/* Bit fields for GPIO_TIMER CDTI2ROUTE */
#define _GPIO_TIMER_CDTI2ROUTE_RESETVALUE                0x00000000UL                               /**< Default value for GPIO_TIMER_CDTI2ROUTE     */
#define _GPIO_TIMER_CDTI2ROUTE_MASK                      0x000F0003UL                               /**< Mask for GPIO_TIMER_CDTI2ROUTE              */
#define _GPIO_TIMER_CDTI2ROUTE_PORT_SHIFT                0                                          /**< Shift value for GPIO_PORT                   */
#define _GPIO_TIMER_CDTI2ROUTE_PORT_MASK                 0x3UL                                      /**< Bit mask for GPIO_PORT                      */
#define _GPIO_TIMER_CDTI2ROUTE_PORT_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_CDTI2ROUTE      */
#define GPIO_TIMER_CDTI2ROUTE_PORT_DEFAULT               (_GPIO_TIMER_CDTI2ROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_TIMER_CDTI2ROUTE*/
#define _GPIO_TIMER_CDTI2ROUTE_PIN_SHIFT                 16                                         /**< Shift value for GPIO_PIN                    */
#define _GPIO_TIMER_CDTI2ROUTE_PIN_MASK                  0xF0000UL                                  /**< Bit mask for GPIO_PIN                       */
#define _GPIO_TIMER_CDTI2ROUTE_PIN_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for GPIO_TIMER_CDTI2ROUTE      */
#define GPIO_TIMER_CDTI2ROUTE_PIN_DEFAULT                (_GPIO_TIMER_CDTI2ROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_TIMER_CDTI2ROUTE*/

/* Bit fields for GPIO_USART ROUTEEN */
#define _GPIO_USART_ROUTEEN_RESETVALUE                   0x00000000UL                              /**< Default value for GPIO_USART_ROUTEEN        */
#define _GPIO_USART_ROUTEEN_MASK                         0x0000001FUL                              /**< Mask for GPIO_USART_ROUTEEN                 */
#define GPIO_USART_ROUTEEN_CSPEN                         (0x1UL << 0)                              /**< CS pin enable control bit                   */
#define _GPIO_USART_ROUTEEN_CSPEN_SHIFT                  0                                         /**< Shift value for GPIO_CSPEN                  */
#define _GPIO_USART_ROUTEEN_CSPEN_MASK                   0x1UL                                     /**< Bit mask for GPIO_CSPEN                     */
#define _GPIO_USART_ROUTEEN_CSPEN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_USART_ROUTEEN         */
#define GPIO_USART_ROUTEEN_CSPEN_DEFAULT                 (_GPIO_USART_ROUTEEN_CSPEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for GPIO_USART_ROUTEEN */
#define GPIO_USART_ROUTEEN_RTSPEN                        (0x1UL << 1)                              /**< RTS pin enable control bit                  */
#define _GPIO_USART_ROUTEEN_RTSPEN_SHIFT                 1                                         /**< Shift value for GPIO_RTSPEN                 */
#define _GPIO_USART_ROUTEEN_RTSPEN_MASK                  0x2UL                                     /**< Bit mask for GPIO_RTSPEN                    */
#define _GPIO_USART_ROUTEEN_RTSPEN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_USART_ROUTEEN         */
#define GPIO_USART_ROUTEEN_RTSPEN_DEFAULT                (_GPIO_USART_ROUTEEN_RTSPEN_DEFAULT << 1) /**< Shifted mode DEFAULT for GPIO_USART_ROUTEEN */
#define GPIO_USART_ROUTEEN_RXPEN                         (0x1UL << 2)                              /**< RX pin enable control bit                   */
#define _GPIO_USART_ROUTEEN_RXPEN_SHIFT                  2                                         /**< Shift value for GPIO_RXPEN                  */
#define _GPIO_USART_ROUTEEN_RXPEN_MASK                   0x4UL                                     /**< Bit mask for GPIO_RXPEN                     */
#define _GPIO_USART_ROUTEEN_RXPEN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_USART_ROUTEEN         */
#define GPIO_USART_ROUTEEN_RXPEN_DEFAULT                 (_GPIO_USART_ROUTEEN_RXPEN_DEFAULT << 2)  /**< Shifted mode DEFAULT for GPIO_USART_ROUTEEN */
#define GPIO_USART_ROUTEEN_CLKPEN                        (0x1UL << 3)                              /**< SCLK pin enable control bit                 */
#define _GPIO_USART_ROUTEEN_CLKPEN_SHIFT                 3                                         /**< Shift value for GPIO_CLKPEN                 */
#define _GPIO_USART_ROUTEEN_CLKPEN_MASK                  0x8UL                                     /**< Bit mask for GPIO_CLKPEN                    */
#define _GPIO_USART_ROUTEEN_CLKPEN_DEFAULT               0x00000000UL                              /**< Mode DEFAULT for GPIO_USART_ROUTEEN         */
#define GPIO_USART_ROUTEEN_CLKPEN_DEFAULT                (_GPIO_USART_ROUTEEN_CLKPEN_DEFAULT << 3) /**< Shifted mode DEFAULT for GPIO_USART_ROUTEEN */
#define GPIO_USART_ROUTEEN_TXPEN                         (0x1UL << 4)                              /**< TX pin enable control bit                   */
#define _GPIO_USART_ROUTEEN_TXPEN_SHIFT                  4                                         /**< Shift value for GPIO_TXPEN                  */
#define _GPIO_USART_ROUTEEN_TXPEN_MASK                   0x10UL                                    /**< Bit mask for GPIO_TXPEN                     */
#define _GPIO_USART_ROUTEEN_TXPEN_DEFAULT                0x00000000UL                              /**< Mode DEFAULT for GPIO_USART_ROUTEEN         */
#define GPIO_USART_ROUTEEN_TXPEN_DEFAULT                 (_GPIO_USART_ROUTEEN_TXPEN_DEFAULT << 4)  /**< Shifted mode DEFAULT for GPIO_USART_ROUTEEN */

/* Bit fields for GPIO_USART CSROUTE */
#define _GPIO_USART_CSROUTE_RESETVALUE                   0x00000000UL                            /**< Default value for GPIO_USART_CSROUTE        */
#define _GPIO_USART_CSROUTE_MASK                         0x000F0003UL                            /**< Mask for GPIO_USART_CSROUTE                 */
#define _GPIO_USART_CSROUTE_PORT_SHIFT                   0                                       /**< Shift value for GPIO_PORT                   */
#define _GPIO_USART_CSROUTE_PORT_MASK                    0x3UL                                   /**< Bit mask for GPIO_PORT                      */
#define _GPIO_USART_CSROUTE_PORT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_USART_CSROUTE         */
#define GPIO_USART_CSROUTE_PORT_DEFAULT                  (_GPIO_USART_CSROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_USART_CSROUTE */
#define _GPIO_USART_CSROUTE_PIN_SHIFT                    16                                      /**< Shift value for GPIO_PIN                    */
#define _GPIO_USART_CSROUTE_PIN_MASK                     0xF0000UL                               /**< Bit mask for GPIO_PIN                       */
#define _GPIO_USART_CSROUTE_PIN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for GPIO_USART_CSROUTE         */
#define GPIO_USART_CSROUTE_PIN_DEFAULT                   (_GPIO_USART_CSROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_USART_CSROUTE */

/* Bit fields for GPIO_USART CTSROUTE */
#define _GPIO_USART_CTSROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_USART_CTSROUTE       */
#define _GPIO_USART_CTSROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_USART_CTSROUTE                */
#define _GPIO_USART_CTSROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_USART_CTSROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_USART_CTSROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_USART_CTSROUTE        */
#define GPIO_USART_CTSROUTE_PORT_DEFAULT                 (_GPIO_USART_CTSROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_USART_CTSROUTE*/
#define _GPIO_USART_CTSROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_USART_CTSROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_USART_CTSROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_USART_CTSROUTE        */
#define GPIO_USART_CTSROUTE_PIN_DEFAULT                  (_GPIO_USART_CTSROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_USART_CTSROUTE*/

/* Bit fields for GPIO_USART RTSROUTE */
#define _GPIO_USART_RTSROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_USART_RTSROUTE       */
#define _GPIO_USART_RTSROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_USART_RTSROUTE                */
#define _GPIO_USART_RTSROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_USART_RTSROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_USART_RTSROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_USART_RTSROUTE        */
#define GPIO_USART_RTSROUTE_PORT_DEFAULT                 (_GPIO_USART_RTSROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_USART_RTSROUTE*/
#define _GPIO_USART_RTSROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_USART_RTSROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_USART_RTSROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_USART_RTSROUTE        */
#define GPIO_USART_RTSROUTE_PIN_DEFAULT                  (_GPIO_USART_RTSROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_USART_RTSROUTE*/

/* Bit fields for GPIO_USART RXROUTE */
#define _GPIO_USART_RXROUTE_RESETVALUE                   0x00000000UL                            /**< Default value for GPIO_USART_RXROUTE        */
#define _GPIO_USART_RXROUTE_MASK                         0x000F0003UL                            /**< Mask for GPIO_USART_RXROUTE                 */
#define _GPIO_USART_RXROUTE_PORT_SHIFT                   0                                       /**< Shift value for GPIO_PORT                   */
#define _GPIO_USART_RXROUTE_PORT_MASK                    0x3UL                                   /**< Bit mask for GPIO_PORT                      */
#define _GPIO_USART_RXROUTE_PORT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_USART_RXROUTE         */
#define GPIO_USART_RXROUTE_PORT_DEFAULT                  (_GPIO_USART_RXROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_USART_RXROUTE */
#define _GPIO_USART_RXROUTE_PIN_SHIFT                    16                                      /**< Shift value for GPIO_PIN                    */
#define _GPIO_USART_RXROUTE_PIN_MASK                     0xF0000UL                               /**< Bit mask for GPIO_PIN                       */
#define _GPIO_USART_RXROUTE_PIN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for GPIO_USART_RXROUTE         */
#define GPIO_USART_RXROUTE_PIN_DEFAULT                   (_GPIO_USART_RXROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_USART_RXROUTE */

/* Bit fields for GPIO_USART CLKROUTE */
#define _GPIO_USART_CLKROUTE_RESETVALUE                  0x00000000UL                             /**< Default value for GPIO_USART_CLKROUTE       */
#define _GPIO_USART_CLKROUTE_MASK                        0x000F0003UL                             /**< Mask for GPIO_USART_CLKROUTE                */
#define _GPIO_USART_CLKROUTE_PORT_SHIFT                  0                                        /**< Shift value for GPIO_PORT                   */
#define _GPIO_USART_CLKROUTE_PORT_MASK                   0x3UL                                    /**< Bit mask for GPIO_PORT                      */
#define _GPIO_USART_CLKROUTE_PORT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for GPIO_USART_CLKROUTE        */
#define GPIO_USART_CLKROUTE_PORT_DEFAULT                 (_GPIO_USART_CLKROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_USART_CLKROUTE*/
#define _GPIO_USART_CLKROUTE_PIN_SHIFT                   16                                       /**< Shift value for GPIO_PIN                    */
#define _GPIO_USART_CLKROUTE_PIN_MASK                    0xF0000UL                                /**< Bit mask for GPIO_PIN                       */
#define _GPIO_USART_CLKROUTE_PIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for GPIO_USART_CLKROUTE        */
#define GPIO_USART_CLKROUTE_PIN_DEFAULT                  (_GPIO_USART_CLKROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_USART_CLKROUTE*/

/* Bit fields for GPIO_USART TXROUTE */
#define _GPIO_USART_TXROUTE_RESETVALUE                   0x00000000UL                            /**< Default value for GPIO_USART_TXROUTE        */
#define _GPIO_USART_TXROUTE_MASK                         0x000F0003UL                            /**< Mask for GPIO_USART_TXROUTE                 */
#define _GPIO_USART_TXROUTE_PORT_SHIFT                   0                                       /**< Shift value for GPIO_PORT                   */
#define _GPIO_USART_TXROUTE_PORT_MASK                    0x3UL                                   /**< Bit mask for GPIO_PORT                      */
#define _GPIO_USART_TXROUTE_PORT_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for GPIO_USART_TXROUTE         */
#define GPIO_USART_TXROUTE_PORT_DEFAULT                  (_GPIO_USART_TXROUTE_PORT_DEFAULT << 0) /**< Shifted mode DEFAULT for GPIO_USART_TXROUTE */
#define _GPIO_USART_TXROUTE_PIN_SHIFT                    16                                      /**< Shift value for GPIO_PIN                    */
#define _GPIO_USART_TXROUTE_PIN_MASK                     0xF0000UL                               /**< Bit mask for GPIO_PIN                       */
#define _GPIO_USART_TXROUTE_PIN_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for GPIO_USART_TXROUTE         */
#define GPIO_USART_TXROUTE_PIN_DEFAULT                   (_GPIO_USART_TXROUTE_PIN_DEFAULT << 16) /**< Shifted mode DEFAULT for GPIO_USART_TXROUTE */
/** @} End of group Parts */

#endif /* EFR32ZG23_GPIO_H */
#ifdef __cplusplus
}
#endif

